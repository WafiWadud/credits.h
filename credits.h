/*
 * MIT License
 *
 * Copyright (c) 2026 Wafi Nashwan Wadud
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * credits.h - Compile-time environment and build information header
 *
 * PURPOSE:
 *   This header file provides compile-time macros that capture and expose
 *   information about the compilation environment, including:
 *   - C language standard version
 *   - POSIX compatibility level
 *   - Target architecture (CPU instruction set)
 *   - Byte order endianness (little/big)
 *   - Operating system platform
 *   - Compilation timestamp
 *   - Compiler version and capabilities
 *
 * DESIGN RATIONALE:
 *   Using compile-time macros instead of runtime functions allows:
 *   1. Zero runtime overhead - all values resolved during preprocessing
 *   2. Linker-time optimization - dead code elimination of unused conditional
 * paths
 *   3. Deterministic output - captures exact build environment at compile time
 *   4. Reduced binary size - no runtime string formatting or library calls
 * needed
 *   5. Embedded environment compatibility - works without standard library
 *   6. Portability - single source compiles correctly on any C99+ compliant
 * platform
 *
 * KEY IMPLEMENTATION DETAILS:
 *   - Preprocessor conditional chains detect C standard via __STDC_VERSION__
 *   - Compiler-defined macros (__linux__, __APPLE__, etc.) identify
 * OS/architecture
 *   - STR/STR2 macros convert numeric version IDs to string literals via
 * stringification
 *   - arrlen() macro calculates array length at compile-time using sizeof
 *   - write() syscall in credits.c provides minimal dependencies (POSIX only)
 *   - Header guard prevents multiple inclusion
 *
 * AUTHOR: Wafi Nashwan Wadud
 * VERSION: 1.0
 * CREATED: 30 January 2026
 *
 * DEPENDENCIES:
 *   - C99 or later (uses __STDC_VERSION__ detection)
 *   - unistd.h (for write syscall and POSIX version detection)
 */

#ifndef TIME_H
#define TIME_H

/**
 * STRING CONCATENATION MACROS
 *
 * These macros convert numeric literals (like __POSIX_VERSION__) into
 * their string representations. The two-level indirection is necessary
 * because the preprocessor's stringification operator (#) must be applied
 * to a macro parameter, not a macro expansion result.
 *
 * Usage: STR(_POSIX_VERSION) converts _POSIX_VERSION value to a string
 */
#define STR2(x) #x
#define STR(x) STR2(x)

/**
 * ARRAY LENGTH CALCULATION MACRO
 *
 * Safely determines the number of elements in a stack-allocated array
 * at compile time. This works by dividing total size by element size.
 *
 * The -1 adjustment in credits.c removes the null terminator.
 */
#define arrlen(x) (sizeof(x) / sizeof((x)[0]))

/**
 * C LANGUAGE STANDARD DETECTION
 *
 * Identifies which C standard the compiler is using based on the
 * __STDC_VERSION__ macro defined by standards-compliant compilers.
 *
 * Standards progression:
 *   - C23 (202311L): Latest, includes <stdbit.h>, _BitInt, etc.
 *   - C17 (201710L): Stable standard with bug fixes
 *   - C11 (201112L): First with _Generic, threads support
 *   - C99 (199901L): Added inline, variable declarations mid-block
 *   - C90/C94 (absent): Legacy versions from 1990s
 *   - Pre-ANSI: Very old compilers without __STDC_VERSION__
 */
#if defined(__STDC_VERSION__)
#if __STDC_VERSION__ >= 202311L
#define C_VERSION "C23"
#elif __STDC_VERSION__ >= 201710L
#define C_VERSION "C17"
#elif __STDC_VERSION__ >= 201112L
#define C_VERSION "C11"
#elif __STDC_VERSION__ >= 199901L
#define C_VERSION "C99"
#else
#define C_VERSION "C90/C94 or Wayyyy in the future"
#endif
#else
#define C_VERSION "pre-ANSI C"
#endif

#include <unistd.h>

/**
 * POSIX COMPLIANCE DETECTION
 *
 * POSIX (Portable Operating System Interface) defines a standard interface
 * for Unix-like systems. _POSIX_VERSION indicates which year the compliance
 * was published (e.g., 200809 = POSIX.1-2008, 201309 = POSIX.1-2013).
 *
 * Compilation environment implications:
 *   - Standard library and syscall availability
 *   - Signal handling and process management
 *   - File I/O semantics and permissions
 */
#if defined(_POSIX_VERSION)
#define POSIX_STR "POSIX compatible (POSIX " STR(_POSIX_VERSION) ")"
#else
#define POSIX_STR "POSIX incompatible"
#endif

/**
 * CPU ARCHITECTURE DETECTION
 *
 * Identifies the instruction set architecture (ISA) for which code
 * is being compiled. Affects:
 *   - Binary compatibility between systems
 *   - Compiler optimizations and code generation
 *   - Available CPU instructions and features
 *   - Register sizes and calling conventions
 *
 * Supported architectures:
 *   - x86_64: 64-bit Intel/AMD, most common server/desktop
 *   - x86: 32-bit Intel/AMD legacy
 *   - AArch64: 64-bit ARM (Apple Silicon, modern phones/tablets)
 *   - ARM: 32-bit ARM legacy (embedded systems)
 *   - RISC-V: Open ISA, emerging embedded systems
 *   - PowerPC: Used in some servers and embedded
 */
#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_STR "x86_64"
#elif defined(__i386__) || defined(_M_IX86)
#define ARCH_STR "x86"
#elif defined(__aarch64__)
#define ARCH_STR "AArch64"
#elif defined(__arm__)
#define ARCH_STR "ARM"
#elif defined(__riscv)
#define ARCH_STR "RISC-V"
#elif defined(__powerpc64__)
#define ARCH_STR "PowerPC64"
#elif defined(__powerpc__)
#define ARCH_STR "PowerPC"
#else
#define ARCH_STR "unknown"
#endif

/**
 * BYTE ORDER (ENDIANNESS) DETECTION
 *
 * Determines whether multi-byte values are stored with the least significant
 * byte first (little-endian) or most significant byte first (big-endian).
 *
 * Significance:
 *   - Network programming (requires big-endian/network byte order)
 *   - Binary file formats and serialization
 *   - Performance optimizations (CPU native order is faster)
 *   - Cross-platform compatibility issues
 *
 * Most modern systems use little-endian (x86, ARM, RISC-V).
 * Network protocols and some embedded systems use big-endian.
 */
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define ENDIAN_STR "little"
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define ENDIAN_STR "big"
#else
#define ENDIAN_STR "unknown"
#endif
#else
#define ENDIAN_STR "unknown"
#endif

/**
 * OPERATING SYSTEM DETECTION
 *
 * Identifies the target operating system kernel. Each OS provides different:
 *   - System calls and library APIs
 *   - Filesystem structure and permissions
 *   - Process and threading models
 *   - Memory management capabilities
 *   - Scheduling and interrupt handling
 *
 * Supported systems:
 *   - Linux: Open-source Unix-like, most servers
 *   - macOS: Apple's Unix-based OS
 *   - *BSD variants: Conservative Unix derivatives
 *   - Windows: Proprietary OS with different paradigms
 */
#if defined(__linux__)
#define OS_STR "Linux"
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_STR "macOS"
#elif defined(__FreeBSD__)
#define OS_STR "FreeBSD"
#elif defined(__NetBSD__)
#define OS_STR "NetBSD"
#elif defined(__OpenBSD__)
#define OS_STR "OpenBSD"
#elif defined(_WIN32)
#define OS_STR "Windows"
#else
#define OS_STR "unknown"
#endif

/**
 * CREDITS OUTPUT MACRO
 *
 * Composes a multi-line string literal containing build information.
 * Uses line continuation with backslashes for readability.
 * Tweak and edit as needed.
 *
 * Output format:
 *   Made by Wafi Nashwan Wadud.
 *
 *   Compiled from [source_file] on [date] at [time]
 *   using [POSIX_version] [compiler_version] with [C_standard]
 *   for [endianness]-endian [architecture] [operating_system].
 *
 * Built-in preprocessor macros used:
 *   - __FILE__: Source file path
 *   - __DATE__: Compilation date (e.g., "Jan 30 2026")
 *   - __TIME__: Compilation time (e.g., "14:23:45")
 *   - __VERSION__: Compiler version string
 */
#define CREDITS                                                                \
  "Made by Wafi Nashwan Wadud.\n\n"                                            \
  "Compiled from " __FILE__ " on " __DATE__ " at " __TIME__ "\n"               \
  "using " POSIX_STR " " __VERSION__ " with " C_VERSION " standard\n"          \
  "for " ENDIAN_STR "-endian " ARCH_STR " " OS_STR ".\n"

#endif // TIME_H
