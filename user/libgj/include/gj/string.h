/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding libc-shaped string/memory helpers (libgj).
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- length / compare / copy ---- */
size_t gj_strlen(const char *sz);
size_t gj_strnlen(const char *sz, size_t cbMax);
int    gj_strcmp(const char *szA, const char *szB);
int    gj_strncmp(const char *szA, const char *szB, size_t cb);
char  *gj_strcpy(char *szDst, const char *szSrc);
char  *gj_strncpy(char *szDst, const char *szSrc, size_t cb);
char  *gj_strcat(char *szDst, const char *szSrc);
char  *gj_strncat(char *szDst, const char *szSrc, size_t cb);
/** BSD-style: always NUL-terminates when cbDst > 0; returns strlen(src). */
size_t gj_strlcpy(char *szDst, const char *szSrc, size_t cbDst);
/** BSD-style: append; returns strlen(src) + strlen(initial dst). */
size_t gj_strlcat(char *szDst, const char *szSrc, size_t cbDst);

/* ---- search ---- */
char  *gj_strchr(const char *sz, int ch);
char  *gj_strrchr(const char *sz, int ch);
char  *gj_strstr(const char *szHay, const char *szNeedle);
void  *gj_memchr(const void *p, int ch, size_t cb);

/* ---- memory ---- */
void  *gj_memcpy(void *pDst, const void *pSrc, size_t cb);
void  *gj_memmove(void *pDst, const void *pSrc, size_t cb);
void  *gj_memset(void *pDst, int c, size_t cb);
int    gj_memcmp(const void *pA, const void *pB, size_t cb);

/* ---- I/O / format (freestanding product path) ---- */
/** Write NUL-terminated string to fd 1 (stdout) via Linux write. */
void   gj_puts(const char *sz);
/** Write to fd via Linux write; returns bytes written or negative errno. */
long   gj_write(int nFd, const void *p, size_t cb);
/** SYS_DEBUG_LOG of a NUL-terminated string (native door log path). */
long   gj_dlog(const char *sz);

/** Decimal format of signed long into buffer (NUL-terminated). Returns length. */
size_t gj_itoa(long i64Val, char *szBuf, size_t cbBuf);
/** Decimal format of unsigned long. Returns length. */
size_t gj_utoa(unsigned long uVal, char *szBuf, size_t cbBuf);
/**
 * Hex format of unsigned long (lowercase, no 0x prefix).
 * fLeadZero: when non-zero, pad to at least cMin digits with leading zeros.
 * Returns length written (excluding NUL).
 */
size_t gj_xtoa(unsigned long uVal, char *szBuf, size_t cbBuf,
               int fLeadZero, size_t cMin);

#ifdef __cplusplus
}
#endif
