#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp> //epsilonEqual()
#include <algorithm>

namespace Geom
{
  using Point2d = glm::vec2;
  using Point3d = glm::vec3;

  enum class RelativePos 
  {
    Left, Right, Beyond, Behind, Between, Origin, Destination
  };

  inline bool Xor(bool x, bool y) {
	  return x ^ y;
  }

  /*
    Return epsilon value used for comparison of floats. Optional Optional float scale factor multiples std::numeric_limits<float>::epsilon() to determine epsilon value.  scale_factor defaults to 100.0 giving a value of 0.000019 
  */
  constexpr float Epsilon(const float scale_factor = 100.0f)
  {
    return std::numeric_limits<float>::epsilon() * scale_factor;
  }

  /*
    TESTED (Catch2)
    Test for equality between 2 float values.  Optional float param scale_factor multiples std::numeric_limits<float>::epsilon() to determine epsilon. scale_factor defaults to 100.0 giving a epsilon of 0.000019
  */
  inline bool Equal(float v1, float v2, const float scale_factor = 100.0f) //link error if not inline
  {
    return std::abs(v1 - v2) < Epsilon(scale_factor);
  }

  inline bool EqualRel(float v1, float v2, float relTol = 1e-4f) {
    return std::fabs(v1 - v2) <= relTol * std::max(std::fabs(v1), std::fabs(v2));
  }

  /*
    TESTED (Catch2)
    Test for equality between 2 Point2d values (alias of glm::vec2).  Optional float param scale_factor multiples std::numeric_limits<float>::epsilon() to determine epsilon. scale_factor defaults to 100.0 giving a epsilon of 0.000019
  */
  inline bool Equal(const Point2d& a, const Point2d& b, const float scale_factor = 1000.0f)
  {
    bool allEqual = glm::all(glm::epsilonEqual(a, b, Epsilon(scale_factor)));
    return allEqual;
  }

  

}