/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright (c) 2017-2018, Lawrence Livermore National Security, LLC.
 *
 * Produced at the Lawrence Livermore National Laboratory
 *
 * LLNL-CODE-741217
 *
 * All rights reserved.
 *
 * This file is part of Axom.
 *
 * For details about use and distribution, please read axom/LICENSE.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include "slic/slic.hpp"
#include "slic/UnitTestLogger.hpp"
using axom::slic::UnitTestLogger;

#include "mint/CellTypes.hpp"
#include "mint/config.hpp"
#include "mint/MeshTypes.hpp"
#include "mint/UniformMesh.hpp"
#include "mint/UnstructuredMesh.hpp"
#include "mint/vtk_utils.hpp"

#include "primal/BoundingBox.hpp"
#include "primal/Sphere.hpp"
#include "primal/Point.hpp"

#include "quest/SignedDistance.hpp"  // quest::SignedDistance
#include "quest_test_utilities.hpp"  // for test-utility functions

// Google Test includes
#include "gtest/gtest.h"

// C/C++ includes
#include <cmath>

// Aliases
namespace mint   = axom::mint;
namespace quest  = axom::quest;
namespace primal = axom::primal;
using UMesh      = axom::mint::UnstructuredMesh< mint::SINGLE_SHAPE >;

// Note: the following macro is intended for debugging purposes. Uncomment the
// following line to enable VTK dumps from this test.
// #define QUEST_SIGNED_DISTANCE_TEST_DUMP_VTK

//------------------------------------------------------------------------------
//  HELPER METHODS
//------------------------------------------------------------------------------
namespace detail
{

/*!
 * \brief Returns the bounding box of the mesh.
 * \param [in] mesh pointer to the mesh instance.
 * \return bb bounding box of the mesh
 */
primal::BoundingBox< double,3 > getBounds( const axom::mint::Mesh* mesh )
{
  SLIC_ASSERT( mesh != AXOM_NULLPTR );

  primal::BoundingBox< double,3 > bb;
  primal::Point< double,3 > pt;

  const int nnodes = mesh->getNumberOfNodes();
  for ( int inode=0 ; inode < nnodes ; ++inode )
  {
    mesh->getNode( inode, pt.data() );
    bb.addPoint( pt );
  }

  return( bb );
}

/*!
 * \brief Generates a uniform mesh surrounding the given triangle mesh.
 * \param [in] mesh pointer to the input mesh.
 * \param [in] umesh pointer to the uniform mesh;
 */
void getUniformMesh( const UMesh* mesh, mint::UniformMesh*& umesh )
{
  SLIC_ASSERT( mesh != AXOM_NULLPTR );
  SLIC_ASSERT( umesh == AXOM_NULLPTR );

  constexpr int N = 16; // number of points along each dimension

  primal::BoundingBox< double,3 > bb = getBounds( mesh );
  bb.expand( 2.0 );

  const double* lo = bb.getMin().data();
  const double* hi = bb.getMax().data();

  // construct an N x N x N grid
  umesh = new mint::UniformMesh( lo, hi, N, N, N );
}


} /* end detail namespace */

//------------------------------------------------------------------------------
TEST( quest_signed_distance, sphere_test )
{
  constexpr double l1norm_expected = 6.7051997372579715;
  constexpr double l2norm_expected = 2.5894400431865519;
  constexpr double linf_expected   = 0.00532092;
  constexpr double TOL             = 1.e-3;

  constexpr double SPHERE_RADIUS  = 0.5;
  constexpr int SPHERE_THETA_RES  = 25;
  constexpr int SPHERE_PHI_RES    = 25;
  const double SPHERE_CENTER[ 3 ] = { 0.0, 0.0, 0.0 };

  primal::Sphere< double,3 > analytic_sphere( SPHERE_RADIUS );

  SLIC_INFO( "Constructing sphere mesh..." );
  UMesh* surface_mesh = new UMesh( 3, mint::TRIANGLE );
  quest::utilities::getSphereSurfaceMesh( surface_mesh,
      SPHERE_CENTER, SPHERE_RADIUS, SPHERE_THETA_RES, SPHERE_PHI_RES );

  SLIC_INFO( "Generating uniform mesh..." );
  mint::UniformMesh* umesh = AXOM_NULLPTR;
  detail::getUniformMesh( surface_mesh, umesh );

  double* phi_computed =
      umesh->createField< double >( "phi_computed", mint::NODE_CENTERED );
  double* phi_expected =
      umesh->createField< double >( "phi_expected", mint::NODE_CENTERED );
  double* phi_diff =
      umesh->createField< double >( "phi_diff", mint::NODE_CENTERED );
  double* phi_err =
      umesh->createField< double >( "phi_err", mint::NODE_CENTERED );

  const int nnodes = umesh->getNumberOfNodes();

  SLIC_INFO( "Generate BVHTree..." );
  axom::quest::SignedDistance< 3 > signed_distance( surface_mesh, 25, 10 );

  SLIC_INFO( "Compute signed distance..." );

  double l1norm = 0.0;
  double l2norm = 0.0;
  double linf   = std::numeric_limits< double >::min( );

  for ( int inode=0 ; inode < nnodes ; ++inode )
  {

    primal::Point< double,3 > pt;
    umesh->getNode( inode, pt.data() );

    phi_computed[ inode ] = signed_distance.computeDistance( pt );
    phi_expected[ inode ] = analytic_sphere.computeSignedDistance( pt.data() );
    EXPECT_NEAR( phi_computed[ inode ], phi_expected[ inode ], 1.e-2 );

    // compute error
    phi_diff[ inode ] = phi_computed[ inode ] - phi_expected[ inode ];
    phi_err[ inode ] = std::fabs( phi_diff[ inode ] );

    // update norms
    l1norm += phi_err[ inode ];
    l2norm += phi_diff[ inode ];
    linf = ( phi_err[ inode ] > linf ) ? phi_err[ inode ] : linf;

  } // END for all nodes

  l2norm = std::sqrt( l2norm );

  SLIC_INFO( "l1 =" << l1norm );
  SLIC_INFO( "l2 =" << l2norm );
  SLIC_INFO( "linf = " << linf );

#ifdef QUEST_SIGNED_DISTANCE_TEST_DUMP_VTK
  mint::write_vtk( umesh, "uniform_mesh.vtk" );
  mint::write_vtk( surface_mesh, "sphere_mesh.vtk" );
#endif

  EXPECT_NEAR( l1norm_expected, l1norm, TOL );
  EXPECT_NEAR( l2norm_expected, l2norm, TOL );
  EXPECT_NEAR( linf_expected, linf, TOL );

  delete surface_mesh;
  delete umesh;

  SLIC_INFO( "Done." );
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  int result = 0;

  ::testing::InitGoogleTest(&argc, argv);

  UnitTestLogger logger;  // create & initialize test logger,

  // finalized when exiting main scope

  result = RUN_ALL_TESTS();

  return result;
}
