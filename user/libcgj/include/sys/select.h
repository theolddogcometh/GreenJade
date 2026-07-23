/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <sys/select.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * fd_set, FD_* macros, select/pselect. Often implemented via poll in libcgj
 * while preserving the classic API for older code.
 *
 * Design notes
 * ------------
 * FD_SETSIZE and bit layout target Linux glibc compatibility for fortify
 * __fdelt_chk. Prefer poll/epoll for new code with large fd counts.
 *
 * Non-goals
 * ---------
 * Efficient O(1) readiness for thousands of fds (use epoll).
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FD_SETSIZE 1024

typedef struct {
    unsigned long fds_bits[(FD_SETSIZE + 63) / 64];
} fd_set;

#define FD_ZERO(p)                                                             \
    do {                                                                       \
        size_t _i;                                                             \
        for (_i = 0; _i < sizeof(*(p)) / sizeof(unsigned long); _i++)          \
            (p)->fds_bits[_i] = 0;                                             \
    } while (0)
#define FD_SET(fd, p)   ((p)->fds_bits[(unsigned)(fd) / 64] |= (1UL << ((unsigned)(fd) % 64)))
#define FD_CLR(fd, p)   ((p)->fds_bits[(unsigned)(fd) / 64] &= ~(1UL << ((unsigned)(fd) % 64)))
#define FD_ISSET(fd, p) (((p)->fds_bits[(unsigned)(fd) / 64] & (1UL << ((unsigned)(fd) % 64))) != 0)

int select(int nFds, fd_set *pRead, fd_set *pWrite, fd_set *pExcept,
           struct timeval *pTv);
int pselect(int nFds, fd_set *pRead, fd_set *pWrite, fd_set *pExcept,
            const struct timespec *pTs, const void *pSigmask);

#ifdef __cplusplus
}
#endif
