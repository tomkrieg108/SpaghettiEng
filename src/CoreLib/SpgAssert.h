# pragma once

#include "CoreLib/Logger.h"

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

#ifdef SPG_ENABLE_ASSERTS
  #define SPG_ASSERT(check) \
    if(!(check)) { \
        SPG_ERROR("Debug assertion failed. File: {}, Line {}", __FILE__, __LINE__); \
        SPG_DEBUGBREAK(); \
    }
#else
  #define SPG_ASSERT(check)
#endif