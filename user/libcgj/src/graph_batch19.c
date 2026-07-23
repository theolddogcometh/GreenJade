/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19: LFS preadv64/pwritev64/llseek/scandirat64,
 * mktemp/mkostemps64, pthread concurrency/kind_np/stackaddr/sigmask_np,
 * DNS ns_name_pack/unpack/skip + res_*ok, ntp_gettimex, lio_listio,
 * getaddrinfo_a family, profiling stubs, modify_ldt/remap_file_pages,
 * inet6/nsap helpers, chflags, strtoimax_l, re_match_2/re_search_2.
 * Integer/pointer only (no SSE doubles).
 */
#include <aio.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <regex.h>
#include <resolv.h>
#include <setjmp.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#ifndef loff_t
typedef long long loff_t;
#endif

/* Existing symbols we wrap / alias */
ssize_t preadv(int nFd, const struct iovec *pIov, int nIovcnt, off_t off);
ssize_t pwritev(int nFd, const struct iovec *pIov, int nIovcnt, off_t off);
ssize_t preadv2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
                int nFlags);
ssize_t pwritev2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
                 int nFlags);
off_t lseek64(int nFd, off_t off, int nWhence);
int scandirat(int nDfd, const char *szPath, struct dirent ***ppList,
              int (*pfnFilter)(const struct dirent *),
              int (*pfnCmp)(const struct dirent **, const struct dirent **));
int mkostemps(char *szTemplate, int nSuffix, int nFlags);
void longjmp(jmp_buf env, int nVal) __attribute__((noreturn));
int ptsname_r(int nFd, char *szBuf, size_t cb);
void obstack_free(void *pH, void *pObj);
int dn_comp(const char *szExp, unsigned char *pComp, int nComp,
            unsigned char **ppDnptrs, unsigned char **ppLastdnptr);
int dn_skipname(const unsigned char *pComp, const unsigned char *pEom);
int ns_name_pton(const char *szSrc, unsigned char *pDst, size_t cbDst);
int ns_name_ntop(const unsigned char *pSrc, char *szDst, size_t cbDst);
int ns_name_uncompress(const unsigned char *pMsg, const unsigned char *pEom,
                       const unsigned char *pSrc, char *szDst, size_t cbDst);
int re_search(void *pBuf, const char *sz, int nSize, int nStart, int nRange,
              void *pRegs);
int re_match(void *pBuf, const char *sz, int nSize, int nStart, void *pRegs);
int pthread_mutexattr_settype(pthread_mutexattr_t *pA, int nType);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *pA, int *pType);
int pthread_mutexattr_setrobust(pthread_mutexattr_t *pA, int nRobust);
int pthread_mutexattr_getrobust(const pthread_mutexattr_t *pA, int *pRobust);
int pthread_mutex_consistent(pthread_mutex_t *pM);
int getaddrinfo(const char *szNode, const char *szService,
                const struct addrinfo *pHints, struct addrinfo **ppRes);
void freeaddrinfo(struct addrinfo *pRes);
int clock_gettime(clockid_t clk, struct timespec *pTs);

#define NR_modify_ldt       154
#define NR_remap_file_pages 216

static long
b19_sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
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
b19_sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

/* ---- LFS / 64-bit aliases ----------------------------------------------- */

ssize_t
preadv64(int nFd, const struct iovec *pIov, int nIovcnt, off_t off)
{
    return preadv(nFd, pIov, nIovcnt, off);
}

ssize_t
pwritev64(int nFd, const struct iovec *pIov, int nIovcnt, off_t off)
{
    return pwritev(nFd, pIov, nIovcnt, off);
}

ssize_t
preadv64v2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
           int nFlags)
{
    return preadv2(nFd, pIov, nIovcnt, off, nFlags);
}

ssize_t
pwritev64v2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
            int nFlags)
{
    return pwritev2(nFd, pIov, nIovcnt, off, nFlags);
}

loff_t
llseek(int nFd, loff_t off, int nWhence)
{
    return (loff_t)lseek64(nFd, (off_t)off, nWhence);
}

loff_t
_llseek(int nFd, loff_t off, int nWhence)
{
    return llseek(nFd, off, nWhence);
}

int
scandirat64(int nDfd, const char *szPath, struct dirent ***ppList,
            int (*pfnFilter)(const struct dirent *),
            int (*pfnCmp)(const struct dirent **, const struct dirent **))
{
    return scandirat(nDfd, szPath, ppList, pfnFilter, pfnCmp);
}

/* ---- mktemp / mkostemps64 ----------------------------------------------- */

char *
mktemp(char *szTemplate)
{
    size_t n;
    size_t i;
    size_t j;
    int nFd;
    static unsigned uSeed;

    if (szTemplate == NULL) {
        errno = EINVAL;
        return NULL;
    }
    n = strlen(szTemplate);
    if (n < 6) {
        errno = EINVAL;
        return NULL;
    }
    /* Require trailing XXXXXX */
    for (i = n - 6; i < n; i++) {
        if (szTemplate[i] != 'X') {
            errno = EINVAL;
            return NULL;
        }
    }
    if (uSeed == 0u) {
        uSeed = (unsigned)getpid() ^ 0x9e3779b9u;
    }
    for (j = 0; j < 64u; j++) {
        uSeed = uSeed * 1103515245u + 12345u;
        for (i = 0; i < 6u; i++) {
            unsigned u = (uSeed >> (i * 5)) & 31u;

            szTemplate[n - 6 + i] =
                (char)((u < 10u) ? ('0' + (char)u) : ('a' + (char)(u - 10u)));
        }
        nFd = open(szTemplate, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (nFd >= 0) {
            (void)close(nFd);
            (void)unlink(szTemplate);
            return szTemplate;
        }
        if (errno != EEXIST) {
            szTemplate[0] = '\0';
            return szTemplate;
        }
    }
    szTemplate[0] = '\0';
    errno = EEXIST;
    return szTemplate;
}

int
mkostemps64(char *szTemplate, int nSuffix, int nFlags)
{
    return mkostemps(szTemplate, nSuffix, nFlags);
}

/* ---- setjmp BSD alias --------------------------------------------------- */

void
_longjmp(jmp_buf env, int nVal)
{
    longjmp(env, nVal);
}

/* ---- pthread concurrency / deprecated attr / kind_np -------------------- */

static int s_nPthreadConcurrency;

int
pthread_setconcurrency(int nLevel)
{
    if (nLevel < 0) {
        return EINVAL;
    }
    s_nPthreadConcurrency = nLevel;
    return 0;
}

int
pthread_getconcurrency(void)
{
    return s_nPthreadConcurrency;
}

int
pthread_attr_setstackaddr(pthread_attr_t *pA, void *pStack)
{
    if (pA == NULL) {
        return EINVAL;
    }
    pA->pStack = pStack;
    return 0;
}

int
pthread_attr_getstackaddr(const pthread_attr_t *pA, void **ppStack)
{
    if (pA == NULL || ppStack == NULL) {
        return EINVAL;
    }
    *ppStack = pA->pStack;
    return 0;
}

/* Historical kind_np ≡ mutex type */
int
pthread_mutexattr_setkind_np(pthread_mutexattr_t *pA, int nKind)
{
    return pthread_mutexattr_settype(pA, nKind);
}

int
pthread_mutexattr_getkind_np(const pthread_mutexattr_t *pA, int *pKind)
{
    return pthread_mutexattr_gettype(pA, pKind);
}

int
pthread_mutexattr_setrobust_np(pthread_mutexattr_t *pA, int nRobust)
{
    return pthread_mutexattr_setrobust(pA, nRobust);
}

int
pthread_mutexattr_getrobust_np(const pthread_mutexattr_t *pA, int *pRobust)
{
    return pthread_mutexattr_getrobust(pA, pRobust);
}

int
pthread_mutex_consistent_np(pthread_mutex_t *pM)
{
    return pthread_mutex_consistent(pM);
}

/* sigmask on attr: store in unused high pad of cpuset when fSigmask set via
 * affinity flag bit 0x40000000 (bring-up; full field would widen attr ABI). */
int
pthread_attr_setsigmask_np(pthread_attr_t *pA, const sigset_t *pSet)
{
    if (pA == NULL) {
        return EINVAL;
    }
    if (pSet == NULL) {
        pA->fAffinity &= ~0x40000000;
        return 0;
    }
    /* Accept and ignore mask content until attr grows a real field */
    (void)pSet;
    pA->fAffinity |= 0x40000000;
    return 0;
}

int
pthread_attr_getsigmask_np(const pthread_attr_t *pA, sigset_t *pSet)
{
    if (pA == NULL || pSet == NULL) {
        return EINVAL;
    }
    if ((pA->fAffinity & 0x40000000) == 0) {
        return ENOENT; /* glibc: no mask stored */
    }
    (void)memset(pSet, 0, sizeof(*pSet));
    return 0;
}

/* ---- DNS name pack/unpack/skip + res_*ok -------------------------------- */

int
ns_name_pack(const char *szSrc, unsigned char *pDst, int nDst,
             const unsigned char **ppDnptrs, const unsigned char **ppLastdnptr)
{
    return dn_comp(szSrc, pDst, nDst, (unsigned char **)ppDnptrs,
                   (unsigned char **)ppLastdnptr);
}

int
ns_name_unpack(const unsigned char *pMsg, const unsigned char *pEom,
               const unsigned char *pSrc, unsigned char *pDst, size_t cbDst)
{
    char aTmp[256];
    int n;

    if (pDst == NULL || cbDst == 0) {
        errno = EINVAL;
        return -1;
    }
    n = ns_name_uncompress(pMsg, pEom, pSrc, aTmp, sizeof(aTmp));
    if (n < 0) {
        return -1;
    }
    return ns_name_pton(aTmp, pDst, cbDst);
}

int
ns_name_skip(const unsigned char **pp, const unsigned char *pEom)
{
    int n;

    if (pp == NULL || *pp == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = dn_skipname(*pp, pEom);
    if (n < 0) {
        return -1;
    }
    *pp += n;
    return 0;
}

/* Hostname label rules (RFC bring-up subset) */
int
res_hnok(const char *sz)
{
    size_t n;
    size_t i;
    int fAlpha = 0;

    if (sz == NULL || sz[0] == '\0') {
        return 0;
    }
    n = strlen(sz);
    if (n > 255) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        char c = sz[i];

        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            fAlpha = 1;
            continue;
        }
        if (c >= '0' && c <= '9') {
            continue;
        }
        if (c == '-' || c == '.') {
            if (i == 0 || i + 1 == n) {
                return 0;
            }
            continue;
        }
        return 0;
    }
    (void)fAlpha;
    return 1;
}

int
res_dnok(const char *sz)
{
    return res_hnok(sz);
}

int
res_mailok(const char *sz)
{
    const char *pAt;

    if (sz == NULL || sz[0] == '\0') {
        return 0;
    }
    pAt = strchr(sz, '@');
    if (pAt == NULL) {
        return res_hnok(sz);
    }
    if (pAt == sz || pAt[1] == '\0') {
        return 0;
    }
    return res_hnok(pAt + 1);
}

int
res_ownok(const char *sz)
{
    return res_hnok(sz);
}

int
__ns_name_pack(const char *szSrc, unsigned char *pDst, int nDst,
               const unsigned char **ppDnptrs,
               const unsigned char **ppLastdnptr)
{
    return ns_name_pack(szSrc, pDst, nDst, ppDnptrs, ppLastdnptr);
}

int
__ns_name_unpack(const unsigned char *pMsg, const unsigned char *pEom,
                 const unsigned char *pSrc, unsigned char *pDst, size_t cbDst)
{
    return ns_name_unpack(pMsg, pEom, pSrc, pDst, cbDst);
}

int
__ns_name_skip(const unsigned char **pp, const unsigned char *pEom)
{
    return ns_name_skip(pp, pEom);
}

int
__res_hnok(const char *sz)
{
    return res_hnok(sz);
}

int
__res_dnok(const char *sz)
{
    return res_dnok(sz);
}

/* ---- ntp_gettimex ------------------------------------------------------- */

struct b19_ntptimeval {
    struct timeval time;
    long maxerror;
    long esterror;
    long tai;
};

int
ntp_gettimex(struct b19_ntptimeval *pNtv)
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
    pNtv->tai = 0;
    return 0;
}

/* ---- lio_listio --------------------------------------------------------- */

int
lio_listio(int nMode, struct aiocb *const pList[], int nEnt,
           struct sigevent *pSig)
{
    (void)nMode;
    (void)pList;
    (void)nEnt;
    (void)pSig;
    errno = ENOSYS;
    return -1;
}

int
lio_listio64(int nMode, struct aiocb *const pList[], int nEnt,
             struct sigevent *pSig)
{
    return lio_listio(nMode, pList, nEnt, pSig);
}

/* ---- getaddrinfo_a (sync fallback / cancel surface) --------------------- */

struct gaicb {
    const char *ar_name;
    const char *ar_service;
    const struct addrinfo *ar_request;
    struct addrinfo *ar_result;
    int __return;
    int __glibc_reserved[7];
};

int
getaddrinfo_a(int nMode, struct gaicb *pList[], int nEnt, struct sigevent *pSig)
{
    int i;

    (void)nMode;
    (void)pSig;
    if (pList == NULL || nEnt < 0) {
        errno = EINVAL;
        return EAI_SYSTEM;
    }
    /* Synchronous bring-up: resolve each request immediately */
    for (i = 0; i < nEnt; i++) {
        if (pList[i] == NULL) {
            continue;
        }
        pList[i]->ar_result = NULL;
        pList[i]->__return =
            getaddrinfo(pList[i]->ar_name, pList[i]->ar_service,
                        pList[i]->ar_request, &pList[i]->ar_result);
    }
    return 0;
}

int
gai_error(struct gaicb *pReq)
{
    if (pReq == NULL) {
        return EAI_SYSTEM;
    }
    return pReq->__return;
}

int
gai_cancel(struct gaicb *pReq)
{
    if (pReq == NULL) {
        return EAI_SYSTEM;
    }
    /* Already completed in sync path */
    return 0; /* AIO_ALLDONE-shaped success */
}

int
gai_suspend(const struct gaicb *const pList[], int nEnt,
            const struct timespec *pTimeout)
{
    (void)pList;
    (void)nEnt;
    (void)pTimeout;
    return 0;
}

/* ---- kernel odds: modify_ldt / remap_file_pages / freeres --------------- */

int
modify_ldt(int nFunc, void *pPtr, unsigned long uBytecount)
{
    return (int)b19_sys_ret(
        b19_sys6(NR_modify_ldt, nFunc, (long)(uintptr_t)pPtr, (long)uBytecount,
                 0, 0, 0));
}

int
remap_file_pages(void *pStart, size_t cb, int nProt, size_t nPgoff, int nFlags)
{
    return (int)b19_sys_ret(b19_sys6(NR_remap_file_pages, (long)(uintptr_t)pStart,
                                     (long)cb, nProt, (long)nPgoff, nFlags, 0));
}

void
__libc_freeres(void)
{
    /* Nothing pooled for host free-on-exit in bring-up */
}

void
_obstack_free(void *pH, void *pObj)
{
    obstack_free(pH, pObj);
}

/* ---- profiling / instrumentation stubs ---------------------------------- */

void
__fentry__(void)
{
}

void
mcount(void)
{
}

void
_mcount(void)
{
}

void
__cyg_profile_func_enter(void *pThis, void *pCall)
{
    (void)pThis;
    (void)pCall;
}

void
__cyg_profile_func_exit(void *pThis, void *pCall)
{
    (void)pThis;
    (void)pCall;
}

void
_dl_mcount_wrapper(void *p)
{
    (void)p;
}

void
_dl_mcount_wrapper_check(void *p)
{
    (void)p;
}

/* ---- GNU regex range variants ------------------------------------------- */

int
re_search_2(void *pBuf, const char *sz1, int nSize1, const char *sz2, int nSize2,
            int nStart, int nRange, void *pRegs, int nStop)
{
    char *pCat;
    int nTotal;
    int n;

    (void)nStop;
    if (pBuf == NULL) {
        return -2;
    }
    if (nSize1 < 0 || nSize2 < 0) {
        return -2;
    }
    if (nSize2 == 0) {
        return re_search(pBuf, sz1, nSize1, nStart, nRange, pRegs);
    }
    if (nSize1 == 0) {
        return re_search(pBuf, sz2, nSize2, nStart, nRange, pRegs);
    }
    nTotal = nSize1 + nSize2;
    pCat = (char *)malloc((size_t)nTotal + 1u);
    if (pCat == NULL) {
        return -2;
    }
    if (nSize1 > 0 && sz1 != NULL) {
        (void)memcpy(pCat, sz1, (size_t)nSize1);
    }
    if (nSize2 > 0 && sz2 != NULL) {
        (void)memcpy(pCat + nSize1, sz2, (size_t)nSize2);
    }
    pCat[nTotal] = '\0';
    n = re_search(pBuf, pCat, nTotal, nStart, nRange, pRegs);
    free(pCat);
    return n;
}

int
re_match_2(void *pBuf, const char *sz1, int nSize1, const char *sz2, int nSize2,
           int nStart, void *pRegs, int nStop)
{
    return re_search_2(pBuf, sz1, nSize1, sz2, nSize2, nStart, 0, pRegs, nStop);
}

/* ---- fortify / ptsname -------------------------------------------------- */

int
__ptsname_r_chk(int nFd, char *szBuf, size_t cb, size_t cbBos)
{
    if (szBuf == NULL || cb == 0) {
        errno = EINVAL;
        return errno;
    }
    if (cbBos != (size_t)-1 && cb > cbBos) {
        abort();
    }
    return ptsname_r(nFd, szBuf, cb);
}

/* ---- multicast source filter stubs -------------------------------------- */

int
getipv4sourcefilter(int nFd, struct in_addr iface, struct in_addr group,
                    uint32_t *pFmode, uint32_t *pNumsrc, struct in_addr *pSlist)
{
    (void)nFd;
    (void)iface;
    (void)group;
    (void)pSlist;
    if (pFmode != NULL) {
        *pFmode = 0;
    }
    if (pNumsrc != NULL) {
        *pNumsrc = 0;
    }
    errno = ENOSYS;
    return -1;
}

int
setipv4sourcefilter(int nFd, struct in_addr iface, struct in_addr group,
                    uint32_t uFmode, uint32_t uNumsrc,
                    const struct in_addr *pSlist)
{
    (void)nFd;
    (void)iface;
    (void)group;
    (void)uFmode;
    (void)uNumsrc;
    (void)pSlist;
    errno = ENOSYS;
    return -1;
}

/* ---- inet6 routing header / option surface (stubs) ---------------------- */

int
inet6_opt_append(void *pExtbuf, socklen_t nExtlen, int nOffset, uint8_t uType,
                 socklen_t nLen, uint8_t uAlign, void **ppDatabuf)
{
    (void)pExtbuf;
    (void)nExtlen;
    (void)nOffset;
    (void)uType;
    (void)nLen;
    (void)uAlign;
    if (ppDatabuf != NULL) {
        *ppDatabuf = NULL;
    }
    errno = ENOSYS;
    return -1;
}

int
inet6_opt_finish(void *pExtbuf, socklen_t nExtlen, int nOffset)
{
    (void)pExtbuf;
    (void)nExtlen;
    (void)nOffset;
    errno = ENOSYS;
    return -1;
}

int
inet6_opt_set_val(void *pDatabuf, int nOffset, void *pVal, socklen_t nVallen)
{
    (void)pDatabuf;
    (void)nOffset;
    (void)pVal;
    (void)nVallen;
    errno = ENOSYS;
    return -1;
}

int
inet6_opt_next(void *pExtbuf, socklen_t nExtlen, int nOffset, uint8_t *pType,
               socklen_t *pLen, void **ppDatabuf)
{
    (void)pExtbuf;
    (void)nExtlen;
    (void)nOffset;
    (void)pType;
    (void)pLen;
    if (ppDatabuf != NULL) {
        *ppDatabuf = NULL;
    }
    return -1;
}

int
inet6_opt_find(void *pExtbuf, socklen_t nExtlen, int nOffset, uint8_t uType,
               socklen_t *pLen, void **ppDatabuf)
{
    (void)pExtbuf;
    (void)nExtlen;
    (void)nOffset;
    (void)uType;
    (void)pLen;
    if (ppDatabuf != NULL) {
        *ppDatabuf = NULL;
    }
    return -1;
}

int
inet6_opt_get_val(void *pDatabuf, int nOffset, void *pVal, socklen_t nVallen)
{
    (void)pDatabuf;
    (void)nOffset;
    (void)pVal;
    (void)nVallen;
    errno = ENOSYS;
    return -1;
}

socklen_t
inet6_rth_space(int nType, int nSegments)
{
    (void)nType;
    (void)nSegments;
    return 0;
}

void *
inet6_rth_add(void *pBp, const struct in6_addr *pAddr)
{
    (void)pBp;
    (void)pAddr;
    return NULL;
}

int
inet6_rth_reverse(const void *pIn, void *pOut)
{
    (void)pIn;
    (void)pOut;
    errno = ENOSYS;
    return -1;
}

int
inet6_rth_segments(const void *pBp)
{
    (void)pBp;
    return -1;
}

struct in6_addr *
inet6_rth_getaddr(const void *pBp, int nIndex)
{
    (void)pBp;
    (void)nIndex;
    return NULL;
}

/* ---- NSAP string convert (hex) ------------------------------------------ */

unsigned char *
inet_nsap_addr(const char *szAscii, unsigned char *pAddr, int nLen)
{
    int nOut = 0;
    int nNibble = -1;
    const char *p;

    if (szAscii == NULL || pAddr == NULL || nLen <= 0) {
        return NULL;
    }
    for (p = szAscii; *p != '\0' && nOut < nLen; p++) {
        int nDig;

        if (*p == '.' || *p == ' ' || *p == '\t') {
            continue;
        }
        if (*p >= '0' && *p <= '9') {
            nDig = *p - '0';
        } else if (*p >= 'a' && *p <= 'f') {
            nDig = *p - 'a' + 10;
        } else if (*p >= 'A' && *p <= 'F') {
            nDig = *p - 'A' + 10;
        } else {
            return NULL;
        }
        if (nNibble < 0) {
            nNibble = nDig;
        } else {
            pAddr[nOut++] = (unsigned char)((nNibble << 4) | nDig);
            nNibble = -1;
        }
    }
    if (nNibble >= 0) {
        return NULL;
    }
    return pAddr;
}

char *
inet_nsap_ntoa(int nLen, const unsigned char *pAddr, char *szAscii)
{
    static char aBuf[64];
    char *p;
    int i;
    static const char aHex[] = "0123456789abcdef";

    if (pAddr == NULL || nLen < 0) {
        return NULL;
    }
    if (szAscii == NULL) {
        szAscii = aBuf;
    }
    p = szAscii;
    for (i = 0; i < nLen && i < 20; i++) {
        if (i != 0) {
            *p++ = '.';
        }
        *p++ = aHex[(pAddr[i] >> 4) & 0xf];
        *p++ = aHex[pAddr[i] & 0xf];
    }
    *p = '\0';
    return szAscii;
}

/* ---- BSD chflags (Linux has no direct equivalent) ----------------------- */

int
chflags(const char *szPath, unsigned long uFlags)
{
    (void)szPath;
    (void)uFlags;
    errno = ENOSYS;
    return -1;
}

int
fchflags(int nFd, unsigned long uFlags)
{
    (void)nFd;
    (void)uFlags;
    errno = ENOSYS;
    return -1;
}

/* ---- locale-aware intmax (ignore locale; parse same as strtoimax) ------- */

intmax_t
strtoimax_l(const char *sz, char **ppEnd, int nBase, void *loc)
{
    (void)loc;
    return strtoimax(sz, ppEnd, nBase);
}

uintmax_t
strtoumax_l(const char *sz, char **ppEnd, int nBase, void *loc)
{
    (void)loc;
    return strtoumax(sz, ppEnd, nBase);
}

/* ---- more underscored aliases ------------------------------------------- */

ssize_t
__preadv64(int nFd, const struct iovec *pIov, int nIovcnt, off_t off)
{
    return preadv64(nFd, pIov, nIovcnt, off);
}

ssize_t
__pwritev64(int nFd, const struct iovec *pIov, int nIovcnt, off_t off)
{
    return pwritev64(nFd, pIov, nIovcnt, off);
}

loff_t
__llseek(int nFd, loff_t off, int nWhence)
{
    return llseek(nFd, off, nWhence);
}

char *
__mktemp(char *szTemplate)
{
    return mktemp(szTemplate);
}

void
__libc_freeres_fn_section(void)
{
    __libc_freeres();
}

int
__modify_ldt(int nFunc, void *pPtr, unsigned long uBytecount)
{
    return modify_ldt(nFunc, pPtr, uBytecount);
}
