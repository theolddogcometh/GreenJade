/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <limits.h> for OpenSSH-portable (HAVE_LIMITS_H).
 * Host POSIX compiles (#include_next gcc include-fixed / glibc).
 */
#pragma once
#include_next <limits.h>

#ifndef MB_LEN_MAX
# define MB_LEN_MAX 16
#endif
#ifndef PATH_MAX
# define PATH_MAX 4096
#endif
#ifndef NAME_MAX
# define NAME_MAX 255
#endif
#ifndef NGROUPS_MAX
# define NGROUPS_MAX 65536
#endif
#ifndef _POSIX_PATH_MAX
# define _POSIX_PATH_MAX 256
#endif
#ifndef _POSIX_HOST_NAME_MAX
# define _POSIX_HOST_NAME_MAX 255
#endif
#ifndef HOST_NAME_MAX
# define HOST_NAME_MAX 64
#endif
#ifndef LOGIN_NAME_MAX
# define LOGIN_NAME_MAX 256
#endif
#ifndef _XOPEN_IOV_MAX
# define _XOPEN_IOV_MAX 16
#endif
#ifndef IOV_MAX
# define IOV_MAX 1024
#endif
#ifndef PIPE_BUF
# define PIPE_BUF 4096
#endif
#ifndef MAXSYMLINKS
# define MAXSYMLINKS 40
#endif
#ifndef SSIZE_MAX
# define SSIZE_MAX LONG_MAX
#endif
#ifndef SIZE_MAX
# define SIZE_MAX ULONG_MAX
#endif
#ifndef SIZE_T_MAX
# define SIZE_T_MAX SIZE_MAX
#endif
