/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room sys/seccomp.h (seccomp graph node).
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SECCOMP_SET_MODE_STRICT  0
#define SECCOMP_SET_MODE_FILTER  1
#define SECCOMP_GET_ACTION_AVAIL 2
#define SECCOMP_GET_NOTIF_SIZES  3

#define SECCOMP_FILTER_FLAG_TSYNC 1

int seccomp(unsigned int uOp, unsigned int uFlags, void *pArgs);

#ifdef __cplusplus
}
#endif
