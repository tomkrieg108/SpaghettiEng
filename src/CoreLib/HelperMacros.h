#pragma once

// ============================================================
//  Helper macros
// ============================================================
#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

#define CONCAT(a, b) a##b

#if defined(_MSC_VER)
    #define DO_PRAGMA(x) __pragma(x)
#elif defined(__GNUC__) || defined(__clang__)
    #define DO_PRAGMA(x) _Pragma(#x)
#else
    #define DO_PRAGMA(x)
#endif

#define TODO(msg) \
    DO_PRAGMA(message(__FILE__ "(" STRINGIFY(__LINE__) "): TODO: " msg))

#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)
#define COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N

#define BIT(x) (1 << x)