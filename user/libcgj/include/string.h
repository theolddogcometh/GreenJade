/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <string.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * C string and memory primitives apps and dynlink graphs expect under the
 * glibc symbol layout: strlen/strcmp/memcpy family, GNU extras (mempcpy,
 * rawmemchr, strcasestr, strverscmp), BSD strlcpy/strlcat, and fortify
 * (__*_chk) nodes for _FORTIFY_SOURCE-shaped binaries.
 *
 * Design notes
 * ------------
 * Parameters use Systems Hungarian (STYLE.md): sz… for C strings, p… for
 * void *, cb… for byte counts. strerror_r here is the GNU form (returns
 * char *); __xpg_strerror_r is the XSI int form for apps that need it.
 *
 * Non-goals
 * ---------
 * Locale-aware collation beyond C-locale strcoll/strxfrm; full multibyte
 * string APIs live in <wchar.h>. Implementations are pure C (no asm required
 * for correctness; hot paths may optimize later).
 *
 * See docs/GLIBC_COMPAT.md for surface status.
 */
#pragma once

#include <stddef.h>
#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Length / compare / copy (ISO C + common POSIX) --------------------- */

size_t  strlen(const char *sz);
size_t  strnlen(const char *sz, size_t nMax);
int     strcmp(const char *szA, const char *szB);
int     strncmp(const char *szA, const char *szB, size_t cb);
char   *strcpy(char *szDst, const char *szSrc);
char   *strncpy(char *szDst, const char *szSrc, size_t cb);
char   *stpcpy(char *szDst, const char *szSrc);   /* returns end of copy */
char   *strcat(char *szDst, const char *szSrc);
char   *strncat(char *szDst, const char *szSrc, size_t cb);
char   *strchr(const char *sz, int ch);
char   *strchrnul(const char *sz, int ch);        /* GNU: end of string if missing */
char   *strrchr(const char *sz, int ch);
char   *strstr(const char *szHay, const char *szNeedle);
char   *strdup(const char *sz);
char   *strndup(const char *sz, size_t n);
size_t  strspn(const char *sz, const char *szAccept);
size_t  strcspn(const char *sz, const char *szReject);
char   *strpbrk(const char *sz, const char *szAccept);
char   *strtok(char *sz, const char *szDelim);
char   *strtok_r(char *sz, const char *szDelim, char **ppSave);
char   *strsep(char **ppString, const char *szDelim); /* BSD; mutates *ppString */

/* ---- Case-insensitive (C locale; *_l take locale_t) --------------------- */

int     strcasecmp(const char *szA, const char *szB);
int     strncasecmp(const char *szA, const char *szB, size_t n);
int     strcasecmp_l(const char *szA, const char *szB, locale_t loc);
int     strncasecmp_l(const char *szA, const char *szB, size_t n,
                      locale_t loc);

/* ---- Path helpers (may mutate input; GNU vs XPG basename) --------------- */

char   *basename(char *szPath);
char   *dirname(char *szPath);
char   *__xpg_basename(char *szPath); /* XPG: never modifies path string */

/* ---- Error / signal text ------------------------------------------------ */

char   *strerror(int nErr);
char   *strsignal(int nSig);
char   *strerror_l(int nErr, locale_t loc);
char   *strsignal_l(int nSig, locale_t loc);
int     __xpg_strerror_r(int nErr, char *szBuf, size_t cb); /* XSI: 0 or errno */
const char *strerrorname_np(int nErr);  /* "EPERM", … or NULL */
const char *strerrordesc_np(int nErr);  /* long description or NULL */
const char *sigabbrev_np(int nSig);     /* "HUP", … or NULL */
const char *sigdescr_np(int nSig);      /* long description or NULL */

/* ---- Memory (byte-oriented; overlap rules as ISO C) --------------------- */

void   *memcpy(void *pDst, const void *pSrc, size_t cb);
void   *memmove(void *pDst, const void *pSrc, size_t cb);
void   *memset(void *pDst, int c, size_t cb);
int     memcmp(const void *pA, const void *pB, size_t cb);
void   *memchr(const void *p, int c, size_t cb);
void   *memmem(const void *pHay, size_t cbHay, const void *pNeedle,
               size_t cbNeedle);
void   *memrchr(const void *p, int c, size_t cb);
void   *memccpy(void *pDst, const void *pSrc, int c, size_t n);
void   *mempcpy(void *pDst, const void *pSrc, size_t cb); /* returns pDst+cb */
void   *__mempcpy(void *pDst, const void *pSrc, size_t cb);
void   *rawmemchr(const void *p, int c);  /* unbounded; UB if c absent */
void   *__rawmemchr(const void *p, int c);
void    explicit_bzero(void *p, size_t cb); /* not elided by optimizer */
void    __bzero(void *p, size_t cb);

/* ---- GNU / internal aliases + light extras ------------------------------ */

char   *__stpcpy(char *szDst, const char *szSrc);
char   *__strdup(const char *sz);
char   *__strndup(const char *sz, size_t n);
int     __strverscmp(const char *szA, const char *szB);
void   *memfrob(void *p, size_t cb); /* reversible obfuscation (not crypto) */
char   *strfry(char *sz);            /* in-place shuffle */
size_t  strlcpy(char *szDst, const char *szSrc, size_t cbDst); /* BSD */
size_t  strlcat(char *szDst, const char *szSrc, size_t cbDst); /* BSD */
char   *stpncpy(char *szDst, const char *szSrc, size_t cb);
char   *strcasestr(const char *szHay, const char *szNeedle);
int     strverscmp(const char *szA, const char *szB); /* version-sort compare */

/*
 * GNU strerror_r: returns a pointer to the message (may be szBuf or static).
 * Not the XSI form that returns int — use __xpg_strerror_r for that ABI.
 */
char   *strerror_r(int nErr, char *szBuf, size_t cb);
int     strcoll(const char *szA, const char *szB); /* C locale ≈ strcmp */
size_t  strxfrm(char *szDst, const char *szSrc, size_t cb);

/* ---- Fortify / _FORTIFY_SOURCE graph nodes ------------------------------ */
/*
 * Compiler-generated calls when fortify is enabled. cbDst is the object size
 * known at the call site; overflow paths call __chk_fail / __fortify_fail.
 */

void    __chk_fail(void);
void    __fortify_fail(const char *szMsg);
long    __fdelt_chk(long d);
long    __fdelt_warn(long d);
void   *__memcpy_chk(void *pDst, const void *pSrc, size_t cb, size_t cbDst);
void   *__memmove_chk(void *pDst, const void *pSrc, size_t cb, size_t cbDst);
void   *__mempcpy_chk(void *pDst, const void *pSrc, size_t cb, size_t cbDst);
void   *__memset_chk(void *pDst, int c, size_t cb, size_t cbDst);
char   *__strcpy_chk(char *szDst, const char *szSrc, size_t cbDst);
char   *__stpcpy_chk(char *szDst, const char *szSrc, size_t cbDst);
char   *__strncpy_chk(char *szDst, const char *szSrc, size_t cb, size_t cbDst);
char   *__strcat_chk(char *szDst, const char *szSrc, size_t cbDst);

#ifdef __cplusplus
}
#endif
