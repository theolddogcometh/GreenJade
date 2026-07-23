/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room strings.h (BSD/glibc ffs family). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int ffs(int n);
int ffsl(long n);
int ffsll(long long n);
int fls(int n);
int flsl(long n);
int flsll(long long n);

void bzero(void *p, size_t cb);
void bcopy(const void *pSrc, void *pDst, size_t cb);
int  bcmp(const void *pA, const void *pB, size_t cb);
char *index(const char *sz, int ch);
char *rindex(const char *sz, int ch);
void swab(const void *pSrc, void *pDst, ssize_t cb);

/* Also provide strcasecmp surface for apps that include strings.h */
int strcasecmp(const char *szA, const char *szB);
int strncasecmp(const char *szA, const char *szB, size_t n);

#ifdef __cplusplus
}
#endif
