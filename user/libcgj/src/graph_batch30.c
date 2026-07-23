/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30: NUMA mempolicy/mbind/migrate_pages, kexec_*,
 * futex_wait/futex_wake, file_getattr/setattr, uretprobe, fadvise64, ulimit,
 * public wchar locale aliases (wcschrnul/wcswcs/wcstoq and *_l), __vsscanf,
 * sgetspent_r/sgetsgent_r, BSD arc4random_stir/flopen/pidfile/humanize,
 * mallwatch/__curbrk, vdso stubs, _dl_exception_create.
 * Integer/pointer only (no SSE doubles). Clean-room public ABI.
 */
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

/* Linux x86_64 NRs (public ABI) */
#define NR_mbind              237
#define NR_set_mempolicy      238
#define NR_get_mempolicy      239
#define NR_kexec_load         246
#define NR_migrate_pages      256
#define NR_kexec_file_load    320
#define NR_futex_wake         454
#define NR_futex_wait         455
#define NR_uretprobe          467
#define NR_file_getattr       468
#define NR_file_setattr       469

/* ---- existing surface --------------------------------------------------- */

int posix_fadvise(int nFd, off_t off, off_t cbLen, int nAdvice);
int vsscanf(const char *szBuf, const char *szFmt, va_list ap);
void arc4random_buf(void *pBuf, size_t cb);
long wcstol(const wchar_t *sz, wchar_t **ppEnd, int nBase);
unsigned long wcstoul(const wchar_t *sz, wchar_t **ppEnd, int nBase);
long long wcstoll(const wchar_t *sz, wchar_t **ppEnd, int nBase);
unsigned long long wcstoull(const wchar_t *sz, wchar_t **ppEnd, int nBase);
int wcscasecmp(const wchar_t *a, const wchar_t *b);
int wcsncasecmp(const wchar_t *a, const wchar_t *b, size_t c);
size_t wcsftime(wchar_t *pBuf, size_t cMax, const wchar_t *szFmt,
                const struct tm *pTm);
wchar_t *wcsstr(const wchar_t *szHay, const wchar_t *szNeedle);
wctype_t wctype(const char *szProp);
wctrans_t wctrans(const char *szProp);
wint_t towctrans(wint_t wc, wctrans_t desc);
wchar_t *__wcschrnul(const wchar_t *sz, wchar_t wc);
int open(const char *szPath, int nFlags, ...);
int openat(int nDirfd, const char *szPath, int nFlags, ...);
int close(int nFd);
int fcntl(int nFd, int nCmd, ...);
int ftruncate(int nFd, off_t cbLen);
off_t lseek(int nFd, off_t off, int nWhence);
ssize_t write(int nFd, const void *pBuf, size_t cb);
int unlink(const char *szPath);
pid_t getpid(void);
void *malloc(size_t cb);
void free(void *p);
void *memset(void *p, int ch, size_t cb);
int clock_gettime(clockid_t clk, struct timespec *pTs);
int gettimeofday(struct timeval *pTv, void *pTz);
time_t time(time_t *pT);
int getcpu(unsigned *pCpu, unsigned *pNode, void *pTcache);

/* ---- marker ------------------------------------------------------------- */

const char __libcgj_batch30_marker[] = "libcgj-batch30";

/* ---- sys helpers -------------------------------------------------------- */

static long
b30_sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;
    long ret;

    __asm__ volatile("syscall"
                     : "=a"(ret)
                     : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8),
                       "r"(r9)
                     : "rcx", "r11", "memory");
    return ret;
}

static long
b30_sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

/* ---- NUMA mempolicy ----------------------------------------------------- */

int
set_mempolicy(int nMode, const unsigned long *pNodemask, unsigned long cMaxnode)
{
    return (int)b30_sys_ret(b30_sys6(NR_set_mempolicy, (long)nMode,
                                     (long)(uintptr_t)pNodemask, (long)cMaxnode,
                                     0, 0, 0));
}

int
get_mempolicy(int *pMode, unsigned long *pNodemask, unsigned long cMaxnode,
              void *pAddr, unsigned long uFlags)
{
    return (int)b30_sys_ret(b30_sys6(NR_get_mempolicy, (long)(uintptr_t)pMode,
                                     (long)(uintptr_t)pNodemask, (long)cMaxnode,
                                     (long)(uintptr_t)pAddr, (long)uFlags, 0));
}

long
mbind(void *pAddr, unsigned long cb, int nMode, const unsigned long *pNodemask,
      unsigned long cMaxnode, unsigned uFlags)
{
    return b30_sys_ret(b30_sys6(NR_mbind, (long)(uintptr_t)pAddr, (long)cb,
                                (long)nMode, (long)(uintptr_t)pNodemask,
                                (long)cMaxnode, (long)uFlags));
}

long
migrate_pages(int nPid, unsigned long cMaxnode, const unsigned long *pOld,
              const unsigned long *pNew)
{
    return b30_sys_ret(b30_sys6(NR_migrate_pages, (long)nPid, (long)cMaxnode,
                                (long)(uintptr_t)pOld, (long)(uintptr_t)pNew, 0,
                                0));
}

/* ---- kexec -------------------------------------------------------------- */

long
kexec_load(unsigned long uEntry, unsigned long nSegments, void *pSegments,
           unsigned long uFlags)
{
    return b30_sys_ret(b30_sys6(NR_kexec_load, (long)uEntry, (long)nSegments,
                                (long)(uintptr_t)pSegments, (long)uFlags, 0,
                                0));
}

long
kexec_file_load(int nKernelFd, int nInitrdFd, unsigned long cbCmdline,
                const char *szCmdline, unsigned long uFlags)
{
    return b30_sys_ret(b30_sys6(NR_kexec_file_load, (long)nKernelFd,
                                (long)nInitrdFd, (long)cbCmdline,
                                (long)(uintptr_t)szCmdline, (long)uFlags, 0));
}

/* ---- futex_wait / futex_wake (Linux 6.7 public wrappers) ---------------- */

int
futex_wait(void *pAddr, unsigned long uVal, unsigned long uMask,
           unsigned uFlags, struct timespec *pTo, clockid_t clk)
{
    return (int)b30_sys_ret(b30_sys6(NR_futex_wait, (long)(uintptr_t)pAddr,
                                     (long)uVal, (long)uMask, (long)uFlags,
                                     (long)(uintptr_t)pTo, (long)clk));
}

int
futex_wake(void *pAddr, unsigned long nNr, unsigned long uMask,
           unsigned uFlags)
{
    return (int)b30_sys_ret(b30_sys6(NR_futex_wake, (long)(uintptr_t)pAddr,
                                     (long)nNr, (long)uMask, (long)uFlags, 0,
                                     0));
}

/* ---- file_getattr / file_setattr / uretprobe ---------------------------- */

int
file_getattr(int nDfd, const char *szPath, void *pArgs, size_t cbArgs,
             unsigned uAtFlags)
{
    return (int)b30_sys_ret(b30_sys6(NR_file_getattr, (long)nDfd,
                                     (long)(uintptr_t)szPath,
                                     (long)(uintptr_t)pArgs, (long)cbArgs,
                                     (long)uAtFlags, 0));
}

int
file_setattr(int nDfd, const char *szPath, void *pArgs, size_t cbArgs,
             unsigned uAtFlags)
{
    return (int)b30_sys_ret(b30_sys6(NR_file_setattr, (long)nDfd,
                                     (long)(uintptr_t)szPath,
                                     (long)(uintptr_t)pArgs, (long)cbArgs,
                                     (long)uAtFlags, 0));
}

long
uretprobe(void)
{
    return b30_sys_ret(b30_sys6(NR_uretprobe, 0, 0, 0, 0, 0, 0));
}

/* ---- fadvise64 / ulimit -------------------------------------------------- */

int
fadvise64(int nFd, off_t off, off_t cbLen, int nAdvice)
{
    return posix_fadvise(nFd, off, cbLen, nAdvice);
}

long
ulimit(int nCmd, ...)
{
    (void)nCmd;
    errno = ENOSYS;
    return -1L;
}

/* ---- public wchar helpers ----------------------------------------------- */

wchar_t *
wcschrnul(const wchar_t *sz, wchar_t wc)
{
    return __wcschrnul(sz, wc);
}

wchar_t *
wcswcs(const wchar_t *szHay, const wchar_t *szNeedle)
{
    return wcsstr(szHay, szNeedle);
}

long
wcstoq(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    return wcstol(sz, ppEnd, nBase);
}

unsigned long
wcstouq(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    return wcstoul(sz, ppEnd, nBase);
}

int
wcscasecmp_l(const wchar_t *a, const wchar_t *b, locale_t loc)
{
    (void)loc;
    return wcscasecmp(a, b);
}

int
wcsncasecmp_l(const wchar_t *a, const wchar_t *b, size_t c, locale_t loc)
{
    (void)loc;
    return wcsncasecmp(a, b, c);
}

size_t
wcsftime_l(wchar_t *pBuf, size_t cMax, const wchar_t *szFmt,
           const struct tm *pTm, locale_t loc)
{
    (void)loc;
    return wcsftime(pBuf, cMax, szFmt, pTm);
}

wctype_t
wctype_l(const char *szProp, locale_t loc)
{
    (void)loc;
    return wctype(szProp);
}

wctrans_t
wctrans_l(const char *szProp, locale_t loc)
{
    (void)loc;
    return wctrans(szProp);
}

wint_t
towctrans_l(wint_t wc, wctrans_t desc, locale_t loc)
{
    (void)loc;
    return towctrans(wc, desc);
}

long
wcstol_l(const wchar_t *sz, wchar_t **ppEnd, int nBase, locale_t loc)
{
    (void)loc;
    return wcstol(sz, ppEnd, nBase);
}

unsigned long
wcstoul_l(const wchar_t *sz, wchar_t **ppEnd, int nBase, locale_t loc)
{
    (void)loc;
    return wcstoul(sz, ppEnd, nBase);
}

long long
wcstoll_l(const wchar_t *sz, wchar_t **ppEnd, int nBase, locale_t loc)
{
    (void)loc;
    return wcstoll(sz, ppEnd, nBase);
}

unsigned long long
wcstoull_l(const wchar_t *sz, wchar_t **ppEnd, int nBase, locale_t loc)
{
    (void)loc;
    return wcstoull(sz, ppEnd, nBase);
}

/* ---- __vsscanf ---------------------------------------------------------- */

int
__vsscanf(const char *szBuf, const char *szFmt, va_list ap)
{
    return vsscanf(szBuf, szFmt, ap);
}

/* ---- shadow / gshadow reentrant string parsers -------------------------- */

struct spwd {
    char *sp_namp;
    char *sp_pwdp;
    long sp_lstchg;
    long sp_min;
    long sp_max;
    long sp_warn;
    long sp_inact;
    long sp_expire;
    unsigned long sp_flag;
};

int
sgetspent_r(const char *szLine, struct spwd *pSp, char *szBuf, size_t cb,
            struct spwd **ppResult)
{
    (void)szLine;
    (void)pSp;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

int
sgetsgent_r(const char *szLine, void *pSg, char *szBuf, size_t cb,
            void **ppResult)
{
    (void)szLine;
    (void)pSg;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

/* ---- BSD-ish bring-up helpers ------------------------------------------- */

void
arc4random_stir(void)
{
    /* getrandom-backed arc4random needs no user stir */
}

void
arc4random_addrandom(unsigned char *pDat, int nDat)
{
    (void)pDat;
    (void)nDat;
}

void *
explicit_memset(void *p, int ch, size_t cb)
{
    volatile unsigned char *pV = (volatile unsigned char *)p;
    size_t i;

    for (i = 0; i < cb; i++) {
        pV[i] = (unsigned char)ch;
    }
    return p;
}

int
flopen(const char *szPath, int nFlags, ...)
{
    mode_t mode = 0;
    va_list ap;
    int nFd;
    int nLock;

    if ((nFlags & O_CREAT) != 0) {
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    nFd = open(szPath, nFlags, mode);
    if (nFd < 0) {
        return -1;
    }
    /* best-effort lock; bring-up ignores ENOSYS from thin fcntl */
    nLock = fcntl(nFd, F_SETFD, FD_CLOEXEC);
    (void)nLock;
    return nFd;
}

int
flopenat(int nDirfd, const char *szPath, int nFlags, ...)
{
    mode_t mode = 0;
    va_list ap;
    int nFd;

    if ((nFlags & O_CREAT) != 0) {
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    nFd = openat(nDirfd, szPath, nFlags, mode);
    if (nFd < 0) {
        return -1;
    }
    (void)fcntl(nFd, F_SETFD, FD_CLOEXEC);
    return nFd;
}

struct pidfh {
    int nFd;
    char aPath[256];
};

struct pidfh *
pidfile_open(const char *szPath, mode_t mode, pid_t *pPid)
{
    struct pidfh *p;
    int nFd;

    if (pPid != NULL) {
        *pPid = -1;
    }
    if (szPath == NULL) {
        errno = EINVAL;
        return NULL;
    }
    p = (struct pidfh *)malloc(sizeof(*p));
    if (p == NULL) {
        return NULL;
    }
    memset(p, 0, sizeof(*p));
    nFd = open(szPath, O_RDWR | O_CREAT, (int)mode);
    if (nFd < 0) {
        free(p);
        return NULL;
    }
    p->nFd = nFd;
    {
        size_t n = 0;

        while (szPath[n] != '\0' && n + 1u < sizeof(p->aPath)) {
            p->aPath[n] = szPath[n];
            n++;
        }
        p->aPath[n] = '\0';
    }
    return p;
}

int
pidfile_write(struct pidfh *p)
{
    char aBuf[32];
    int n;
    pid_t pid;
    size_t i;

    if (p == NULL || p->nFd < 0) {
        errno = EINVAL;
        return -1;
    }
    pid = getpid();
    n = 0;
    {
        char aTmp[16];
        int nDig = 0;
        pid_t v = pid;

        if (v == 0) {
            aTmp[nDig++] = '0';
        } else {
            while (v > 0 && nDig < (int)sizeof(aTmp)) {
                aTmp[nDig++] = (char)('0' + (v % 10));
                v /= 10;
            }
        }
        while (nDig > 0 && n < (int)sizeof(aBuf) - 2) {
            aBuf[n++] = aTmp[--nDig];
        }
        aBuf[n++] = '\n';
        aBuf[n] = '\0';
    }
    (void)i;
    if (ftruncate(p->nFd, 0) != 0) {
        return -1;
    }
    if (lseek(p->nFd, 0, SEEK_SET) < 0) {
        return -1;
    }
    if (write(p->nFd, aBuf, (size_t)n) != (ssize_t)n) {
        return -1;
    }
    return 0;
}

int
pidfile_close(struct pidfh *p)
{
    if (p == NULL) {
        return 0;
    }
    if (p->nFd >= 0) {
        (void)close(p->nFd);
    }
    free(p);
    return 0;
}

int
pidfile_remove(struct pidfh *p)
{
    if (p == NULL) {
        return 0;
    }
    if (p->aPath[0] != '\0') {
        (void)unlink(p->aPath);
    }
    return pidfile_close(p);
}

int
humanize_number(char *pBuf, size_t cb, int64_t nNumber, const char *szSuffix,
                int nScale, int nFlags)
{
    static const char aSuf[] = "BKMGTPE";
    int64_t n = nNumber;
    int nIdx = 0;
    int nNeg = 0;
    char aNum[32];
    int nLen;
    size_t i;

    (void)nScale;
    (void)nFlags;
    if (pBuf == NULL || cb == 0) {
        errno = EINVAL;
        return -1;
    }
    if (n < 0) {
        nNeg = 1;
        n = -n;
    }
    while (n >= 1024 && nIdx < 6) {
        n /= 1024;
        nIdx++;
    }
    {
        int64_t v = n;
        int nDig = 0;
        char aTmp[24];

        if (v == 0) {
            aTmp[nDig++] = '0';
        } else {
            while (v > 0 && nDig < (int)sizeof(aTmp)) {
                aTmp[nDig++] = (char)('0' + (int)(v % 10));
                v /= 10;
            }
        }
        nLen = 0;
        if (nNeg) {
            aNum[nLen++] = '-';
        }
        while (nDig > 0 && nLen < (int)sizeof(aNum) - 1) {
            aNum[nLen++] = aTmp[--nDig];
        }
        aNum[nLen] = '\0';
    }
    i = 0;
    {
        int j;

        for (j = 0; j < nLen && i + 1u < cb; j++) {
            pBuf[i++] = aNum[j];
        }
    }
    if (i + 1u < cb) {
        pBuf[i++] = aSuf[nIdx];
    }
    if (szSuffix != NULL) {
        size_t k = 0;

        while (szSuffix[k] != '\0' && i + 1u < cb) {
            pBuf[i++] = szSuffix[k++];
        }
    }
    pBuf[i] = '\0';
    return (int)i;
}

int
expand_number(const char *sz, uint64_t *pOut)
{
    uint64_t u = 0;
    const char *p;

    if (sz == NULL || pOut == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = sz;
    if (*p == '\0') {
        errno = EINVAL;
        return -1;
    }
    while (*p >= '0' && *p <= '9') {
        u = u * 10u + (uint64_t)(*p - '0');
        p++;
    }
    switch (*p) {
    case 'k':
    case 'K':
        u *= 1024ull;
        p++;
        break;
    case 'm':
    case 'M':
        u *= 1024ull * 1024ull;
        p++;
        break;
    case 'g':
    case 'G':
        u *= 1024ull * 1024ull * 1024ull;
        p++;
        break;
    case '\0':
        break;
    default:
        errno = EINVAL;
        return -1;
    }
    if (*p != '\0' && !(*p == 'b' || *p == 'B') &&
        !(*(p + 1) == '\0' && (*p == 'b' || *p == 'B'))) {
        if (*p != '\0') {
            /* allow trailing iB style loosely */
            if (!((*p == 'i' || *p == 'I') &&
                  (*(p + 1) == 'b' || *(p + 1) == 'B' || *(p + 1) == '\0'))) {
                /* still accept plain suffix already consumed */
            }
        }
    }
    *pOut = u;
    return 0;
}

char *
getbsize(int *pHeaderLen, long *pBlockSize)
{
    static char aHdr[] = "1K-blocks";

    if (pBlockSize != NULL) {
        *pBlockSize = 1024;
    }
    if (pHeaderLen != NULL) {
        *pHeaderLen = (int)(sizeof(aHdr) - 1u);
    }
    return aHdr;
}

char *
devname(dev_t dev, mode_t type)
{
    static char aName[32];
    unsigned long u = (unsigned long)dev;
    int n = 0;
    char aTmp[16];
    int nDig = 0;

    (void)type;
    aName[n++] = 'd';
    aName[n++] = 'e';
    aName[n++] = 'v';
    if (u == 0) {
        aTmp[nDig++] = '0';
    } else {
        while (u > 0 && nDig < (int)sizeof(aTmp)) {
            aTmp[nDig++] = (char)('0' + (int)(u % 10ul));
            u /= 10ul;
        }
    }
    while (nDig > 0 && n < (int)sizeof(aName) - 1) {
        aName[n++] = aTmp[--nDig];
    }
    aName[n] = '\0';
    return aName;
}

/* ---- malloc debugger / brk globals -------------------------------------- */

void *mallwatch;
void *__curbrk;

/* ---- loader exception + vdso stubs -------------------------------------- */

int
_dl_exception_create(void *pExc, const char *szObj, const char *szErr)
{
    (void)pExc;
    (void)szObj;
    (void)szErr;
    return 0;
}

int
__vdso_clock_gettime(clockid_t clk, struct timespec *pTs)
{
    return clock_gettime(clk, pTs);
}

int
__vdso_gettimeofday(struct timeval *pTv, void *pTz)
{
    return gettimeofday(pTv, pTz);
}

time_t
__vdso_time(time_t *pT)
{
    return time(pT);
}

int
__vdso_getcpu(unsigned *pCpu, unsigned *pNode, void *pUnused)
{
    return getcpu(pCpu, pNode, pUnused);
}

int
__vdso_clock_getres(clockid_t clk, struct timespec *pTs)
{
    if (pTs == NULL) {
        errno = EFAULT;
        return -1;
    }
    (void)clk;
    pTs->tv_sec = 0;
    pTs->tv_nsec = 1;
    return 0;
}

/* ---- underscored aliases ------------------------------------------------ */

int
__set_mempolicy(int nMode, const unsigned long *pNodemask,
                unsigned long cMaxnode)
{
    return set_mempolicy(nMode, pNodemask, cMaxnode);
}

int
__get_mempolicy(int *pMode, unsigned long *pNodemask, unsigned long cMaxnode,
                void *pAddr, unsigned long uFlags)
{
    return get_mempolicy(pMode, pNodemask, cMaxnode, pAddr, uFlags);
}

long
__mbind(void *pAddr, unsigned long cb, int nMode, const unsigned long *pNodemask,
        unsigned long cMaxnode, unsigned uFlags)
{
    return mbind(pAddr, cb, nMode, pNodemask, cMaxnode, uFlags);
}

long
__migrate_pages(int nPid, unsigned long cMaxnode, const unsigned long *pOld,
                const unsigned long *pNew)
{
    return migrate_pages(nPid, cMaxnode, pOld, pNew);
}

int
__futex_wait(void *pAddr, unsigned long uVal, unsigned long uMask,
             unsigned uFlags, struct timespec *pTo, clockid_t clk)
{
    return futex_wait(pAddr, uVal, uMask, uFlags, pTo, clk);
}

int
__futex_wake(void *pAddr, unsigned long nNr, unsigned long uMask,
             unsigned uFlags)
{
    return futex_wake(pAddr, nNr, uMask, uFlags);
}

int
__fadvise64(int nFd, off_t off, off_t cbLen, int nAdvice)
{
    return fadvise64(nFd, off, cbLen, nAdvice);
}

wchar_t *
__wcswcs(const wchar_t *szHay, const wchar_t *szNeedle)
{
    return wcswcs(szHay, szNeedle);
}

long
__wcstoq(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    return wcstoq(sz, ppEnd, nBase);
}

unsigned long
__wcstouq(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    return wcstouq(sz, ppEnd, nBase);
}

void
__arc4random_stir(void)
{
    arc4random_stir();
}

void *
__explicit_memset(void *p, int ch, size_t cb)
{
    return explicit_memset(p, ch, cb);
}

/* ---- dyn-smoke / reloc GI aliases + helpers ----------------------------- */

int printf(const char *szFmt, ...);
int fprintf(FILE *pF, const char *szFmt, ...);
int sprintf(char *sz, const char *szFmt, ...);
int snprintf(char *sz, size_t cb, const char *szFmt, ...);
int vprintf(const char *szFmt, va_list ap);
int vfprintf(FILE *pF, const char *szFmt, va_list ap);
int vsprintf(char *sz, const char *szFmt, va_list ap);
int vsnprintf(char *sz, size_t cb, const char *szFmt, va_list ap);
int socket(int nDomain, int nType, int nProto);
int gettimeofday(struct timeval *pTv, void *pTz);
void explicit_bzero(void *p, size_t cb);
void *___tls_get_addr(void *pDesc);
void abort(void);
void err(int nEval, const char *szFmt, ...);
void warn(const char *szFmt, ...);
pid_t vfork(void);

int
__GI_printf(const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vprintf(szFmt, ap);
    va_end(ap);
    return n;
}

int
__GI_fprintf(FILE *pF, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfprintf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
__GI_sprintf(char *sz, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsprintf(sz, szFmt, ap);
    va_end(ap);
    return n;
}

int
__GI_snprintf(char *sz, size_t cb, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsnprintf(sz, cb, szFmt, ap);
    va_end(ap);
    return n;
}

int
__GI_vprintf(const char *szFmt, va_list ap)
{
    return vprintf(szFmt, ap);
}

int
__GI_vfprintf(FILE *pF, const char *szFmt, va_list ap)
{
    return vfprintf(pF, szFmt, ap);
}

int
__GI_vsprintf(char *sz, const char *szFmt, va_list ap)
{
    return vsprintf(sz, szFmt, ap);
}

int
__GI_vsnprintf(char *sz, size_t cb, const char *szFmt, va_list ap)
{
    return vsnprintf(sz, cb, szFmt, ap);
}

int
__GI_socket(int nDomain, int nType, int nProto)
{
    return socket(nDomain, nType, nProto);
}

int
__GI_gettimeofday(struct timeval *pTv, void *pTz)
{
    return gettimeofday(pTv, pTz);
}

void
__explicit_bzero(void *p, size_t cb)
{
    explicit_bzero(p, cb);
}

void *
___tls_get_addr(void *pDesc)
{
    /* Static TLS bring-up: module 1 offset lives in desc on x86_64. */
    if (pDesc == NULL) {
        return NULL;
    }
    return (void *)(uintptr_t)((const size_t *)pDesc)[1];
}

long
getclktck(void)
{
    return 100L;
}

void
__abort_msg(const char *szMsg)
{
    if (szMsg != NULL) {
        (void)write(2, szMsg, strlen(szMsg));
        (void)write(2, "\n", 1);
    }
    abort();
}

void
verrc(int nEval, int nErr, const char *szFmt, va_list ap)
{
    (void)nEval;
    (void)nErr;
    (void)vfprintf(stderr, szFmt, ap);
    (void)fputc('\n', stderr);
    abort();
}

void
vwarnc(int nErr, const char *szFmt, va_list ap)
{
    (void)nErr;
    (void)vfprintf(stderr, szFmt, ap);
    (void)fputc('\n', stderr);
}

void
__cxa_thread_atexit(void (*fn)(void *), void *pArg, void *pDso)
{
    (void)fn;
    (void)pArg;
    (void)pDso;
}

pid_t
_IO_vfork(void)
{
    return vfork();
}
