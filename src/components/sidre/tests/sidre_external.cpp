/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and
 * further review from Lawrence Livermore National Laboratory.
 */

#include "gtest/gtest.h"

#include <vector>

#include "sidre/sidre.hpp"

using asctoolkit::sidre::DataBuffer;
using asctoolkit::sidre::DataGroup;
using asctoolkit::sidre::DataStore;
using asctoolkit::sidre::DataView;
using asctoolkit::sidre::DataType;

//------------------------------------------------------------------------------
// Test DataBuffer::declareExternal()
//------------------------------------------------------------------------------
TEST(sidre_external, declare_external_buffer)
{
  DataStore * ds   = new DataStore();

  const int len = 11;

  int * idata = new int[len];
  double * ddata = new double[len];

  for (int ii = 0 ; ii < len ; ++ii)
  {
    idata[ii] = ii;
    ddata[ii] = idata[ii] * 2.0;
  }

  DataBuffer * dbuff_0 = ds->createBuffer();
  DataBuffer * dbuff_1 = ds->createBuffer();
  DataBuffer * dbuff_2 = ds->createBuffer();

  dbuff_0->allocate(asctoolkit::sidre::DOUBLE_ID, len);
  dbuff_1->declare(asctoolkit::sidre::INT_ID, len);
  dbuff_1->setExternalData(idata);
  dbuff_2->declare(asctoolkit::sidre::DOUBLE_ID, len);
  dbuff_2->setExternalData(ddata);

  EXPECT_EQ(dbuff_0->isExternal(), false);
  EXPECT_EQ(dbuff_1->isExternal(), true);
  EXPECT_EQ(dbuff_2->isExternal(), true);

  EXPECT_EQ(dbuff_0->getTotalBytes(), sizeof(CONDUIT_NATIVE_DOUBLE)*len);
  EXPECT_EQ(dbuff_1->getTotalBytes(), sizeof(CONDUIT_NATIVE_INT)*len);
  EXPECT_EQ(dbuff_2->getTotalBytes(), sizeof(CONDUIT_NATIVE_DOUBLE)*len);

  ds->print();

  delete ds;
  delete [] idata;
  delete [] ddata;
}

//------------------------------------------------------------------------------
// Test DataGroup::createExternalView()
//------------------------------------------------------------------------------
TEST(sidre_external, create_external_view)
{
  DataStore * ds   = new DataStore();
  DataGroup * root = ds->getRoot();

  const int len = 11;

  int * idata = new int[len];
  double * ddata = new double[len];

  for (int ii = 0 ; ii < len ; ++ii)
  {
    idata[ii] = ii;
    ddata[ii] = idata[ii] * 2.0;
  }

  (void) root->createExternalView("idata", idata,
                                  DataType::c_int(len));
  (void) root->createExternalView("ddata", ddata,
                                  DataType::c_double(len));
  EXPECT_EQ(root->getNumViews(), 2u);

  root->getView("idata")->print();
  root->getView("ddata")->print();

  int * idata_chk = root->getView("idata")->getData();
  for (int ii = 0 ; ii < len ; ++ii)
  {
    EXPECT_EQ(idata_chk[ii], idata[ii]);
  }

  double * ddata_chk = root->getView("ddata")->getData();
  for (int ii = 0 ; ii < len ; ++ii)
  {
    EXPECT_EQ(ddata_chk[ii], ddata[ii]);
  }

  delete ds;
  delete [] idata;
  delete [] ddata;
}

//------------------------------------------------------------------------------
// Test DataGroup::save(), DataGroup::load() with external buffers
//------------------------------------------------------------------------------
TEST(sidre_external, save_load_external_view)
{
  DataStore * ds   = new DataStore();
  DataGroup * root = ds->getRoot();

  const int len = 11;

  int * idata = new int[len];
  double * ddata = new double[len];

  for (int ii = 0 ; ii < len ; ++ii)
  {
    idata[ii] = ii;
    ddata[ii] = idata[ii] * 2.0;
  }

  (void) root->createExternalView("idata", idata,
                                  DataType::c_int(len));
  (void) root->createExternalView("ddata", ddata,
                                  DataType::c_double(len));
  EXPECT_EQ(root->getNumViews(), 2u);
  EXPECT_EQ(root->getView("idata")->getBuffer()->isExternal(), true);
  EXPECT_EQ(root->getView("ddata")->getBuffer()->isExternal(), true);

  root->getView("idata")->print();
  root->getView("ddata")->print();

  ds->getRoot()->save("out_sidre_external_save_restore_external_view", "conduit");

  ds->print();


  DataStore * ds2 = new DataStore();

  ds2->getRoot()->load("out_sidre_external_save_restore_external_view","conduit");

  ds2->print();

  DataGroup * root2 = ds2->getRoot();

  EXPECT_EQ(root2->getNumViews(), 2u);
  EXPECT_EQ(root2->getView("idata")->getBuffer()->isExternal(), false);
  EXPECT_EQ(root2->getView("ddata")->getBuffer()->isExternal(), false);

  int * idata_chk = root2->getView("idata")->getData();
  for (int ii = 0 ; ii < len ; ++ii)
  {
    EXPECT_EQ(idata_chk[ii], idata[ii]);
  }

  double * ddata_chk = root2->getView("ddata")->getData();
  for (int ii = 0 ; ii < len ; ++ii)
  {
    EXPECT_EQ(ddata_chk[ii], ddata[ii]);
  }

  delete ds;
  delete ds2;
  delete [] idata;
  delete [] ddata;
}
