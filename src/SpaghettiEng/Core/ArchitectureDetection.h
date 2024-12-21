
//TODO - do this in cmake if possible.  Is this even needed?  See ChatGPT "Cross-platform OpenGL CMake setup"
#pragma once

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


