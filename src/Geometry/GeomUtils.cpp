#include "GeomUtils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <Common/Common.h>
#include <glm/gtx/norm.hpp> //for length2() (length squared)
#include <numbers> //for PI
#include <numeric> //iota

namespace Geom
{
  constexpr float pi_reciprical = 1/std::numbers::pi;

  Line2d GetBisector(Point2d const& p1, Point2d const& p2)
  {
    SPG_ASSERT(!Equal(p1,p2));
    Point2d midpoint = ComputeMidPoint(p1,p2);
    glm::vec2 v = p2-p1;
    glm::vec2 v_perp = glm::vec2(v.y, -v.x);

    Line2d bisector = Line2d::FromPointAndDirection(midpoint,v_perp);
    return bisector;
  }

  Point2d ComputeIntersection(Line2d const& l1, Line2d const& l2)
  {
    Point2d intersection;
    bool valid = ComputeIntersection(l1.start_point, l1.end_point, l2.start_point, l2.end_point, intersection);
    SPG_ASSERT(valid); //Triggers if they are parallel
    return intersection;
  }

  float ComputeSignedArea(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    //returns Det(a->b, a->c)*0.5.  
    return 0.5f * ((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));
  }

  RelativePos Orientation2d(const Point2d& a, const Point2d& b, const Point2d& c)
  { //Vid 10
    float area = ComputeSignedArea(a, b, c);
    if(Geom::Equal(area,0.0f,1000.0f))  
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

  bool Collinear(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    float det = ((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y)); //det(a->b, a->c)
    return Geom::Equal(det*0.5f,0);
  }

  bool Collinear(const LineSeg2D& seg, const Point2d& p)
  {
    return Collinear(seg.start, seg.end, p);
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

  bool SegContainsPoint(const LineSeg2D& seg, Point2d p)
  {
    if(Geom::Equal(seg.start.x, seg.end.x)) {//Vertical seg
      if ((p.y > std::min(seg.start.y, seg.end.y)) && (p.y < std::max(seg.start.y, seg.end.y)));
        return Geom::Collinear(seg.start, seg.end, p);
    }
    
    if(Geom::Equal(seg.start.y, seg.end.y)) { //Horizontal seg
      if ( (p.x > std::min(seg.start.x, seg.end.x)) && (p.x < std::max(seg.start.x, seg.end.x)));
        return Geom::Collinear(seg.start, seg.end, p);
    }
    
    if( (p.x > std::min(seg.start.x, seg.end.x)) && (p.x < std::max(seg.start.x, seg.end.x)) &&
        (p.y > std::min(seg.start.y, seg.end.y)) && (p.y < std::max(seg.start.y, seg.end.y)) )
    { //Diagonal seg
      return Geom::Collinear(seg.start, seg.end, p);
    }
    return false;  
  }

  bool SegIncludesPoint(const LineSeg2D& seg, Point2d p)
  {
    if(Geom::Equal(seg.start, p))
      return true;
    if(Geom::Equal(seg.end, p))
      return true;  
    return SegContainsPoint(seg,p);
  }

  bool IsHorizontal(const LineSeg2D& seg) {
    return Geom::Equal(seg.start.y, seg.end.y);
  }

  bool IsVertical(const LineSeg2D& seg) {
    return Geom::Equal(seg.start.x, seg.end.x);
  }

  bool Equal(const LineSeg2D& seg1, const LineSeg2D& seg2) 
  {
    // bool eq1 = Geom::Equal(seg1.start, seg2.start) && Equal(seg1.end, seg2.end);
    // bool eq2 = Geom::Equal(seg1.start, seg2.end) && Equal(seg1.end, seg2.start);
    // return eq1 || eq2;

    auto seg1_min_x = std::min(seg1.start.x,seg1.end.x);
    auto seg2_min_x = std::min(seg2.start.x,seg2.end.x); 
    if(!Geom::Equal(seg1_min_x,seg2_min_x))
      return false;
    auto seg1_max_x = std::max(seg1.start.x,seg1.end.x);
    auto seg2_max_x = std::max(seg2.start.x,seg2.end.x);  
    if(!Geom::Equal(seg1_max_x,seg2_max_x))
      return false;

    auto seg1_min_y = std::min(seg1.start.y,seg1.end.y);
    auto seg2_min_y = std::min(seg2.start.y,seg2.end.y); 
    if(!Geom::Equal(seg1_min_y,seg2_min_y))
      return false;
    auto seg1_max_y = std::max(seg1.start.y,seg1.end.y);
    auto seg2_max_y = std::max(seg2.start.y,seg2.end.y);  
    if(!Geom::Equal(seg1_max_y,seg2_max_y))
      return false;

    return true;  
  }

  //Todo - more testing needed!
  bool IntersectionExists(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2)
  { //Vid 14
    Point2d a = line_seg1.start;
    Point2d b = line_seg1.end;
    Point2d c = line_seg2.start;
    Point2d d = line_seg2.end;

    // if(SegIncludesPoint(line_seg1, line_seg2.start))
    //   return true;  
    // if(SegIncludesPoint(line_seg1, line_seg2.end))
    //   return true;   
    // if(SegIncludesPoint(line_seg2, line_seg1.start))
    //   return true;   
    // if(SegIncludesPoint(line_seg2, line_seg2.end))
    //   return true;   

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
  { //Vid 13 (Same as Penny's method - see notes)
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

  float ComputeAngleInDegrees(const LineSeg2D& seg1, const LineSeg2D& seg2)
  {
    /*
      NOTE:
      area of parallelogram = |det(u,v)|
      area of parallelogram = base * height = |u|*|h| => |h| = |det(u,v)| /|u|
      tan(angle) = |h| / proj(v onto u) = det(u,v) / dot(u,v)

      u->v oriented CCW => det(u,v) > 0
      u->v oriented CW =>  det(u,v) < 0
      angle(u,v) acute => dot(u,v) > 0
      angle(u,v) obtuse => dot(u,v) < 0
      std::atan2() uses the above to return quadrant
      return value [-pi,pi] radians

      This method is much faster than (which needs sq roots)
        auto ab = glm::normalize(b-a);
        auto bc = glm::normalize(c-b);
        const auto dot = glm::dot(ab, bc);
        const auto angle = glm::acos(dot);
        return glm::degrees(angle);
    */

    glm::vec2 u = seg1.end - seg1.start;
    glm::vec2 v = seg2.end - seg2.start;
    float dot = u.x * v.x + u.y * v.y;
    float det = u.x * v.y - u.y * v.x;
    if(det == -0)
      det = 0;
    return std::atan2(det, dot) * 180.0f * pi_reciprical;
  }

  float ComputeAngleInDegrees(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    LineSeg2D seg1{a,b}, seg2{b,c};
    return ComputeAngleInDegrees(seg1,seg2);
  }

  float ComputeSubtendedAngleInDegrees(const Point2d& a, const Point2d& b, const Point2d& c)
  {
    float angle = ComputeAngleInDegrees(a,b,c); 
    angle = (angle < 0 ? -angle : angle); //output range [0,180]
    return 180 - angle; //subtended angle is complementary angle.
  }

#ifdef _WIN32
  float AngleLinePlaneInDegrees(const Line3d& line, const Plane& plane)
  { //Vid 16
    return 90 - AngleLines<3,float>(line.direction, plane.normal);
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
#endif

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

  // See LevelBuilder project
  //https://www.geeksforgeeks.org/area-of-a-polygon-with-given-n-ordered-vertices/
  //https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
  // This gives opposite sign to the ChatGPT on below
#if 0
  float SignedArea(const std::vector<Point2d>& pts)
  {
    if(pts.size() < 3)
      return 0;
    // Initialze area 
    float area = 0.0;

    // Calculate value of shoelace formula 
    int n = pts.size();
    int j = n - 1;
    for (int i = 0; i < n; i++)
    {
      area += (pts[j].x + pts[i].x) * (pts[j].y - pts[i].y);
      j = i;  // j is previous vertex to i 
    }
    return area / 2;
  }
#endif

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

  //CCW => +ve, CW=>-ve (ChatGPT's version)
  float SignedArea(const std::vector<Point2d>& pts) 
  {
     if (pts.size() < 3)
        return 0.0;

      double area = 0.0;
      for (size_t i = 0; i < pts.size(); ++i)
      {
          const auto& p = pts[i];
          const auto& q = pts[(i + 1) % pts.size()];
          area += p.x * q.y - q.x * p.y;
      }
      return area * 0.5;
  }

  // CCW ordering starting from +ve x-axis
  void SortRadially(std::vector<Point2d>& pts)
  {
    Point2d centroid = ComputeCentroid(pts);
    float cy = centroid.y;
    float cx = centroid.x;

    std::sort(pts.begin(), pts.end(),
      [&](const Point2d& a, const Point2d& b)
      {
        //atan2 returns angles in (-PI,PI])
        return std::atan2(a.y - cy, a.x - cx) <
               std::atan2(b.y - cy, b.x - cx);
      }
    );
  }

  void ForceCCW(std::vector<Point2d>& pts) 
  {
    if(SignedArea(pts) <0 )
      std::reverse(pts.begin(), pts.end());
  }

  // See LevelBuilder project
  //https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html
  bool PointInPolygon(const std::vector<Point2d>& pts, const Point2d& test)
  {
    uint32_t nvert = pts.size();
    if(nvert < 3)
      return false;
    uint32_t i, j;
    double verty_i, verty_j, vertx_i, vertx_j;
    double test_x = test.x;
    double test_y = test.y;
    bool result = false;
    for (i = 0, j = nvert - 1; i < nvert; j = i++)
    {
      verty_i = pts[i].y;
      verty_j = pts[j].y;
      vertx_i = pts[i].x;
      vertx_j = pts[j].x;

      if (((verty_i>test_y) != (verty_j>test.y)) &&
        (test.x < (vertx_j - vertx_i) * (test.y - verty_i) / (verty_j - verty_i) + vertx_i))
        result = !result;
    }
    return result;
  }

  // See LevelBuilder project
  //Barycentric Technique - supposedly faster than the above
  //http://blackpawn.com/texts/pointinpoly/default.html
  bool PointInTriangle(Point2d& a, Point2d& b, Point2d& c, Point2d& p)
  {
    glm::vec2 v0 = c-a;
    glm::vec2 v1 = b-a;
    glm::vec2 v2 = p-a;
    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    // Compute barycentric coordinates
    float denom = (dot00 * dot11 - dot01 * dot01);
    if(Equal(denom,0))
      return false;
    float invDenom = 1 / denom;
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u > 0) && (v > 0) && (u + v < 1);
  }

  Point2d ComputeCentroid(const std::vector<Point2d*> points)
  {
    Point2d centroid = {0, 0};
    for (const auto& p : points) {
        centroid.x += p->x;
        centroid.y += p->y;
    }
    centroid.x /= points.size();
    centroid.y /= points.size();
    return centroid;
  }

  float SignedArea(const std::vector<Point2d*> pts)
  {
    if (pts.size() < 3)
        return 0.0;

      double area = 0.0;
      for (size_t i = 0; i < pts.size(); ++i)
      {
          const auto p = pts[i];
          const auto q = pts[(i + 1) % pts.size()];
          area += p->x * q->y - q->x * p->y;
      }
      return area * 0.5;
  }
  
  void ForceCCW(std::vector<Point2d*> pts)
  {
    if(SignedArea(pts) <0 )
      std::reverse(pts.begin(), pts.end());
  }

  void SortRadially(std::vector<Point2d*> pts)
  {
    Point2d centroid = ComputeCentroid(pts);
    float cy = centroid.y;
    float cx = centroid.x;

    std::sort(pts.begin(), pts.end(),
      [&](Point2d* a, Point2d* b)
      {
        //atan2 returns angles in (-PI,PI])
        return std::atan2(a->y - cy, a->x - cx) <
               std::atan2(b->y - cy, b->x - cx);
      }
    );
  }

  bool PointInPolygon(const std::vector<Point2d*> pts, const Point2d& test)
  {
    uint32_t nvert = pts.size();
    if(nvert < 3)
      return false;
    uint32_t i, j;
    double verty_i, verty_j, vertx_i, vertx_j;
    double test_x = test.x;
    double test_y = test.y;
    bool result = false;
    for (i = 0, j = nvert - 1; i < nvert; j = i++)
    {
      verty_i = pts[i]->y;
      verty_j = pts[j]->y;
      vertx_i = pts[i]->x;
      vertx_j = pts[j]->x;

      if (((verty_i>test_y) != (verty_j>test.y)) &&
        (test.x < (vertx_j - vertx_i) * (test.y - verty_i) / (verty_j - verty_i) + vertx_i))
        result = !result;
    }
    return result;
  }

  // See LevelBuilder project
  //http://blackpawn.com/texts/pointinpoly/default.html
#if 0
  bool SameSide(Point2d& p1, Point2d& p2, Point2d& a, Point2d& b)
  {
    glm::vec2 ab = b-a;
    glm::vec2 a_p1 = p1-a;
    glm::vec2 a_p2 = p2-a;
    float cp1 = glm::cross(ab, a_p1);
    float cp2 = glm::cross(ab, a_p2);
    return cp1 * cp2 > 0;	//effectively the dot product
  }
   // See LevelBuilder project
  bool PointInTriangle2(Point2d& a, Point2d& b, Point2d& c, Point2d& p)
  {
    if (!SameSide(p, a, b, c))
      return false;
    if (!SameSide(p, b, a, c))
      return false;
    if (!SameSide(p, c, a, b))
      return false;
    return true;
  }
#endif

}