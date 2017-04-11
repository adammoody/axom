// wrapIOManager.h
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
 * \file wrapIOManager.h
 * \brief Shroud generated wrapper for IOManager class
 */
// For C users and C++ implementation

#ifndef WRAPIOMANAGER_H
#define WRAPIOMANAGER_H

#include "mpi.h"
#include "sidre/wrapGroup.h"

#ifdef __cplusplus
extern "C" {
#endif

// declaration of wrapped types
struct s_SPIO_iomanager;
typedef struct s_SPIO_iomanager SPIO_iomanager;

// splicer begin class.IOManager.C_definition
// splicer end class.IOManager.C_definition

SPIO_iomanager * SPIO_iomanager_new(MPI_Fint com);

void SPIO_iomanager_delete(SPIO_iomanager * self);

void SPIO_iomanager_write(SPIO_iomanager * self, SIDRE_group * group, int num_files, const char * file_string, const char * protocol);

void SPIO_iomanager_write_bufferify(SPIO_iomanager * self, SIDRE_group * group, int num_files, const char * file_string, int Lfile_string, const char * protocol, int Lprotocol);

void SPIO_iomanager_write_group_to_root_file(SPIO_iomanager * self, SIDRE_group * group, const char * file_name);

void SPIO_iomanager_write_group_to_root_file_bufferify(SPIO_iomanager * self, SIDRE_group * group, const char * file_name, int Lfile_name);

void SPIO_iomanager_read_0(SPIO_iomanager * self, SIDRE_group * group, const char * file_string, const char * protocol);

void SPIO_iomanager_read_0_bufferify(SPIO_iomanager * self, SIDRE_group * group, const char * file_string, int Lfile_string, const char * protocol, int Lprotocol);

void SPIO_iomanager_read_1(SPIO_iomanager * self, SIDRE_group * group, const char * root_file);

void SPIO_iomanager_read_1_bufferify(SPIO_iomanager * self, SIDRE_group * group, const char * root_file, int Lroot_file);

void SPIO_iomanager_load_external_data(SPIO_iomanager * self, SIDRE_group * group, const char * root_file);

void SPIO_iomanager_load_external_data_bufferify(SPIO_iomanager * self, SIDRE_group * group, const char * root_file, int Lroot_file);

#ifdef __cplusplus
}
#endif

#endif  // WRAPIOMANAGER_H
