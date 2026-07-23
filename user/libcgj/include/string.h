/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped string.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t  strlen(const char *sz);
size_t  strnlen(const char *sz, size_t nMax);
int     strcmp(const char *szA, const char *szB);
int     strncmp(const char *szA, const char *szB, size_t cb);
char   *strcpy(char *szDst, const char *szSrc);
char   *strncpy(char *szDst, const char *szSrc, size_t cb);
char   *stpcpy(char *szDst, const char *szSrc);
char   *strcat(char *szDst, const char *szSrc);
char   *strncat(char *szDst, const char *szSrc, size_t cb);
char   *strchr(const char *sz, int ch);
char   *strchrnul(const char *sz, int ch);
char   *strrchr(const char *sz, int ch);
char   *strstr(const char *szHay, const char *szNeedle);
char   *strdup(const char *sz);
char   *strndup(const char *sz, size_t n);
size_t  strspn(const char *sz, const char *szAccept);
size_t  strcspn(const char *sz, const char *szReject);
char   *strpbrk(const char *sz, const char *szAccept);
char   *strtok(char *sz, const char *szDelim);
char   *strtok_r(char *sz, const char *szDelim, char **ppSave);
char   *strsep(char **ppString, const char *szDelim);
int     strcasecmp(const char *szA, const char *szB);
int     strncasecmp(const char *szA, const char *szB, size_t n);
int     strcasecmp_l(const char *szA, const char *szB, locale_t loc);
int     strncasecmp_l(const char *szA, const char *szB, size_t n,
                      locale_t loc);
char   *basename(char *szPath);
char   *dirname(char *szPath);
char   *__xpg_basename(char *szPath);
char   *strerror(int nErr);
char   *strsignal(int nSig);
char   *strerror_l(int nErr, locale_t loc);
char   *strsignal_l(int nSig, locale_t loc);
int     __xpg_strerror_r(int nErr, char *szBuf, size_t cb);
const char *strerrorname_np(int nErr);
const char *strerrordesc_np(int nErr);
const char *sigabbrev_np(int nSig);
const char *sigdescr_np(int nSig);
void   *memcpy(void *pDst, const void *pSrc, size_t cb);
void   *memmove(void *pDst, const void *pSrc, size_t cb);
void   *memset(void *pDst, int c, size_t cb);
int     memcmp(const void *pA, const void *pB, size_t cb);
void   *memchr(const void *p, int c, size_t cb);
void   *memmem(const void *pHay, size_t cbHay, const void *pNeedle,
               size_t cbNeedle);
void   *memrchr(const void *p, int c, size_t cb);
void   *memccpy(void *pDst, const void *pSrc, int c, size_t n);
void   *mempcpy(void *pDst, const void *pSrc, size_t cb);
void   *__mempcpy(void *pDst, const void *pSrc, size_t cb);
void   *rawmemchr(const void *p, int c);
void   *__rawmemchr(const void *p, int c);
void    explicit_bzero(void *p, size_t cb);
void    __bzero(void *p, size_t cb);
char   *__stpcpy(char *szDst, const char *szSrc);
char   *__strdup(const char *sz);
char   *__strndup(const char *sz, size_t n);
int     __strverscmp(const char *szA, const char *szB);
void   *memfrob(void *p, size_t cb);
char   *strfry(char *sz);
size_t  strlcpy(char *szDst, const char *szSrc, size_t cbDst);
size_t  strlcat(char *szDst, const char *szSrc, size_t cbDst);
char   *stpncpy(char *szDst, const char *szSrc, size_t cb);
char   *strcasestr(const char *szHay, const char *szNeedle);
int     strverscmp(const char *szA, const char *szB);
/* GNU strerror_r (returns char *); not the XSI int form */
char   *strerror_r(int nErr, char *szBuf, size_t cb);
int     strcoll(const char *szA, const char *szB);
size_t  strxfrm(char *szDst, const char *szSrc, size_t cb);

/* Fortify / _FORTIFY_SOURCE graph nodes */
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
