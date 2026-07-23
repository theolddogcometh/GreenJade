/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/types.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
