#pragma once
//Ref: chantGPT - 'Architecture macro comparison'

// ============================================================
//  OS detection — MSVC, GCC, and Clang compatible
// ============================================================

#define OS_WINDOWS     0
#define OS_LINUX       0
#define OS_MACOS       0
#define OS_ANDROID     0
#define OS_IOS         0
#define OS_BSD         0
#define OS_UNIX        0
#define OS_UNKNOWN     0

// ============================================================
//  Windows
// ============================================================
#if defined(_WIN32) || defined(_WIN64)
#  undef  OS_WINDOWS
#  define OS_WINDOWS 1

// ============================================================
//  Apple Platforms (macOS / iOS / iPadOS / tvOS)
// ============================================================
#elif defined(__APPLE__) && defined(__MACH__)
#  include <TargetConditionals.h>
#  if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#    undef  OS_IOS
#    define OS_IOS 1
#  elif defined(TARGET_OS_MAC) && TARGET_OS_MAC
#    undef  OS_MACOS
#    define OS_MACOS 1
#  else
#    undef  OS_UNKNOWN
#    define OS_UNKNOWN 1
#  endif

// ============================================================
//  Android
// ============================================================
#elif defined(__ANDROID__)
#  undef  OS_ANDROID
#  define OS_ANDROID 1

// ============================================================
//  Linux
// ============================================================
#elif defined(__linux__)
#  undef  OS_LINUX
#  define OS_LINUX 1

// ============================================================
//  BSD variants
// ============================================================
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#  undef  OS_BSD
#  define OS_BSD 1

// ============================================================
//  Generic UNIX (fallback)
// ============================================================
#elif defined(__unix__) || defined(__unix)
#  undef  OS_UNIX
#  define OS_UNIX 1

// ============================================================
//  Unknown
// ============================================================
#else
#  undef  OS_UNKNOWN
#  define OS_UNKNOWN 1
#endif

// ============================================================
//  Optional: human-readable name
// ============================================================
#if OS_WINDOWS
#  define OS_NAME "Windows"
#elif OS_MACOS
#  define OS_NAME "macOS"
#elif OS_IOS
#  define OS_NAME "iOS"
#elif OS_ANDROID
#  define OS_NAME "Android"
#elif OS_LINUX
#  define OS_NAME "Linux"
#elif OS_BSD
#  define OS_NAME "BSD"
#elif OS_UNIX
#  define OS_NAME "UNIX"
#else
#  define OS_NAME "Unknown"
#endif