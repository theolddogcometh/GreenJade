/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/timeb.h (ftime). Not GNU glibc.
 */
#pragma once

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timeb {
    time_t         time;
    unsigned short millitm;
    short          timezone;
    short          dstflag;
};

int ftime(struct timeb *pTb);

#ifdef __cplusplus
}
#endif
