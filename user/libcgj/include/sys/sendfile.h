/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/sendfile.h (subset). Not GNU glibc.
 */
#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t sendfile(int nOutFd, int nInFd, off_t *pOffset, size_t cbCount);

#ifdef __cplusplus
}
#endif
