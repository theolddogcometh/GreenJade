/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20: utime/madvise64/lockf64, __memmem, moncontrol,
 * libc dlopen aliases, reentrant utmp/sgrp/alias, fortify wide printf + mbs,
 * isctype, __nl_langinfo_l, mcheck/mtrace, mq_getsetattr, __ns_name_*,
 * __libc_init_first/fcntl64/pthread_init.
 * Integer/pointer only (no SSE doubles).
 */
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <langinfo.h>
#include <locale.h>
#include <mqueue.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <utmp.h>
#include <wchar.h>

/* Wrappers / existing */
void *memmem(const void *pHay, size_t cbHay, const void *pNeedle,
             size_t cbNeedle);
int madvise(void *pAddr, size_t cb, int nAdvice);
int lockf(int nFd, int nCmd, off_t cbLen);
int monstartup(unsigned long uLow, unsigned long uHigh);
void *dlopen(const char *szFile, int nMode);
void *dlsym(void *pHandle, const char *szSymbol);
int dlclose(void *pHandle);
char *nl_langinfo(nl_item item);
char *nl_langinfo_l(nl_item item, locale_t loc);
int mq_getattr(mqd_t mq, struct mq_attr *pAttr);
int mq_setattr(mqd_t mq, const struct mq_attr *pNew, struct mq_attr *pOld);
mqd_t mq_open(const char *szName, int nFlags, ...);
int fcntl(int nFd, int nCmd, ...);
int ns_name_compress(const char *szSrc, unsigned char *pDst, size_t cbDst,
                     const unsigned char **ppDnptrs,
                     const unsigned char **ppLastdnptr);
int ns_name_ntop(const unsigned char *pSrc, char *szDst, size_t cbDst);
int ns_name_pton(const char *szSrc, unsigned char *pDst, size_t cbDst);
int ns_name_uncompress(const unsigned char *pMsg, const unsigned char *pEom,
                       const unsigned char *pSrc, char *szDst, size_t cbDst);
void __chk_fail(void);
int wprintf(const wchar_t *szFmt, ...);
int swprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, ...);
int vwprintf(const wchar_t *szFmt, va_list ap);
int vfwprintf(FILE *pF, const wchar_t *szFmt, va_list ap);
int vswprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, va_list ap);
wchar_t *fgetws(wchar_t *sz, int n, FILE *pF);
wchar_t *fgetws_unlocked(wchar_t *sz, int n, FILE *pF);
size_t mbsnrtowcs(wchar_t *pDst, const char **ppSrc, size_t cbSrc, size_t cLen,
                  mbstate_t *pSt);
size_t mbsrtowcs(wchar_t *pDst, const char **ppSrc, size_t cLen, mbstate_t *pSt);
size_t mbstowcs(wchar_t *pDst, const char *szSrc, size_t cLen);
size_t wcsnrtombs(char *pDst, const wchar_t **ppSrc, size_t cSrc, size_t cb,
                  mbstate_t *pSt);
const unsigned short **__ctype_b_loc(void);

/* sgrp / aliasent shapes (match batch17 local layouts) */
struct sgrp {
    char *sg_namp;
    char *sg_passwd;
    char **sg_adm;
    char **sg_mem;
};

struct aliasent {
    char *alias_name;
    size_t alias_members_len;
    char **alias_members;
    int alias_local;
};

struct sgrp *getsgent(void);
struct sgrp *getsgnam(const char *szName);
struct sgrp *fgetsgent(FILE *pF);
struct aliasent *getaliasent(void);
struct aliasent *getaliasbyname(const char *szName);

/* ---- utime (legacy) ----------------------------------------------------- */

int
utime(const char *szPath, const struct utimbuf *pTimes)
{
    struct timespec aTs[2];

    if (pTimes == NULL) {
        return utimensat(AT_FDCWD, szPath, NULL, 0);
    }
    aTs[0].tv_sec = pTimes->actime;
    aTs[0].tv_nsec = 0;
    aTs[1].tv_sec = pTimes->modtime;
    aTs[1].tv_nsec = 0;
    return utimensat(AT_FDCWD, szPath, aTs, 0);
}

/* ---- madvise / lockf LFS ------------------------------------------------ */

int
madvise64(void *pAddr, size_t cb, int nAdvice)
{
    return madvise(pAddr, cb, nAdvice);
}

int
__madvise(void *pAddr, size_t cb, int nAdvice)
{
    return madvise(pAddr, cb, nAdvice);
}

int
lockf64(int nFd, int nCmd, off_t cbLen)
{
    return lockf(nFd, nCmd, cbLen);
}

/* ---- memmem underscore -------------------------------------------------- */

void *
__memmem(const void *pHay, size_t cbHay, const void *pNeedle, size_t cbNeedle)
{
    return memmem(pHay, cbHay, pNeedle, cbNeedle);
}

/* ---- gmon control ------------------------------------------------------- */

void
moncontrol(int nMode)
{
    (void)nMode;
}

void
__moncontrol(int nMode)
{
    moncontrol(nMode);
}

int
__monstartup(unsigned long uLow, unsigned long uHigh)
{
    return monstartup(uLow, uHigh);
}

/* ---- libc internal dlopen aliases --------------------------------------- */

void *
__libc_dlopen_mode(const char *szName, int nMode)
{
    return dlopen(szName, nMode);
}

void *
__libc_dlsym(void *pHandle, const char *szSymbol)
{
    return dlsym(pHandle, szSymbol);
}

int
__libc_dlclose(void *pHandle)
{
    return dlclose(pHandle);
}

/* ---- reentrant utmp (empty stream) -------------------------------------- */

int
getutent_r(struct utmp *pBuf, struct utmp **ppResult)
{
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    (void)pBuf;
    errno = 0;
    return -1;
}

int
getutid_r(const struct utmp *pId, struct utmp *pBuf, struct utmp **ppResult)
{
    (void)pId;
    (void)pBuf;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    errno = 0;
    return -1;
}

int
getutline_r(const struct utmp *pLine, struct utmp *pBuf, struct utmp **ppResult)
{
    (void)pLine;
    (void)pBuf;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    errno = 0;
    return -1;
}

/* ---- reentrant sgrp / alias --------------------------------------------- */

int
getsgent_r(struct sgrp *pSg, char *szBuf, size_t cb, struct sgrp **ppResult)
{
    (void)pSg;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

int
getsgnam_r(const char *szName, struct sgrp *pSg, char *szBuf, size_t cb,
           struct sgrp **ppResult)
{
    (void)szName;
    (void)pSg;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

int
fgetsgent_r(FILE *pF, struct sgrp *pSg, char *szBuf, size_t cb,
            struct sgrp **ppResult)
{
    (void)pF;
    (void)pSg;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

int
getaliasent_r(struct aliasent *pAl, char *szBuf, size_t cb,
              struct aliasent **ppResult)
{
    (void)pAl;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

int
getaliasbyname_r(const char *szName, struct aliasent *pAl, char *szBuf,
                 size_t cb, struct aliasent **ppResult)
{
    (void)szName;
    (void)pAl;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

/* ---- fortify wide printf / mbs ------------------------------------------ */

int
__wprintf_chk(int nFlag, const wchar_t *szFmt, ...)
{
    va_list ap;
    int r;

    (void)nFlag;
    va_start(ap, szFmt);
    r = vwprintf(szFmt, ap);
    va_end(ap);
    return r;
}

int
__vwprintf_chk(int nFlag, const wchar_t *szFmt, va_list ap)
{
    (void)nFlag;
    return vwprintf(szFmt, ap);
}

int
__vfwprintf_chk(FILE *pF, int nFlag, const wchar_t *szFmt, va_list ap)
{
    (void)nFlag;
    return vfwprintf(pF, szFmt, ap);
}

int
__swprintf_chk(wchar_t *pBuf, size_t cBos, int nFlag, size_t cLen,
               const wchar_t *szFmt, ...)
{
    va_list ap;
    int r;

    (void)nFlag;
    if (cBos != (size_t)-1 && cLen > cBos) {
        __chk_fail();
    }
    va_start(ap, szFmt);
    r = vswprintf(pBuf, cLen, szFmt, ap);
    va_end(ap);
    return r;
}

int
__vswprintf_chk(wchar_t *pBuf, size_t cBos, int nFlag, size_t cLen,
                const wchar_t *szFmt, va_list ap)
{
    (void)nFlag;
    if (cBos != (size_t)-1 && cLen > cBos) {
        __chk_fail();
    }
    return vswprintf(pBuf, cLen, szFmt, ap);
}

wchar_t *
__fgetws_chk(wchar_t *sz, size_t cBos, int n, FILE *pF)
{
    if (n > 0 && cBos != (size_t)-1 && (size_t)n > cBos) {
        __chk_fail();
    }
    return fgetws(sz, n, pF);
}

wchar_t *
__fgetws_unlocked_chk(wchar_t *sz, size_t cBos, int n, FILE *pF)
{
    if (n > 0 && cBos != (size_t)-1 && (size_t)n > cBos) {
        __chk_fail();
    }
    return fgetws_unlocked(sz, n, pF);
}

size_t
__mbsnrtowcs_chk(wchar_t *pDst, const char **ppSrc, size_t cbSrc, size_t cLen,
                 mbstate_t *pSt, size_t cBos)
{
    if (pDst != NULL && cBos != (size_t)-1 && cLen > cBos) {
        __chk_fail();
    }
    return mbsnrtowcs(pDst, ppSrc, cbSrc, cLen, pSt);
}

size_t
__mbsrtowcs_chk(wchar_t *pDst, const char **ppSrc, size_t cLen, mbstate_t *pSt,
                size_t cBos)
{
    if (pDst != NULL && cBos != (size_t)-1 && cLen > cBos) {
        __chk_fail();
    }
    return mbsrtowcs(pDst, ppSrc, cLen, pSt);
}

size_t
__mbstowcs_chk(wchar_t *pDst, const char *szSrc, size_t cLen, size_t cBos)
{
    if (pDst != NULL && cBos != (size_t)-1 && cLen > cBos) {
        __chk_fail();
    }
    return mbstowcs(pDst, szSrc, cLen);
}

size_t
__wcsnrtombs_chk(char *pDst, const wchar_t **ppSrc, size_t cSrc, size_t cb,
                 mbstate_t *pSt, size_t cBos)
{
    if (pDst != NULL && cBos != (size_t)-1 && cb > cBos) {
        __chk_fail();
    }
    return wcsnrtombs(pDst, ppSrc, cSrc, cb, pSt);
}

/* ---- isctype / ascii_l -------------------------------------------------- */

int
__isctype(int c, int nMask)
{
    const unsigned short **pp;

    if (c < 0 || c > 255) {
        return 0;
    }
    pp = __ctype_b_loc();
    if (pp == NULL || *pp == NULL) {
        return 0;
    }
    return (int)((*pp)[c] & (unsigned short)nMask);
}

int
isctype(int c, int nMask)
{
    return __isctype(c, nMask);
}

int
__isascii_l(int c, locale_t loc)
{
    (void)loc;
    return (c >= 0 && c <= 0x7f) ? 1 : 0;
}

/* ---- locale langinfo underscore ----------------------------------------- */

char *
__nl_langinfo_l(nl_item item, locale_t loc)
{
    return nl_langinfo_l(item, loc);
}

/* ---- mqueue ------------------------------------------------------------- */

int
mq_getsetattr(mqd_t mq, const struct mq_attr *pNew, struct mq_attr *pOld)
{
    if (pNew == NULL) {
        return mq_getattr(mq, pOld);
    }
    return mq_setattr(mq, pNew, pOld);
}

mqd_t
__mq_open_2(const char *szName, int nOflag)
{
    /* Fortify: O_CREAT without mode is invalid for open-like APIs */
    if ((nOflag & O_CREAT) != 0) {
        errno = EINVAL;
        return (mqd_t)-1;
    }
    return mq_open(szName, nOflag);
}

/* ---- mcheck / mtrace stubs ---------------------------------------------- */

int
mcheck(void (*pfnAbort)(int nStatus))
{
    (void)pfnAbort;
    return 0;
}

int
mcheck_pedantic(void (*pfnAbort)(int nStatus))
{
    (void)pfnAbort;
    return 0;
}

void
mcheck_check_all(void)
{
}

int
mprobe(void *p)
{
    (void)p;
    return 0; /* MCHECK_OK-shaped */
}

void
mtrace(void)
{
}

void
muntrace(void)
{
}

/* ---- libc init / fcntl / pthread init ----------------------------------- */

void
__libc_init_first(int nArgc, char **ppArgv, char **ppEnv)
{
    (void)nArgc;
    (void)ppArgv;
    (void)ppEnv;
}

int
__libc_fcntl64(int nFd, int nCmd, ...)
{
    va_list ap;
    void *pArg;
    int r;

    va_start(ap, nCmd);
    pArg = va_arg(ap, void *);
    va_end(ap);
    r = fcntl(nFd, nCmd, pArg);
    return r;
}

int
__libc_pthread_init(void)
{
    return 0;
}

/* ---- ns_name underscored aliases ---------------------------------------- */

int
__ns_name_compress(const char *szSrc, unsigned char *pDst, size_t cbDst,
                   const unsigned char **ppDnptrs,
                   const unsigned char **ppLastdnptr)
{
    return ns_name_compress(szSrc, pDst, cbDst, ppDnptrs, ppLastdnptr);
}

int
__ns_name_ntop(const unsigned char *pSrc, char *szDst, size_t cbDst)
{
    return ns_name_ntop(pSrc, szDst, cbDst);
}

int
__ns_name_pton(const char *szSrc, unsigned char *pDst, size_t cbDst)
{
    return ns_name_pton(szSrc, pDst, cbDst);
}

int
__ns_name_uncompress(const unsigned char *pMsg, const unsigned char *pEom,
                     const unsigned char *pSrc, char *szDst, size_t cbDst)
{
    return ns_name_uncompress(pMsg, pEom, pSrc, szDst, cbDst);
}

/* ---- more underscored path/sys helpers ---------------------------------- */

int
__utime(const char *szPath, const struct utimbuf *pTimes)
{
    return utime(szPath, pTimes);
}

int
__lockf64(int nFd, int nCmd, off_t cbLen)
{
    return lockf64(nFd, nCmd, cbLen);
}

void
__mtrace(void)
{
    mtrace();
}

void
__muntrace(void)
{
    muntrace();
}
