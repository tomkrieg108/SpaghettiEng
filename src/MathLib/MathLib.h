#pragma once

#include "MathLib/FloatingPoint.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp> //for length2() (length squared)

#include "CoreLib/Core.h"
//#include <glm/gtc/type_aligned.hpp>

namespace SpgMth
{
  //namespace Lina
  //{
    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec3;

    using Point2d = glm::vec2;
    using Point3d = glm::vec3;
    using Point4d = glm::vec4;

    using DVec2 = glm::dvec2;
    using DVec3 = glm::dvec3;
    using DVec4 = glm::dvec4;

    using DPoint2d = glm::dvec2;
    using DPoint3d = glm::dvec3;
    using DPoint4d = glm::dvec4;

    using Mat2 = glm::mat2;
    using Mat3 = glm::mat3;
    using Mat4 = glm::mat4;

    using Quat = glm::quat;

    template <typename T>
    inline auto Dot(T v1, T v2) {return glm::dot(v1,v2);}
    //using Vec4a = glm::aligned_highp_vec4;
  //}


  using Real = float;
  inline constexpr Real REAL_MAX = std::numeric_limits<Real>::max(); 
 
  

  void Init();
}



/*
  packed vec3 is 12 bytes, not 16:  important when sending data to the GPU
  aligned vec3 - might add padding to make it 16 bytes so that it aligns with SIMD regs
*/

