/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <wchar.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Wide string (wcs*), wide memory (wmem*), conversion (mbrtowc/wcrtomb…),
 * wide stdio (fwprintf/fwscanf), and open_wmemstream. Complements <stdio.h>
 * byte streams and <uchar.h> char16/32.
 *
 * Design notes
 * ------------
 * C locale / UTF-8 oriented bring-up; MB_CUR_MAX is small. Orientation of
 * FILE (byte vs wide) is tracked in struct gj_cgj_file (nWide).
 *
 * Non-goals
 * ---------
 * Full ISO 10646 locale multibyte state machines for every encoding.
 */
#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <locale.h>

struct tm; /* wcsftime — full def in time.h */

#ifdef __cplusplus
extern "C" {
#endif

/* Linux x86_64: 32-bit wchar_t */
#ifndef __cplusplus
typedef int wchar_t;
#endif

typedef int wint_t;
typedef unsigned int wctype_t;
typedef unsigned int wctrans_t;

#define WEOF ((wint_t)(-1))

/* Restartable conversion state (UTF-8 bring-up) */
typedef struct {
    unsigned int __uCount; /* expected continuation bytes remaining */
    unsigned int __uValue; /* partial codepoint */
} mbstate_t;

/* Orientation: fwide */
#define _IO_BYTE_ORIENTED  (-1)
#define _IO_WIDE_ORIENTED  1

int     fwide(FILE *pF, int nMode);
wint_t  fputwc(wchar_t wc, FILE *pF);
wint_t  putwc(wchar_t wc, FILE *pF);
wint_t  putwchar(wchar_t wc);
wint_t  fgetwc(FILE *pF);
wint_t  getwc(FILE *pF);
wint_t  getwchar(void);
int     fputws(const wchar_t *sz, FILE *pF);
wchar_t *fgetws(wchar_t *sz, int n, FILE *pF);
wint_t  ungetwc(wint_t wc, FILE *pF);

size_t  wcslen(const wchar_t *sz);
int     wcscmp(const wchar_t *a, const wchar_t *b);
int     wcsncmp(const wchar_t *a, const wchar_t *b, size_t c);
wchar_t *wcscpy(wchar_t *pDst, const wchar_t *szSrc);
wchar_t *wcsncpy(wchar_t *pDst, const wchar_t *szSrc, size_t c);
wchar_t *wcscat(wchar_t *pDst, const wchar_t *szSrc);
wchar_t *wcsncat(wchar_t *pDst, const wchar_t *szSrc, size_t c);
size_t  wcsnlen(const wchar_t *sz, size_t cMax);
wchar_t *wcsstr(const wchar_t *szHay, const wchar_t *szNeedle);
wchar_t *wcspbrk(const wchar_t *sz, const wchar_t *szAccept);
size_t  wcsspn(const wchar_t *sz, const wchar_t *szAccept);
size_t  wcscspn(const wchar_t *sz, const wchar_t *szReject);
wchar_t *wcschr(const wchar_t *sz, wchar_t wc);
wchar_t *wcsrchr(const wchar_t *sz, wchar_t wc);
wchar_t *wcsdup(const wchar_t *sz);
int     wcscasecmp(const wchar_t *a, const wchar_t *b);
int     wcsncasecmp(const wchar_t *a, const wchar_t *b, size_t c);
int     wcscoll(const wchar_t *a, const wchar_t *b);
size_t  wcsxfrm(wchar_t *pDst, const wchar_t *szSrc, size_t c);

wchar_t *wmemcpy(wchar_t *pDst, const wchar_t *pSrc, size_t c);
wchar_t *wmemmove(wchar_t *pDst, const wchar_t *pSrc, size_t c);
wchar_t *wmemset(wchar_t *pDst, wchar_t wc, size_t c);
int     wmemcmp(const wchar_t *a, const wchar_t *b, size_t c);
wchar_t *wmemchr(const wchar_t *p, wchar_t wc, size_t c);

/* Multibyte / wide conversion (UTF-8) */
int     mbsinit(const mbstate_t *pSt);
size_t  mbrtowc(wchar_t *pWc, const char *pS, size_t n, mbstate_t *pSt);
size_t  __mbrtowc(wchar_t *pWc, const char *pS, size_t n, mbstate_t *pSt);
size_t  wcrtomb(char *pS, wchar_t wc, mbstate_t *pSt);
size_t  mbsrtowcs(wchar_t *pDst, const char **ppSrc, size_t cLen,
                  mbstate_t *pSt);
size_t  wcsrtombs(char *pDst, const wchar_t **ppSrc, size_t cb,
                  mbstate_t *pSt);
size_t  mbsnrtowcs(wchar_t *pDst, const char **ppSrc, size_t cbSrc,
                   size_t cLen, mbstate_t *pSt);
size_t  __mbsnrtowcs(wchar_t *pDst, const char **ppSrc, size_t nmc, size_t len,
                     mbstate_t *pSt);
size_t  wcsnrtombs(char *pDst, const wchar_t **ppSrc, size_t cSrc, size_t cb,
                   mbstate_t *pSt);
size_t  mbstowcs(wchar_t *pDst, const char *szSrc, size_t cLen);
size_t  wcstombs(char *pDst, const wchar_t *szSrc, size_t cb);

long               wcstol(const wchar_t *sz, wchar_t **ppEnd, int nBase);
unsigned long      wcstoul(const wchar_t *sz, wchar_t **ppEnd, int nBase);
long long          wcstoll(const wchar_t *sz, wchar_t **ppEnd, int nBase);
unsigned long long wcstoull(const wchar_t *sz, wchar_t **ppEnd, int nBase);
wchar_t           *wcstok(wchar_t *sz, const wchar_t *szDelim, wchar_t **ppSave);
size_t             wcsftime(wchar_t *pBuf, size_t cMax, const wchar_t *szFmt,
                            const struct tm *pTm);
int     mbtowc(wchar_t *pWc, const char *pS, size_t n);
int     wctomb(char *pS, wchar_t wc);
int     mblen(const char *pS, size_t n);
wint_t  btowc(int c);
int     wctob(wint_t wc);

/* Wide character classification / case (ASCII-centric bring-up) */
int iswalnum(wint_t wc);
int iswalpha(wint_t wc);
int iswblank(wint_t wc);
int iswcntrl(wint_t wc);
int iswdigit(wint_t wc);
int iswgraph(wint_t wc);
int iswlower(wint_t wc);
int iswprint(wint_t wc);
int iswpunct(wint_t wc);
int iswspace(wint_t wc);
int iswupper(wint_t wc);
int iswxdigit(wint_t wc);
int iswctype(wint_t wc, wctype_t desc);
wctype_t wctype(const char *szProp);
wint_t towlower(wint_t wc);
wint_t towupper(wint_t wc);
/* locale_t wide ctype (C locale bring-up) */
int iswalnum_l(wint_t wc, locale_t loc);
int iswalpha_l(wint_t wc, locale_t loc);
int iswblank_l(wint_t wc, locale_t loc);
int iswcntrl_l(wint_t wc, locale_t loc);
int iswdigit_l(wint_t wc, locale_t loc);
int iswgraph_l(wint_t wc, locale_t loc);
int iswlower_l(wint_t wc, locale_t loc);
int iswprint_l(wint_t wc, locale_t loc);
int iswpunct_l(wint_t wc, locale_t loc);
int iswspace_l(wint_t wc, locale_t loc);
int iswupper_l(wint_t wc, locale_t loc);
int iswxdigit_l(wint_t wc, locale_t loc);
int iswctype_l(wint_t wc, wctype_t desc, locale_t loc);
wint_t towlower_l(wint_t wc, locale_t loc);
wint_t towupper_l(wint_t wc, locale_t loc);
wint_t towctrans(wint_t wc, wctrans_t desc);
wctrans_t wctrans(const char *szProp);

/* Wide printf subset */
int     fwprintf(FILE *pF, const wchar_t *szFmt, ...);
int     wprintf(const wchar_t *szFmt, ...);
int     swprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, ...);
int     vfwprintf(FILE *pF, const wchar_t *szFmt, va_list ap);
int     vwprintf(const wchar_t *szFmt, va_list ap);
int     vswprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, va_list ap);

/* Wide scanf subset */
int     fwscanf(FILE *pF, const wchar_t *szFmt, ...);
int     wscanf(const wchar_t *szFmt, ...);
int     swscanf(const wchar_t *szBuf, const wchar_t *szFmt, ...);
int     vfwscanf(FILE *pF, const wchar_t *szFmt, va_list ap);
int     vwscanf(const wchar_t *szFmt, va_list ap);
int     vswscanf(const wchar_t *szBuf, const wchar_t *szFmt, va_list ap);

FILE   *open_wmemstream(wchar_t **ppBuf, size_t *pcb);

int     wcwidth(wchar_t wc);
int     wcswidth(const wchar_t *sz, size_t n);

size_t  mbrlen(const char *pS, size_t n, mbstate_t *pSt);
wchar_t *wcpcpy(wchar_t *pDst, const wchar_t *szSrc);
wchar_t *wcpncpy(wchar_t *pDst, const wchar_t *szSrc, size_t c);
wchar_t *wmempcpy(wchar_t *pDst, const wchar_t *pSrc, size_t c);

#ifdef __cplusplus
}
#endif
