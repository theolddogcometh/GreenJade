/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <sys/types.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Fundamental POSIX/Linux scalar aliases used across unistd, sys/stat, ipc,
 * and sockets: pid_t, uid_t, off_t, mode_t, time_t, etc. on LP64 Linux shape
 * (off_t is long; no separate off64_t split required for product arches).
 *
 * Design notes
 * ------------
 * Prefer these typedefs in public headers over raw int/long so ABI reviews
 * stay greppable. Hungarian: n for ints, cb for sizes elsewhere; type names
 * themselves are not Hungarian (STYLE.md).
 *
 * Non-goals
 * ---------
 * Every BSD/historical alias; add only when a graph needs the symbol.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Process / file / time scalars (LP64 Linux shape) ------------------- */

typedef long          ssize_t;
typedef int           pid_t;
typedef unsigned      uid_t;
typedef unsigned      gid_t;
typedef long          off_t;
typedef unsigned long mode_t;
typedef unsigned long dev_t;
typedef unsigned long ino_t;
typedef unsigned long nlink_t;
typedef long          blksize_t;
typedef long          blkcnt_t;
typedef long          time_t;
typedef long          clock_t;
typedef int           clockid_t;
typedef unsigned long useconds_t;
typedef unsigned      id_t;
typedef int           key_t;

#ifdef __cplusplus
}
#endif
