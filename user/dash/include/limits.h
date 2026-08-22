/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding <limits.h> so dash does not pull host glibc features.h
 * (which would define __GLIBC__ and take glibc-only dash paths).
 * Dual MIT OR Apache-2.0.
 */
#ifndef DASH_GJ_LIMITS_H
#define DASH_GJ_LIMITS_H

#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 255
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#define SHRT_MIN (-32768)
#define SHRT_MAX 32767
#define USHRT_MAX 65535
#define INT_MIN (-2147483647 - 1)
#define INT_MAX 2147483647
#define UINT_MAX 4294967295U
#define LONG_MIN (-9223372036854775807L - 1)
#define LONG_MAX 9223372036854775807L
#define ULONG_MAX 18446744073709551615UL
#define LLONG_MIN (-9223372036854775807LL - 1)
#define LLONG_MAX 9223372036854775807LL
#define ULLONG_MAX 18446744073709551615ULL

#ifndef MB_LEN_MAX
#define MB_LEN_MAX 16
#endif
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#ifndef NAME_MAX
#define NAME_MAX 255
#endif
#ifndef PIPE_BUF
#define PIPE_BUF 4096
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX LONG_MAX
#endif

#endif /* DASH_GJ_LIMITS_H */
