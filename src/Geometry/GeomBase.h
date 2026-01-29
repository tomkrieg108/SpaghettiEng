#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp> //epsilonEqual()
#include <vector>
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
    constexpr auto eps = std::numeric_limits<float>::epsilon();
    return fabs(v1 - v2) <= 32.0* eps * std::max(1.0f, std::max(fabs(v1), fabs(v2)));
  }

  inline bool Equal(double v1, double v2)
  {
    constexpr auto eps = std::numeric_limits<double>::epsilon();
    return std::fabs(v1 - v2) <= 1000.0 * eps * std::max(1.0, std::max(fabs(v1), fabs(v2)));
  }

   //ULP: Unit in last place.  Distance between current and next possible number
   //Each arithmetic op introduces ~ 0.5 ULP error 
   //Todo - the next 2 are not currently used anywhere
  inline bool EqualULP_1Sided(float a, float b, const float, int max_ulps = 8) 
  {
    float spacing = std::nextafter(a, INFINITY) - a;
    return std::fabs(a - b) <= max_ulps * spacing;
  }

  inline bool EqualULP(float a, float b, const float, int max_ulps = 4) 
  {
    float spacing_a = std::nextafter(a, INFINITY) - a;
    float spacing_b = std::nextafter(b, INFINITY) - b;
    float tol = max_ulps * std::max(spacing_a, spacing_b);
    return std::fabs(a - b) <= tol;
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