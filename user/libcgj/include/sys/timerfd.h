/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/timerfd.h (subset). Not GNU glibc.
 */
#pragma once

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TFD_CLOEXEC             02000000
#define TFD_NONBLOCK            04000
#define TFD_TIMER_ABSTIME       (1 << 0)
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)

int timerfd_create(int nClockid, int nFlags);
int timerfd_settime(int nFd, int nFlags, const struct itimerspec *pNew,
                    struct itimerspec *pOld);
int timerfd_gettime(int nFd, struct itimerspec *pCur);

#ifdef __cplusplus
}
#endif
