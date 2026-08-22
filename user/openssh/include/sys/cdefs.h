/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/cdefs.h> for OpenSSH-portable (HAVE_SYS_CDEFS_H).
 * Host POSIX compiles need glibc __BEGIN_DECLS / __GNUC_PREREQ.
 * DUT -ffreestanding: do not #include_next glibc (libcgj has no cdefs).
 * Do not #define __unused on DUT: libcgj sys/stat.h uses it as a field.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <sys/cdefs.h>
#endif

#ifdef __cplusplus
# ifndef __BEGIN_DECLS
#  define __BEGIN_DECLS extern "C" {
#  define __END_DECLS   }
# endif
#else
# ifndef __BEGIN_DECLS
#  define __BEGIN_DECLS
#  define __END_DECLS
# endif
#endif

#ifndef __P
# define __P(x) x
#endif

#ifndef __THROW
# ifdef __cplusplus
#  define __THROW throw()
# else
#  define __THROW
# endif
#endif

#ifndef __CONCAT
# define __CONCAT(a, b) a##b
#endif

#ifndef __STRING
# define __STRING(x) #x
#endif

#ifndef __unused
# if __STDC_HOSTED__
#  define __unused __attribute__((__unused__))
# endif
#endif

#ifndef __GNUC_PREREQ
# define __GNUC_PREREQ(maj, min) \
    ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#endif
