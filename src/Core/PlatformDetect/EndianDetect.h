#pragma once
//Ref: chantGPT - 'Architecture macro comparison'

// ============================================================
//  Endianness detection — works with MSVC, GCC, Clang, etc.
// ============================================================

#define ENDIAN_LITTLE   0
#define ENDIAN_BIG      0
#define ENDIAN_UNKNOWN  0

// ============================================================
//  1. Compiler and standard macros (most reliable)
// ============================================================

// C++20 introduced endian detection in <bit>
#if __has_include(<bit>)
#  include <bit>
#  if defined(__cpp_lib_endian) || defined(__cpp_lib_bit_cast)
#    include <bit>
#    if std::endian::native == std::endian::little
#      undef  ENDIAN_LITTLE
#      define ENDIAN_LITTLE 1
#    elif std::endian::native == std::endian::big
#      undef  ENDIAN_BIG
#      define ENDIAN_BIG 1
#    else
#      undef  ENDIAN_UNKNOWN
#      define ENDIAN_UNKNOWN 1
#    endif
#  endif
#endif

// ============================================================
//  2. GCC / Clang predefined macros
// ============================================================
#if !ENDIAN_LITTLE && !ENDIAN_BIG
#  if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
#    if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#      undef  ENDIAN_LITTLE
#      define ENDIAN_LITTLE 1
#    elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#      undef  ENDIAN_BIG
#      define ENDIAN_BIG 1
#    endif
#  endif
#endif

// ============================================================
//  3. MSVC (all mainstream Windows targets are little-endian)
// ============================================================
#if !ENDIAN_LITTLE && !ENDIAN_BIG
#  if defined(_MSC_VER)
#    undef  ENDIAN_LITTLE
#    define ENDIAN_LITTLE 1
#  endif
#endif

// ============================================================
//  4. Fallback runtime test (optional, only used if unknown)
// ============================================================
#if !ENDIAN_LITTLE && !ENDIAN_BIG
#  include <cstdint>
#  static inline int detect_endianness_runtime() {
#    uint16_t x = 0x1;
#    return (*reinterpret_cast<unsigned char*>(&x) == 0x1) ? 0 : 1; // 0=little, 1=big
#  }
#  define ENDIAN_RUNTIME_CHECK 1
#  define ENDIAN_UNKNOWN 1
#else
#  define ENDIAN_RUNTIME_CHECK 0
#endif

// ============================================================
//  Human-readable name
// ============================================================
#if ENDIAN_LITTLE
#  define ENDIAN_NAME "Little Endian"
#elif ENDIAN_BIG
#  define ENDIAN_NAME "Big Endian"
#else
#  define ENDIAN_NAME "Unknown Endian"
#endif