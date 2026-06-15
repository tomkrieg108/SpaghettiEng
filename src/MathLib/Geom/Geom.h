# pragma once

#include "MathLib/MathLib.h"
#include "MathLib/Geom/Line.h"
#include "MathLib/Geom/Plane.h"

namespace SpgMth
{

  // ==== From GeomBase.h ========================================
  enum class RelativePos 
  {
    Left, Right, Beyond, Behind, Between, Origin, Destination
  };

  inline bool Xor(bool x, bool y) 
  {
    return x ^ y;
  }

  constexpr float Epsilon(const float scale_factor = 100.0f)
  {
    return std::numeric_limits<float>::epsilon() * scale_factor;
  }
    
  inline bool Equal(float v1, float v2, const float scale_factor = 100.0f)
  {
    return SpgMth::NumUtils::Equal(v1,v2);
  }

  inline bool Equal(double v1, double v2)
  {
    return SpgMth::NumUtils::Equal(v1,v2);
  }

  inline bool Equal(const Point2d& a, const Point2d& b, const float scale_factor = 1000.0f)
  {
    return SpgMth::NumUtils::Equal(a,b);
  }

  //===========================================================================

  Point2d ComputeMidPoint(Point2d const& p1, Point2d const& p2);

  Line2d GetBisector(Point2d const& p1, Point2d const& p2);

  Point2d ComputeIntersection(Line2d const& l1, Line2d const& l2);

  float ComputeSignedArea(const Point2d& a, const Point2d& b, const Point2d& c);

  RelativePos Orientation2d(const Point2d& a, const Point2d& b, const Point2d& c);

  RelativePos Orientation2d(const LineSeg2D& line_seg, const Point2d& p);

  bool Collinear(const glm::vec3& a, const glm::vec3& b);

  bool Collinear(const Point3d& a, const Point3d& b, const Point3d& c);

  bool Collinear(const Point2d& a, const Point2d& b, const Point2d& c);

  bool Collinear(const LineSeg2D& seg, const Point2d& p);

  bool SegContainsPoint(const LineSeg2D& seg, Point2d p);

  bool IsHorizontal(const LineSeg2D& seg);

  bool IsVertical(const LineSeg2D& seg);

  bool Equal(const LineSeg2D& seg1, const LineSeg2D& seg2);

  bool SegIncludesPoint(const LineSeg2D& seg, Point2d p);

  bool IntersectionExists(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2);

  bool StrictIntersectionExists(const LineSeg2D& line_seg1, const LineSeg2D& line_seg2);

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

  float ComputeAngleInDegrees(const LineSeg2D& seg1, const LineSeg2D& seg2);

  float ComputeAngleInDegrees(const Point2d& a, const Point2d& b, const Point2d& c);

  float ComputeSubtendedAngleInDegrees(const Point2d& a, const Point2d& b, const Point2d& c);

  #ifdef _WIN32
  float AngleLinePlaneInDegrees(const Line3d& line, const Plane& plane);

  float AnglePlanes(const Plane& p1, const Plane& p2);

  //Returns angle in degrees - lines in 2D can either be parallel or intersect
  float AngleLines2D(const Line2d& l1, const Line2d& l2);

  //3D lines in 2D can either be parallel or intersect or skewed
  float AngleLines3D(const Line3d& l1, const Line3d& l2);
#endif

  bool Coplanar(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

  bool Coplanar(const Point3d& a, const Point3d& b, const Point3d& c, const Point3d& d);

  float Distance(const Line3d& line, const Point3d& C);

  float Distance(const Plane& p, Point3d& Q);

  bool Left(const LineSeg2D& line_seg, const Point2d& p);

  bool Right(const LineSeg2D& line_seg, const Point2d& p);

  bool Between(const LineSeg2D& line_seg, const Point2d& p);

  bool Behind(const LineSeg2D& line_seg, const Point2d& p);

  bool Beyond(const LineSeg2D& line_seg, const Point2d& p);

  bool LeftOrBeyond(const LineSeg2D& line_seg, const Point2d& p);

  bool IsConvex(const Point2d& a, const Point2d& b, const Point2d& c);

  float ScalarTripleProduct(glm::vec3 a, glm::vec3 b, glm::vec3 c);

  Point2d ComputeCentroid(const std::vector<Point2d>& points);

  float SignedArea(const std::vector<Point2d>& pts);
  
  void ForceCCW(std::vector<Point2d>& pts);

  void SortRadially(std::vector<Point2d>& pts);

  bool PointInPolygon(const std::vector<Point2d>& pts, const Point2d& test);

  bool PointInTriangle(Point2d& a, Point2d& b, Point2d& c, Point2d& p);

  Point2d ComputeCentroid(const std::vector<Point2d*> points);

  float SignedArea(const std::vector<Point2d*> pts);
  
  void ForceCCW(std::vector<Point2d*> pts);

  void SortRadially(std::vector<Point2d*> pts);

  bool PointInPolygon(const std::vector<Point2d*> pts, const Point2d& test);

  //===================================

  struct Circle
  {
    Point2d center;
    Real radius;
  };

  Circle ComputeCircumCircle(Point2d& a, Point2d& b, Point2d& c);

  //====================================

  struct BoundingBox
  {
    float top = std::numeric_limits<float>::lowest();
    float bottom = std::numeric_limits<float>::max();
    float right = std::numeric_limits<float>::lowest();
    float left = std::numeric_limits<float>::max();

    void Update(Point2d p){
      if(p.y > top)
        top = p.y;
      if(p.y < bottom)
        bottom = p.y;
      if(p.x > right)
        right = p.x;
      if(p.x < left)
        left = p.x;  
    }

    float Width() {
      return right - left;
    }
 
    float Height() {
      return top - bottom;
    }

    void AddBorder(float size) {
      top += size;
      bottom -= size;
      right += size;
      left -= size;
    }

    //Return Bounding box as vector of Point2D, CCW orientation
    std::vector<Point2d> GetPoints() {
      std::vector<Point2d> points = {
         {right,top}, {left,top}, {left,bottom}, {right,bottom}
      };
      return points;
    }

    std::vector<LineSeg2D> GetLineSegs() {
       std::vector<LineSeg2D> segs = {
         {{right,top},    {left,top}},
         {{left,top},     {left,bottom}},
         {{left,bottom},  {right,bottom}},
         {{right,bottom},   {right,top}} 
       };
       return segs;
    }
  };
  
}