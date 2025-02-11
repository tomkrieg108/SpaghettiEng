#pragma once
#include "GeomBase.h"
#include "Line.h"

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

  bool IntersectionExists(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2);

  bool ComputeIntersection(const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d, Point2d& out_intersect_point);

  bool ComputeIntersection(const LineSeg2D& line1, const LineSeg2D& line2, Point2d& out_intersect_point);

  /*
    Note:
    These angle functions give the angles between the vectors defined by the LineSegs, or defined by vectors AB, BC
    This is different from the angle subtended by points A,B,C.  This subtended angle is: AngleSubtend = (180 - abs(Angle)) for the interior (accute) angle, or 360 - (AngleSubtend) = 180 + abs(Angle)
  */

  //Angle between seg1 -> seg2 always [0,180) 
  float ComputeAngleInDegrees(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2);

  //Angle between a->b->c - same result as above - always [0,180)
  float ComputeAngleInDegrees(const Point2d& a, const Point2d& b, const Point2d& c);

  //Angle between a->b->c, same as above except signed. i.e.  CCW=> [0,180), CW =>(-180,0]
  float ComputeAngleInDegreesChatGPT(const Point2d& a, const Point2d& b, const Point2d& c);

  

  Point2d ComputeCentroid(const std::vector<Point2d>& points);

 
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


}