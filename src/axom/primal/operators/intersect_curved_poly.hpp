// Copyright (c) 2017-2019, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

/*!
 * \file intersect.hpp
 *
 * \brief Consists of functions to test intersection among geometric primitives.
 */

#ifndef INTERSECTION_CURVED_POLYGON_HPP_
#define INTERSECTION_CURVED_POLYGON_HPP_

#include "axom/primal/geometry/BoundingBox.hpp"
#include "axom/primal/geometry/OrientedBoundingBox.hpp"
#include "axom/primal/geometry/Point.hpp"
#include "axom/primal/geometry/Ray.hpp"
#include "axom/primal/geometry/Segment.hpp"
#include "axom/primal/geometry/Sphere.hpp"
#include "axom/primal/geometry/Triangle.hpp"
#include "axom/primal/geometry/BezierCurve.hpp"
#include "axom/primal/geometry/CurvedPolygon.hpp"

#include "axom/core/utilities/Utilities.hpp"

#include "axom/primal/operators/squared_distance.hpp"
#include "axom/primal/operators/intersect.hpp"

namespace axom
{
namespace primal
{

template < typename T >
class IntersectionInfo;
/*
template <typename T, int NDIMS>
bool orient(BezierCurve<T,NDIMS> c1, BezierCurve<T,NDIMS> c2, T s, T t);
*/
/*!
 * \brief Test whether CurvedPolygons p1 and p2 intersect.
 * \return status true iff p1 intersects with p2, otherwise false.
 *
 * \param p1, p2 CurvedPolygon objects to intersect
 * \param pnew vector of type CurvedPolygon holding intersection regions oriented as the original curves were. 
 */
template < typename T, int NDIMS>
bool intersect_polygon(CurvedPolygon< T, NDIMS>& p1,
                       CurvedPolygon< T, NDIMS>& p2,
                       std::vector<CurvedPolygon< T, NDIMS>>& pnew
                       )
{
  // Object to store intersections
  std::vector<std::vector<IntersectionInfo<T>>> E1IntData(p1.numEdges());
  std::vector<std::vector<IntersectionInfo<T>>> E2IntData(p2.numEdges());
  IntersectionInfo<T> firstinter; // Need to do orientation test on first intersection
  
  // Find all intersections and store
  int numinters=0;
  for (int i = 0; i < p1.numEdges(); ++i)
  {
    for (int j = 0; j < p2.numEdges(); ++j)
    {
      std::vector<T> p1times;
      std::vector<T> p2times;
      intersect(p1[i],p2[j],p1times,p2times,1e-15);
      for (int k =0; k< static_cast<int>(p1times.size()); ++k)
      {
        E1IntData[i].push_back({p1times[k],i,p2times[k],j,numinters+k+1});
        E2IntData[j].push_back({p2times[k],j,p1times[k],i,numinters+k+1});
        if (numinters==0)
        {
          firstinter={p1times[0],i,p2times[0],j,1};
        }
      }
      numinters+=p1times.size();
    }
  }
  for (int i = 0; i < p1.numEdges(); ++i)
  {
    std::sort( E1IntData[i].begin(), E1IntData[i].end() );
    std::sort( E2IntData[i].begin(), E2IntData[i].end() );
  }

  // Orient the first intersection point to be sure we get the intersection
  bool orientation = !orient(p1[firstinter.myEdge],p2[firstinter.otherEdge],firstinter.myTime,firstinter.otherTime);
  
  // Objects to store completely split polygons (split at every intersection point) and vector with unique id for each
  // intersection and zeros for corners of original polygons.
  std::vector<int> edgelabels[2];
  CurvedPolygon<T,NDIMS> psplit[2];
  psplit[0]=p1;
  psplit[1]=p2;
  int addedints=0;
  for (int i = 0; i < p1.numEdges(); ++i)
  {
    edgelabels[0].push_back(0);
    for (int j = 0; j < static_cast<int>(E1IntData[i].size()); ++j)
    {
      psplit[0].splitEdge(i+addedints,E1IntData[i][j].myTime);
      edgelabels[0].insert(edgelabels[0].begin()+i+addedints,E1IntData[i][j].numinter);
      addedints+=1;
      for (int k = j+1; k < static_cast<int>(E1IntData[i].size()); ++k)
      {
        E1IntData[i][k].myTime=(E1IntData[i][k].myTime-E1IntData[i][j].myTime)/(1-E1IntData[i][j].myTime);
      }
    }
  }

  addedints=0;
  for (int i = 0; i < p2.numEdges(); ++i)
  {
    edgelabels[1].push_back(0);
    for (int j = 0; j < static_cast<int>(E2IntData[i].size()); ++j)
    {
      psplit[1].splitEdge(i+addedints,E2IntData[i][j].myTime);
      edgelabels[1].insert(edgelabels[1].begin()+i+addedints,E2IntData[i][j].numinter);
      addedints+=1;
      for (int k = j+1; k < static_cast<int>(E2IntData[i].size()); ++k)
      {
        E2IntData[i][k].myTime=(E2IntData[i][k].myTime-E2IntData[i][j].myTime)/(1-E2IntData[i][j].myTime);
      }
    }
  }

  // This performs the directional walking method using the completely split polygon
  std::vector<std::vector<int>::iterator> usedlabels;
  if (numinters==0) {return false;}  // If there are no intersections, return false
  else
  {
    bool addingcurves=true;
    int startinter=1; // Start at the first intersection
    int nextinter;
    bool currentelement=orientation;
    int currentit= std::find(edgelabels[currentelement].begin(),edgelabels[currentelement].end(),startinter)-edgelabels[currentelement].begin();
    int startit = currentit;
    int nextit = (currentit+1)%edgelabels[0].size();
    nextinter=edgelabels[currentelement][nextit];
    while (numinters>0)
    { 
      CurvedPolygon<T,NDIMS> aPart; // To store the current intersection polygon (could be multiple)
      while (!(nextit==startit && currentelement==orientation))
      {
        if (nextit==currentit )
          {
            nextit=(currentit+1)%edgelabels[0].size();
          }
        nextinter=edgelabels[currentelement][nextit];
        while (nextinter==0)
        {
          currentit=nextit;
          if (addingcurves)
          {
            aPart.addEdge(psplit[currentelement][nextit]);
          }
          nextit=(currentit+1)%edgelabels[0].size();
          nextinter=edgelabels[currentelement][nextit];
        }
        if (addingcurves)
        {
          aPart.addEdge(psplit[currentelement][nextit]);
          currentelement=!currentelement;
          nextit = std::find(edgelabels[currentelement].begin(),edgelabels[currentelement].end(),nextinter)-edgelabels[currentelement].begin();
          currentit=nextit;
          numinters-=1;
        }
        else 
        {
          addingcurves=true;
          currentit=nextit;
          nextit = std::find(edgelabels[currentelement].begin(),edgelabels[currentelement].end(),nextinter)-edgelabels[currentelement].begin();
        }
      }
      pnew.push_back(aPart);
    }
    addingcurves=false;
  }
  return true;
}

// This determines with curve is "more" counterclockwise using the cross product of tangents
template <typename T, int NDIMS>
bool orient(const BezierCurve<T,NDIMS> c1, const BezierCurve<T,NDIMS> c2, T s, T t)
{
  Point<T,NDIMS> dc1s = c1.dt(s);
  Point<T,NDIMS> dc2t = c2.dt(t);
  Point<T,NDIMS> origin = primal::Point< T, NDIMS >::make_point(0.0, 0.0);
  auto orientation = detail::twoDcross(dc1s,dc2t, origin);
  return (orientation<0);
}

// A class for storing intersection points so they can be easily sorted by parameter value
template <typename T>
class IntersectionInfo
{
  public:
    T myTime;
    int myEdge;
    T otherTime;
    int otherEdge;
    int numinter;
  bool operator<(IntersectionInfo other)
  {
    return myTime<other.myTime;
  }
};

} // namespace primal
} // namespace axom

#endif // PRIMAL_INTERSECTION_CURVED_POLYGON_HPP_