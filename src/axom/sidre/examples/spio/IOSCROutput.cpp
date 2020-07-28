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

#ifdef AXOM_USE_SCR
#include "scr.h"
#endif

using axom::sidre::Group;
using axom::sidre::DataStore;
using axom::sidre::DataType;
using axom::sidre::IOManager;
using namespace axom::utilities;

/**************************************************************************
 * Subroutine:  main
 * Purpose   :
 *************************************************************************/

int main(int argc, char* argv[])
{
  MPI_Init(&argc, &argv);
  SCR_Init();
  axom::slic::UnitTestLogger logger;

  SLIC_ERROR_IF(argc != 2,
                "Missing command line arguments. \n\t"
                << "Usage: spio_IOOutput <base_file_name>");

  std::string file_base;
  if (argc == 2)
  {
    file_base = argv[1];
  }
  else
  {
    return 0;
  }

  int my_rank, num_ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  // Tell SCR we're starting an output set.
  // Each SCR output set should be given a name.
  // This name should be user-friendly, because
  // end users may need to type it at a command line.
  // It should also encode enough information that
  // one can construct the full path to the file on the
  // parallel file system.
  SCR_Start_output(file_base.c_str(), SCR_FLAG_OUTPUT);

  DataStore* ds = new DataStore();
  SLIC_ASSERT(ds);
  Group* root = ds->getRoot();

  Group* flds = root->createGroup("fields");
  Group* flds2 = root->createGroup("fields2");

  Group* ga = flds->createGroup("a");
  Group* gb = flds2->createGroup("b");
  ga->createViewScalar<int>("i0", my_rank + 101);
  gb->createViewScalar<int>("i1", 4*my_rank*my_rank + 404);

  // One must write with MPI_COMM_WORLD when using SCR.
  // Also, the number of files should be the same as
  // the number of ranks in the job.
  IOManager writer(MPI_COMM_WORLD, true);
  writer.write(root, num_ranks, file_base, "sidre_hdf5");

  MPI_Barrier(MPI_COMM_WORLD);

  delete ds;

  // Tell SCR this process has completed its output.
  // Each process should set valid=1 if it wrote its portion
  // of the output successfully and valid=0 otherwise.
  // SCR executes an allreduce across processes to identify
  // whether all ranks succeeded.
  // If any rank failed, SCR considers the output
  // to be invalid.
  // The return code will be SCR_SUCCESS if all procs
  // succeeded.
  int valid = 1;
  int complete_rc = SCR_Complete_output(valid);
  SLIC_ERROR_IF(complete_rc != SCR_SUCCESS,
                "SCR output failed.\n");

  SCR_Finalize();
  MPI_Finalize();

  return 0;
}
