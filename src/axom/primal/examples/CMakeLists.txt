# Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
# other Axom Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
#------------------------------------------------------------------------------
# Primal examples
#------------------------------------------------------------------------------

set(primal_example_depends
        core
        primal
        fmt
        )

blt_add_executable(
    NAME        primal_introduction_ex
    SOURCES     primal_introduction.cpp
    OUTPUT_DIR  ${EXAMPLE_OUTPUT_DIRECTORY}
    DEPENDS_ON  ${primal_example_depends}
    FOLDER      axom/primal/examples
    )

