#pragma once

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

  inline bool Equal(Real v1, Real v2)
  {
    constexpr auto eps = std::numeric_limits<Real>::epsilon();
    return std::fabs(v1 - v2) <= 1000.0 * eps * std::max(1.0, std::max(fabs(v1), fabs(v2)));
  }

  void HelloMathLib();
}