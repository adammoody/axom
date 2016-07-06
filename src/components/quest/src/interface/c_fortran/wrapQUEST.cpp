// wrapQUEST.cpp
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
// wrapQUEST.cpp
#include "wrapQUEST.h"
#include <string>
#include "quest/quest.hpp"

extern "C" {
namespace quest {

void QUEST_initialize(MPI_Fint comm, const char * fileName, bool requiresDistance, int ndims, int maxElements, int maxLevels)
{
// splicer begin function.initialize
const std::string SH_fileName(fileName);
initialize(MPI_Comm_f2c(comm), SH_fileName, requiresDistance, ndims, maxElements, maxLevels);
return;
// splicer end function.initialize
}

void QUEST_initialize_bufferify(MPI_Fint comm, const char * fileName, int LfileName, bool requiresDistance, int ndims, int maxElements, int maxLevels)
{
// splicer begin function.initialize_bufferify
const std::string SH_fileName(fileName, LfileName);
initialize(MPI_Comm_f2c(comm), SH_fileName, requiresDistance, ndims, maxElements, maxLevels);
return;
// splicer end function.initialize_bufferify
}

void QUEST_finalize()
{
// splicer begin function.finalize
finalize();
return;
// splicer end function.finalize
}

double QUEST_distance(double x, double y, double z)
{
// splicer begin function.distance
double rv = distance(x, y, z);
return rv;
// splicer end function.distance
}

int QUEST_inside(double x, double y, double z)
{
// splicer begin function.inside
int rv = inside(x, y, z);
return rv;
// splicer end function.inside
}

// splicer begin additional_functions
// splicer end additional_functions

}  // namespace quest
}  // extern "C"