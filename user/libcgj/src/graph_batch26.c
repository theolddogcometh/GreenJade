/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26: missing underscored string/locale/tree
 * aliases, fortify wide mempcpy/wcpcpy, NSS lookup/hash/files data,
 * file-change detection stubs, stdio underflow hooks, write_nocancel,
 * cpuid feature leaf. Integer/pointer only (no SSE doubles).
 */
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

char *strtok_r(char *sz, const char *szDelim, char **ppSave);
long strtol_l(const char *sz, char **ppEnd, int nBase, locale_t loc);
unsigned long strtoul_l(const char *sz, char **ppEnd, int nBase, locale_t loc);
long long strtoll_l(const char *sz, char **ppEnd, int nBase, locale_t loc);
unsigned long long strtoull_l(const char *sz, char **ppEnd, int nBase,
                              locale_t loc);
long wcstol(const wchar_t *sz, wchar_t **ppEnd, int nBase);
unsigned long wcstoul(const wchar_t *sz, wchar_t **ppEnd, int nBase);
long long wcstoll(const wchar_t *sz, wchar_t **ppEnd, int nBase);
unsigned long long wcstoull(const wchar_t *sz, wchar_t **ppEnd, int nBase);
size_t wcsftime(wchar_t *pBuf, size_t cMax, const wchar_t *szFmt,
                const struct tm *pTm);
int wcscasecmp(const wchar_t *a, const wchar_t *b);
int wcsncasecmp(const wchar_t *a, const wchar_t *b, size_t c);
wctype_t wctype(const char *szProp);
wctrans_t wctrans(const char *szProp);
wint_t towctrans(wint_t wc, wctrans_t desc);
wchar_t *wcpcpy(wchar_t *pDst, const wchar_t *pSrc);
wchar_t *wcpncpy(wchar_t *pDst, const wchar_t *pSrc, size_t c);
wchar_t *wmempcpy(wchar_t *pDst, const wchar_t *pSrc, size_t c);
void *tdelete(const void *pKey, void **ppRoot,
              int (*fnCmp)(const void *, const void *));
void *tfind(const void *pKey, void *const *ppRoot,
            int (*fnCmp)(const void *, const void *));
void *tsearch(const void *pKey, void **ppRoot,
              int (*fnCmp)(const void *, const void *));
void twalk(const void *pRoot, void (*fnAction)(const void *, int, int));
void twalk_r(const void *pRoot,
             void (*fnAction)(const void *, int, void *), void *pClosure);
void __chk_fail(void);
int vfscanf(FILE *pF, const char *szFmt, va_list ap);
extern char **environ;

/* ---- small-string / strtok internals ------------------------------------ */

size_t
__strspn_c1(const char *sz, char c0)
{
    char a[2];

    a[0] = c0;
    a[1] = '\0';
    return strspn(sz, a);
}

size_t
__strspn_c2(const char *sz, char c0, char c1)
{
    char a[3];

    a[0] = c0;
    a[1] = c1;
    a[2] = '\0';
    return strspn(sz, a);
}

size_t
__strspn_c3(const char *sz, char c0, char c1, char c2)
{
    char a[4];

    a[0] = c0;
    a[1] = c1;
    a[2] = c2;
    a[3] = '\0';
    return strspn(sz, a);
}

char *
__strtok_r(char *sz, const char *szDelim, char **ppSave)
{
    return strtok_r(sz, szDelim, ppSave);
}

char *
__strtok_r_1c(char *sz, char cDelim, char **ppSave)
{
    char aDelim[2];

    aDelim[0] = cDelim;
    aDelim[1] = '\0';
    return strtok_r(sz, aDelim, ppSave);
}

/* ---- ctype / locale underscored ----------------------------------------- */

int
_tolower(int c)
{
    return tolower(c);
}

int
_toupper(int c)
{
    return toupper(c);
}

long
__strtol_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    return strtol_l(sz, ppEnd, nBase, loc);
}

unsigned long
__strtoul_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    return strtoul_l(sz, ppEnd, nBase, loc);
}

long long
__strtoll_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    return strtoll_l(sz, ppEnd, nBase, loc);
}

unsigned long long
__strtoull_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    return strtoull_l(sz, ppEnd, nBase, loc);
}

ssize_t
__strfmon_l(char *sz, size_t cbMax, locale_t loc, const char *szFmt, ...)
{
    size_t i;

    (void)loc;
    /* Varargs consumed only to match ABI; literal fmt copy (no FP). */
    {
        va_list ap;

        va_start(ap, szFmt);
        (void)ap;
        va_end(ap);
    }
    if (sz == NULL || szFmt == NULL || cbMax == 0) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i + 1 < cbMax && szFmt[i] != '\0'; i++) {
        sz[i] = szFmt[i];
    }
    sz[i] = '\0';
    return (ssize_t)i;
}

/* ---- tree underscored aliases ------------------------------------------- */

void *
__tdelete(const void *pKey, void **ppRoot,
          int (*fnCmp)(const void *, const void *))
{
    return tdelete(pKey, ppRoot, fnCmp);
}

void *
__tfind(const void *pKey, void *const *ppRoot,
        int (*fnCmp)(const void *, const void *))
{
    return tfind(pKey, ppRoot, fnCmp);
}

void *
__tsearch(const void *pKey, void **ppRoot,
          int (*fnCmp)(const void *, const void *))
{
    return tsearch(pKey, ppRoot, fnCmp);
}

void
__twalk(const void *pRoot, void (*fnAction)(const void *, int, int))
{
    twalk(pRoot, fnAction);
}

void
__twalk_r(const void *pRoot, void (*fnAction)(const void *, int, void *),
          void *pClosure)
{
    twalk_r(pRoot, fnAction, pClosure);
}

/* ---- wide ctype / convert / fortify ------------------------------------- */

wint_t
__towctrans(wint_t wc, wctrans_t desc)
{
    return towctrans(wc, desc);
}

wint_t
__towctrans_l(wint_t wc, wctrans_t desc, locale_t loc)
{
    (void)loc;
    return towctrans(wc, desc);
}

wctrans_t
__wctrans_l(const char *szProp, locale_t loc)
{
    (void)loc;
    return wctrans(szProp);
}

wctype_t
__wctype_l(const char *szProp, locale_t loc)
{
    (void)loc;
    return wctype(szProp);
}

int
__wcscasecmp_l(const wchar_t *a, const wchar_t *b, locale_t loc)
{
    (void)loc;
    return wcscasecmp(a, b);
}

int
__wcsncasecmp_l(const wchar_t *a, const wchar_t *b, size_t c, locale_t loc)
{
    (void)loc;
    return wcsncasecmp(a, b, c);
}

size_t
__wcsftime_l(wchar_t *pBuf, size_t cMax, const wchar_t *szFmt,
             const struct tm *pTm, locale_t loc)
{
    (void)loc;
    return wcsftime(pBuf, cMax, szFmt, pTm);
}

long
__wcstol_l(const wchar_t *sz, wchar_t **ppEnd, int nBase, locale_t loc)
{
    (void)loc;
    return wcstol(sz, ppEnd, nBase);
}

unsigned long
__wcstoul_l(const wchar_t *sz, wchar_t **ppEnd, int nBase, locale_t loc)
{
    (void)loc;
    return wcstoul(sz, ppEnd, nBase);
}

long long
__wcstoll_l(const wchar_t *sz, wchar_t **ppEnd, int nBase, locale_t loc)
{
    (void)loc;
    return wcstoll(sz, ppEnd, nBase);
}

unsigned long long
__wcstoull_l(const wchar_t *sz, wchar_t **ppEnd, int nBase, locale_t loc)
{
    (void)loc;
    return wcstoull(sz, ppEnd, nBase);
}

long
__wcstol_internal(const wchar_t *sz, wchar_t **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return wcstol(sz, ppEnd, nBase);
}

unsigned long
__wcstoul_internal(const wchar_t *sz, wchar_t **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return wcstoul(sz, ppEnd, nBase);
}

long long
__wcstoll_internal(const wchar_t *sz, wchar_t **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return wcstoll(sz, ppEnd, nBase);
}

unsigned long long
__wcstoull_internal(const wchar_t *sz, wchar_t **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return wcstoull(sz, ppEnd, nBase);
}

wchar_t *
__wcpcpy_chk(wchar_t *pDst, const wchar_t *pSrc, size_t cDst)
{
    size_t n;

    if (pDst == NULL || pSrc == NULL) {
        return NULL;
    }
    n = 0;
    while (pSrc[n] != L'\0') {
        n++;
    }
    if (cDst != (size_t)-1 && n + 1 > cDst) {
        __chk_fail();
    }
    return wcpcpy(pDst, pSrc);
}

wchar_t *
__wcpncpy_chk(wchar_t *pDst, const wchar_t *pSrc, size_t c, size_t cDst)
{
    if (pDst == NULL || pSrc == NULL) {
        return NULL;
    }
    if (cDst != (size_t)-1 && c > cDst) {
        __chk_fail();
    }
    return wcpncpy(pDst, pSrc, c);
}

wchar_t *
__wmempcpy_chk(wchar_t *pDst, const wchar_t *pSrc, size_t c, size_t cDst)
{
    if (pDst == NULL || pSrc == NULL) {
        return NULL;
    }
    if (cDst != (size_t)-1 && c > cDst) {
        __chk_fail();
    }
    return wmempcpy(pDst, pSrc, c);
}

/* ---- stdio / I/O hooks -------------------------------------------------- */

int
__vfscanf(FILE *pF, const char *szFmt, va_list ap)
{
    return vfscanf(pF, szFmt, ap);
}

int
__underflow(FILE *pF)
{
    (void)pF;
    return EOF;
}

int
__woverflow(void *pF, int nCh)
{
    (void)pF;
    (void)nCh;
    return EOF;
}

wint_t
__wuflow(void *pF)
{
    (void)pF;
    return (wint_t)WEOF;
}

wint_t
__wunderflow(void *pF)
{
    (void)pF;
    return (wint_t)WEOF;
}

ssize_t
__write_nocancel(int nFd, const void *pBuf, size_t cb)
{
    return (ssize_t)write(nFd, pBuf, cb);
}

/* ---- NSS hash / lookup / files data ------------------------------------- */

uint32_t
__nss_hash(const void *pKey, size_t cb)
{
    const unsigned char *p = (const unsigned char *)pKey;
    uint32_t u = 5381u;
    size_t i;

    if (p == NULL) {
        return 0;
    }
    for (i = 0; i < cb; i++) {
        u = ((u << 5) + u) + (uint32_t)p[i];
    }
    return u;
}

int
__nss_lookup(void **ppNi, void **ppFct, const char *szFct)
{
    (void)szFct;
    if (ppNi != NULL) {
        *ppNi = NULL;
    }
    if (ppFct != NULL) {
        *ppFct = NULL;
    }
    return 0; /* not found */
}

int
__nss_passwd_lookup(void **ppNi, void **ppFct)
{
    return __nss_lookup(ppNi, ppFct, "getpwnam_r");
}

int
__nss_group_lookup(void **ppNi, void **ppFct)
{
    return __nss_lookup(ppNi, ppFct, "getgrnam_r");
}

int
__nss_hosts_lookup(void **ppNi, void **ppFct)
{
    return __nss_lookup(ppNi, ppFct, "gethostbyname_r");
}

/* Opaque files-data cookie — fail-closed stream surface */
struct b26_nss_files_data {
    FILE *pF;
    int fOpen;
};

int
__nss_files_data_open(struct b26_nss_files_data *pData, const char *szPath)
{
    if (pData == NULL || szPath == NULL) {
        errno = EINVAL;
        return -1;
    }
    pData->pF = fopen(szPath, "r");
    pData->fOpen = (pData->pF != NULL) ? 1 : 0;
    if (!pData->fOpen) {
        return -1;
    }
    return 0;
}

void
__nss_files_data_put(struct b26_nss_files_data *pData)
{
    (void)pData;
}

void
__nss_files_data_setent(struct b26_nss_files_data *pData)
{
    if (pData != NULL && pData->pF != NULL) {
        (void)fseek(pData->pF, 0, SEEK_SET);
    }
}

void
__nss_files_data_endent(struct b26_nss_files_data *pData)
{
    if (pData != NULL && pData->pF != NULL) {
        (void)fclose(pData->pF);
        pData->pF = NULL;
        pData->fOpen = 0;
    }
}

FILE *
__nss_files_fopen(const char *szPath)
{
    if (szPath == NULL) {
        errno = EINVAL;
        return NULL;
    }
    return fopen(szPath, "r");
}

ssize_t
__nss_readline(FILE *pF, char *szBuf, size_t cb)
{
    char *p;

    if (pF == NULL || szBuf == NULL || cb == 0) {
        errno = EINVAL;
        return -1;
    }
    p = fgets(szBuf, (int)cb, pF);
    if (p == NULL) {
        return -1;
    }
    return (ssize_t)strlen(szBuf);
}

int
__nss_parse_line_result(char *szLine, char **ppFields, size_t nFields,
                        char cDelim)
{
    size_t i;
    char *p;

    if (szLine == NULL || ppFields == NULL || nFields == 0) {
        return -1;
    }
    p = szLine;
    for (i = 0; i < nFields; i++) {
        ppFields[i] = p;
        while (*p != '\0' && *p != cDelim && *p != '\n') {
            p++;
        }
        if (*p == cDelim) {
            *p++ = '\0';
        } else if (*p == '\n') {
            *p = '\0';
            i++;
            break;
        } else {
            i++;
            break;
        }
    }
    while (i < nFields) {
        ppFields[i++] = NULL;
    }
    return 0;
}

/* netgroup internal stream — fail-closed */
int
__internal_setnetgrent(const char *szNetgroup, void *pState)
{
    (void)szNetgroup;
    (void)pState;
    errno = ENOENT;
    return 0;
}

int
__internal_getnetgrent_r(char **ppHost, char **ppUser, char **ppDomain,
                         void *pState, char *szBuf, size_t cb)
{
    (void)ppHost;
    (void)ppUser;
    (void)ppDomain;
    (void)pState;
    (void)szBuf;
    (void)cb;
    return 0;
}

void
__internal_endnetgrent(void *pState)
{
    (void)pState;
}

/* ---- file change detection (glibc internal shape) ----------------------- */

struct b26_file_change {
    long aMtim[2]; /* matches struct stat st_mtim[2] on this ABI */
    off_t cbSize;
    int fValid;
};

void
__file_change_detection_for_stat(struct b26_file_change *pFc,
                                 const struct stat *pSt)
{
    if (pFc == NULL) {
        return;
    }
    if (pSt == NULL) {
        pFc->fValid = 0;
        return;
    }
    pFc->cbSize = pSt->st_size;
    pFc->aMtim[0] = pSt->st_mtim[0];
    pFc->aMtim[1] = pSt->st_mtim[1];
    pFc->fValid = 1;
}

void
__file_change_detection_for_path(struct b26_file_change *pFc, const char *szPath)
{
    struct stat st;

    if (pFc == NULL) {
        return;
    }
    if (szPath == NULL || stat(szPath, &st) != 0) {
        pFc->fValid = 0;
        return;
    }
    __file_change_detection_for_stat(pFc, &st);
}

void
__file_change_detection_for_fp(struct b26_file_change *pFc, FILE *pF)
{
    struct stat st;
    int nFd;

    if (pFc == NULL) {
        return;
    }
    if (pF == NULL) {
        pFc->fValid = 0;
        return;
    }
    nFd = fileno(pF);
    if (nFd < 0 || fstat(nFd, &st) != 0) {
        pFc->fValid = 0;
        return;
    }
    __file_change_detection_for_stat(pFc, &st);
}

int
__file_is_unchanged(const struct b26_file_change *pFc, const struct stat *pSt)
{
    if (pFc == NULL || !pFc->fValid || pSt == NULL) {
        return 0;
    }
    if (pFc->cbSize != pSt->st_size) {
        return 0;
    }
    if (pFc->aMtim[0] != pSt->st_mtim[0] || pFc->aMtim[1] != pSt->st_mtim[1]) {
        return 0;
    }
    return 1;
}

/* ---- misc desktop graph nodes ------------------------------------------- */

void
__netlink_assert_response(int nFd, ssize_t nGot)
{
    (void)nFd;
    (void)nGot;
}

/* host-shape feature leaf query — return 0 when unavailable */
int
__x86_get_cpuid_feature_leaf(unsigned int uLeaf, unsigned int uSubleaf,
                             unsigned int *pEax, unsigned int *pEbx,
                             unsigned int *pEcx, unsigned int *pEdx)
{
    (void)uLeaf;
    (void)uSubleaf;
    if (pEax != NULL) {
        *pEax = 0;
    }
    if (pEbx != NULL) {
        *pEbx = 0;
    }
    if (pEcx != NULL) {
        *pEcx = 0;
    }
    if (pEdx != NULL) {
        *pEdx = 0;
    }
    return 0;
}

/* Historic _environ pointer — kept in sync with environ. */
char **_environ;

__attribute__((constructor)) static void
b26_ctor(void)
{
    _environ = environ;
}
