// Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

/**************************************************************************
 *************************************************************************/

#include "mpi.h"

#include "axom/core.hpp"
#include "axom/slic.hpp"
#include "axom/sidre.hpp"

#include "CLI11/CLI11.hpp"

#ifdef AXOM_USE_SCR
#include "scr.h"
#endif

using axom::sidre::Group;
using axom::sidre::DataStore;
using axom::sidre::DataType;
using axom::sidre::IOManager;
using namespace axom::utilities;

/** Load checkpoint from SCR, returns true if a checkpoint was loaded
 *  and false otherwise */
bool loadRestart(MPI_Comm comm, const std::string& file_base, DataStore* ds)
{
  // SCR restart loop
  // The application can continue trying to read a checkpoint
  // until it succeeds or it exhausts all available checkpoints.
  int have_restart = 0;
  int restarted = 0;
  do
  {
    // Ask SCR if there is a checkpoint to read.
    // SCR loads the next most recent checkpoint that
    // it has not internally marked as "failed".
    // If there is no checkpoint, SCR will set have_restart to 0.
    // SCR will set have_restart to 1 if a checkpoint is
    // available, and in that case, it copies the checkpoint name into ckptname.
    // This name is the same string the application provided when
    // it called SCR_Start_output to first write the checkpoint.
    // SCR_Have_restart must be called by all processes in MPI_COMM_WORLD.
    char ckptname[SCR_MAX_FILENAME];
    SCR_Have_restart(&have_restart, ckptname);
    if (have_restart)
    {
      // Tell SCR we're starting to read the checkpoint.
      // One must only call SCR_Start_restart if SCR_Have_restart
      // reports that there is a checkpoint to restart from.
      // SCR returns the same name in ckptname as SCR_Have_restart.
      // SCR_Start_restart must be called by all processes in MPI_COMM_WORLD.
      SCR_Start_restart(ckptname);

      // It is common to use the checkpoint name to compute
      // the path to the checkpoint file.
      // One should build the path to the file on the parallel
      // file system.
      std::string root_file = std::string(ckptname) + "." + file_base + ".root";

      // read the restart data into our dataset
      IOManager reader(comm, true);
      reader.read(ds->getRoot(), root_file, false);

      // Tell SCR whether this process succeeded reading its checkpoint.
      // Each process should set valid=1 if it read its portion
      // of the checkpoint successfully and valid=0 otherwise.
      // SCR executes an allreduce across processes to identify
      // whether all ranks succeeded.
      // If any rank failed, SCR considers the checkpoint
      // to be invalid, and then SCR will load the next most
      // recent checkpoint if one exists.
      // The return code will be SCR_SUCCESS if all procs
      // succeeded.
      // SCR_Complete_restart must be called by all processes in MPI_COMM_WORLD.
      int valid = 1;
      int rc = SCR_Complete_restart(valid);
      restarted = (rc == SCR_SUCCESS);
    }
  }
  while (have_restart && !restarted);

  // tell the caller whether we loaded a restart from SCR
  return (bool) restarted;
}

/** Ask SCR whether we should take a defensive checkpoint */
bool needCheckpoint(void)
{
  // Optionally ask SCR whether it's time to checkpoint.
  // For defensive checkpoints, SCR can guide an application
  // to an efficient checkpoint frequency based on metrics
  // like checkpoint cost and expected failure rate.  It is
  // not required that an application call SCR_Need_checkpoint.
  // Also, an application is free to checkpoint or not, regardless
  // of the value of the flag returned from SCR_Need_checkpoint.
  // SCR_Need_checkpoint must be called by all processes in MPI_COMM_WORLD.
  int need_checkpoint;
  SCR_Need_checkpoint(&need_checkpoint);
  return (bool) need_checkpoint;
}

/** Write a checkpoint via SCR */
bool dumpCheckpoint(MPI_Comm comm, const std::string& file_base, int t,
                    int num_files, DataStore* ds)
{
  // Tell SCR we're starting a checkpoint.
  // Each SCR output set should be given a name.
  // This name should be user-friendly, because
  // end users may need to type it at a command line.
  // It should also encode enough information that
  // one can construct the full path to the file on the
  // parallel file system.
  // SCR_Start_output must be called by all processes in MPI_COMM_WORLD.
  std::string ckptname = "time." + std::to_string(t);
  SCR_Start_output(ckptname.c_str(), SCR_FLAG_CHECKPOINT);

  // build name to checkpoint file
  std::string ckpt_path = ckptname + "." + file_base;

  // write out the Datastore
  IOManager writer(comm, true);
  writer.write(ds->getRoot(), num_files, ckpt_path, "sidre_hdf5");

  // Tell SCR this process has completed its checkpoint.
  // Each process should set valid=1 if it wrote its portion
  // of the checkpoint successfully and valid=0 otherwise.
  // SCR executes an allreduce across processes to identify
  // whether all ranks succeeded.
  // If any rank failed, SCR considers the checkpoint
  // to be invalid.
  // The return code will be SCR_SUCCESS if all procs
  // succeeded.
  // SCR_Complete_output must be called by all processes in MPI_COMM_WORLD.
  int valid = 1;
  int complete_rc = SCR_Complete_output(valid);
  return (complete_rc == SCR_SUCCESS);
}

/** Ask SCR if the run should stop executing */
bool shouldExit(void)
{
  // Optionally, an application can ask SCR whether it should
  // stop executing.  When using SCR to cache datasets, it is
  // important to exit a run early enough to leave sufficient
  // time for SCR to flush cached datasets to the parallel
  // file system before the job's time limit expires.
  // If SCR_Should_exit indicates that the application should exit,
  // the application should stop executing and call SCR_Finalize.
  // SCR_Should_exit must be called by all processes in MPI_COMM_WORLD.
  int should_exit;
  SCR_Should_exit(&should_exit);
  return (bool) should_exit;
}

/** Simple structure to hold the parsed command line arguments */
struct CommandLineArguments
{
  int m_numSteps;
  int m_numFiles;
  std::string m_fileBase;

  CommandLineArguments()
    : m_numSteps(1)
    , m_numFiles(0)
    , m_fileBase("test.hdf")
  {}

  void parse(int argc, char** argv, CLI::App& app);
};

/** Parse the command line arguments */
void CommandLineArguments::parse(int argc, char** argv, CLI::App& app)
{
  app.add_option("-s,--steps", m_numSteps,
                 "Number of time steps")
  ->check(CLI::PositiveNumber);

  app.add_option("-n,--num", m_numFiles,
                 "Number of files per checkpoint")
  ->check(CLI::PositiveNumber);

  app.add_option("-f,--file", m_fileBase,
                 "Base name of checkpoint files");

  app.get_formatter()->column_width(35);

  // Could throw an exception
  app.parse(argc, argv);
}

/** Terminates execution */
void quitProgram(int exitCode = 0)
{
  MPI_Finalize();
  exit(exitCode);
}

/**************************************************************************
 * Subroutine:  main
 * Purpose   :  This demonstrates use of SCR with SPIO to restart from a
 *              checkpoint, if one exists, and to periodically write
 *              new checkpoints during an application timestep loop.
 *************************************************************************/

int main(int argc, char* argv[])
{
  axom::slic::UnitTestLogger logger;

  MPI_Init(&argc, &argv);

  int my_rank, num_ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  // parse the command line arguments
  CommandLineArguments args;
  CLI::App app {"SCR Checkpoint/Restart example"};

  try
  {
    args.parse(argc, argv, app);
  }
  catch (const CLI::ParseError &e)
  {
    int retval = -1;
    if(my_rank==0)
    {
      retval = app.exit(e);
    }
    MPI_Bcast(&retval, 1, MPI_INT, 0, MPI_COMM_WORLD);
    quitProgram(retval);
  }

  // Extract values from command line.
  int num_steps = args.m_numSteps;
  int num_files = args.m_numFiles;
  std::string file_base = args.m_fileBase;

  // Default to write a file per process.
  if (num_files == 0)
  {
    num_files = num_ranks;
  }

  // SCR_Init must be called by all processes in MPI_COMM_WORLD,
  // and it must be called after MPI_Init.
  SCR_Init();

  // These variables serve as our timestep counter and
  // its starting value.
  int t = 0;
  int t_start = 0;

  // This lone variable is an example to represent the
  // internal compute state of the application.
  int my_data = my_rank;

  DataStore* ds = new DataStore();
  SLIC_ASSERT(ds);

  // Attempt to load a restart from SCR.
  bool restarted = loadRestart(MPI_COMM_WORLD, file_base, ds);
  if (restarted)
  {
    // We successfully read a restart from SCR.
    // Use the dataset it filled in to initialize our state,
    // such as our timestep counter.
    t_start = ds->getRoot()->getView("timestep")->getScalar();
    my_data = ds->getRoot()->getView("state")->getScalar();

    // The timestep recorded in the checkpoint is one that
    // was last computed, so start this run on the next step.
    t_start++;
  }
  else
  {
    // We did not load a restart, so initialize the datastore.
    Group* root = ds->getRoot();
    root->createViewScalar<int>("timestep", t);
    root->createViewScalar<int>("state", my_data);
  }

  // Application work loop
  for (t = t_start ; t < t_start + num_steps ; t++)
  {
    /////////////////////////
    // Do actual work which changes internal state ...
    /////////////////////////
    my_data += 1;

    // Optionally, one can ask SCR whether it's time to checkpoint.
    // This call is purely advisory for defensive checkpoints,
    // and the application is free to checkpoint whenever it needs to.
    if (needCheckpoint())
    {
      // Update the Datastore to capture current state.
      ds->getRoot()->getView("timestep")->setScalar<int>(t);
      ds->getRoot()->getView("state")->setScalar<int>(my_data);

      // Write checkpoint via SCR.
      // One must write with MPI_COMM_WORLD when using SCR.
      // Also, the number of files should be the same as
      // the number of ranks in the job.
      dumpCheckpoint(MPI_COMM_WORLD, file_base, t, num_files, ds);

      // When using SCR to cache datasets, the application should
      // exit early enough to leave SCR time to flush those datasets
      // before the job allocation expires.
      if (shouldExit())
      {
        break;
      }
    }
  }

  delete ds;

  // SCR_Finalize must be called by all processes in MPI_COMM_WORLD,
  // and it must be called before MPI_Finalize.
  // Among other things, SCR_Finalize flushes cached datasets to the
  // parallel file system.  It also signals the SCR run scripts
  // that the job should *not* be restarted within the current job
  // allocation.
  SCR_Finalize();

  MPI_Finalize();

  return 0;
}
