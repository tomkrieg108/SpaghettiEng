#include "GeomUtils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp> //for length2() (length squared)
#include <numbers> 

namespace Geom
{
  float ComputeSignedArea(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    //Det()*0.5
    return 0.5f * ((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));
  }

  float ComputeCrossProduct(const Point2d& o, const Point2d& a, const Point2d& b)
  {
    //Det() Samas above withut the 0.5 scaling1!!!
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
  }

  RelativePos Orientation2d(const Point2d& a, const Point2d& b, const Point2d& c)
  { //Vid 10
    float area = ComputeSignedArea(a, b, c);
    if(Geom::Equal(area,0.0f))  
      area = 0.0f;

    if (area > 0.0f) //CCW orientation
		  return RelativePos::Left;
    if (area < 0.0f)
      return RelativePos::Right; //CW orientation
    if (Geom::Equal(a,c))
      return RelativePos::Origin;
    if (Geom::Equal(b,c))
      return RelativePos::Destination;

    glm::vec2 ab = b - a;
    glm::vec2 ac = c - a;
    if ((ab.x * ac.x < 0.0f) || (ab.y * ac.y < 0.0f))
      return RelativePos::Behind;
    if ( glm::length2(ab) < glm::length2(ac)) //length squared to avoid sq root
      return RelativePos::Beyond;
    
    return RelativePos::Between;
  }

  bool SegContainsPoint(const LineSeg2D& seg, Point2d p)
  {
    if(Geom::Equal(seg.start.x, seg.end.x)) //Vertical seg
      return ((p.y > std::min(seg.start.y, seg.end.y)) && (p.y < std::max(seg.start.y, seg.end.y)));
    
    if(Geom::Equal(seg.start.y, seg.end.y))  //Horizontal seg
      return ( (p.x > std::min(seg.start.x, seg.end.x)) && (p.x < std::max(seg.start.x, seg.end.x)));
    
    if( (p.x > std::min(seg.start.x, seg.end.x)) && (p.x < std::max(seg.start.x, seg.end.x)) &&
        (p.y > std::min(seg.start.y, seg.end.y)) && (p.y < std::max(seg.start.y, seg.end.y)) )
    { //Diagonal seg
      return Geom::Collinear(seg.start, seg.end, p);
    }
    return false;  
  }

  bool IntersectionExists(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2)
  { //Vid 14
    Point2d a = line_seg1.start;
    Point2d b = line_seg1.end;
    Point2d c = line_seg2.start;
    Point2d d = line_seg2.end;

    if (   Orientation2d(line_seg1, c) == RelativePos::Between
        || Orientation2d(line_seg1, d) == RelativePos::Between
        || Orientation2d(line_seg2, a) == RelativePos::Between
        || Orientation2d(line_seg2, b) == RelativePos::Between)
    {
      return true;
    }
    return  Xor(Left(line_seg1, c), Left(line_seg1, d)) &&
            Xor(Left(line_seg2, a), Left(line_seg2, b));
  }

  bool StrictIntersectionExists(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2)
  {
    Point2d a = line_seg1.start;
    Point2d b = line_seg1.end;
    Point2d c = line_seg2.start;
    Point2d d = line_seg2.end;

    return  Xor(Left(line_seg1, c), Left(line_seg1, d)) &&
            Xor(Left(line_seg2, a), Left(line_seg2, b));
  }

  bool ComputeIntersection(const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d, Point2d& out_intersect_point)
  { //Vid 13
    glm::vec2 AB = b - a;
    glm::vec2 CD = d - c;

    //Normal vector to CD
    glm::vec2 normal{ CD.y, -CD.x };

    //Denominator = n.(b-a)
    auto denominator = glm::dot(normal, AB);

    if (Equal(denominator, 0))
      return false;  // Lines are parallel or colinear
    
    auto AC = c - a;
    auto numerator = glm::dot(normal, AC);
    auto t = numerator / denominator;
    out_intersect_point.x = a.x + t * AB.x;
    out_intersect_point.y = a.y + t * AB.y;
    return true;
  }

  bool ComputeIntersection(const LineSeg2D& line1, const LineSeg2D& line2, Point2d& out_intersect_point)
  { //Vid 13
    return ComputeIntersection(line1.start, line1.end, line2.start, line2.end, out_intersect_point);
  }

  bool ComputeIntersection(const Line3d& line, const Plane& plane, Point3d& out_intersect_point)
  { //Ref vid 20
    auto n = plane.normal;
    auto D = plane.d;
    auto d = line.direction;
    auto p = line.start_point;

    auto nd = glm::dot(n, d);
    if (Equal(nd, 0))
        return false; //parallel or colliner

    auto t = (-1*glm::dot(n,p) + D) / nd;
    out_intersect_point.x = p.x + t * d.x;
    out_intersect_point.y = p.y + t * d.y;
    out_intersect_point.z = p.z + t * d.z;
    return true;
  }

  bool ComputeIntersection(const Plane& plane1, const Plane& plane2, Line3d& out_intersect_line)
  { //Ref vid 21
    auto n1 = plane1.normal;
    auto n2 = plane2.normal;
    auto d1 = plane1.d;
    auto d2 = plane2.d;

    auto direction = glm::cross(n1, n2);

    if (Equal(glm::length(direction), 0))
        return false;

    auto dot_n1_n2 = glm::dot(n1, n2);
    auto denom = dot_n1_n2 - 1;
    auto a = (d2 - dot_n1_n2 - d1) / denom;
    auto b = (d1 - dot_n1_n2 - d2) / denom;
    auto point = n1 * a + n2 * b;	//point on the intersecting line

    Line3d line{ n1, n1 + direction };
    out_intersect_line = line;
    return true;
  }

  float ComputeAngleInDegrees(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2)
  {
    const float dot =  glm::dot(line_seg1.dir_vec, line_seg2.dir_vec);
    const auto theta = glm::acos(dot); //[0,PI)
    return glm::degrees(theta);
  }

  float ComputeAngleInDegrees(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    auto ab = glm::normalize(b-a);
    auto bc = glm::normalize(c-b);
    const auto dot = glm::dot(ab, bc);
    const auto angle = glm::acos(dot);
    return glm::degrees(angle);
  }

  float ComputeAngleInDegreesChatGPT(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    float abx = b.x - a.x, aby = b.y - a.y;
    float bcx = c.x - b.x, bcy = c.y - b.y;
    float dot = abx * bcx + aby * bcy;
    float det = abx * bcy - aby * bcx;
    return std::atan2(det, dot) * 180.0f / std::numbers::pi;
  }

  float AngleLinePlaneInDegrees(const Line3d& line, const Plane& plane)
  { //Vid 16
    return 90 - AngleLines(line.direction, plane.normal);
  }

  float AnglePlanes(const Plane& p1, const Plane& p2)
  { //Vid 16
    return AngleLines(p1.normal, p2.normal);
  }

  //Returns angle in degrees - lines in 2D can either be parallel or intersect
  float AngleLines2D(const Line2d& l1, const Line2d& l2)
  { //Vid 16
    return AngleLines(l1.direction, l2.direction);
  }

  //3D lines in 2D can either be parallel or intersect or skewed
  float AngleLines3D(const Line3d& l1, const Line3d& l2)
  { //Vid 16
    return AngleLines(l1.direction, l2.direction);
  }

  Point2d ComputeCentroid(const std::vector<Point2d>& points)
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

  bool Collinear(const glm::vec3& a, const glm::vec3& b)
  { //Vid 17
    //checking that a.x/b.x = a.y/b.y = a.x=z/b.z
    auto v1 = a.x * b.y - a.y * b.x;
    auto v2 = a.y * b.z - a.z * b.y;
    auto v3 = a.x * b.z - a.z * b.x;
    return Equal(v1, 0) && Equal(v2, 0) && Equal(v3, 0);
  }

  bool Collinear(const Point3d& a, const Point3d& b, const Point3d& c)
  { //Vid 17
    auto AB = b - a;
    auto AC = c - a;
    return Collinear(AB, AC);
  }

  bool Collinear(const glm::vec2& a, const glm::vec2& b)
  {
    return Equal(a.x * b.y, a.y*b.x);
  }

  bool Collinear(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    return Collinear(b-a, c-a);
  }

  bool Coplanar(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
  { //Vid 17
    float volume = ScalarTripleProduct(a, b, c);
    return Equal(volume, 0);
  }

  bool Coplanar(const Point3d& a, const Point3d& b, const Point3d& c, const Point3d& d)
  { //Vid 17
    auto AB = b - a;
    auto AC = c - a;
    auto AD = d - a;
    float volume = ScalarTripleProduct(AB, AC, AD);
    return Equal(volume, 0);
  }

  float Distance(const Line3d& line, const Point3d& C)
  { //Vid 18
    //derived formula - see vid 18 t1 - v.(Y-a) v is the unit vector on the line, a is a known point on the line, want to get distnce from point Y to line

    auto AC = C - line.start_point;
    auto t = glm::dot(line.direction, AC);

    auto xt = line.start_point + line.direction * t;
    auto dist_vect = xt - C;
    return glm::length(dist_vect);
    //rays: t< 0 closest point is the start point on the ray
    //segment t<0 => start point, t>0=> end point
  }

  float distance(const Plane& p, Point3d& Q)
  { //Derived in vid 19
    auto result = glm::dot(p.normal, Q) - p.d;
    return result;

    //Note: return value is positive ig Q is on the positive side of the place otherwise negative.  So can use as an orientation check
  }

}