#pragma once
//Ref: chantGPT - 'Architecture macro comparison'


// ============================================================
//  Architecture detection — MSVC, GCC, and Clang compatible
// ============================================================

#define ARCH_X86_32   0
#define ARCH_X64      0
#define ARCH_ARM32    0
#define ARCH_ARM64    0
#define ARCH_MIPS     0
#define ARCH_PPC      0
#define ARCH_RISCV    0
#define ARCH_UNKNOWN  0

// ============================================================
//  MSVC
// ============================================================
#if defined(_M_X64)
#  undef  ARCH_X64
#  define ARCH_X64 1
#elif defined(_M_IX86)
#  undef  ARCH_X86_32
#  define ARCH_X86_32 1
#elif defined(_M_ARM64)
#  undef  ARCH_ARM64
#  define ARCH_ARM64 1
#elif defined(_M_ARM)
#  undef  ARCH_ARM32
#  define ARCH_ARM32 1
#elif defined(_M_MIPS)
#  undef  ARCH_MIPS
#  define ARCH_MIPS 1
#elif defined(_M_PPC)
#  undef  ARCH_PPC
#  define ARCH_PPC 1
#else

// ============================================================
//  GCC / Clang
// ============================================================
#  if defined(__x86_64__) || defined(__amd64__)
#    undef  ARCH_X64
#    define ARCH_X64 1
#  elif defined(__i386__)
#    undef  ARCH_X86_32
#    define ARCH_X86_32 1
#  elif defined(__aarch64__)
#    undef  ARCH_ARM64
#    define ARCH_ARM64 1
#  elif defined(__arm__)
#    undef  ARCH_ARM32
#    define ARCH_ARM32 1
#  elif defined(__mips__)
#    undef  ARCH_MIPS
#    define ARCH_MIPS 1
#  elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)
#    undef  ARCH_PPC
#    define ARCH_PPC 1
#  elif defined(__riscv)
#    undef  ARCH_RISCV
#    define ARCH_RISCV 1
#  else
#    undef  ARCH_UNKNOWN
#    define ARCH_UNKNOWN 1
#  endif
#endif

// ============================================================
//  Convenience macros
// ============================================================
#if ARCH_X64 || ARCH_ARM64 || ARCH_RISCV
#  define ARCH_64BIT 1
#else
#  define ARCH_64BIT 0
#endif

#if ARCH_X86_32 || ARCH_ARM32 || ARCH_MIPS || ARCH_PPC
#  define ARCH_32BIT 1
#else
#  define ARCH_32BIT 0
#endif

// ============================================================
//  Optional: human-readable name
// ============================================================
#if ARCH_X64
#  define ARCH_NAME "x64"
#elif ARCH_X86_32
#  define ARCH_NAME "x86"
#elif ARCH_ARM64
#  define ARCH_NAME "ARM64"
#elif ARCH_ARM32
#  define ARCH_NAME "ARM32"
#elif ARCH_MIPS
#  define ARCH_NAME "MIPS"
#elif ARCH_PPC
#  define ARCH_NAME "PowerPC"
#elif ARCH_RISCV
#  define ARCH_NAME "RISC-V"
#else
#  define ARCH_NAME "Unknown"
#endif

