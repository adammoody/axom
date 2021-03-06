# Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
# other Axom Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
#------------------------------------------------------------------------------
# Slic component
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Specify all headers/sources
#------------------------------------------------------------------------------
set(slic_headers
    core/Logger.hpp
    core/LogStream.hpp
    core/MessageLevel.hpp
    core/UnitTestLogger.hpp

    interface/slic.hpp
    interface/slic_macros.hpp

    internal/stacktrace.hpp

    streams/GenericOutputStream.hpp
    )

set(slic_sources
    core/Logger.cpp
    core/LogStream.cpp

    interface/slic.cpp

    internal/stacktrace.cpp

    streams/GenericOutputStream.cpp
    )

if (ENABLE_MPI)
    list(APPEND slic_headers streams/SynchronizedStream.hpp)
    list(APPEND slic_sources streams/SynchronizedStream.cpp)
    if (AXOM_ENABLE_LUMBERJACK)
        list(APPEND slic_headers streams/LumberjackStream.hpp)
        list(APPEND slic_sources streams/LumberjackStream.cpp)
    endif()
endif()

if(SHROUD_FOUND)
    add_subdirectory(interface)
endif()

if (ENABLE_FORTRAN)
    list(APPEND slic_headers
        interface/c_fortran/wrapSLIC.h
        interface/c_fortran/typesSLIC.h
    )
    list(APPEND slic_sources
        interface/c_fortran/wrapSLIC.cpp
        interface/c_fortran/wrapfslic.f
    )
endif()

#------------------------------------------------------------------------------
# Make/Install the library
#------------------------------------------------------------------------------
set(slic_depends core)

blt_list_append( TO slic_depends ELEMENTS lumberjack IF ${AXOM_ENABLE_LUMBERJACK} )
blt_list_append( TO slic_depends ELEMENTS mpi IF ${ENABLE_MPI} )
blt_list_append( TO slic_depends ELEMENTS dbghelp IF ${WIN32} )

blt_add_library(NAME        slic
                SOURCES     ${slic_sources}
                HEADERS     ${slic_headers}
                DEPENDS_ON  ${slic_depends}
                FOLDER      axom/slic
                OBJECT      TRUE
                )

axom_write_unified_header( NAME slic
                           HEADERS ${slic_headers}
                           )

axom_install_component( NAME    slic
                        HEADERS ${slic_headers}
                        )


#------------------------------------------------------------------------------
# Add examples
#------------------------------------------------------------------------------
if (AXOM_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

#------------------------------------------------------------------------------
# Add tests
#------------------------------------------------------------------------------
if (AXOM_ENABLE_TESTS)
  add_subdirectory(tests)
endif()

#------------------------------------------------------------------------------
# Add code checks
#------------------------------------------------------------------------------
axom_add_code_checks(PREFIX slic)
