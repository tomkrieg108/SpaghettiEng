#pragma once
#include "GeomBase.h"
#include "Line.h"
#include "Plane.h"

namespace Geom
{
  //Signed area of triangled formed by points a,b,a.  CCW => +ve, CW=> -ve
  float ComputeSignedArea(const Point2d& a, const Point2d& b, const Point2d& c); 

  // oa x ob
  float ComputeCrossProduct(const Point2d& o, const Point2d& a, const Point2d& b);

  // Orientation of point c relative to a->b
  RelativePos Orientation2d(const Point2d& a, const Point2d& b, const Point2d& c);

   // Orientation of point p relative to line_seg
  inline RelativePos Orientation2d(const LineSeg2D& line_seg, const Point2d& p)
  {
    return Orientation2d(line_seg.start, line_seg.end, p);
  }

  /*
    Return true if point is strictly colinear with segs endpoints (does not coincide with either endpoint)
    Same as calling Orientation2d(seg,p) == RelativePos::Between, but faster
  */
  bool SegContainsPoint(const LineSeg2D& seg, Point2d p);

  /*
    Return true if segs cross in their interior, or an endpoint of one seg is colinear with the other
    Returns false if any endpoints coincident.
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

  /*
    Note:
    These angle functions give the angles between the vectors defined by the LineSegs, or defined by vectors AB, BC
    This is different from the angle subtended by points A,B,C.  This subtended angle is: AngleSubtend = (180 - abs(Angle)) for the interior (accute) angle, or 360 - (AngleSubtend) = 180 + abs(Angle)
  */

  template <uint32_t Dim, typename T>
  float AngleLines(const glm::vec<Dim, T>& v1, const  glm::vec < Dim, T>& v2)
  { //Assumes v1, v2 are normalised
    const auto dot = glm::dot(v1, v2); 
    const auto theta = glm::acos(fabs(dot));
    return glm::degrees(theta);
  }

  //Angle between seg1 -> seg2 always [0,180) 
  float ComputeAngleInDegrees(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2);

  //Angle between a->b->c - same result as above - always [0,180)
  float ComputeAngleInDegrees(const Point2d& a, const Point2d& b, const Point2d& c);

  //Angle between a->b->c, same as above except signed. i.e.  CCW=> [0,180), CW =>(-180,0]
  float ComputeAngleInDegreesChatGPT(const Point2d& a, const Point2d& b, const Point2d& c);

  float AngleLinePlaneInDegrees(const Line3d& line, const Plane& plane);

  float AnglePlanes(const Plane& p1, const Plane& p2);

  //Returns angle in degrees - lines in 2D can either be parallel or intersect
  float AngleLines2D(const Line2d& l1, const Line2d& l2);

  //3D lines in 2D can either be parallel or intersect or skewed
  float AngleLines3D(const Line3d& l1, const Line3d& l2);

  Point2d ComputeCentroid(const std::vector<Point2d>& points);

  bool Collinear(const glm::vec3& a, const glm::vec3& b);

  bool Collinear(const Point3d& a, const Point3d& b, const Point3d& c);

  bool Collinear(const glm::vec2& a, const glm::vec2& b);

  bool Collinear(const Point2d& a, const Point2d& b, const Point2d& c);

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

  inline float CrossProduct2D(glm::vec2 v1, glm::vec3 v2)
  {
    return v1.x * v2.y - v1.y * v2.x;
  }

  inline float ScalarTripleProduct(glm::vec3 a, glm::vec3 b, glm::vec3 c)
  {
    //same as det[a b c] = volume of parallelapiped = 6 * vol of tetrahedron
    return glm::dot(a, glm::cross(b, c));
  }

}