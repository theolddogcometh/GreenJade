/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped utime.h. Not GNU glibc.
 */
#pragma once

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct utimbuf {
    time_t actime;
    time_t modtime;
};

int utime(const char *szPath, const struct utimbuf *pTimes);
int __utime(const char *szPath, const struct utimbuf *pTimes);

#ifdef __cplusplus
}
#endif
