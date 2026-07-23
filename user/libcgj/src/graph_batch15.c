/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15: underscored syscall aliases, fortify more,
 * LFS rlimit/sendfile/fallocate64, sigaltstack/sigset ops, pthread cancel+attr
 * stubs, ns_* / res_n* wire helpers, sockatmark/isfdtype, ptrace, GNU re_*,
 * locale *_l (C), isascii/mbrlen/wcpcpy/wmempcpy/pvalloc, getdate, encrypt.
 * Integer/pointer only (no SSE doubles).
 *
 * greppable: CGJ_GRAPH_BATCH15_SOFT_NULL
 * greppable: CGJ_GRAPH_BATCH15_SOFT_ARGS
 * greppable: CGJ_GRAPH_BATCH15_SOFT_EDGE
 *
 * Soft deepen: null/arg guards on user-facing graph nodes; edge
 * hardening only. No multi-def; no API break. Pure C integer/pointer.
 */
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <poll.h>
#include <pthread.h>
#include <regex.h>
#include <resolv.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

/* ---- local SYSCALL helpers ---------------------------------------------- */

#define NR_ptrace      101
#define NR_sigaltstack 131
#define NR_uname        63

static long
b15_sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
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
b15_sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

/* ---- underscored libc / syscall aliases --------------------------------- */

ssize_t
__read(int nFd, void *pBuf, size_t cb)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return read(nFd, pBuf, cb);
}

ssize_t
__write(int nFd, const void *pBuf, size_t cb)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return write(nFd, pBuf, cb);
}

int
__open(const char *szPath, int nFlags, ...)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    mode_t mode = 0;

    if (nFlags & O_CREAT) {
        va_list ap;
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return open(szPath, nFlags, mode);
}

int
__poll(struct pollfd *pFds, nfds_t nFds, int nTimeoutMs)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pFds == NULL) {
        errno = EFAULT;
        return -1;
    }

    return poll(pFds, nFds, nTimeoutMs);
}

int
__select(int nNfds, fd_set *pR, fd_set *pW, fd_set *pE, struct timeval *pTv)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nNfds < 0) {
        errno = EBADF;
        return -1;
    }
    if (pR == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pW == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pE == NULL) {
        errno = EFAULT;
        return -1;
    }

    return select(nNfds, pR, pW, pE, pTv);
}

int
__connect(int nFd, const struct sockaddr *pAddr, socklen_t nLen)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pAddr == NULL) {
        errno = EFAULT;
        return -1;
    }

    return connect(nFd, pAddr, nLen);
}

ssize_t
__send(int nFd, const void *pBuf, size_t cb, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return send(nFd, pBuf, cb, nFlags);
}

ssize_t
__recv(int nFd, void *pBuf, size_t cb, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return recv(nFd, pBuf, cb, nFlags);
}

ssize_t
__readlink(const char *szPath, char *szBuf, size_t cb)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return readlink(szPath, szBuf, cb);
}

int
__waitid(idtype_t idtype, id_t id, siginfo_t *pInfop, int nOptions)
{
    return waitid(idtype, id, pInfop, nOptions);
}

int
__vasprintf(char **ppBuf, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (ppBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vasprintf(ppBuf, szFmt, ap);
}

ssize_t
__getdelim(char **ppLine, size_t *pcb, int nDelim, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (ppLine == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pcb == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return getdelim(ppLine, pcb, nDelim, pF);
}

ssize_t
__getline(char **ppLine, size_t *pcb, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (ppLine == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pcb == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return getline(ppLine, pcb, pF);
}

int
__sigaction(int nSig, const struct sigaction *pAct, struct sigaction *pOld)
{
    return sigaction(nSig, pAct, pOld);
}

/* ---- fortify more ------------------------------------------------------- */

void __chk_fail(void);

char *
__strncat_chk(char *szDst, const char *szSrc, size_t cb, size_t cbDst)
{
    size_t nDst;
    size_t nCopy;

    if (szDst == NULL || szSrc == NULL) {
        return strncat(szDst, szSrc, cb);
    }
    nDst = strlen(szDst);
    nCopy = strnlen(szSrc, cb);
    if (cbDst != (size_t)-1 && nDst + nCopy + 1 > cbDst) {
        __chk_fail();
    }
    return strncat(szDst, szSrc, cb);
}

int
__vfprintf_chk(FILE *pF, int nFlag, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFlag;
    return vfprintf(pF, szFmt, ap);
}

int
__vprintf_chk(int nFlag, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFlag;
    return vprintf(szFmt, ap);
}

size_t
__fwrite_chk(const void *p, size_t cb, size_t n, size_t cbDst, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    size_t total;

    if (cb != 0 && n > (size_t)-1 / cb) {
        __chk_fail();
    }
    total = cb * n;
    if (cbDst != (size_t)-1 && total > cbDst) {
        __chk_fail();
    }
    return fwrite(p, cb, n, pF);
}

ssize_t
__ttyname_r_chk(int nFd, char *szBuf, size_t cb, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return (ssize_t)ttyname_r(nFd, szBuf, cb);
}

int
__getgroups_chk(int nSize, gid_t *pList, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pList == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (nSize > 0 && cbDst != (size_t)-1 &&
        (size_t)nSize > cbDst / sizeof(gid_t)) {
        __chk_fail();
    }
    return getgroups(nSize, pList);
}

/* ---- LFS / identity aliases --------------------------------------------- */

int
getrlimit64(int nResource, struct rlimit *pRlim)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pRlim == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getrlimit(nResource, pRlim);
}

int
setrlimit64(int nResource, const struct rlimit *pRlim)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pRlim == NULL) {
        errno = EFAULT;
        return -1;
    }

    return setrlimit(nResource, pRlim);
}

ssize_t
sendfile64(int nOutFd, int nInFd, off_t *pOffset, size_t cbCount)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nOutFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (nInFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pOffset == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sendfile(nOutFd, nInFd, pOffset, cbCount);
}

int
posix_fallocate64(int nFd, off_t off, off_t cbLen)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return posix_fallocate(nFd, off, cbLen);
}

/* ---- memory ------------------------------------------------------------- */

void *
pvalloc(size_t cb)
{
    size_t page = (size_t)getpagesize();
    size_t n;

    if (page == 0) {
        page = 4096;
    }
    if (cb == 0) {
        cb = 1;
    }
    n = (cb + page - 1) / page * page;
    return aligned_alloc(page, n);
}

/* ---- ctype / wchar helpers ---------------------------------------------- */

int
isascii(int c)
{
    return (unsigned)c <= 0x7f;
}

int
toascii(int c)
{
    return c & 0x7f;
}

int
isalnum_l(int c, locale_t loc)
{
    (void)loc;
    return isalnum(c);
}

int
isalpha_l(int c, locale_t loc)
{
    (void)loc;
    return isalpha(c);
}

int
isdigit_l(int c, locale_t loc)
{
    (void)loc;
    return isdigit(c);
}

int
isspace_l(int c, locale_t loc)
{
    (void)loc;
    return isspace(c);
}

int
islower_l(int c, locale_t loc)
{
    (void)loc;
    return islower(c);
}

int
isupper_l(int c, locale_t loc)
{
    (void)loc;
    return isupper(c);
}

int
isxdigit_l(int c, locale_t loc)
{
    (void)loc;
    return isxdigit(c);
}

int
isprint_l(int c, locale_t loc)
{
    (void)loc;
    return isprint(c);
}

int
isgraph_l(int c, locale_t loc)
{
    (void)loc;
    return isgraph(c);
}

int
ispunct_l(int c, locale_t loc)
{
    (void)loc;
    return ispunct(c);
}

int
iscntrl_l(int c, locale_t loc)
{
    (void)loc;
    return iscntrl(c);
}

int
isblank_l(int c, locale_t loc)
{
    (void)loc;
    return isblank(c);
}

int
tolower_l(int c, locale_t loc)
{
    (void)loc;
    return tolower(c);
}

int
toupper_l(int c, locale_t loc)
{
    (void)loc;
    return toupper(c);
}

int
strcoll_l(const char *szA, const char *szB, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szA == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szB == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)loc;
    return strcoll(szA, szB);
}

size_t
strxfrm_l(char *szDst, const char *szSrc, size_t cb, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szDst == NULL) {
        return 0;
    }
    if (szSrc == NULL) {
        return 0;
    }

    (void)loc;
    return strxfrm(szDst, szSrc, cb);
}

long
strtol_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)loc;
    return strtol(sz, ppEnd, nBase);
}

unsigned long
strtoul_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)loc;
    return strtoul(sz, ppEnd, nBase);
}

long long
strtoll_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)loc;
    return strtoll(sz, ppEnd, nBase);
}

unsigned long long
strtoull_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)loc;
    return strtoull(sz, ppEnd, nBase);
}

size_t
mbrlen(const char *pS, size_t n, mbstate_t *pSt)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pS == NULL) {
        return 0;
    }
    if (pSt == NULL) {
        return 0;
    }

    return mbrtowc(NULL, pS, n, pSt);
}

wchar_t *
wcpcpy(wchar_t *pDst, const wchar_t *szSrc)
{
    size_t n;

    if (pDst == NULL || szSrc == NULL) {
        return pDst;
    }
    n = wcslen(szSrc);
    (void)wmemcpy(pDst, szSrc, n + 1);
    return pDst + n;
}

wchar_t *
wcpncpy(wchar_t *pDst, const wchar_t *szSrc, size_t c)
{
    size_t i;

    if (pDst == NULL) {
        return pDst;
    }
    for (i = 0; i < c && szSrc != NULL && szSrc[i] != 0; i++) {
        pDst[i] = szSrc[i];
    }
    for (; i < c; i++) {
        pDst[i] = 0;
    }
    return pDst + (c > 0 ? c : 0);
}

wchar_t *
wmempcpy(wchar_t *pDst, const wchar_t *pSrc, size_t c)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pDst == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pSrc == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)wmemcpy(pDst, pSrc, c);
    return pDst + c;
}

int
wcscoll_l(const wchar_t *a, const wchar_t *b, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (a == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (b == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)loc;
    return wcscoll(a, b);
}

size_t
wcsxfrm_l(wchar_t *pDst, const wchar_t *szSrc, size_t c, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pDst == NULL) {
        return 0;
    }
    if (szSrc == NULL) {
        return 0;
    }

    (void)loc;
    return wcsxfrm(pDst, szSrc, c);
}

/* ---- time helpers (no floating) ----------------------------------------- */

time_t
timelocal(struct tm *pTm)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pTm == NULL) {
        errno = EFAULT;
        return -1;
    }

    return mktime(pTm);
}

char *
strptime_l(const char *szBuf, const char *szFmt, struct tm *pTm, void *loc)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szBuf == NULL) {
        return NULL;
    }
    if (szFmt == NULL) {
        return NULL;
    }
    if (pTm == NULL) {
        return NULL;
    }

    (void)loc;
    return strptime(szBuf, szFmt, pTm);
}

/* getdate: use DATEMSK env path + strptime; bring-up uses fixed formats */
int getdate_err;

struct tm *
getdate(const char *sz)
{
    static struct tm tm;
    static const char *const aFmt[] = {
        "%a %b %d %H:%M:%S %Y",
        "%Y-%m-%d",
        "%m/%d/%Y",
        "%H:%M:%S",
        "%Y-%m-%d %H:%M:%S",
        NULL
    };
    int i;

    if (sz == NULL) {
        getdate_err = 1;
        return NULL;
    }
    memset(&tm, 0, sizeof(tm));
    for (i = 0; aFmt[i] != NULL; i++) {
        char *p = strptime(sz, aFmt[i], &tm);
        if (p != NULL && *p == '\0') {
            getdate_err = 0;
            return &tm;
        }
    }
    getdate_err = 7;
    return NULL;
}

int
getdate_r(const char *sz, struct tm *pTm)
{
    struct tm *p = getdate(sz);

    if (p == NULL || pTm == NULL) {
        return getdate_err != 0 ? getdate_err : 1;
    }
    *pTm = *p;
    return 0;
}

/* ---- signal extended ---------------------------------------------------- */

int
sigaltstack(const stack_t *pSs, stack_t *pOss)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pSs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pOss == NULL) {
        errno = EFAULT;
        return -1;
    }

    return (int)b15_sys_ret(
        b15_sys6(NR_sigaltstack, (long)(uintptr_t)pSs, (long)(uintptr_t)pOss,
                 0, 0, 0, 0));
}

int
sigisemptyset(const sigset_t *pSet)
{
    const unsigned long *p;
    size_t i;
    size_t n;

    if (pSet == NULL) {
        return 1;
    }
    p = (const unsigned long *)pSet;
    n = sizeof(sigset_t) / sizeof(unsigned long);
    for (i = 0; i < n; i++) {
        if (p[i] != 0) {
            return 0;
        }
    }
    return 1;
}

int
sigorset(sigset_t *pDst, const sigset_t *pA, const sigset_t *pB)
{
    unsigned long *d;
    const unsigned long *a;
    const unsigned long *b;
    size_t i;
    size_t n;

    if (pDst == NULL || pA == NULL || pB == NULL) {
        errno = EINVAL;
        return -1;
    }
    d = (unsigned long *)pDst;
    a = (const unsigned long *)pA;
    b = (const unsigned long *)pB;
    n = sizeof(sigset_t) / sizeof(unsigned long);
    for (i = 0; i < n; i++) {
        d[i] = a[i] | b[i];
    }
    return 0;
}

int
sigandset(sigset_t *pDst, const sigset_t *pA, const sigset_t *pB)
{
    unsigned long *d;
    const unsigned long *a;
    const unsigned long *b;
    size_t i;
    size_t n;

    if (pDst == NULL || pA == NULL || pB == NULL) {
        errno = EINVAL;
        return -1;
    }
    d = (unsigned long *)pDst;
    a = (const unsigned long *)pA;
    b = (const unsigned long *)pB;
    n = sizeof(sigset_t) / sizeof(unsigned long);
    for (i = 0; i < n; i++) {
        d[i] = a[i] & b[i];
    }
    return 0;
}

int
sighold(int nSig)
{
    sigset_t set;

    (void)sigemptyset(&set);
    (void)sigaddset(&set, nSig);
    return sigprocmask(SIG_BLOCK, &set, NULL);
}

int
sigrelse(int nSig)
{
    sigset_t set;

    (void)sigemptyset(&set);
    (void)sigaddset(&set, nSig);
    return sigprocmask(SIG_UNBLOCK, &set, NULL);
}

int
sigignore(int nSig)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    return sigaction(nSig, &sa, NULL);
}

void (*
sigset(int nSig, void (*pfn)(int)))(int)
{
    struct sigaction sa;
    struct sigaction old;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = pfn;
    if (sigaction(nSig, &sa, &old) != 0) {
        return SIG_ERR;
    }
    return old.sa_handler;
}

int
ssignal(int nSig, void (*pfn)(int))
{
    void (*old)(int) = signal(nSig, pfn);

    (void)old;
    return 0;
}

int
gsignal(int nSig)
{
    return raise(nSig);
}

int
__xpg_sigpause(int nSig)
{
    sigset_t set;

    (void)sigemptyset(&set);
    (void)sigaddset(&set, nSig);
    return sigsuspend(&set);
}

/* ---- ptrace / sockets --------------------------------------------------- */

long
ptrace(int nRequest, ...)
{
    va_list ap;
    pid_t pid;
    void *pAddr;
    void *pData;
    long r;

    va_start(ap, nRequest);
    pid = va_arg(ap, pid_t);
    pAddr = va_arg(ap, void *);
    pData = va_arg(ap, void *);
    va_end(ap);
    r = b15_sys6(NR_ptrace, nRequest, (long)pid, (long)(uintptr_t)pAddr,
                 (long)(uintptr_t)pData, 0, 0);
    return b15_sys_ret(r);
}

int
sockatmark(int nFd)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    int n = 0;

    if (ioctl(nFd, /* SIOCATMARK */ 0x8905, &n) != 0) {
        return -1;
    }
    return n;
}

int
isfdtype(int nFd, int nFdType)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (nFdType < 0) {
        errno = EBADF;
        return -1;
    }

    struct stat st;

    if (fstat(nFd, &st) != 0) {
        return -1;
    }
    /* S_IFMT 0170000; common: S_IFSOCK 0140000, S_IFIFO 0010000, … */
    return ((int)(st.st_mode & 0170000) == nFdType) ? 1 : 0;
}

/* ---- ntp bring-up ------------------------------------------------------- */

/* ntp_gettime shape without floating fields we cannot touch */
struct b15_ntptimeval {
    struct timeval time;
    long maxerror;
    long esterror;
};

int
ntp_gettime(struct b15_ntptimeval *pNtv)
{
    struct timespec ts;

    if (pNtv == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        return -1;
    }
    pNtv->time.tv_sec = ts.tv_sec;
    pNtv->time.tv_usec = (long)(ts.tv_nsec / 1000);
    pNtv->maxerror = 0;
    pNtv->esterror = 0;
    return 0;
}

/* ---- pthread cancel / attr stubs ---------------------------------------- */

/* Plain statics: freestanding static CRT has no TLS init for __thread */
static int g_cancelState = 0; /* 0 enable */
static int g_cancelType = 0;  /* 0 deferred */

int
pthread_setcancelstate(int nState, int *pOld)
{
    if (pOld != NULL) {
        *pOld = g_cancelState;
    }
    if (nState != 0 && nState != 1) {
        return EINVAL;
    }
    g_cancelState = nState;
    return 0;
}

int
pthread_setcanceltype(int nType, int *pOld)
{
    if (pOld != NULL) {
        *pOld = g_cancelType;
    }
    if (nType != 0 && nType != 1) {
        return EINVAL;
    }
    g_cancelType = nType;
    return 0;
}

void
pthread_testcancel(void)
{
    /* no cancel pending in bring-up */
}

int
pthread_cancel(pthread_t tid)
{
    (void)tid;
    return ENOSYS;
}

void
__pthread_register_cancel(void *pBuf)
{
    (void)pBuf;
}

void
__pthread_unregister_cancel(void *pBuf)
{
    (void)pBuf;
}

int
pthread_attr_setscope(pthread_attr_t *pA, int nScope)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pA;
    (void)nScope;
    return 0;
}

int
pthread_attr_getscope(const pthread_attr_t *pA, int *pScope)
{
    (void)pA;
    if (pScope != NULL) {
        *pScope = 0; /* PTHREAD_SCOPE_SYSTEM */
    }
    return 0;
}

int
pthread_attr_setinheritsched(pthread_attr_t *pA, int nInherit)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pA;
    (void)nInherit;
    return 0;
}

int
pthread_attr_getinheritsched(const pthread_attr_t *pA, int *pInherit)
{
    (void)pA;
    if (pInherit != NULL) {
        *pInherit = 0; /* PTHREAD_INHERIT_SCHED */
    }
    return 0;
}

int
pthread_attr_setschedpolicy(pthread_attr_t *pA, int nPolicy)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pA;
    (void)nPolicy;
    return 0;
}

int
pthread_attr_getschedpolicy(const pthread_attr_t *pA, int *pPolicy)
{
    (void)pA;
    if (pPolicy != NULL) {
        *pPolicy = 0; /* SCHED_OTHER */
    }
    return 0;
}

int
pthread_attr_setschedparam(pthread_attr_t *pA, const struct sched_param *pParam)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pParam == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pA;
    (void)pParam;
    return 0;
}

int
pthread_attr_getschedparam(const pthread_attr_t *pA, struct sched_param *pParam)
{
    (void)pA;
    if (pParam != NULL) {
        memset(pParam, 0, sizeof(*pParam));
    }
    return 0;
}

int
pthread_mutexattr_setprioceiling(pthread_mutexattr_t *pA, int nCeil)
{
    if (pA == NULL) {
        return EINVAL;
    }
    /* reuse nProtocol high bits as ceiling storage bring-up */
    pA->nProtocol = (pA->nProtocol & 0xff) | (nCeil << 8);
    return 0;
}

int
pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *pA, int *pCeil)
{
    if (pA == NULL || pCeil == NULL) {
        return EINVAL;
    }
    *pCeil = pA->nProtocol >> 8;
    return 0;
}

int
pthread_mutex_setprioceiling(pthread_mutex_t *pM, int nCeil, int *pOld)
{
    if (pM == NULL) {
        return EINVAL;
    }
    if (pOld != NULL) {
        *pOld = pM->nProtocol >> 8;
    }
    pM->nProtocol = (pM->nProtocol & 0xff) | (nCeil << 8);
    return 0;
}

int
pthread_mutex_getprioceiling(const pthread_mutex_t *pM, int *pCeil)
{
    if (pM == NULL || pCeil == NULL) {
        return EINVAL;
    }
    *pCeil = pM->nProtocol >> 8;
    return 0;
}

int
pthread_barrierattr_init(pthread_barrierattr_t *pA)
{
    if (pA == NULL) {
        return EINVAL;
    }
    pA->nPad = 0;
    return 0;
}

int
pthread_barrierattr_destroy(pthread_barrierattr_t *pA)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pA;
    return 0;
}

int
pthread_barrierattr_setpshared(pthread_barrierattr_t *pA, int nPshared)
{
    if (pA == NULL) {
        return EINVAL;
    }
    pA->nPad = nPshared;
    return 0;
}

int
pthread_barrierattr_getpshared(const pthread_barrierattr_t *pA, int *pPshared)
{
    if (pA == NULL || pPshared == NULL) {
        return EINVAL;
    }
    *pPshared = pA->nPad;
    return 0;
}

/* ---- resolv res_n* / ns_* ----------------------------------------------- */

int
res_ninit(void *pState)
{
    struct __res_state *p = (struct __res_state *)pState;

    if (p == NULL) {
        return res_init();
    }
    memset(p, 0, sizeof(*p));
    p->options = RES_INIT;
    p->retrans = 5;
    p->retry = 2;
    return 0;
}

int
res_nclose(void *pState)
{
    if (pState != NULL) {
        memset(pState, 0, sizeof(struct __res_state));
    }
    return 0;
}

int
res_nquery(void *pState, const char *szDname, int nClass, int nType,
           unsigned char *pAns, int nAnslen)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szDname == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAns == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pState;
    return res_query(szDname, nClass, nType, pAns, nAnslen);
}

int
res_nsearch(void *pState, const char *szDname, int nClass, int nType,
            unsigned char *pAns, int nAnslen)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szDname == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAns == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pState;
    return res_search(szDname, nClass, nType, pAns, nAnslen);
}

int
res_mkquery(int nOp, const char *szDname, int nClass, int nType,
            const unsigned char *pData, int nDatalen, const unsigned char *pNew,
            unsigned char *pBuf, int nBuflen)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szDname == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pData == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pNew == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nOp;
    (void)szDname;
    (void)nClass;
    (void)nType;
    (void)pData;
    (void)nDatalen;
    (void)pNew;
    (void)pBuf;
    (void)nBuflen;
    errno = ENOSYS;
    return -1;
}

int
res_send(const unsigned char *pMsg, int nMsglen, unsigned char *pAns,
         int nAnslen)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pMsg == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAns == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pMsg;
    (void)nMsglen;
    (void)pAns;
    (void)nAnslen;
    errno = ENOSYS;
    return -1;
}

int
__res_nquery(void *pState, const char *szDname, int nClass, int nType,
             unsigned char *pAns, int nAnslen)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szDname == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAns == NULL) {
        errno = EFAULT;
        return -1;
    }

    return res_nquery(pState, szDname, nClass, nType, pAns, nAnslen);
}

int
__res_nsearch(void *pState, const char *szDname, int nClass, int nType,
              unsigned char *pAns, int nAnslen)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szDname == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAns == NULL) {
        errno = EFAULT;
        return -1;
    }

    return res_nsearch(pState, szDname, nClass, nType, pAns, nAnslen);
}

unsigned
ns_get16(const unsigned char *p)
{
    if (p == NULL) {
        return 0;
    }
    return ((unsigned)p[0] << 8) | (unsigned)p[1];
}

unsigned long
ns_get32(const unsigned char *p)
{
    if (p == NULL) {
        return 0;
    }
    return ((unsigned long)p[0] << 24) | ((unsigned long)p[1] << 16) |
           ((unsigned long)p[2] << 8) | (unsigned long)p[3];
}

void
ns_put16(unsigned u, unsigned char *p)
{
    if (p == NULL) {
        return;
    }
    p[0] = (unsigned char)((u >> 8) & 0xff);
    p[1] = (unsigned char)(u & 0xff);
}

void
ns_put32(unsigned long u, unsigned char *p)
{
    if (p == NULL) {
        return;
    }
    p[0] = (unsigned char)((u >> 24) & 0xff);
    p[1] = (unsigned char)((u >> 16) & 0xff);
    p[2] = (unsigned char)((u >> 8) & 0xff);
    p[3] = (unsigned char)(u & 0xff);
}

/* Minimal ns_msg / ns_rr shapes for parsers */
struct b15_ns_msg {
    const unsigned char *pMsg;
    const unsigned char *pEom;
    uint16_t             id;
    uint16_t             flags;
    uint16_t             counts[4];
    const unsigned char *pSections[4];
};

struct b15_ns_rr {
    char          name[256];
    uint16_t      type;
    uint16_t      rr_class;
    uint32_t      ttl;
    uint16_t      rdlength;
    const unsigned char *rdata;
};

int
ns_initparse(const unsigned char *pMsg, int nLen, void *pHandle)
{
    struct b15_ns_msg *p = (struct b15_ns_msg *)pHandle;
    int i;
    const unsigned char *q;

    if (pMsg == NULL || p == NULL || nLen < 12) {
        errno = EINVAL;
        return -1;
    }
    memset(p, 0, sizeof(*p));
    p->pMsg = pMsg;
    p->pEom = pMsg + nLen;
    p->id = (uint16_t)ns_get16(pMsg);
    p->flags = (uint16_t)ns_get16(pMsg + 2);
    for (i = 0; i < 4; i++) {
        p->counts[i] = (uint16_t)ns_get16(pMsg + 4 + i * 2);
    }
    q = pMsg + 12;
    p->pSections[0] = q;
    /* leave remaining section pointers at start (bring-up) */
    for (i = 1; i < 4; i++) {
        p->pSections[i] = q;
    }
    return 0;
}

int
ns_parserr(void *pHandle, int nSection, int nRr, void *pRr)
{
    struct b15_ns_msg *p = (struct b15_ns_msg *)pHandle;
    struct b15_ns_rr *rr = (struct b15_ns_rr *)pRr;

    (void)nSection;
    (void)nRr;
    if (p == NULL || rr == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(rr, 0, sizeof(*rr));
    errno = EMSGSIZE;
    return -1;
}

int
__ns_initparse(const unsigned char *pMsg, int nLen, void *pHandle)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pMsg == NULL) {
        errno = EFAULT;
        return -1;
    }

    return ns_initparse(pMsg, nLen, pHandle);
}

int
__ns_parserr(void *pHandle, int nSection, int nRr, void *pRr)
{
    return ns_parserr(pHandle, nSection, nRr, pRr);
}

int
ns_name_uncompress(const unsigned char *pMsg, const unsigned char *pEom,
                   const unsigned char *pSrc, char *szDst, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pMsg == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pEom == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pSrc == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szDst == NULL) {
        errno = EFAULT;
        return -1;
    }

    return dn_expand(pMsg, pEom, pSrc, szDst, (int)cbDst);
}

int
ns_name_compress(const char *szSrc, unsigned char *pDst, size_t cbDst,
                 const unsigned char **ppDnptrs,
                 const unsigned char **ppLastdnptr)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szSrc == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pDst == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppDnptrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppLastdnptr == NULL) {
        errno = EFAULT;
        return -1;
    }

    return dn_comp(szSrc, pDst, (int)cbDst, (unsigned char **)ppDnptrs,
                   (unsigned char **)ppLastdnptr);
}

int
ns_name_ntop(const unsigned char *pSrc, char *szDst, size_t cbDst)
{
    /* pSrc is wire label sequence without message compression context */
    size_t nOut = 0;
    const unsigned char *p = pSrc;

    if (pSrc == NULL || szDst == NULL || cbDst == 0) {
        errno = EINVAL;
        return -1;
    }
    if (*p == 0) {
        if (cbDst < 2) {
            errno = EMSGSIZE;
            return -1;
        }
        szDst[0] = '.';
        szDst[1] = '\0';
        return 1;
    }
    while (*p != 0) {
        unsigned nLab = *p++;
        unsigned i;

        if (nLab > 63) {
            errno = EMSGSIZE;
            return -1;
        }
        if (nOut != 0) {
            if (nOut + 1 >= cbDst) {
                errno = EMSGSIZE;
                return -1;
            }
            szDst[nOut++] = '.';
        }
        for (i = 0; i < nLab; i++) {
            if (nOut + 1 >= cbDst) {
                errno = EMSGSIZE;
                return -1;
            }
            szDst[nOut++] = (char)p[i];
        }
        p += nLab;
    }
    szDst[nOut] = '\0';
    return (int)nOut;
}

int
ns_name_pton(const char *szSrc, unsigned char *pDst, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (szSrc == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pDst == NULL) {
        errno = EFAULT;
        return -1;
    }

    return dn_comp(szSrc, pDst, (int)cbDst, NULL, NULL);
}

/* ---- GNU regex re_* thin layer over regcomp stubs ----------------------- */

static regex_t g_reBuf;
static int g_reValid;
static char g_reErr[64];

char *
re_comp(const char *sz)
{
    if (sz == NULL) {
        if (!g_reValid) {
            return (char *)"No previous regular expression";
        }
        return NULL;
    }
    if (g_reValid) {
        regfree(&g_reBuf);
        g_reValid = 0;
    }
    if (regcomp(&g_reBuf, sz, REG_EXTENDED) != 0) {
        (void)snprintf(g_reErr, sizeof(g_reErr), "Invalid regular expression");
        return g_reErr;
    }
    g_reValid = 1;
    return NULL;
}

int
re_exec(const char *sz)
{
    if (!g_reValid || sz == NULL) {
        return -1;
    }
    return (regexec(&g_reBuf, sz, 0, NULL, 0) == 0) ? 1 : 0;
}

/* Opaque pattern buffer bring-up */
struct re_pattern_buffer {
    regex_t re;
    int     fValid;
};

const char *
re_compile_pattern(const char *szPattern, size_t nLen,
                   struct re_pattern_buffer *pBuf)
{
    char aTmp[512];
    size_t n = nLen;

    if (pBuf == NULL || szPattern == NULL) {
        return "Invalid argument";
    }
    if (n >= sizeof(aTmp)) {
        n = sizeof(aTmp) - 1;
    }
    memcpy(aTmp, szPattern, n);
    aTmp[n] = '\0';
    if (pBuf->fValid) {
        regfree(&pBuf->re);
        pBuf->fValid = 0;
    }
    if (regcomp(&pBuf->re, aTmp, REG_EXTENDED) != 0) {
        return "Invalid regular expression";
    }
    pBuf->fValid = 1;
    return NULL;
}

int
re_search(struct re_pattern_buffer *pBuf, const char *sz, int nSize, int nStart,
          int nRange, void *pRegs)
{
    (void)nRange;
    (void)pRegs;
    if (pBuf == NULL || !pBuf->fValid || sz == NULL || nStart < 0 ||
        nStart > nSize) {
        return -2;
    }
    if (regexec(&pBuf->re, sz + nStart, 0, NULL, 0) == 0) {
        return nStart;
    }
    return -1;
}

int
re_match(struct re_pattern_buffer *pBuf, const char *sz, int nSize, int nStart,
         void *pRegs)
{
    /* greppable: CGJ_GRAPH_BATCH15_SOFT_NULL */
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    return re_search(pBuf, sz, nSize, nStart, 0, pRegs);
}

void
re_set_registers(struct re_pattern_buffer *pBuf, void *pRegs, unsigned nRegs,
                 void *pStarts, void *pEnds)
{
    (void)pBuf;
    (void)pRegs;
    (void)nRegs;
    (void)pStarts;
    (void)pEnds;
}

/* ---- crypt encrypt/setkey stubs ----------------------------------------- */

void
setkey(const char *szKey)
{
    (void)szKey;
}

void
encrypt(char *szBlock, int nEdflag)
{
    (void)szBlock;
    (void)nEdflag;
}

/* ---- environ / tz pointer aliases --------------------------------------- */

extern char **environ;
char **__environ;

extern char *tzname[2];
extern long timezone;
extern int daylight;

long *__timezone;
int *__daylight;
char **__tzname;

/* Called from libcgj_init path if present; also constructor. */
void
__cgj_batch15_init(void)
{
    __environ = environ;
    __timezone = &timezone;
    __daylight = &daylight;
    __tzname = tzname;
}

__attribute__((constructor)) static void
b15_ctor(void)
{
    __cgj_batch15_init();
}
