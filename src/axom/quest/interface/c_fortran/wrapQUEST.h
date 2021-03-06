// wrapQUEST.h
// This is generated code, do not edit
//
// Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
/**
 * \file wrapQUEST.h
 * \brief Shroud generated wrapper for quest namespace
 */
// For C users and C++ implementation

#ifndef WRAPQUEST_H
#define WRAPQUEST_H

#ifdef AXOM_USE_MPI
#include "mpi.h"
#endif
#include "typesQUEST.h"
#ifndef __cplusplus
#include <stdbool.h>
#endif

// splicer begin CXX_declarations
// splicer end CXX_declarations

#ifdef __cplusplus
extern "C" {
#endif

// splicer begin C_declarations
// splicer end C_declarations

#ifdef AXOM_USE_MPI
int QUEST_inout_init_mpi(const char* fileName, MPI_Fint comm);
#endif

#ifdef AXOM_USE_MPI
int QUEST_inout_init_mpi_bufferify(const char* fileName, int LfileName,
                                   MPI_Fint comm);
#endif

#ifndef AXOM_USE_MPI
int QUEST_inout_init_serial(const char* fileName);
#endif

#ifndef AXOM_USE_MPI
int QUEST_inout_init_serial_bufferify(const char* fileName, int LfileName);
#endif

bool QUEST_inout_initialized();

int QUEST_inout_set_verbose(bool verbosity);

int QUEST_inout_set_vertex_weld_threshold(double thresh);

bool QUEST_inout_evaluate_0(double x, double y);

bool QUEST_inout_evaluate_1(double x, double y, double z);

int QUEST_inout_mesh_min_bounds(double* coords);

int QUEST_inout_mesh_max_bounds(double* coords);

int QUEST_inout_mesh_center_of_mass(double* coords);

int QUEST_inout_get_dimension();

int QUEST_inout_finalize();

#ifdef AXOM_USE_MPI
int QUEST_signed_distance_init_mpi(const char* file, MPI_Fint comm);
#endif

#ifdef AXOM_USE_MPI
int QUEST_signed_distance_init_mpi_bufferify(const char* file, int Lfile,
                                             MPI_Fint comm);
#endif

#ifndef AXOM_USE_MPI
int QUEST_signed_distance_init_serial(const char* file);
#endif

#ifndef AXOM_USE_MPI
int QUEST_signed_distance_init_serial_bufferify(const char* file, int Lfile);
#endif

bool QUEST_signed_distance_initialized();

void QUEST_signed_distance_get_mesh_bounds(double* lo, double* hi);

void QUEST_signed_distance_set_dimension(int dim);

void QUEST_signed_distance_set_closed_surface(bool status);

void QUEST_signed_distance_set_max_levels(int maxLevels);

void QUEST_signed_distance_set_max_occupancy(int maxOccupancy);

void QUEST_signed_distance_set_verbose(bool status);

void QUEST_signed_distance_use_shared_memory(bool status);

double QUEST_signed_distance_evaluate(double x, double y, double z);

void QUEST_signed_distance_finalize();

#ifdef __cplusplus
}
#endif

#endif  // WRAPQUEST_H
