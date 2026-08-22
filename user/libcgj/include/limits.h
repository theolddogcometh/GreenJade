/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <limits.h> extras on top of the compiler header.
 * OpenSSH sshd wants PATH_MAX / HOST_NAME_MAX. Dual DoD B OPEN.
 */
#pragma once

#include_next <limits.h>

#ifndef MB_LEN_MAX
#define MB_LEN_MAX 16
#endif
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#ifndef NAME_MAX
#define NAME_MAX 255
#endif
#ifndef NGROUPS_MAX
#define NGROUPS_MAX 65536
#endif
#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 256
#endif
#ifndef _POSIX_HOST_NAME_MAX
#define _POSIX_HOST_NAME_MAX 255
#endif
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif
#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 256
#endif
#ifndef TTY_NAME_MAX
#define TTY_NAME_MAX 32
#endif
#ifndef IOV_MAX
#define IOV_MAX 1024
#endif
#ifndef NZERO
#define NZERO 20
#endif
#ifndef PIPE_BUF
#define PIPE_BUF 4096
#endif
#ifndef MAXSYMLINKS
#define MAXSYMLINKS 40
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX LONG_MAX
#endif
#ifndef SIZE_MAX
#define SIZE_MAX ULONG_MAX
#endif
#ifndef SIZE_T_MAX
#define SIZE_T_MAX SIZE_MAX
#endif
