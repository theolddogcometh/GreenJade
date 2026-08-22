/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <sys/cdefs.h> for libcgj (HAVE_SYS_CDEFS_H).
 * Do not define __unused: struct stat / utmp use a pad field historically
 * named that way on some trees; keep the attribute macro off.
 * Dual DoD B OPEN.
 */
#pragma once

#ifdef __cplusplus
#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS   }
#endif
#else
#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS
#define __END_DECLS
#endif
#endif

#ifndef __P
#define __P(x) x
#endif

#ifndef __THROW
#ifdef __cplusplus
#define __THROW throw()
#else
#define __THROW
#endif
#endif

#ifndef __CONCAT
#define __CONCAT(a, b) a##b
#endif

#ifndef __STRING
#define __STRING(x) #x
#endif

#ifndef __GNUC_PREREQ
#define __GNUC_PREREQ(maj, min) \
    ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#endif

#ifndef offsetof
#define offsetof(type, member) __builtin_offsetof(type, member)
#endif
