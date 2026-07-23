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
/** Span of chars in set; returns count of leading chars in szAccept. */
size_t gj_strspn(const char *sz, const char *szAccept);
/** Span of chars not in set; returns count of leading chars outside szReject. */
size_t gj_strcspn(const char *sz, const char *szReject);

/* ---- case (ASCII only; freestanding soft) ---- */
int    gj_tolower(int ch);
int    gj_toupper(int ch);
int    gj_strcasecmp(const char *szA, const char *szB);
int    gj_strncasecmp(const char *szA, const char *szB, size_t cb);

/* ---- character class (ASCII; 0/1) ---- */
int    gj_isdigit(int ch);
int    gj_isalpha(int ch);
int    gj_isalnum(int ch);
int    gj_isspace(int ch);
int    gj_isxdigit(int ch);
int    gj_isprint(int ch);

/* ---- memory ---- */
void  *gj_memcpy(void *pDst, const void *pSrc, size_t cb);
void  *gj_memmove(void *pDst, const void *pSrc, size_t cb);
void  *gj_memset(void *pDst, int c, size_t cb);
int    gj_memcmp(const void *pA, const void *pB, size_t cb);
/** Like memcpy but stops after first ch; returns one past last written or NULL. */
void  *gj_memccpy(void *pDst, const void *pSrc, int ch, size_t cb);

/* ---- parse (freestanding; no locale) ---- */
/**
 * Parse signed decimal (optional leading whitespace / +/-).
 * On overflow clamps to LONG_MIN/LONG_MAX-shaped long extremes of host long.
 */
long   gj_atol(const char *sz);
/**
 * Parse unsigned integer. nBase 0 → auto (0x hex / 0 octal / decimal);
 * 2..36 explicit. ppEnd optional; on no convert *ppEnd stays at sz and ret 0.
 */
unsigned long gj_strtoul(const char *sz, char **ppEnd, int nBase);
/** Signed wrapper around gj_strtoul with leading sign. */
long   gj_strtol(const char *sz, char **ppEnd, int nBase);

/* ---- I/O / format (freestanding product path) ---- */
/** Write NUL-terminated string to fd 1 (stdout) via Linux write. */
void   gj_puts(const char *sz);
/** Write to fd via Linux write; returns bytes written or negative errno. */
long   gj_write(int nFd, const void *p, size_t cb);
/** Read from fd via Linux read; returns bytes or negative errno. */
long   gj_read(int nFd, void *p, size_t cb);
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
/**
 * Soft bounded snprintf-lite: supports %% %s %c %d %u %x %p (no width/prec
 * except optional %0N for hex via fLeadZero path is not full printf).
 * %d/%u/%x take long / unsigned long / unsigned long; %p takes void*.
 * Returns bytes that would be written excluding NUL (truncated if needed).
 * Not a full libc printf — freestanding smoke/log helper only.
 */
size_t gj_snprintf(char *szBuf, size_t cbBuf, const char *szFmt, ...);

#ifdef __cplusplus
}
#endif
