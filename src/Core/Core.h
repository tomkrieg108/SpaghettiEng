
/*
  Need to be mindful of header-only bloat in Core - since it will be used throughout the project. 
  Template heavy header files here will increase build time a lot

  Keep headers here as lean as possible, use forward dec where possible

*/

/*
  header oder
   - Paired header
   - C system headers
   - C++ stad lib
   - 3rd party libs
   - Project headers

   - <> for external headers, "" for project headers
   - treat statics libs as project headers, not external 
   - Alphabetize
   - Use pathing for project headers, even if in same directory
   - Blank lines to seperate categories
*/


#pragma once

#include "Core/PlatformDetect/ArchDetect.h"
#include "Core/PlatformDetect/CompilerDetect.h"
#include "Core/PlatformDetect/OSDetect.h"
// #include "PlatformDetect/EndianDetect.h

// ============================================================
//  Helper macros
// ============================================================
#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

#define CONCAT(a, b) a##b

#define BIT(x) (1 << x)

// #define TODO(msg) \
//     #pragma message(__FILE__ "(" STRINGIFY(__LINE__) "): TODO: " msg)

// #define SPG_STR(s) #s
// #define SPG_MACRO_STRING_VALUE(S) SPG_STR(s)
// #define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)
// #define COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N

// =============================================================

#include "Logger.h"
//#include "LibCheck.h"

#define SPG_LIB_LINK_CHECK
//#define SPG_CALLBACK_CHECK


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





