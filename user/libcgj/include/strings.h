/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <strings.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * BSD/legacy string helpers: bcopy/bzero/bcmp, index/rindex, ffs/ffsl/ffsll,
 * strcasecmp (also in string.h). Complements <string.h>.
 *
 * Design notes
 * ------------
 * New code should prefer memcpy/memset/strchr. These exist for graphs that
 * still include <strings.h>.
 *
 * Non-goals
 * ---------
 * Locale-aware case folding beyond C locale.
 *
 * Soft residual lean (this unit; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   - BSD/legacy string helpers; C-locale case fold residual only
 *   - Soft!=product: libcgj strings surface != UDX/DDI product close
 *   - product = UDX/DDI + hot/cold Linux ABI; freestanding rtl/USB SKIP
 *   - agent PASS != close Dual DoD; no stamp storms; bar v2026.08.04.75 parent-only
 *   - dual MIT OR Apache-2.0; no GPL; no Linux .ko product AC (G-AC-1)
 * Grep: libcgj strings.h: soft residual lean Soft!=product
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
