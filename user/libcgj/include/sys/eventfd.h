/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped eventfd (subset). Not GNU glibc.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EFD_SEMAPHORE (1 << 0)
#define EFD_CLOEXEC   02000000
#define EFD_NONBLOCK  04000

int eventfd(unsigned int u32Init, int nFlags);
int eventfd_read(int nFd, uint64_t *pVal);
int eventfd_write(int nFd, uint64_t u64Val);

#ifdef __cplusplus
}
#endif
