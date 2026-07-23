/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped poll (subset). Not GNU glibc.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POLLIN   0x0001
#define POLLPRI  0x0002
#define POLLOUT  0x0004
#define POLLERR  0x0008
#define POLLHUP  0x0010
#define POLLNVAL 0x0020

struct pollfd {
    int   fd;
    short events;
    short revents;
};

typedef int nfds_t;

struct timespec; /* full def in <time.h> */

int poll(struct pollfd *pFds, nfds_t nFds, int nTimeoutMs);
/* pSigmask is const sigset_t * when <signal.h> is included */
int ppoll(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTs,
          const void *pSigmask);

#ifdef __cplusplus
}
#endif
