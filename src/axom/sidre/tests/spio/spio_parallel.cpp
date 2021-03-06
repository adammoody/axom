// Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

/* An excerpt from this test file is used in the Sidre Sphinx documentation,
 * denoted by the comment strings
 *
 * parallel_io_headers_start
 * parallel_io_headers_end
 * parallel_io_save_start
 * parallel_io_save_end
 * parallel_io_load_start
 * parallel_io_load_end
 *
 * prepended with an underscore.
 */

#include "gtest/gtest.h"

// _parallel_io_headers_start
#include "axom/config.hpp"   // for AXOM_USE_HDF5

#include "conduit_relay.hpp"

#ifdef AXOM_USE_HDF5
#include "conduit_relay_io_hdf5.hpp"
#endif

#include "axom/sidre/core/sidre.hpp"
#include "axom/sidre/spio/IOManager.hpp"
#include "fmt/fmt.hpp"

#include "mpi.h"
// _parallel_io_headers_end

using axom::sidre::Group;
using axom::sidre::DataStore;
using axom::sidre::DataType;
using axom::sidre::View;
using axom::sidre::IOManager;

namespace
{
#ifdef AXOM_USE_HDF5
const std::string PROTOCOL = "sidre_hdf5";
const std::string ROOT_EXT = ".root";
#else
// Note: 'sidre_json' does not work for this test
// since it doesn't preserve bitwidth sizes (e.g. int)
const std::string PROTOCOL = "sidre_conduit_json";
const std::string ROOT_EXT = ".conduit_json.root";
#endif

/** Returns the number of output files for spio  */
int numOutputFiles(int numRanks)
{
#ifdef AXOM_USE_HDF5
  return std::max( numRanks / 2, 1);
#else
  return numRanks;
#endif
}


}

//------------------------------------------------------------------------------

TEST(spio_parallel, parallel_writeread)
{
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int num_ranks;
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  const int num_output = numOutputFiles (num_ranks);

  /*
   * Create a DataStore and give it a small hierarchy of groups and views.
   *
   * The views are filled with repeatable nonsense data that will vary based
   * on rank.
   */
  DataStore* ds = new DataStore();

  Group* root = ds->getRoot();

  Group* flds = root->createGroup("fields");
  Group* flds2 = root->createGroup("fields2");

  Group* ga = flds->createGroup("a");
  Group* gb = flds2->createGroup("b");
  ga->createViewScalar<int>("i0", 101*my_rank);
  gb->createView("i1")->allocate(DataType::c_int(10));
  int* i1_vals = gb->getView("i1")->getData();

  for(int i=0 ; i<10 ; i++)
  {
    i1_vals[i] = (i+10) * (404-my_rank-i);
  }

  // The namespace qualifying IOManager is not necessary for compilation
  // (because are already using axom::sidre::IOManager), but we've kept it
  // because the following code is used in the documentation as an example.
  // _parallel_io_save_start
  /*
   * Contents of the DataStore written to files with IOManager.
   */
  int num_files = num_output;
  axom::sidre::IOManager writer(MPI_COMM_WORLD);

  const std::string file_name = "out_spio_parallel_write_read";

  writer.write(root, num_files, file_name, PROTOCOL);

  std::string root_name = file_name + ROOT_EXT;
  // _parallel_io_save_end

  /*
   * Extra stuff to exercise writeGroupToRootFile
   * Note: This is only valid for the 'sidre_hdf5' protocol
   */
  MPI_Barrier(MPI_COMM_WORLD);
  if (my_rank == 0 && PROTOCOL == "sidre_hdf5")
  {
    DataStore* dsextra = new DataStore();
    Group* extra = dsextra->getRoot()->createGroup("extra");
    extra->createViewScalar<double>("dval", 1.1);
    Group* child = extra->createGroup("child");
    child->createViewScalar<int>("ival", 7);
    child->createViewString("word0", "hello");
    child->createViewString("word1", "world");

    writer.writeGroupToRootFile(extra, root_name);

    Group* path_test = dsextra->getRoot()->createGroup("path_test");

    path_test->createViewScalar<int>("path_val", 9);
    path_test->createViewString("word2", "again");

    writer.writeGroupToRootFileAtPath(path_test, root_name, "extra/child");

    View* view_test =
      dsextra->getRoot()->createViewString("word3", "new_view");

    writer.writeViewToRootFileAtPath(view_test,
                                     root_name,
                                     "extra/child/path_test");

    delete dsextra;
  }
  MPI_Barrier(MPI_COMM_WORLD);

  /*
   * Read the root file on rank 1, unless this is a serial run.
   */
  if ( (my_rank == 1 || num_ranks == 1) && PROTOCOL == "sidre_hdf5" )
  {

    conduit::Node n;
    conduit::relay::io::load(root_name + ":extra", "hdf5", n);

    double dval = n["dval"].to_double();

    EXPECT_TRUE(dval > 1.0000009 && dval < 1.1000001);

    EXPECT_EQ(n["child"]["ival"].to_int(), 7);
    EXPECT_EQ(n["child"]["word0"].as_string(), "hello");
    EXPECT_EQ(n["child"]["word1"].as_string(), "world");
    EXPECT_EQ(n["child"]["path_test"]["path_val"].to_int(), 9);
    EXPECT_EQ(n["child"]["path_test"]["word2"].as_string(), "again");
    EXPECT_EQ(n["child"]["path_test"]["word3"].as_string(), "new_view");

  }

  // _parallel_io_load_start
  /*
   * Create another DataStore that holds nothing but the root group.
   */
  DataStore* ds2 = new DataStore();

  /*
   * Read from the files that were written above.
   */
  IOManager reader(MPI_COMM_WORLD);


  reader.read(ds2->getRoot(), root_name);
  // _parallel_io_load_end


  /*
   * Verify that the contents of ds2 match those written from ds.
   */
  EXPECT_TRUE(ds2->getRoot()->isEquivalentTo(root));

  int testvalue =
    ds->getRoot()->getGroup("fields")->getGroup("a")->getView("i0")->getData();
  int testvalue2 =
    ds2->getRoot()->getGroup("fields")->getGroup("a")->getView("i0")->getData();

  EXPECT_EQ(testvalue, testvalue2);

  View* view_i1_orig =
    ds->getRoot()->getGroup("fields2")->getGroup("b")->getView("i1");
  View* view_i1_restored =
    ds2->getRoot()->getGroup("fields2")->getGroup("b")->getView("i1");

  int num_elems = view_i1_orig->getNumElements();
  EXPECT_EQ(view_i1_restored->getNumElements(), num_elems);
  if (view_i1_restored->getNumElements() == num_elems)
  {
    int* i1_orig = view_i1_orig->getData();
    int* i1_restored = view_i1_restored->getData();

    for (int i = 0 ; i < num_elems ; ++i)
    {
      EXPECT_EQ(i1_orig[i], i1_restored[i]);
    }
  }

  delete ds;
  delete ds2;

}

//----------------------------------------------------------------------
TEST(spio_parallel, write_read_write)
{
  int my_rank, num_ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  const int num_files = numOutputFiles (num_ranks);

  std::stringstream sstr;
  sstr << "out_spio_WRW_" << num_ranks;
  std::string filename = sstr.str();

  // Initialize a datastore and dump to disk
  DataStore* ds = new DataStore();
  ds->getRoot()->createViewScalar("grp/i",2);
  ds->getRoot()->createViewScalar("grp/f",3.0);
  IOManager writer_a(MPI_COMM_WORLD);
  writer_a.write(ds->getRoot(), num_files, filename, PROTOCOL);

  // Create another DataStore to read into.
  DataStore ds_r;
  IOManager reader(MPI_COMM_WORLD);
  reader.read(ds_r.getRoot(), filename + ROOT_EXT);

  // Dump this datastore to disk.
  // Regression for sidre_hdf5 protocol:
  // This used to produce the following HDF5 error:
  //  HDF5-DIAG: Error detected in HDF5 (1.8.16) thread 0:
  //    #000: H5F.c line 522 in H5Fcreate(): unable to create file
  //      major: File accessibility
  //      minor: Unable to open file
  //    #001: H5Fint.c line 1024 in H5F_open(): unable to truncate a file which
  // is already open
  //      major: File accessibility
  //      minor: Unable to open file
  IOManager writer_b(MPI_COMM_WORLD);
  writer_b.write(ds_r.getRoot(), num_files, filename, PROTOCOL);
}

//------------------------------------------------------------------------------
TEST(spio_parallel, external_writeread)
{
  if( PROTOCOL != "sidre_hdf5")
  {
    SUCCEED()
      << "Loading external data in spio only currently supported "
      << " for 'sidre_hdf5' protocol";
    return;
  }

  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int num_ranks;
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  const int num_output = numOutputFiles (num_ranks);

  const int nvals = 10;
  int orig_vals1[nvals], orig_vals2[nvals];
  for(int i=0 ; i<10 ; i++)
  {
    orig_vals1[i] = (i+10) * (404-my_rank-i);
    orig_vals2[i] = (i+10) * (404-my_rank-i) + 20;
  }

  /*
   * Create a DataStore and give it a small hierarchy of groups and views.
   *
   * The views are filled with repeatable nonsense data that will vary based
   * on rank.
   */
  DataStore* ds1 = new DataStore();

  Group* root1 = ds1->getRoot();

  Group* flds = root1->createGroup("fields");
  Group* flds2 = root1->createGroup("fields2");

  Group* ga = flds->createGroup("a");
  Group* gb = flds2->createGroup("b");
  ga->createView("external_array", axom::sidre::INT_ID, nvals, orig_vals1);
  gb->createView("external_undescribed")->setExternalDataPtr(orig_vals2);

  /*
   * Contents of the DataStore written to files with IOManager.
   */
  int num_files = num_output;
  IOManager writer(MPI_COMM_WORLD);

  const std::string file_name = "out_spio_external_write_read";

  writer.write(root1, num_files, file_name, PROTOCOL);

  /*
   * Create another DataStore than holds nothing but the root group.
   */
  DataStore* ds2 = new DataStore();
  Group* root2 = ds2->getRoot();

  /*
   * Read from the files that were written above.
   */
  IOManager reader(MPI_COMM_WORLD);

  reader.read(root2, file_name + ROOT_EXT);

  int restored_vals1[nvals], restored_vals2[nvals];
  for (int i = 0 ; i < nvals ; ++i)
  {
    restored_vals1[i] = -1;
    restored_vals2[i] = -1;
  }

  View* view1 = root2->getView("fields/a/external_array");
  view1->setExternalDataPtr(restored_vals1);

  View* view2 = root2->getView("fields2/b/external_undescribed");
  view2->setExternalDataPtr(restored_vals2);

  reader.loadExternalData(root2, "out_spio_external_write_read.root");

  enum SpioTestResult
  {
    SPIO_TEST_SUCCESS = 0,
    HIERARCHY_ERROR   = 1<<0,
    EXT_ARRAY_ERROR   = 1<<1,
    EXT_UNDESC_ERROR  = 1<<2
  };
  int result = SPIO_TEST_SUCCESS;

  /*
   * Verify that the contents of ds2 match those written from ds.
   */
  EXPECT_TRUE(ds2->getRoot()->isEquivalentTo(root1));
  if (!ds2->getRoot()->isEquivalentTo(root1))
  {
    result |= HIERARCHY_ERROR;
  }
  SLIC_WARNING_IF( result & HIERARCHY_ERROR, "Tree layouts don't match");

  EXPECT_EQ(view1->getNumElements(), nvals);
  if (view1->getNumElements() != nvals)
  {
    result |= EXT_ARRAY_ERROR;
  }
  else
  {
    for (int i = 0 ; i < nvals ; ++i)
    {
      EXPECT_EQ(orig_vals1[i], restored_vals1[i]);
      if (orig_vals1[i] != restored_vals1[i])
      {
        result |= EXT_ARRAY_ERROR;
        break;
      }
    }
  }
  SLIC_WARNING_IF( result & EXT_ARRAY_ERROR,
                   "External_array was not correctly loaded");

  /*
   * external_undescribed was not written to disk (since it is undescribed)
   * make sure it was not read in.
   */
  for (int i = 0 ; i < nvals ; ++i)
  {
    EXPECT_EQ(-1, restored_vals2[i]);
    if (-1 != restored_vals2[i])
    {
      result |= EXT_UNDESC_ERROR;
      break;
    }
  }
  SLIC_WARNING_IF( result & EXT_UNDESC_ERROR,
                   "External_undescribed data was modified.");

  delete ds1;
  delete ds2;

}

//----------------------------------------------------------------------
TEST(spio_parallel, irregular_writeread)
{
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int num_ranks;
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  const int num_output = numOutputFiles (num_ranks);

  /*
   * Create a DataStore and give it a small hierarchy of groups and views.
   *
   * The views are filled with repeatable nonsense data that will vary based
   * on rank.
   */
  DataStore* ds1 = new DataStore();

  Group* root1 = ds1->getRoot();

  int num_fields = my_rank + 2;

  for (int f = 0 ; f < num_fields ; ++f)
  {
    std::ostringstream ostream;
    ostream << "fields" << f;
    Group* flds = root1->createGroup(ostream.str());

    int num_subgroups = ((f+my_rank)%3) + 1;
    for (int g = 0 ; g < num_subgroups ; ++g)
    {
      std::ostringstream gstream;
      gstream << "subgroup" << g;
      Group* sg = flds->createGroup(gstream.str());

      std::ostringstream vstream;
      vstream << "view" << g;
      if (g % 2)
      {
        sg->createView(vstream.str())->allocate(DataType::c_int(10+my_rank));
        int* vals = sg->getView(vstream.str())->getData();

        for(int i=0 ; i<10+my_rank ; i++)
        {
          vals[i] = (i+10) * (404-my_rank-i-g-f);
        }

      }
      else
      {
        sg->createViewScalar<int>(vstream.str(), 101*my_rank*(f+g+1));
      }
    }
  }


  /*
   * Contents of the DataStore written to files with IOManager.
   */
  int num_files = num_output;
  IOManager writer(MPI_COMM_WORLD);

  const std::string file_name = "out_spio_irregular_write_read";
  writer.write(root1, num_files, file_name, PROTOCOL);

  /*
   * Create another DataStore that holds nothing but the root group.
   */
  DataStore* ds2 = new DataStore();

  /*
   * Read from the files that were written above.
   */
  IOManager reader(MPI_COMM_WORLD);

  reader.read(ds2->getRoot(), file_name + ROOT_EXT);


  /*
   * Verify that the contents of ds2 match those written from ds.
   */
  EXPECT_TRUE(ds2->getRoot()->isEquivalentTo(root1));

  for (int f = 0 ; f < num_fields ; ++f)
  {
    std::ostringstream ostream;
    ostream << "fields" << f;
    Group* flds1 = ds1->getRoot()->getGroup(ostream.str());
    Group* flds2 = ds2->getRoot()->getGroup(ostream.str());

    int num_subgroups = ((f+my_rank)%3) + 1;
    for (int g = 0 ; g < num_subgroups ; ++g)
    {
      std::ostringstream gstream;
      gstream << "subgroup" << g;
      Group* sg1 = flds1->getGroup(gstream.str());
      Group* sg2 = flds2->getGroup(gstream.str());

      std::ostringstream vstream;
      vstream << "view" << g;
      if (g % 2)
      {

        View* view_orig = sg1->getView(vstream.str());
        View* view_restored = sg2->getView(vstream.str());

        int num_elems = view_orig->getNumElements();
        EXPECT_EQ(view_restored->getNumElements(), num_elems);
        int* vals_orig = view_orig->getData();
        int* vals_restored = view_restored->getData();

        for (int i = 0 ; i < num_elems ; ++i)
        {
          EXPECT_EQ(vals_orig[i], vals_restored[i]);
        }
      }
      else
      {
        int testvalue1 = sg1->getView(vstream.str())->getData();
        int testvalue2 = sg2->getView(vstream.str())->getData();

        EXPECT_EQ(testvalue1, testvalue2);
      }
    }
  }

  delete ds1;
  delete ds2;
}

//------------------------------------------------------------------------------
TEST(spio_parallel, preserve_writeread)
{
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int num_ranks;
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  const int num_output = numOutputFiles (num_ranks);

  /*
   * Create a DataStore and give it a small hierarchy of groups and views.
   *
   * The views are filled with repeatable nonsense data that will vary based
   * on rank.
   */
  DataStore* ds = new DataStore();

  Group* root = ds->getRoot();

  Group* flds = root->createGroup("fields");
  Group* flds2 = root->createGroup("fields2");

  Group* ga = flds->createGroup("a");
  Group* gb = flds2->createGroup("b");
  ga->createViewScalar<int>("i0", 101*my_rank);
  gb->createView("i1")->allocate(DataType::c_int(10));
  int* i1_vals = gb->getView("i1")->getData();

  for(int i=0 ; i<10 ; i++)
  {
    i1_vals[i] = (i+10) * (404-my_rank-i);
  }

  /*
   * Contents of the DataStore written to files with IOManager.
   */
  int num_files = num_output;
  IOManager writer(MPI_COMM_WORLD);

  const std::string file_name = "out_spio_preserve_write_read";
  writer.write(root, num_files, file_name, PROTOCOL);

  std::string root_name = file_name + ROOT_EXT;

  /*
   * Extra stuff to exercise preserve_contents option
   */
  MPI_Barrier(MPI_COMM_WORLD);
  DataStore* dsextra = new DataStore();
  Group* extra = dsextra->getRoot()->createGroup("extra");
  extra->createViewScalar<double>("dval", 1.1);
  Group* child = extra->createGroup("child");
  child->createViewScalar<int>("ival", 7);
  child->createViewString("word0", "hello");
  child->createViewString("word1", "world");

  const std::string extra_file_name = "out_spio_extra";
  writer.write(extra, num_files, extra_file_name, PROTOCOL);
  std::string extra_root = extra_file_name + ROOT_EXT;

  MPI_Barrier(MPI_COMM_WORLD);

  /*
   * Create another DataStore that holds nothing but the root group.
   */
  DataStore* ds2 = new DataStore();

  /*
   * Read from the files that were written above.
   */
  IOManager reader(MPI_COMM_WORLD);

  reader.read(ds2->getRoot(), root_name);

  /*
   * Verify that the contents of ds2 match those written from ds.
   */
  EXPECT_TRUE(ds2->getRoot()->isEquivalentTo(root));

  int testvalue =
    ds->getRoot()->getGroup("fields")->getGroup("a")->getView("i0")->getData();
  int testvalue2 =
    ds2->getRoot()->getGroup("fields")->getGroup("a")->getView("i0")->getData();

  EXPECT_EQ(testvalue, testvalue2);

  View* view_i1_orig =
    ds->getRoot()->getGroup("fields2")->getGroup("b")->getView("i1");
  View* view_i1_restored =
    ds2->getRoot()->getGroup("fields2")->getGroup("b")->getView("i1");

  int num_elems = view_i1_orig->getNumElements();
  EXPECT_EQ(view_i1_restored->getNumElements(), num_elems);

  int* i1_orig = view_i1_orig->getData();
  int* i1_restored = view_i1_restored->getData();

  for (int i = 0 ; i < num_elems ; ++i)
  {
    EXPECT_EQ(i1_orig[i], i1_restored[i]);
  }

  /*
   * Read in extra file while preserving contents
   */
  Group* extra_fields = ds2->getRoot()->getGroup("fields");
  reader.read(extra_fields, extra_root, true);

  /*
   * Test one of the pre-existing Views to show it's unchanged.
   */
  int testvalue_extra = extra_fields->getGroup("a")->getView("i0")->getData();
  EXPECT_EQ(testvalue_extra, testvalue2);

  /*
   * Test the data from the extra file.
   */
  double dval = extra_fields->getView("dval")->getData();

  EXPECT_TRUE(dval > 1.0000009 && dval < 1.1000001);

  int ival = extra_fields->getView("child/ival")->getData();
  EXPECT_EQ(ival, 7);

  EXPECT_EQ(std::string(extra_fields->getView(
                          "child/word0")->getString()), "hello");

  EXPECT_EQ(std::string(extra_fields->getView(
                          "child/word1")->getString()), "world");

  delete ds;
  delete ds2;
  delete dsextra;
}


TEST(spio_parallel, parallel_increase_procs)
{
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  int num_ranks;
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  EXPECT_TRUE(num_ranks > my_rank);

#ifdef AXOM_USE_HDF5
  /*
   * This tests the ability to read data when the run that is doing the
   * reading has more processors than the run that created the files
   * being read.  In this test we split the world communicator into
   * smaller communicators to simulate a run that dumps data from
   * only ranks 0 and 1 (only rank 0 if running on <= 2 processors).
   * Then the read occurs with an IOManager constructed with
   * MPI_COMM_WORLD.
   *
   * This functionality only works with the sidre_hdf5 protocol, so this
   * section of code is inside the #ifdef guards.
   */

  int top_output_rank = 1;
  if (num_ranks <= 2)
  {
    top_output_rank = 0;
  }

  // Split the communicator so that ranks up to and including
  // top_output_rank have their own communicator for the output step.
  MPI_Comm split_comm;

  if (my_rank <= top_output_rank)
  {
    MPI_Comm_split(MPI_COMM_WORLD, 0, my_rank, &split_comm);
  }
  else
  {
    MPI_Comm_split(MPI_COMM_WORLD, my_rank, 0, &split_comm);
  }

  DataStore* ds = new DataStore();
  if (my_rank <= top_output_rank)
  {

    Group* root = ds->getRoot();

    Group* flds = root->createGroup("fields");
    Group* flds2 = root->createGroup("fields2");

    Group* ga = flds->createGroup("a");
    Group* gb = flds2->createGroup("b");
    ga->createViewScalar<int>("i0", 101*my_rank);
    gb->createView("i1")->allocate(DataType::c_int(10));
    int* i1_vals = gb->getView("i1")->getData();

    for(int i=0 ; i<10 ; i++)
    {
      i1_vals[i] = (i+10) * (404-my_rank-i);
    }

    int num_files = 1;
    axom::sidre::IOManager writer(split_comm);

    const std::string file_name = "out_spio_parallel_increase_procs";

    writer.write(root, num_files, file_name, PROTOCOL);

  }

  /*
   * The reading section of this test will execute a read on all
   * ranks of MPI_COMM_WORLD, even though the write was only on one or two
   * ranks.  The read will load data on the ranks that wrote data and add
   * nothing on higher ranks.
   */
  DataStore* ds2 = new DataStore();

  IOManager reader(MPI_COMM_WORLD);

  const std::string root_name = "out_spio_parallel_increase_procs.root";
  reader.read(ds2->getRoot(), root_name);

  /*
   * Verify that the contents of ds2 on rank 0 match those written from ds.
   */

  if (my_rank <= top_output_rank)
  {
    EXPECT_TRUE(my_rank != 0 || ds2->getRoot()->isEquivalentTo(ds->getRoot()));

    int testvalue =
      ds->getRoot()->getGroup("fields")->getGroup("a")->getView("i0")->getData();
    int testvalue2 =
      ds2->getRoot()->getGroup("fields")->getGroup("a")->getView("i0")->getData();

    EXPECT_EQ(testvalue, testvalue2);

    View* view_i1_orig =
      ds->getRoot()->getGroup("fields2")->getGroup("b")->getView("i1");
    View* view_i1_restored =
      ds2->getRoot()->getGroup("fields2")->getGroup("b")->getView("i1");

    int num_elems = view_i1_orig->getNumElements();
    EXPECT_EQ(view_i1_restored->getNumElements(), num_elems);
    if (view_i1_restored->getNumElements() == num_elems)
    {
      int* i1_orig = view_i1_orig->getData();
      int* i1_restored = view_i1_restored->getData();

      for (int i = 0 ; i < num_elems ; ++i)
      {
        EXPECT_EQ(i1_orig[i], i1_restored[i]);
      }
    }
  }
  else
  {
    EXPECT_FALSE(ds2->getRoot()->hasGroup("fields"));
    EXPECT_FALSE(ds2->getRoot()->hasGroup("fields2"));
    EXPECT_EQ(ds2->getRoot()->getNumGroups(), 0);
    EXPECT_EQ(ds2->getRoot()->getNumViews(), 0);
  }

  delete ds2;
  delete ds;

  MPI_Comm_free(&split_comm);

#endif

}

TEST(spio_parallel, parallel_decrease_procs)
{
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  int num_ranks;
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  EXPECT_TRUE(num_ranks > my_rank);

#ifdef AXOM_USE_HDF5
  /*
   * This tests the ability to read data when the run that is doing the
   * reading has fewer processors than the run that created the files
   * being read.  In this test we dump data using all ranks on the world
   * communicator, and then to read the data we split the world communicator
   * into smaller communicators in order to read data using only ranks 0
   * and 1 (only rank 0 if running on <= 2 processors).
   *
   * This functionality only works with the sidre_hdf5 protocol, so this
   * section of code is inside the #ifdef guards.
   */

  DataStore* ds = new DataStore();
  Group* root = ds->getRoot();

  Group* flds = root->createGroup("fields");
  Group* flds2 = root->createGroup("fields2");

  Group* ga = flds->createGroup("a");
  Group* gb = flds2->createGroup("b");
  ga->createViewScalar<int>("i0", 101*my_rank);
  gb->createView("i1")->allocate(DataType::c_int(10));
  int* i1_vals = gb->getView("i1")->getData();

  for(int i=0 ; i<10 ; i++)
  {
    i1_vals[i] = (i+10) * (404-my_rank-i);
  }

  int num_files = num_ranks;
  axom::sidre::IOManager writer(MPI_COMM_WORLD);

  const std::string file_name = "out_spio_parallel_decrease_procs";

  writer.write(root, num_files, file_name, PROTOCOL);

  int top_input_rank = 1;
  if (num_ranks <= 2)
  {
    top_input_rank = 0;
  }

  // Split the communicator so that ranks up to and including
  // top_input_rank have their own communicator for the input step.
  MPI_Comm split_comm;

  if (my_rank <= top_input_rank)
  {
    MPI_Comm_split(MPI_COMM_WORLD, 0, my_rank, &split_comm);
  }
  else
  {
    MPI_Comm_split(MPI_COMM_WORLD, my_rank, 0, &split_comm);
  }

  /*
   * The reading section of this test will execute a read on ranks 0 and 1,
   * or only 0 if running on <= 2 processors.
   */
  DataStore* ds2 = new DataStore();

  if (my_rank <= top_input_rank)
  {
    IOManager reader(split_comm);

    const std::string root_name = "out_spio_parallel_decrease_procs.root";
    reader.read(ds2->getRoot(), root_name);

    Group* ds2_root = ds2->getRoot();

    int num_output_ranks = 1;
    if (num_ranks > 1)
    {
      EXPECT_TRUE(ds2_root->hasView("reduced_input_ranks"));
    }

    if (ds2->getRoot()->hasView("reduced_input_ranks"))
    {
      num_output_ranks = ds2_root->getView("reduced_input_ranks")->getData();
    }

    for (int output_rank = my_rank ; output_rank < num_output_ranks ;
         output_rank += (top_input_rank+1))
    {
      /*
       * Verify that the contents of ds2 on rank 0 match those written from ds.
       */
      std::string output_name = fmt::sprintf("rank_%07d/sidre_input",
                                             output_rank);

      int testvalue = 101*output_rank;
      int testvalue2 =
        ds2_root->getGroup(output_name)->getGroup("fields")->getGroup("a")->
        getView("i0")->getData();

      EXPECT_EQ(testvalue, testvalue2);

      View* view_i1_orig =
        ds->getRoot()->getGroup("fields2")->getGroup("b")->getView("i1");
      View* view_i1_restored =
        ds2_root->getGroup(output_name)->getGroup("fields2")->getGroup("b")->
        getView("i1");

      int num_elems = view_i1_orig->getNumElements();
      EXPECT_EQ(view_i1_restored->getNumElements(), num_elems);
      if (view_i1_restored->getNumElements() == num_elems)
      {
        int* i1_restored = view_i1_restored->getData();

        for (int i = 0 ; i < num_elems ; ++i)
        {
          EXPECT_EQ((i+10) * (404-output_rank-i), i1_restored[i]);
        }
      }
    }
  }
  else
  {
    EXPECT_EQ(ds2->getRoot()->getNumGroups(), 0);
    EXPECT_EQ(ds2->getRoot()->getNumViews(), 0);
  }

  delete ds2;
  delete ds;

  MPI_Comm_free(&split_comm);

#endif

}



#include "axom/slic/core/UnitTestLogger.hpp"
using axom::slic::UnitTestLogger;

//----------------------------------------------------------------------
int main(int argc, char* argv[])
{
  int result = 0;
  ::testing::InitGoogleTest(&argc, argv);

  UnitTestLogger logger;  // create & initialize test logger,

  MPI_Init(&argc, &argv);
  result = RUN_ALL_TESTS();
  MPI_Finalize();

  return result;
}
