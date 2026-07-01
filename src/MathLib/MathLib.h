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
    using Vec4 = glm::vec4;

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
    inline auto Dot(const T& v1, const T& v2) {return glm::dot(v1,v2);}

    template <typename T>
    inline auto Length(const T& v) {return glm::length(v);}

    template <typename T>
    inline auto Distance(const T& v1, const T& v2) {return glm::distance(v1,v2);}

    template <typename T>
    inline auto Normalize(const T& v) {return glm::normalize(v);}

    template <typename T>
    inline auto Abs(const T& v) {return glm::abs(v);}

    template <typename T>
    inline auto Sqrt(const T& v) {return glm::sqrt(v);}

    template <typename T>
    inline auto Sin(const T& v) {return glm::sin(v);}

    template <typename T>
    inline auto Cos(const T& v) {return glm::cos(v);}

    template <typename T>
    inline auto Exp(const T& v) {return glm::exp(v);}

    inline Mat4 Transpose(const Mat4& m) { return glm::transpose(m); }

    inline Mat4 Inverse(const Mat4& m)   { return glm::inverse(m); }

    inline const float* GetPtr(const Mat4& m) { 
        return &m[0].x; // Works perfectly for GLM
    }
    inline const float* GetPtr(const Vec3& v) { 
        return &v.x; 
    }
    //using Vec4a = glm::aligned_highp_vec4;
  //}


  using Real = float;
  inline constexpr Real REAL_MAX = std::numeric_limits<Real>::max(); 
 
}

/*
  packed vec3 is 12 bytes, not 16:  important when sending data to the GPU
  aligned vec3 - might add padding to make it 16 bytes so that it aligns with SIMD regs
*/

// Logging =================================================================

#include <spdlog/spdlog.h>
#include <glm/gtx/string_cast.hpp>

template<glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L,T,Q>> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::vec<L,T,Q>& v, FormatContext& ctx) const  {
		return fmt::format_to(ctx.out(), "{}", glm::to_string(v));
	}
};

template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<C,R,T,Q>> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::mat<C,R,T,Q>& m, FormatContext& ctx) const  {
		return fmt::format_to(ctx.out(), "{}", glm::to_string(m));
	}
};

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::qua<T, Q>& q, FormatContext& ctx) const  {
		return fmt::format_to(ctx.out(), "{}", glm::to_string(q));
	}
};