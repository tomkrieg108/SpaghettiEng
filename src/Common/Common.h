#pragma once
#include "Logger.h"
#include "SmartPointerUtils.h"
#include "LibCheck.h"

//#define SPG_LIB_LINK_CHECK
//#define SPG_CALLBACK_CHECK

//Todo - do this in cmake if possible.  Is this even needed?  See ChatGPT "Cross-platform OpenGL CMake setup"
#if defined(__x86_64__) || defined(_M_X64)
  #define SPG_ARCH_X64
#elif defined(__i386__) || defined(_M_IX86)
  #define SPG_ARCH_X86
#elif defined(__arm__) || defined(_M_ARM)
  #define SPG_ARCH_ARM32
#elif defined(__aarch64__) || defined(_M_ARM64)
   #define SPG_ARCH_ARM64
#else
  #defined SPG_ARCH_UNKNOWN
#endif

#ifdef SPG_DEBUG
  #define SPG_ENABLE_ASSERTS
  #ifdef _MSC_VER
    #define SPG_DEBUGBREAK() __debugbreak()
  #elif defined(__GNUC__) || defined(__clang__)
    //__GNUC__ is defined for GCC, MinGW, Cygwin)
    #include <csignal>
    #define SPG_DEBUGBREAK() raise(SIGTRAP)
  #else
    #error "Platform does not support debugbreak."
  #endif
#endif

// #define SPG_STR(s) #s
// #define SPG_MACRO_STRING_VALUE(S) SPG_STR(s)
// #define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)
// #define COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N

#define BIT(x) (1 << x)

#ifdef SPG_ENABLE_ASSERTS
  #define SPG_ASSERT(check) \
    if(!(check)) { \
        SPG_ERROR("Debug assertion failed. File: {}, Line {}", __FILE__, __LINE__); \
        SPG_DEBUGBREAK(); \
    }
#else
  #define SPG_ASSERT(check)
#endif





