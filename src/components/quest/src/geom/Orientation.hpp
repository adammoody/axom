/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and further
 * review from Lawrence Livermore National Laboratory.
 */


/*
 * $Id$
 */

/*!
 *******************************************************************************
 * \file
 *
 * \brief Consists of a set of templated (overloaded) routines used to calculate
 *  the orientation of a given point to another geometric entity.
 *
 * \date Dec 17, 2015
 * \author George Zagaris (zagaris2@llnl.gov)
 *
 *******************************************************************************
 */

#ifndef ORIENTATION_HPP_
#define ORIENTATION_HPP_


#include "quest/Determinants.hpp"
#include "quest/Point.hpp"
#include "quest/Segment.hpp"
#include "quest/Triangle.hpp"

#include "slic/slic.hpp"

#include "common/Utilities.hpp"

namespace quest
{


/*!
 *******************************************************************************
 * \brief Enumerates possible return values for orientation methods
 *******************************************************************************
 */
enum OrientedSide {
    ON_BOUNDARY,       /*!< point is on boundary of the given primitive      */
    ON_POSITIVE_SIDE,  /*!< point is on positive side of the given primitive */
    ON_NEGATIVE_SIDE   /*!< point is on negative side of the given primitive */
};

/*!
 *******************************************************************************
 * \brief Computes the orientation of the given point, p, with respect to a
 *  supplied oriented triangle.
 * \param [in] p the query point.
 * \param [in] tri oriented triangle.
 * \return the
 * \note The return value can be one of the following:
 * <ul>
 *  <li> ON_BOUNDARY      </li>
 *  <li> ON_POSITIVE_SIDE </li>
 *  <li> ON_NEGATIVE_SIDE </li>
 * </ul>
 *******************************************************************************
 */
template < typename T >
inline
int orientation( const Point< T,3 >& p, const Triangle< T,3 >& tri )
{

   double det = math::determinant( tri.A()[0], tri.A()[1], tri.A()[2], 1.0,
                                   tri.B()[0], tri.B()[1], tri.B()[2], 1.0,
                                   tri.C()[0], tri.C()[1], tri.C()[2], 1.0,
                                         p[0],       p[1],       p[2], 1.0  );

   int orient = -1;

   if ( asctoolkit::utilities::isNearlyEqual( det, 0.0, 1.0e-9 ) ) {

       orient = ON_BOUNDARY;

   } else if ( det < 0.0f ) {

       // outside
       orient = ON_POSITIVE_SIDE;

   } else {

       // inside
       orient = ON_NEGATIVE_SIDE;

   }

   return orient;
}

/*!
 *******************************************************************************
 * \brief Computes the orientation of the given point, p, with respect to a
 *  supplied oriented segment.
 * \param [in] p the query point.
 * \param [in] seg the user-supplied segment.
 * \return the orientation of the point with respect to the given segment.
 * \note The return value can be one of the following:
 * <ul>
 *  <li> ON_BOUNDARY, when the point is collinear with the points that define
 *       the segment.
 *  </li>
 *  <li> ON_POSITIVE_SIDE, when the point is clockwise, i.e., to the right of
 *       the directed segment.
 *  </li>
 *  <li> ON_NEGATIVE_SIDE, when the point is counter-clockwise, i.e., to the
 *       left of the directed segment.
 *  </li>
 * </ul>
 *******************************************************************************
 */
template < typename T >
inline
int orientation( const Point< T,2 >& p, const Segment< T,2 >& seg )
{
   double det = math::determinant( seg.source()[0], seg.source()[1], 1.0,
                                   seg.target()[0], seg.target()[1], 1.0,
                                   p[0],            p[1],            1.0  );

   int orient = -1;

   if ( asctoolkit::utilities::isNearlyEqual( det, 0.0 ) ) {

      // collinear
      orient = ON_BOUNDARY;

   } else if ( det < 0.0f ) {

      // outside, clockwise, to the right
      orient = ON_POSITIVE_SIDE;

   } else {

      // inside, counter-clockwise, to the left
      orient = ON_NEGATIVE_SIDE;

   }

   return orient;
}

}
#endif /* ORIENTATION_HPP_ */