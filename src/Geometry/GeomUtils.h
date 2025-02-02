#pragma once
#include "GeomBase.h"
#include "Line.h"

namespace Geom
{
  float SignedAreaTriangle2d(const Point2d& a, const Point2d& b, const Point2d& c); 

  // Orientation of point c relative to a->b
  RelativePos Orientation2d(const Point2d& a, const Point2d& b, const Point2d& c);

  bool LineSegs2dIntersect(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2);

  bool GetLines2dIntersectPoint(const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d, Point2d& out_intersect_point);

  bool GetLines2dIntersectPoint(const LineSeg2D& line1, const LineSeg2D& line2, Point2d& out_intersect_point);

  float Angle_LineSeg2D_LineSeg2d(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2);


  // Orientation of point p relative to line_seg
  inline RelativePos Orientation2d(const LineSeg2D& line_seg, const Point2d& p)
  {
    return Orientation2d(line_seg.start, line_seg.end, p);
  }

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

  Point2d ComputeCentroid2d(const std::vector<Point2d>& points);


}