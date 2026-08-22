/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <sys/param.h> for libcgj. OpenSSH sshd (HAVE_SYS_PARAM_H).
 * Dual DoD B OPEN. Soft!=product. G-AC-1. No stamp bump.
 */
#pragma once

#include <limits.h>
#include <sys/types.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN PATH_MAX
#endif
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif
#ifndef NBBY
#define NBBY 8
#endif
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef howmany
#define howmany(x, y) (((x) + ((y) - 1)) / (y))
#endif
#ifndef HOWMANY
#define HOWMANY howmany
#endif
#ifndef roundup
#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#endif
#ifndef ALIGNBYTES
#define ALIGNBYTES (sizeof(long) - 1)
#endif
#ifndef ALIGN
#define ALIGN(p) (((unsigned long)(p) + ALIGNBYTES) & ~ALIGNBYTES)
#endif
#ifndef MAXSYMLINKS
#define MAXSYMLINKS 40
#endif
#ifndef NOFILE
#define NOFILE 256
#endif
#ifndef NGROUPS
#define NGROUPS 65536
#endif
