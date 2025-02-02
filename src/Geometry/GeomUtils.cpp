#include "GeomUtils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp> //for length2() (length squared)

namespace Geom
{
  float SignedAreaTriangle2d(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    //Det()*0.5.  CCW => +ve, CW => -ve
    return 0.5f * ((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));
  }

  RelativePos Orientation2d(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    float area = SignedAreaTriangle2d(a, b, c);
    if(Geom::Equal(area,0.0f))  
      area = 0.0f;

    glm::vec2 ab = b - a;
    glm::vec2 ac = c - a;

    if (area > 0.0) //CCW orientationab
		  return RelativePos::Left;
    if (area < 0.0)
        return RelativePos::Right; //CW orientation
    if ((ab.x * ac.x < 0.0f) || (ab.y * ac.y < 0.0f))
        return RelativePos::Behind;
    if ( glm::length2(ab) < glm::length2(ac)) //length squared to avoid sq root
        return RelativePos::Beyond;
    if (Geom::Equal(a,c))
        return RelativePos::Origin;
    if (Geom::Equal(b,c))
        return RelativePos::Destination;

    return RelativePos::Between;
  }

  bool LineSegs2dIntersect(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2)
  {
    // if one of the end points of a segment is in between other segment endpoints we consider it as intersection.
    Point2d a = line_seg1.start;
    Point2d b = line_seg1.end;
    Point2d c = line_seg2.start;
    Point2d d = line_seg2.end;

    if (Orientation2d(line_seg1, c) == RelativePos::Between
        || Orientation2d(line_seg1, d) == RelativePos::Between
        || Orientation2d(line_seg2, a) == RelativePos::Between
        || Orientation2d(line_seg2, b) == RelativePos::Between)
    {
        return true;
    }

    return Xor (Left(line_seg1, c), Left(line_seg1, d)) &&
          Xor(Left(line_seg2, a), Left(line_seg2, b));
  }

  bool GetLines2dIntersectPoint(const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d, Point2d& out_intersect_point)
  {
    glm::vec2 AB = b - a;
    glm::vec2 CD = d - c;

    //Normal vector to CD
    glm::vec2 normal{ CD.y, -CD.x };

    //Denominator = n.(b-a)
    auto denominator = glm::dot(normal, AB);

    if (!Equal(denominator, 0))
    {
        auto AC = c - a;
        auto numerator = glm::dot(normal, AC);
        auto t = numerator / denominator;
        out_intersect_point.x = a.x + t * AB.x;
        out_intersect_point.y = a.y + t * AB.y;
        return true;
    }
    else
    {
        // Lines are parallel or colinear
        return false;
    }

    return false;
  }

  bool GetLines2dIntersectPoint(const LineSeg2D& line1, const LineSeg2D& line2, Point2d& out_intersect_point)
  {
    return GetLines2dIntersectPoint(line1.start, line1.end, line2.start, line2.end, out_intersect_point);
  }

  float Angle_LineSeg2D_LineSeg2d(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2)
  {
    const float dot =  glm::dot(line_seg1.dir_vec, line_seg2.dir_vec);
    //const auto theta = glm::acos(std::fabs(dot));
    const auto theta = glm::acos(dot);
    return glm::degrees(theta);
  }

  Point2d ComputeCentroid2d(const std::vector<Point2d>& points)
  {
    Point2d centroid = {0, 0};
    for (const auto& p : points) {
        centroid.x += p.x;
        centroid.y += p.y;
    }
    centroid.x /= points.size();
    centroid.y /= points.size();
    return centroid;
  }

}