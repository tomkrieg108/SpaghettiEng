#pragma once

#include <algorithm> // std::max
#include <glm/glm.hpp>


namespace MathX
{
  namespace NumUtils
  {

    enum class Precision {
      Strict,  // ~10 * eps
      Default, // ~100 * eps
      Loose    // ~1000 * eps
    };

    template <typename T>
    struct Tolerance {
        T abs;
        T rel;
    };

    // Internal helper to map intent to math constants
    template <typename T>
    inline constexpr Tolerance<T> GetTolerance(Precision p) {
        constexpr T eps = std::numeric_limits<T>::epsilon();
        switch (p) {
            case Precision::Strict:  return { eps * T(10),   eps * T(10)   };
            case Precision::Loose:   return { eps * T(1000), eps * T(1000) };
            case Precision::Default: 
            default:                 return { eps * T(100),  eps * T(100)  };
        }
    }

    // --- SCALAR CHECKS ---
    template <typename T>
    inline bool Equal(T a, T b, Precision p = Precision::Default) {
        const auto tol = GetTolerance<T>(p);
        const T diff = std::fabs(a - b);
        // Hybrid check: Absolute for near-zero, Relative for large numbers
        return (diff <= tol.abs) || (diff <= tol.rel * std::max(std::fabs(a), std::fabs(b)));
    }

    template <typename T>
    inline bool IsNearlyZero(T val, Precision p = Precision::Default) {
        return std::fabs(val) <= GetTolerance<T>(p).abs;
    }

    // --- VECTOR CHECKS (GLM) ---
    template <glm::length_t L, typename T, glm::qualifier Q>
    inline bool Equal(const glm::vec<L, T, Q>& a, const glm::vec<L, T, Q>& b, Precision p = Precision::Default) {
        const auto tol = GetTolerance<T>(p);
        const auto diff = glm::abs(a - b);
        const auto max_val = glm::max(glm::abs(a), glm::abs(b));
        
        // Vectorized component-wise comparison
        const auto is_equal = glm::lessThanEqual(diff, glm::vec<L, T, Q>(tol.abs)) || 
                              glm::lessThanEqual(diff, tol.rel * max_val);
        
        return glm::all(is_equal);
    }

    template <glm::length_t L, typename T, glm::qualifier Q>
    inline bool IsNearlyZero(const glm::vec<L, T, Q>& v, Precision p = Precision::Default) {
        const T absTol = GetTolerance<T>(p).abs;
        // Faster squared-length check to avoid sqrt
        return glm::dot(v, v) <= (absTol * absTol);
    }

    // --- SNAPPING / CLAMPING ---

    /** * If the value is within the tolerance of zero, forces it to exactly 0.
     */
    template <typename T>
    inline T SnapToZero(T val, Precision p = Precision::Default) {
        return IsNearlyZero(val, p) ? T(0) : val;
    }

    /** * If 'val' is nearly equal to 'target', returns 'target'. 
     * Useful for snapping to grid lines, 1.0, or Pi.
     */
    template <typename T>
    inline T SnapToValue(T val, T target, Precision p = Precision::Default) {
        return Equal(val, target, p) ? target : val;
    }

    // --- VECTORIZED SNAPPING ---
    template <glm::length_t L, typename T, glm::qualifier Q>
    inline glm::vec<L, T, Q> SnapToZero(const glm::vec<L, T, Q>& v, Precision p = Precision::Default) {
        // Component-wise snap
        const auto tol = GetTolerance<T>(p);
        glm::vec<L, T, Q> result;
        for (glm::length_t i = 0; i < L; ++i) {
            result[i] = (std::fabs(v[i]) <= tol.abs) ? T(0) : v[i];
        }
        return result;
    }

    /**
     * Standard clamp utility (wrapper for std::clamp but supporting our Precision logic)
     * This ensures a value is strictly between min and max.
     */
    template <typename T>
    inline T SafeClamp(T val, T min, T max) {
        return std::max(min, std::min(max, val));
    }

    // --- FUZZY INEQUALITIES ---

    /** * Returns true if a is greater than b OR nearly equal to b.
     * Use this for "boundary" checks (e.g., if (val >= 0.0)).
     */
    template <typename T>
    inline bool NearlyGreater(T a, T b, Precision p = Precision::Default) {
        return (a > b) || Equal(a, b, p);
    }

    /** * Returns true if a is less than b OR nearly equal to b.
     */
    template <typename T>
    inline bool NearlyLess(T a, T b, Precision p = Precision::Default) {
        return (a < b) || Equal(a, b, p);
    }

    /**
     * Returns true if a is strictly greater than b and NOT nearly equal.
     * Useful for ensuring a gap exists between two values.
     */
    template <typename T>
    inline bool StrictlyGreater(T a, T b, Precision p = Precision::Default) {
        return (a > b) && !Equal(a, b, p);
    }

    /**
     * Returns true if a is strictly less than b and NOT nearly equal.
     */
    template <typename T>
    inline bool StrictlyLess(T a, T b, Precision p = Precision::Default) {
        return (a < b) && !Equal(a, b, p);
    }

    // --- VECTORIZED 
    template <glm::length_t L, typename T, glm::qualifier Q>
    inline bool AllNearlyGreater(const glm::vec<L, T, Q>& a, const glm::vec<L, T, Q>& b, Precision p = Precision::Default) {
        // True if every component of a is >= (nearly) b
        glm::vec<L,bool> res;
        for (glm::length_t i = 0; i < L; ++i) 
          res[i] = NearlyGreater(a[i], b[i], p);
        return glm::all(res);
    }

    // --- SAFETY CHECKS ---

    template <typename T>
    inline bool IsNaN(T val) { return std::isnan(val); }

    template <typename T>
    inline bool IsInf(T val) { return std::isinf(val); }

    /**
     * Returns true if the number is neither NaN nor Infinity.
     * This is the "gold standard" check for valid data.
     */
    template <typename T>
    inline bool IsFinite(T val) { return std::isfinite(val); }

    // --- VECTORIZED SAFETY ---

    template <glm::length_t L, typename T, glm::qualifier Q>
    inline bool AnyNaN(const glm::vec<L, T, Q>& v) {
        for (glm::length_t i = 0; i < L; ++i) {
          if (std::isnan(v[i])) 
            return true;
          return false;
        }
    }

    template <glm::length_t L, typename T, glm::qualifier Q>
    inline bool AllFinite(const glm::vec<L, T, Q>& v) {
        for (glm::length_t i = 0; i < L; ++i) {
          if (!std::isfinite(v[i])) 
            return false;
          return true;
        }
    }

    /**
     * A robust normalization helper.
     * Prevents crashes if the vector is zero, NaN, or infinite.
     */
    template <glm::length_t L, typename T, glm::qualifier Q>
    inline glm::vec<L, T, Q> SafeNormalize(const glm::vec<L, T, Q>& v, const glm::vec<L, T, Q>& fallback = glm::vec<L, T, Q>(0)) {
        T sqMag = glm::dot(v, v);
        if (sqMag > GetTolerance<T>(Precision::Strict).abs && std::isfinite(sqMag)) {
            return v * glm::inversesqrt(sqMag);
        }
        return fallback;
    }

  } //namespace NumUtils

} // namespace MathX