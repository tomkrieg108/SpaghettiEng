#pragma once

#include "Math/FloatingPoint.h"

#include <algorithm> //std::max()
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>



namespace MathX
{
  using Real = double;

  using Vec2 = glm::vec<2,Real,glm::packed_highp>; // glm::dvec2
  using Vec3 = glm::vec<3,Real,glm::packed_highp>; // glm::dvec3
  using Mat3 = glm::mat<3,3,Real,glm::packed_highp>; // glm::dmat3
  using Mat4 = glm::mat<4,4,Real,glm::packed_highp>; // glm::dmat4

  using Point2 = Vec2;
  using Point3 = Vec3;

  inline constexpr Real REAL_MAX = std::numeric_limits<Real>::max(); 
  

  struct Circle
  {
    Point2 center;
    Real radius;
  };

  Circle ComputeCircumCircle(Point2& a, Point2& b, Point2& c);


  void HelloMathLib();
}