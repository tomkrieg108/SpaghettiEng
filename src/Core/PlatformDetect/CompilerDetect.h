#pragma once
//Ref: chantGPT - 'Architecture macro comparison'

#include "Core/HelperMacros.h"

// ============================================================
//  Compiler detection — MSVC, Clang, GCC, Intel, etc.
// ============================================================

#define COMPILER_MSVC      0
#define COMPILER_CLANG     0
#define COMPILER_GCC       0
#define COMPILER_INTEL     0
#define COMPILER_LLVM      0
#define COMPILER_UNKNOWN   0

// ============================================================
//  Microsoft Visual C++
// ============================================================
#if defined(_MSC_VER)
#  undef  COMPILER_MSVC
#  define COMPILER_MSVC 1

// Version decoding (e.g. 1938 → MSVC 19.38)
#  define COMPILER_VERSION_MAJOR (_MSC_VER / 100)
#  define COMPILER_VERSION_MINOR (_MSC_VER % 100)
#  define COMPILER_VERSION_STR "MSVC " STRINGIFY(_MSC_VER)

// ============================================================
//  Clang (Apple Clang or LLVM Clang)
// ============================================================
#elif defined(__clang__)
#  undef  COMPILER_CLANG
#  define COMPILER_CLANG 1
#  define COMPILER_VERSION_MAJOR __clang_major__
#  define COMPILER_VERSION_MINOR __clang_minor__
#  define COMPILER_VERSION_PATCH __clang_patchlevel__
#  define COMPILER_VERSION_STR "Clang " STRINGIFY(__clang_major__) "." STRINGIFY(__clang_minor__)

// ============================================================
//  GCC
// ============================================================
#elif defined(__GNUC__)
#  undef  COMPILER_GCC
#  define COMPILER_GCC 1
#  define COMPILER_VERSION_MAJOR __GNUC__
#  define COMPILER_VERSION_MINOR __GNUC_MINOR__
#  define COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#  define COMPILER_VERSION_STR "GCC " STRINGIFY(__GNUC__) "." STRINGIFY(__GNUC_MINOR__)

// ============================================================
//  Intel (classic or oneAPI)
// ============================================================
#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)
#  undef  COMPILER_INTEL
#  define COMPILER_INTEL 1
#  ifdef __INTEL_COMPILER
#    define COMPILER_VERSION_MAJOR (__INTEL_COMPILER / 100)
#    define COMPILER_VERSION_MINOR (__INTEL_COMPILER % 100)
#  endif
#  define COMPILER_VERSION_STR "Intel C++"

// ============================================================
//  LLVM-based (generic fallback for other LLVM frontends)
// ============================================================
#elif defined(__llvm__)
#  undef  COMPILER_LLVM
#  define COMPILER_LLVM 1
#  define COMPILER_VERSION_STR "LLVM-based compiler"

// ============================================================
//  Unknown
// ============================================================
#else
#  undef  COMPILER_UNKNOWN
#  define COMPILER_UNKNOWN 1
#  define COMPILER_VERSION_STR "Unknown compiler"
#endif



// ============================================================
//  Human-readable compiler name
// ============================================================
#if COMPILER_MSVC
#  define COMPILER_NAME "MSVC"
#elif COMPILER_CLANG
#  define COMPILER_NAME "Clang"
#elif COMPILER_GCC
#  define COMPILER_NAME "GCC"
#elif COMPILER_INTEL
#  define COMPILER_NAME "Intel C++"
#elif COMPILER_LLVM
#  define COMPILER_NAME "LLVM"
#else
#  define COMPILER_NAME "Unknown"
#endif