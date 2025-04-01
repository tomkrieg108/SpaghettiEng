#pragma once
#include "GeomBase.h"
#include "Line.h"
#include "Plane.h"
#include <vector> //todo compile errors without this - should be included in pch???

namespace Geom
{
  /*
    TESTED (Catch2)
    Returns signed area of triangle formed by points a,b,c. CCW otiented=> +ve, CW=> negative. Returns zero if colinear.  Check for equality with zero using Geom::Equal()
  */
  float ComputeSignedArea(const Point2d& a, const Point2d& b, const Point2d& c); 

  /*
    TESTED (Catch2)
    Orientation of point c relative to a->b.  Returns Origin when c==a, Destination when c==b, Between when c is colinear with a->b and strictly bwteen them, Beyond when c is colinear when a->b and strictly ahead, Behind when  c is colinear when a->b and strictly behind, Left with c is ledt of a->b, Right when c is right of a->b
  */
  RelativePos Orientation2d(const Point2d& a, const Point2d& b, const Point2d& c);

   /*
    TESTED (Catch2) - contains a single call:  Collinear(seg.start, seg.end, p)
    Orientation of point p relative to line_seg (a->b).  Returns Origin when c==a, Destination when c==b, Between when c is colinear with a->b and strictly bwteen them, Beyond when c is colinear when a->b and strictly ahead, Behind when  c is colinear when a->b and strictly behind, Left with c is ledt of a->b, Right when c is right of a->b
   */
  inline RelativePos Orientation2d(const LineSeg2D& line_seg, const Point2d& p)
  {
    return Orientation2d(line_seg.start, line_seg.end, p);
  }

  bool Collinear(const glm::vec3& a, const glm::vec3& b);

  bool Collinear(const Point3d& a, const Point3d& b, const Point3d& c);

  /*
    TESTED (Catch2)
    Returns true when a, b, c are collinear, or any points coincide
  */
  bool Collinear(const Point2d& a, const Point2d& b, const Point2d& c);

  /*
    TESTED (Catch2)
    Returns true if point p is collinear with start or end point of seg
  */
  bool Collinear(const LineSeg2D& seg, const Point2d& p);

  /*
    Return true if point is collinear with segs endpoints, lies between the endpoints, does not coincide with either endpoint.
  */
  bool SegContainsPoint(const LineSeg2D& seg, Point2d p);

  bool IsHorizontal(const LineSeg2D& seg);

  bool IsVertical(const LineSeg2D& seg);


  /*
    Return true if point is collinear with segs endpoints, lies between the endpoints, may coincide with either endpoint.
  */
  bool SegIncludesPoint(const LineSeg2D& seg, Point2d p);

  /*
    Return true if segs cross in their interior, or an endpoint of one seg is colinear with the other,
    or any endpoints coincident.
  */
  bool IntersectionExists(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2);

  /*
    Only true if segs cross.  False for coincidend endpoints or enpoints colinear with other seg
    Todo = better to return a std_pair containing intersection point and success flag?
  */
  bool StrictIntersectionExists(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2);

/*
  Todo = better to return a std_pair containing intersection point and success flag?
*/
  bool ComputeIntersection(const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d, Point2d& out_intersect_point);

  bool ComputeIntersection(const LineSeg2D& line1, const LineSeg2D& line2, Point2d& out_intersect_point);

  bool ComputeIntersection(const Line3d& line, const Plane& plane, Point3d& out_intersect_point);

  bool ComputeIntersection(const Plane& plane1, const Plane& plane2, Line3d& out_intersect_line);

 
  template <uint32_t Dim, typename T>
  float AngleLines(const glm::vec<Dim, T>& v1, const  glm::vec < Dim, T>& v2)
  { //Assumes v1, v2 are normalised
    const auto dot = glm::dot(v1, v2); 
    const auto theta = glm::acos(fabs(dot));
    return glm::degrees(theta);
  }

  /*
    TESTED (Catch2)
    Angle in degrees between seg1 and seg2. Output range [-180,180].  Oriented CCW => positive.  Otherwise negative
  */
  float ComputeAngleInDegrees(const LineSeg2D& seg1, const LineSeg2D& seg2);

  /*
    TESTED (Catch2)
    Angle in degrees between vectors a->b, b->c. Output range [-180,180].  Oriented CCW => positive.  Otherwise negative.
  */
  float ComputeAngleInDegrees(const Point2d& a, const Point2d& b, const Point2d& c);

  /*
    TESTED (Catch2)
    Angle subtended by sides b->a (initial side) and b->c (terminal side).  i.e. the central angle at point b subtended by side a->c. Output range [0,180].
  */
  float ComputeSubtendedAngleInDegrees(const Point2d& a, const Point2d& b, const Point2d& c);

  float AngleLinePlaneInDegrees(const Line3d& line, const Plane& plane);

  float AnglePlanes(const Plane& p1, const Plane& p2);

  //Returns angle in degrees - lines in 2D can either be parallel or intersect
  float AngleLines2D(const Line2d& l1, const Line2d& l2);

  //3D lines in 2D can either be parallel or intersect or skewed
  float AngleLines3D(const Line3d& l1, const Line3d& l2);

  Point2d ComputeCentroid(const std::vector<Point2d>& points);

  bool Coplanar(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

  bool Coplanar(const Point3d& a, const Point3d& b, const Point3d& c, const Point3d& d);

  float Distance(const Line3d& line, const Point3d& C);

  float Distance(const Plane& p, Point3d& Q);

  inline bool Left(const LineSeg2D& line_seg, const Point2d& p)
  {
    return Orientation2d(line_seg, p) == RelativePos::Left;
  }

  inline bool Right(const LineSeg2D& line_seg, const Point2d& p)
  {
    return Orientation2d(line_seg, p) == RelativePos::Right;
  }

  inline bool Between(const LineSeg2D& line_seg, const Point2d& p)
  {
    return Orientation2d(line_seg, p) == RelativePos::Between;
  }

  inline bool Behind(const LineSeg2D& line_seg, const Point2d& p)
  {
    return Orientation2d(line_seg, p) == RelativePos::Behind;
  }

  inline bool Beyond(const LineSeg2D& line_seg, const Point2d& p)
  {
    return Orientation2d(line_seg, p) == RelativePos::Beyond;
  }

  inline bool LeftOrBeyond(const LineSeg2D& line_seg, const Point2d& p)
  {
    return Left(line_seg, p) || Beyond(line_seg, p);
  }

  // Is the subtended angle a->b->c convex
  inline bool IsConvex(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    return Left({a,b},c);
  }

  inline float ScalarTripleProduct(glm::vec3 a, glm::vec3 b, glm::vec3 c)
  {
    //same as det[a b c] = volume of parallelapiped = 6 * vol of tetrahedron
    return glm::dot(a, glm::cross(b, c));
  }

}