/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/param.h> for OpenSSH-portable (HAVE_SYS_PARAM_H).
 * Host POSIX compiles (#include_next glibc). DUT: libcgj has no param.h.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <sys/param.h>
#else
# include <limits.h>
#endif

#ifndef MAXPATHLEN
# define MAXPATHLEN PATH_MAX
#endif
#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN 64
#endif
#ifndef NBBY
# define NBBY 8
#endif
#ifndef MIN
# define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
# define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef howmany
# define howmany(x, y) (((x) + ((y) - 1)) / (y))
#endif
#ifndef roundup
# define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#endif
#ifndef ALIGNBYTES
# define ALIGNBYTES (sizeof(long) - 1)
#endif
#ifndef ALIGN
# define ALIGN(p) (((unsigned long)(p) + ALIGNBYTES) & ~ALIGNBYTES)
#endif
#ifndef MAXSYMLINKS
# define MAXSYMLINKS 40
#endif
#ifndef NOFILE
# define NOFILE 256
#endif
