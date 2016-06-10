// wrapDataStore.h
// This is generated code, do not edit
//
// Copyright (c) 2015, Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory.
//
// All rights reserved.
//
// This source code cannot be distributed without permission and
// further review from Lawrence Livermore National Laboratory.
//
/**
 * \file wrapDataStore.h
 * \brief Shroud generated wrapper for DataStore class
 */
// For C users and C++ implementation

#ifndef WRAPDATASTORE_H
#define WRAPDATASTORE_H

#include "sidre/SidreTypes.h"
#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

// declaration of wrapped types
struct s_SIDRE_databuffer;
typedef struct s_SIDRE_databuffer SIDRE_databuffer;
struct s_SIDRE_datagroup;
typedef struct s_SIDRE_datagroup SIDRE_datagroup;
struct s_SIDRE_datastore;
typedef struct s_SIDRE_datastore SIDRE_datastore;

// splicer begin class.DataStore.C_definition
// splicer end class.DataStore.C_definition

SIDRE_datastore * SIDRE_datastore_new();

void SIDRE_datastore_delete(SIDRE_datastore * self);

SIDRE_datagroup * SIDRE_datastore_get_root(SIDRE_datastore * self);

SIDRE_databuffer * SIDRE_datastore_get_buffer(SIDRE_datastore * self,
                                              SIDRE_IndexType idx);

SIDRE_databuffer * SIDRE_datastore_create_buffer_empty(SIDRE_datastore * self);

SIDRE_databuffer * SIDRE_datastore_create_buffer_from_type(
  SIDRE_datastore * self, int type, SIDRE_SidreLength num_elems);

void SIDRE_datastore_destroy_buffer(SIDRE_datastore * self, SIDRE_IndexType id);

size_t SIDRE_datastore_get_num_buffers(SIDRE_datastore * self);

void SIDRE_datastore_print(SIDRE_datastore * self);

void SIDRE_datastore_save_0(SIDRE_datastore * self, const char * file_path,
                            const char * protocol);

void SIDRE_datastore_save_0_bufferify(SIDRE_datastore * self,
                                      const char * file_path, int Lfile_path,
                                      const char * protocol, int Lprotocol);

void SIDRE_datastore_save_1(SIDRE_datastore * self, const char * file_path,
                            const char * protocol,
                            const SIDRE_datagroup * group);

void SIDRE_datastore_save_1_bufferify(SIDRE_datastore * self,
                                      const char * file_path, int Lfile_path,
                                      const char * protocol, int Lprotocol,
                                      const SIDRE_datagroup * group);

void SIDRE_datastore_load_0(SIDRE_datastore * self, const char * file_path,
                            const char * protocol);

void SIDRE_datastore_load_0_bufferify(SIDRE_datastore * self,
                                      const char * file_path, int Lfile_path,
                                      const char * protocol, int Lprotocol);

void SIDRE_datastore_load_1(SIDRE_datastore * self, const char * file_path,
                            const char * protocol, SIDRE_datagroup * group);

void SIDRE_datastore_load_1_bufferify(SIDRE_datastore * self,
                                      const char * file_path, int Lfile_path,
                                      const char * protocol, int Lprotocol,
                                      SIDRE_datagroup * group);

void SIDRE_datastore_load_external_data_0(SIDRE_datastore * self,
                                          const char * file_path,
                                          const char * protocol);

void SIDRE_datastore_load_external_data_0_bufferify(SIDRE_datastore * self,
                                                    const char * file_path,
                                                    int Lfile_path,
                                                    const char * protocol,
                                                    int Lprotocol);

void SIDRE_datastore_load_external_data_1(SIDRE_datastore * self,
                                          const char * file_path,
                                          const char * protocol,
                                          SIDRE_datagroup * group);

void SIDRE_datastore_load_external_data_1_bufferify(SIDRE_datastore * self,
                                                    const char * file_path,
                                                    int Lfile_path,
                                                    const char * protocol,
                                                    int Lprotocol,
                                                    SIDRE_datagroup * group);

#ifdef __cplusplus
}
#endif

#endif  // WRAPDATASTORE_H
