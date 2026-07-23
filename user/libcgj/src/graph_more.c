/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Additional glibc graph nodes: LFS aliases, underscored aliases, clone,
 * closefrom, clock, signal legacy, c16/c32, bindresvport, host id, etc.
 * Integer-only (no SSE doubles).
 */
#include <dirent.h>
#include <errno.h>
#include <execinfo.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <sys/types.h>
#include <time.h>
#include <uchar.h>
#include <unistd.h>
#include <wchar.h>

/* ---- LFS / stdio aliases ------------------------------------------------ */

int
alphasort64(const struct dirent **ppA, const struct dirent **ppB)
{
    return alphasort(ppA, ppB);
}

int
creat64(const char *szPath, mode_t mode)
{
    return creat(szPath, mode);
}

int
fcntl64(int nFd, int nCmd, ...)
{
    va_list ap;
    long arg = 0;

    va_start(ap, nCmd);
    arg = va_arg(ap, long);
    va_end(ap);
    return fcntl(nFd, nCmd, arg);
}

int
fallocate64(int nFd, int nMode, off_t off, off_t cbLen)
{
    return fallocate(nFd, nMode, off, cbLen);
}

FILE *
fopen64(const char *szPath, const char *szMode)
{
    return fopen(szPath, szMode);
}

int
fgetpos64(FILE *pF, fpos_t *pPos)
{
    return fgetpos(pF, pPos);
}

int
fsetpos64(FILE *pF, const fpos_t *pPos)
{
    return fsetpos(pF, pPos);
}

/* ---- underscored string / stdio aliases --------------------------------- */

void
__bzero(void *p, size_t cb)
{
    (void)memset(p, 0, cb);
}

void *
__mempcpy(void *pDst, const void *pSrc, size_t cb)
{
    return mempcpy(pDst, pSrc, cb);
}

char *
__stpcpy(char *szDst, const char *szSrc)
{
    return stpcpy(szDst, szSrc);
}

void *
__rawmemchr(const void *p, int c)
{
    return rawmemchr(p, c);
}

char *
__strdup(const char *sz)
{
    return strdup(sz);
}

int
__strverscmp(const char *szA, const char *szB)
{
    return strverscmp(szA, szB);
}

int
__asprintf(char **ppBuf, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vasprintf(ppBuf, szFmt, ap);
    va_end(ap);
    return n;
}

int
__backtrace(void **ppBuffer, int nSize)
{
    return backtrace(ppBuffer, nSize);
}

char **
__backtrace_symbols(void *const *ppBuffer, int nSize)
{
    return backtrace_symbols(ppBuffer, nSize);
}

void
__backtrace_symbols_fd(void *const *ppBuffer, int nSize, int nFd)
{
    backtrace_symbols_fd(ppBuffer, nSize, nFd);
}

int
__clock_gettime(clockid_t clk, struct timespec *pTs)
{
    return clock_gettime(clk, pTs);
}

/* ---- misc stdlib / process ---------------------------------------------- */

void
cfree(void *p)
{
    free(p);
}

clock_t
clock(void)
{
    struct tms t;

    if (times(&t) == (clock_t)-1) {
        return (clock_t)-1;
    }
    return t.tms_utime + t.tms_stime;
}

#ifndef CLOCKS_PER_SEC
/* exported via time.h; keep value for local use */
#define CLOCKS_PER_SEC ((clock_t)1000000)
#endif

/* ---- closefrom / fcloseall (fexecve lives in linux_sys_more.c) ----------- */

int
closefrom(int nLowfd)
{
    int n;

    if (nLowfd < 0) {
        errno = EINVAL;
        return -1;
    }
    /* Prefer close_range when available */
    n = close_range((unsigned)nLowfd, ~0u, 0);
    if (n == 0) {
        return 0;
    }
    {
        int fd;
        int nMax = getdtablesize();

        if (nMax < 0) {
            nMax = 1024;
        }
        for (fd = nLowfd; fd < nMax; fd++) {
            (void)close(fd);
        }
    }
    return 0;
}

int
fcloseall(void)
{
    /* Bring-up: only known std streams; full FILE list not tracked */
    (void)fflush(NULL);
    return 0;
}

/* ---- tty name helpers --------------------------------------------------- */

char *
ctermid(char *sz)
{
    static char aBuf[8] = "/dev/tty";

    if (sz == NULL) {
        return aBuf;
    }
    strcpy(sz, "/dev/tty");
    return sz;
}

char *
cuserid(char *sz)
{
    static char aBuf[32] = "root";
    const char *p = getenv("LOGNAME");

    if (p == NULL) {
        p = getenv("USER");
    }
    if (p == NULL) {
        p = "root";
    }
    if (sz == NULL) {
        size_t n = strlen(p);

        if (n >= sizeof(aBuf)) {
            n = sizeof(aBuf) - 1u;
        }
        memcpy(aBuf, p, n);
        aBuf[n] = '\0';
        return aBuf;
    }
    {
        size_t n = strlen(p);

        if (n > 8) {
            n = 8; /* historical L_cuserid */
        }
        memcpy(sz, p, n);
        sz[n] = '\0';
        return sz;
    }
}

/* ---- signal legacy ------------------------------------------------------ */

sighandler_t
bsd_signal(int nSig, sighandler_t pHandler)
{
    struct sigaction sa;
    struct sigaction so;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = pHandler;
    sa.sa_flags = SA_RESTART;
    (void)sigemptyset(&sa.sa_mask);
    if (sigaction(nSig, &sa, &so) != 0) {
        return SIG_ERR;
    }
    return so.sa_handler;
}

sighandler_t
sysv_signal(int nSig, sighandler_t pHandler)
{
    struct sigaction sa;
    struct sigaction so;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = pHandler;
    sa.sa_flags = SA_RESETHAND | SA_NODEFER;
    (void)sigemptyset(&sa.sa_mask);
    if (sigaction(nSig, &sa, &so) != 0) {
        return SIG_ERR;
    }
    return so.sa_handler;
}

sighandler_t
__sysv_signal(int nSig, sighandler_t pHandler)
{
    return sysv_signal(nSig, pHandler);
}

int
sigblock(int nMask)
{
    sigset_t set;
    sigset_t old;
    int i;

    (void)sigemptyset(&set);
    for (i = 1; i < 32; i++) {
        if (nMask & (1 << (i - 1))) {
            (void)sigaddset(&set, i);
        }
    }
    if (sigprocmask(SIG_BLOCK, &set, &old) != 0) {
        return -1;
    }
    {
        int nOld = 0;

        for (i = 1; i < 32; i++) {
            if (sigismember(&old, i)) {
                nOld |= (1 << (i - 1));
            }
        }
        return nOld;
    }
}

int
sigsetmask(int nMask)
{
    sigset_t set;
    sigset_t old;
    int i;

    (void)sigemptyset(&set);
    for (i = 1; i < 32; i++) {
        if (nMask & (1 << (i - 1))) {
            (void)sigaddset(&set, i);
        }
    }
    if (sigprocmask(SIG_SETMASK, &set, &old) != 0) {
        return -1;
    }
    {
        int nOld = 0;

        for (i = 1; i < 32; i++) {
            if (sigismember(&old, i)) {
                nOld |= (1 << (i - 1));
            }
        }
        return nOld;
    }
}

int
siggetmask(void)
{
    sigset_t old;
    int i;
    int nOld = 0;

    if (sigprocmask(SIG_SETMASK, NULL, &old) != 0) {
        return -1;
    }
    for (i = 1; i < 32; i++) {
        if (sigismember(&old, i)) {
            nOld |= (1 << (i - 1));
        }
    }
    return nOld;
}

int
sigpause(int nSig)
{
    sigset_t set;

    if (sigprocmask(SIG_BLOCK, NULL, &set) != 0) {
        return -1;
    }
    (void)sigdelset(&set, nSig);
    return sigsuspend(&set);
}

int
__sigpause(int nSig)
{
    return sigpause(nSig);
}

/* ---- uchar / c16 c32 ---------------------------------------------------- */

size_t
c32rtomb(char *pS, char32_t c32, mbstate_t *pSt)
{
    wchar_t wc = (wchar_t)c32;

    return wcrtomb(pS, wc, pSt);
}

size_t
c16rtomb(char *pS, char16_t c16, mbstate_t *pSt)
{
    /* BMP-only bring-up (no surrogate pairs) */
    return c32rtomb(pS, (char32_t)c16, pSt);
}

size_t
mbrtoc32(char32_t *pC32, const char *pS, size_t n, mbstate_t *pSt)
{
    wchar_t wc = 0;
    size_t r = mbrtowc(&wc, pS, n, pSt);

    if (r == (size_t)-1 || r == (size_t)-2) {
        return r;
    }
    if (pC32 != NULL) {
        *pC32 = (char32_t)wc;
    }
    return r;
}

size_t
mbrtoc16(char16_t *pC16, const char *pS, size_t n, mbstate_t *pSt)
{
    char32_t c32 = 0;
    size_t r = mbrtoc32(&c32, pS, n, pSt);

    if (r == (size_t)-1 || r == (size_t)-2) {
        return r;
    }
    if (pC16 != NULL) {
        *pC16 = (char16_t)(c32 & 0xffffu);
    }
    return r;
}

/* ---- arch_prctl (clone lives in linux_sys_more.c) ----------------------- */

#define NR_arch_prctl 158

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;
    long ret;

    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory");
    return ret;
}

static long
sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

int
arch_prctl(int nCode, unsigned long uAddr)
{
    return (int)sys_ret(sys6(NR_arch_prctl, nCode, (long)uAddr, 0, 0, 0, 0));
}

int
__arch_prctl(int nCode, unsigned long uAddr)
{
    return arch_prctl(nCode, uAddr);
}

/* Alias for clone from linux_sys_more.c */
int clone(int (*fn)(void *), void *pStack, int nFlags, void *pArg, ...);

int
__clone(int (*pfn)(void *), void *pStack, int nFlags, void *pArg, ...)
{
    va_list ap;
    pid_t *pPtid = NULL;
    void *pTls = NULL;
    pid_t *pCtid = NULL;

    va_start(ap, pArg);
    pPtid = va_arg(ap, pid_t *);
    pTls = va_arg(ap, void *);
    pCtid = va_arg(ap, pid_t *);
    va_end(ap);
    return clone(pfn, pStack, nFlags, pArg, pPtid, pTls, pCtid);
}

/* ---- cmsg / bindresvport / hostid --------------------------------------- */

struct cmsghdr *
__cmsg_nxthdr(struct msghdr *pMsg, struct cmsghdr *pCmsg)
{
    if (pMsg == NULL) {
        return NULL;
    }
    if (pCmsg == NULL) {
        return CMSG_FIRSTHDR(pMsg);
    }
    return CMSG_NXTHDR(pMsg, pCmsg);
}

int
bindresvport(int nFd, struct sockaddr_in *pSin)
{
    struct sockaddr_in sin;
    int port;

    if (pSin == NULL) {
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        pSin = &sin;
    }
    for (port = 600; port < 1024; port++) {
        pSin->sin_port = htons((uint16_t)port);
        if (bind(nFd, (struct sockaddr *)pSin, sizeof(*pSin)) == 0) {
            return 0;
        }
        if (errno != EADDRINUSE) {
            return -1;
        }
    }
    errno = EAGAIN;
    return -1;
}

long
gethostid(void)
{
    /* Bring-up constant; product may override via sethostid */
    return 0x7f010001L;
}

int
sethostid(long nId)
{
    (void)nId;
    errno = EPERM;
    return -1;
}

/* ---- memfrob / strfry --------------------------------------------------- */

void *
memfrob(void *p, size_t cb)
{
    unsigned char *q = (unsigned char *)p;
    size_t i;

    if (p == NULL) {
        return NULL;
    }
    for (i = 0; i < cb; i++) {
        q[i] ^= 42u;
    }
    return p;
}

char *
strfry(char *sz)
{
    size_t n;
    size_t i;

    if (sz == NULL) {
        return NULL;
    }
    n = strlen(sz);
    for (i = 0; i + 1 < n; i++) {
        size_t j = i + (size_t)(rand() % (int)(n - i));
        char t = sz[i];

        sz[i] = sz[j];
        sz[j] = t;
    }
    return sz;
}

/* ---- sched underscored aliases ------------------------------------------ */

int
__sched_yield(void)
{
    return sched_yield();
}

/* ---- adjtimex alias ----------------------------------------------------- */

struct timex; /* full in sys/timex.h */

int adjtimex(struct timex *pBuf);
int
__adjtimex(struct timex *pBuf)
{
    return adjtimex(pBuf);
}
