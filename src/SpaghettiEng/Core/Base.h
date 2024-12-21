#pragma once

#include "ArchitectureDetection.h"
#include "Log.h"

#ifdef SPG_DEBUG
  #define SPG_ENABLE_ASSERTS
  #ifdef _MSC_VER
    #define SPG_DEBUGBREAK() __debugbreak()
  #elif defined(__GNUC__) || defined(__clang__)
    //__GNUC__ is defined for GCC, MinGW, Cygwin)
    #define SPG_DEBUGBREAK() __builtin_trap()
    //#include <csignal>
    //#define SPG_DEBUGBREAK() raise(SIGTRAP)
  #else
    #error "Platform does not support debugbreak just yet"
  #endif
#endif

#define SPG_STR(s) #s
#define SPG_MACRO_STRING_VALUE(S) SPG_STR(s)

#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)
#define COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N

#define BIT(x) (1 << x)

#ifdef SPG_ENABLE_ASSERTS
  #define SPG_ASSERT(check) \
    if(!(check)) { \
        LOG_ERROR("Debug assertion failed. File: {}, Line {}", __FILE__, __LINE__); \
        SPG_DEBUGBREAK(); \
    }
#else
  #define SPG_ASSERT(check)
#endif // ENABLE_ASSERTS



