/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped ioctl (subset). Not GNU glibc.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int ioctl(int nFd, unsigned long u64Req, ...);

#ifdef __cplusplus
}
#endif
