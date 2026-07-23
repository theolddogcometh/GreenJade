/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28: malloc hooks, termcap stubs, bsearch_r,
 * wcslcpy/wcslcat, _IO_*_unlocked, GI aliases, stack_chk_local, longjmp
 * aliases, robust-list / set_thread_area / mseal / map_shadow_stack,
 * fmtcheck/fparseln/fgetwln, crypt_r stubs, thr_yield, dl/tls helpers,
 * isoc99 wide printf aliases. Integer/pointer only (no SSE doubles).
 */
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

/* Linux x86_64 NRs (public ABI) */
#define NR_set_thread_area     205
#define NR_get_robust_list     274
#define NR_set_robust_list     273
#define NR_move_pages          279
#define NR_io_pgetevents       333
#define NR_rt_tgsigqueueinfo   297
#define NR_mseal               462
#define NR_map_shadow_stack    451
#define NR_set_mempolicy_home_node 450
#define NR_listmount           458
#define NR_statmount           457
#define NR_lsm_get_self_attr   459
#define NR_lsm_set_self_attr   460
#define NR_lsm_list_modules    461
#define NR_futex               202
/* futex ops (public Linux ABI) */
#define FUTEX_REQUEUE          3

void __stack_chk_fail(void);
void longjmp(jmp_buf env, int nVal) __attribute__((noreturn));
void siglongjmp(sigjmp_buf env, int nVal) __attribute__((noreturn));
int ferror(FILE *pF);
int feof(FILE *pF);
int getc_unlocked(FILE *pF);
int putc_unlocked(int ch, FILE *pF);
int ferror_unlocked(FILE *pF);
int feof_unlocked(FILE *pF);
int open(const char *szPath, int nFlags, ...);
int close(int nFd);
ssize_t read(int nFd, void *pBuf, size_t cb);
ssize_t write(int nFd, const void *pBuf, size_t cb);
void *mmap(void *pAddr, size_t cb, int nProt, int nFlags, int nFd, off_t off);
int munmap(void *pAddr, size_t cb);
void *malloc(size_t cb);
void free(void *p);
void *calloc(size_t n, size_t cb);
void *realloc(void *p, size_t cb);
void abort(void) __attribute__((noreturn));
void exit(int nCode) __attribute__((noreturn));
void _exit(int nCode) __attribute__((noreturn));
pid_t fork(void);
int pthread_yield(void);
int __cxa_atexit(void (*fn)(void *), void *pArg, void *pDso);
int __libc_start_main(int (*fnMain)(int, char **, char **), int nArgc,
                      char **ppArgv, void (*fnInit)(void), void (*fnFini)(void),
                      void (*fnRtld)(void), void *pStackEnd);
wint_t fgetwc(FILE *pF);
int fgetc(FILE *pF);
int renameat2(int nOlddfd, const char *szOld, int nNewdfd, const char *szNew,
              unsigned int uFlags);
int closefrom(int nLowfd);
int issetugid(void);
int memfd_secret(unsigned uFlags);
ssize_t preadv2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
                int nFlags);
ssize_t pwritev2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
                 int nFlags);
int epoll_pwait2(int nEpfd, void *pEvents, int nMaxevents,
                 const struct timespec *pTimeout, const sigset_t *pSigmask);
int fsopen(const char *szFsName, unsigned uFlags);
int fsmount(int nFd, unsigned uFlags, unsigned uMsFlags);
int landlock_create_ruleset(const void *pAttr, size_t cb, uint32_t uFlags);
int io_uring_setup(unsigned nEntries, void *pParams);
int wprintf(const wchar_t *szFmt, ...);
int swprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, ...);
int vwprintf(const wchar_t *szFmt, va_list ap);
int vswprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, va_list ap);
long strtol(const char *sz, char **ppEnd, int nBase);
unsigned long strtoul(const char *sz, char **ppEnd, int nBase);

/* ---- sys helpers -------------------------------------------------------- */

static long
b28_sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
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
b28_sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

/* ---- malloc debugger hooks (classic glibc data surface) ----------------- */

void *(*__malloc_hook)(size_t cb, const void *pCaller);
void (*__free_hook)(void *p, const void *pCaller);
void *(*__realloc_hook)(void *p, size_t cb, const void *pCaller);
void *(*__memalign_hook)(size_t nAlign, size_t cb, const void *pCaller);
void (*__after_morecore_hook)(void);
void (*__malloc_initialize_hook)(void);

void
__malloc_check_init(void)
{
    /* no-op bring-up: hooks remain NULL unless the app sets them */
}

/* ---- pthread bring-up hooks --------------------------------------------- */

void
__pthread_initialize_minimal(void)
{
}

void
__libpthread_version_placeholder(void)
{
}

int
thr_yield(void)
{
    return pthread_yield();
}

/* ---- stack protector / longjmp aliases ---------------------------------- */

void
__stack_chk_fail_local(void)
{
    __stack_chk_fail();
}

void
__libc_longjmp(jmp_buf env, int nVal)
{
    longjmp(env, nVal);
}

void
__libc_siglongjmp(sigjmp_buf env, int nVal)
{
    siglongjmp(env, nVal);
}

/* ---- _IO_* unlocked ----------------------------------------------------- */

int
_IO_ferror_unlocked(FILE *pF)
{
    return ferror_unlocked(pF);
}

int
_IO_feof_unlocked(FILE *pF)
{
    return feof_unlocked(pF);
}

int
_IO_getc_unlocked(FILE *pF)
{
    return getc_unlocked(pF);
}

int
_IO_putc_unlocked(int ch, FILE *pF)
{
    return putc_unlocked(ch, pF);
}

/* ---- bsearch_r (GNU) ---------------------------------------------------- */

void *
bsearch_r(const void *pKey, const void *pBase, size_t n, size_t cb,
          int (*pfnCmp)(const void *, const void *, void *), void *pArg)
{
    size_t lo = 0;
    size_t hi = n;
    const unsigned char *p = (const unsigned char *)pBase;

    if (pKey == NULL || pBase == NULL || pfnCmp == NULL || cb == 0) {
        return NULL;
    }
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        int c = pfnCmp(pKey, p + mid * cb, pArg);

        if (c == 0) {
            return (void *)(uintptr_t)(p + mid * cb);
        }
        if (c < 0) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }
    return NULL;
}

/* ---- BSD wcslcpy / wcslcat ---------------------------------------------- */

size_t
wcslcpy(wchar_t *pDst, const wchar_t *pSrc, size_t cMax)
{
    size_t n = 0;

    if (pSrc == NULL) {
        if (pDst != NULL && cMax > 0) {
            pDst[0] = L'\0';
        }
        return 0;
    }
    while (pSrc[n] != L'\0') {
        n++;
    }
    if (pDst == NULL || cMax == 0) {
        return n;
    }
    {
        size_t i;
        size_t cCopy = (n < cMax - 1) ? n : (cMax - 1);

        for (i = 0; i < cCopy; i++) {
            pDst[i] = pSrc[i];
        }
        pDst[cCopy] = L'\0';
    }
    return n;
}

size_t
wcslcat(wchar_t *pDst, const wchar_t *pSrc, size_t cMax)
{
    size_t nDst = 0;
    size_t nSrc = 0;

    if (pDst != NULL && cMax > 0) {
        while (nDst < cMax && pDst[nDst] != L'\0') {
            nDst++;
        }
    }
    if (pSrc != NULL) {
        while (pSrc[nSrc] != L'\0') {
            nSrc++;
        }
    }
    if (pDst == NULL || cMax == 0 || nDst >= cMax) {
        return nDst + nSrc;
    }
    {
        size_t i;
        size_t cRoom = cMax - nDst - 1;
        size_t cCopy = (nSrc < cRoom) ? nSrc : cRoom;

        for (i = 0; i < cCopy; i++) {
            pDst[nDst + i] = pSrc[i];
        }
        pDst[nDst + cCopy] = L'\0';
    }
    return nDst + nSrc;
}

/* ---- termcap stubs (ncurses bring-up) ----------------------------------- */

int
tgetent(char *pBuf, const char *szName)
{
    (void)pBuf;
    (void)szName;
    /* 0 = no entry; apps often continue with hardcoded fallbacks */
    return 0;
}

int
tgetflag(const char *szId)
{
    (void)szId;
    return 0;
}

int
tgetnum(const char *szId)
{
    (void)szId;
    return -1;
}

char *
tgetstr(const char *szId, char **ppArea)
{
    (void)szId;
    (void)ppArea;
    return NULL;
}

char *
tgoto(const char *szCap, int nCol, int nRow)
{
    (void)szCap;
    (void)nCol;
    (void)nRow;
    return (char *)"";
}

int
tputs(const char *sz, int nAffcnt, int (*fnPutc)(int))
{
    const char *p;

    (void)nAffcnt;
    if (sz == NULL || fnPutc == NULL) {
        return -1;
    }
    for (p = sz; *p != '\0'; p++) {
        if (fnPutc((unsigned char)*p) < 0) {
            return -1;
        }
    }
    return 0;
}

int
setupterm(char *szTerm, int nFout, int *pErr)
{
    (void)szTerm;
    (void)nFout;
    if (pErr != NULL) {
        *pErr = 0;
    }
    errno = ENOSYS;
    return -1;
}

/* ---- BSD string helpers ------------------------------------------------- */

const char *
fmtcheck(const char *szFmt, const char *szTemplate)
{
    if (szFmt == NULL) {
        return (szTemplate != NULL) ? szTemplate : "";
    }
    if (szTemplate == NULL) {
        return szFmt;
    }
    /* Bring-up: accept caller format (full printf-spec compare not required). */
    (void)szTemplate;
    return szFmt;
}

char *
fparseln(FILE *pF, size_t *pLen, size_t *pLineno, const char aDelim[3],
         int nFlags)
{
    static char aBuf[512];
    size_t n = 0;
    int ch;

    (void)aDelim;
    (void)nFlags;
    if (pF == NULL) {
        return NULL;
    }
    if (pLineno != NULL) {
        (*pLineno)++;
    }
    while (n + 1 < sizeof(aBuf)) {
        ch = fgetc(pF);
        if (ch == EOF) {
            if (n == 0) {
                return NULL;
            }
            break;
        }
        if (ch == '\n') {
            break;
        }
        aBuf[n++] = (char)ch;
    }
    aBuf[n] = '\0';
    if (pLen != NULL) {
        *pLen = n;
    }
    return aBuf;
}

wchar_t *
fgetwln(FILE *pF, size_t *pLen)
{
    static wchar_t aBuf[256];
    size_t n = 0;
    wint_t wc;

    if (pF == NULL) {
        return NULL;
    }
    while (n + 1 < sizeof(aBuf) / sizeof(aBuf[0])) {
        wc = fgetwc(pF);
        if (wc == (wint_t)WEOF) {
            if (n == 0) {
                return NULL;
            }
            break;
        }
        if (wc == L'\n') {
            break;
        }
        aBuf[n++] = (wchar_t)wc;
    }
    aBuf[n] = L'\0';
    if (pLen != NULL) {
        *pLen = n;
    }
    return aBuf;
}

/* ---- crypt stubs (no DES; clean-room ENOSYS) ---------------------------- */

char *
fcrypt(const char *szKey, const char *szSalt)
{
    (void)szKey;
    (void)szSalt;
    errno = ENOSYS;
    return NULL;
}

void
encrypt_r(char *pBlock, int nEdflag, void *pData)
{
    (void)pBlock;
    (void)nEdflag;
    (void)pData;
    errno = ENOSYS;
}

void
setkey_r(const char *szKey, void *pData)
{
    (void)szKey;
    (void)pData;
    errno = ENOSYS;
}

/* ---- Linux thread / mm extras ------------------------------------------- */

long
set_robust_list(void *pHead, size_t cbLen)
{
    return b28_sys_ret(b28_sys6(NR_set_robust_list, (long)(uintptr_t)pHead,
                                (long)cbLen, 0, 0, 0, 0));
}

long
__set_robust_list(void *pHead, size_t cbLen)
{
    return set_robust_list(pHead, cbLen);
}

long
get_robust_list(int nPid, void **ppHead, size_t *pLen)
{
    return b28_sys_ret(b28_sys6(NR_get_robust_list, nPid,
                                (long)(uintptr_t)ppHead, (long)(uintptr_t)pLen,
                                0, 0, 0));
}

long
__get_robust_list(int nPid, void **ppHead, size_t *pLen)
{
    return get_robust_list(nPid, ppHead, pLen);
}

int
futex_requeue(volatile int *pAddr1, volatile int *pAddr2, int nWaiters,
              int nRequeue)
{
    return (int)b28_sys_ret(b28_sys6(NR_futex, (long)(uintptr_t)pAddr1,
                                     FUTEX_REQUEUE, nWaiters, 0,
                                     (long)(uintptr_t)pAddr2, nRequeue));
}

int
__futex_requeue(volatile int *pAddr1, volatile int *pAddr2, int nWaiters,
                int nRequeue)
{
    return futex_requeue(pAddr1, pAddr2, nWaiters, nRequeue);
}

long
move_pages(int nPid, unsigned long nCount, void **ppPages, const int *pNodes,
           int *pStatus, int nFlags)
{
    return b28_sys_ret(b28_sys6(NR_move_pages, nPid, (long)nCount,
                                (long)(uintptr_t)ppPages,
                                (long)(uintptr_t)pNodes,
                                (long)(uintptr_t)pStatus, nFlags));
}

long
__move_pages(int nPid, unsigned long nCount, void **ppPages, const int *pNodes,
             int *pStatus, int nFlags)
{
    return move_pages(nPid, nCount, ppPages, pNodes, pStatus, nFlags);
}

int
io_pgetevents(void *pCtx, long nMin, long nMax, void *pEvents, void *pTimeout,
              const sigset_t *pSigmask)
{
    return (int)b28_sys_ret(b28_sys6(NR_io_pgetevents, (long)(uintptr_t)pCtx,
                                     nMin, nMax, (long)(uintptr_t)pEvents,
                                     (long)(uintptr_t)pTimeout,
                                     (long)(uintptr_t)pSigmask));
}

int
__io_pgetevents(void *pCtx, long nMin, long nMax, void *pEvents, void *pTimeout,
                const sigset_t *pSigmask)
{
    return io_pgetevents(pCtx, nMin, nMax, pEvents, pTimeout, pSigmask);
}

int
rt_tgsigqueueinfo(pid_t nTgid, pid_t nTid, int nSig, void *pInfo)
{
    return (int)b28_sys_ret(b28_sys6(NR_rt_tgsigqueueinfo, nTgid, nTid, nSig,
                                     (long)(uintptr_t)pInfo, 0, 0));
}

int
consttime_memequal(const void *pA, const void *pB, size_t cb)
{
    const unsigned char *a = (const unsigned char *)pA;
    const unsigned char *b = (const unsigned char *)pB;
    unsigned char uDiff = 0;
    size_t i;

    if (cb == 0) {
        return 1;
    }
    if (a == NULL || b == NULL) {
        return 0;
    }
    for (i = 0; i < cb; i++) {
        uDiff |= (unsigned char)(a[i] ^ b[i]);
    }
    return uDiff == 0;
}

int
__consttime_memequal(const void *pA, const void *pB, size_t cb)
{
    return consttime_memequal(pA, pB, cb);
}

int
set_thread_area(void *pUinfo)
{
    return (int)b28_sys_ret(b28_sys6(NR_set_thread_area,
                                     (long)(uintptr_t)pUinfo, 0, 0, 0, 0, 0));
}

int
mseal(void *pAddr, size_t cb, unsigned long uFlags)
{
    return (int)b28_sys_ret(b28_sys6(NR_mseal, (long)(uintptr_t)pAddr,
                                     (long)cb, (long)uFlags, 0, 0, 0));
}

void *
map_shadow_stack(void *pAddr, size_t cb, unsigned int uFlags)
{
    long r = b28_sys6(NR_map_shadow_stack, (long)(uintptr_t)pAddr, (long)cb,
                      (long)uFlags, 0, 0, 0);

    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return (void *)(uintptr_t)-1;
    }
    return (void *)(uintptr_t)r;
}

int
set_mempolicy_home_node(unsigned long uStart, unsigned long uLen,
                        unsigned long uHomeNode, unsigned long uFlags)
{
    return (int)b28_sys_ret(b28_sys6(NR_set_mempolicy_home_node, (long)uStart,
                                     (long)uLen, (long)uHomeNode, (long)uFlags,
                                     0, 0));
}

int
listmount(uint64_t uMntId, uint64_t *pLast, uint64_t *pList, size_t nList,
          unsigned int uFlags)
{
    return (int)b28_sys_ret(b28_sys6(NR_listmount, (long)uMntId,
                                     (long)(uintptr_t)pLast,
                                     (long)(uintptr_t)pList, (long)nList,
                                     (long)uFlags, 0));
}

int
statmount(uint64_t uMntId, void *pReq, void *pBuf, size_t cbBuf,
          unsigned int uFlags)
{
    return (int)b28_sys_ret(b28_sys6(NR_statmount, (long)uMntId,
                                     (long)(uintptr_t)pReq,
                                     (long)(uintptr_t)pBuf, (long)cbBuf,
                                     (long)uFlags, 0));
}

int
lsm_get_self_attr(unsigned int uAttr, void *pCtx, size_t *pSize,
                  unsigned int uFlags)
{
    return (int)b28_sys_ret(b28_sys6(NR_lsm_get_self_attr, (long)uAttr,
                                     (long)(uintptr_t)pCtx,
                                     (long)(uintptr_t)pSize, (long)uFlags, 0,
                                     0));
}

int
lsm_set_self_attr(unsigned int uAttr, void *pCtx, size_t nSize,
                  unsigned int uFlags)
{
    return (int)b28_sys_ret(b28_sys6(NR_lsm_set_self_attr, (long)uAttr,
                                     (long)(uintptr_t)pCtx, (long)nSize,
                                     (long)uFlags, 0, 0));
}

int
lsm_list_modules(uint64_t *pIds, size_t *pSize, unsigned int uFlags)
{
    return (int)b28_sys_ret(b28_sys6(NR_lsm_list_modules,
                                     (long)(uintptr_t)pIds,
                                     (long)(uintptr_t)pSize, (long)uFlags, 0, 0,
                                     0));
}

/* ---- underscored aliases for already-exported public APIs --------------- */

int
__renameat2(int nOlddfd, const char *szOld, int nNewdfd, const char *szNew,
            unsigned int uFlags)
{
    return renameat2(nOlddfd, szOld, nNewdfd, szNew, uFlags);
}

int
__closefrom(int nLowfd)
{
    return closefrom(nLowfd);
}

int
__issetugid(void)
{
    return issetugid();
}

int
__mseal(void *pAddr, size_t cb, unsigned long uFlags)
{
    return mseal(pAddr, cb, uFlags);
}

void *
__map_shadow_stack(void *pAddr, size_t cb, unsigned int uFlags)
{
    return map_shadow_stack(pAddr, cb, uFlags);
}

int
__set_mempolicy_home_node(unsigned long uStart, unsigned long uLen,
                          unsigned long uHomeNode, unsigned long uFlags)
{
    return set_mempolicy_home_node(uStart, uLen, uHomeNode, uFlags);
}

int
__statmount(uint64_t uMntId, void *pReq, void *pBuf, size_t cbBuf,
            unsigned int uFlags)
{
    return statmount(uMntId, pReq, pBuf, cbBuf, uFlags);
}

int
__listmount(uint64_t uMntId, uint64_t *pLast, uint64_t *pList, size_t nList,
            unsigned int uFlags)
{
    return listmount(uMntId, pLast, pList, nList, uFlags);
}

int
__lsm_get_self_attr(unsigned int uAttr, void *pCtx, size_t *pSize,
                    unsigned int uFlags)
{
    return lsm_get_self_attr(uAttr, pCtx, pSize, uFlags);
}

int
__lsm_set_self_attr(unsigned int uAttr, void *pCtx, size_t nSize,
                    unsigned int uFlags)
{
    return lsm_set_self_attr(uAttr, pCtx, nSize, uFlags);
}

int
__lsm_list_modules(uint64_t *pIds, size_t *pSize, unsigned int uFlags)
{
    return lsm_list_modules(pIds, pSize, uFlags);
}

int
__memfd_secret(unsigned uFlags)
{
    return memfd_secret(uFlags);
}

ssize_t
__preadv2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off, int nFlags)
{
    return preadv2(nFd, pIov, nIovcnt, off, nFlags);
}

ssize_t
__pwritev2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
           int nFlags)
{
    return pwritev2(nFd, pIov, nIovcnt, off, nFlags);
}

int
__epoll_pwait2(int nEpfd, void *pEvents, int nMaxevents,
               const struct timespec *pTimeout, const sigset_t *pSigmask)
{
    return epoll_pwait2(nEpfd, pEvents, nMaxevents, pTimeout, pSigmask);
}

int
__fsopen(const char *szFsName, unsigned uFlags)
{
    return fsopen(szFsName, uFlags);
}

int
__fsmount(int nFd, unsigned uFlags, unsigned uMsFlags)
{
    return fsmount(nFd, uFlags, uMsFlags);
}

int
__landlock_create_ruleset(const void *pAttr, size_t cb, uint32_t uFlags)
{
    return landlock_create_ruleset(pAttr, cb, uFlags);
}

int
__io_uring_setup(unsigned nEntries, void *pParams)
{
    return io_uring_setup(nEntries, pParams);
}

/* ---- dynlink / TLS helpers (bring-up stubs) ----------------------------- */

struct link_map;

int
_dl_find_dso_for_object(const void *pAddr, struct link_map **ppMap)
{
    (void)pAddr;
    if (ppMap != NULL) {
        *ppMap = NULL;
    }
    return 0;
}

void
_dl_get_tls_static_info(size_t *pSize, size_t *pAlign)
{
    if (pSize != NULL) {
        *pSize = 0;
    }
    if (pAlign != NULL) {
        *pAlign = 16;
    }
}

void *
__emutls_get_address(void *pControl)
{
    /* Minimal emulated TLS: treat control block as {size, align, obj, offset*}
     * and allocate once into the pointer slot after three size_t words. */
    size_t *p = (size_t *)pControl;
    void **ppObj;

    if (p == NULL) {
        return NULL;
    }
    ppObj = (void **)(void *)(p + 3);
    if (*ppObj == NULL) {
        size_t cb = p[0];

        if (cb == 0) {
            cb = 8;
        }
        *ppObj = calloc(1, cb);
    }
    return *ppObj;
}

/* ---- isoc99 wide printf aliases ----------------------------------------- */

int
__isoc99_wprintf(const wchar_t *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vwprintf(szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_swprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vswprintf(pBuf, cLen, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_vwprintf(const wchar_t *szFmt, va_list ap)
{
    return vwprintf(szFmt, ap);
}

int
__isoc99_vswprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, va_list ap)
{
    return vswprintf(pBuf, cLen, szFmt, ap);
}

/* ---- addseverity (obsolete) --------------------------------------------- */

int
addseverity(const char *szDir)
{
    (void)szDir;
    errno = ENOSYS;
    return -1;
}

/* ---- getopt flag used by glibc getopt internals ------------------------- */

int __getopt_initialized = 0;

/* ---- strtoq internal aliases (strtoq itself lives in graph_batch3) ------ */

long strtoq(const char *sz, char **ppEnd, int nBase);
unsigned long strtouq(const char *sz, char **ppEnd, int nBase);

long
__strtoq_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return strtoq(sz, ppEnd, nBase);
}

unsigned long
__strtouq_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return strtouq(sz, ppEnd, nBase);
}

/* ---- __GI_ hidden aliases (common reloc targets) ------------------------ */

void *
__GI_memcpy(void *pDst, const void *pSrc, size_t cb)
{
    return memcpy(pDst, pSrc, cb);
}

void *
__GI_memset(void *pDst, int ch, size_t cb)
{
    return memset(pDst, ch, cb);
}

size_t
__GI_strlen(const char *sz)
{
    return strlen(sz);
}

int
__GI_memcmp(const void *pA, const void *pB, size_t cb)
{
    return memcmp(pA, pB, cb);
}

void *
__GI_memmove(void *pDst, const void *pSrc, size_t cb)
{
    return memmove(pDst, pSrc, cb);
}

int
__GI_strcmp(const char *szA, const char *szB)
{
    return strcmp(szA, szB);
}

int
__GI_strncmp(const char *szA, const char *szB, size_t n)
{
    return strncmp(szA, szB, n);
}

char *
__GI_strcpy(char *pDst, const char *pSrc)
{
    return strcpy(pDst, pSrc);
}

char *
__GI_strncpy(char *pDst, const char *pSrc, size_t n)
{
    return strncpy(pDst, pSrc, n);
}

int
__GI_open(const char *szPath, int nFlags, ...)
{
    mode_t mode = 0;
    va_list ap;

    if ((nFlags & O_CREAT) != 0) {
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return open(szPath, nFlags, mode);
}

int
__GI_close(int nFd)
{
    return close(nFd);
}

ssize_t
__GI_read(int nFd, void *pBuf, size_t cb)
{
    return read(nFd, pBuf, cb);
}

ssize_t
__GI_write(int nFd, const void *pBuf, size_t cb)
{
    return write(nFd, pBuf, cb);
}

void *
__GI_mmap(void *pAddr, size_t cb, int nProt, int nFlags, int nFd, off_t off)
{
    return mmap(pAddr, cb, nProt, nFlags, nFd, off);
}

int
__GI_munmap(void *pAddr, size_t cb)
{
    return munmap(pAddr, cb);
}

void *
__GI_malloc(size_t cb)
{
    return malloc(cb);
}

void
__GI_free(void *p)
{
    free(p);
}

void *
__GI_calloc(size_t n, size_t cb)
{
    return calloc(n, cb);
}

void *
__GI_realloc(void *p, size_t cb)
{
    return realloc(p, cb);
}

void
__GI_abort(void)
{
    abort();
}

void
__GI_exit(int nCode)
{
    exit(nCode);
}

void
__GI__exit(int nCode)
{
    _exit(nCode);
}

pid_t
__GI_fork(void)
{
    return fork();
}

int
__GI_pthread_mutex_lock(pthread_mutex_t *pM)
{
    return pthread_mutex_lock(pM);
}

int
__GI_pthread_mutex_unlock(pthread_mutex_t *pM)
{
    return pthread_mutex_unlock(pM);
}

int
__GI_pthread_create(pthread_t *pTh, const pthread_attr_t *pAttr,
                    void *(*fn)(void *), void *pArg)
{
    return pthread_create(pTh, pAttr, fn, pArg);
}

int
__GI_pthread_join(pthread_t pTh, void **ppRet)
{
    return pthread_join(pTh, ppRet);
}

int
__GI___cxa_atexit(void (*fn)(void *), void *pArg, void *pDso)
{
    return __cxa_atexit(fn, pArg, pDso);
}

int
__GI___libc_start_main(int (*fnMain)(int, char **, char **), int nArgc,
                       char **ppArgv, void (*fnInit)(void),
                       void (*fnFini)(void), void (*fnRtld)(void),
                       void *pStackEnd)
{
    return __libc_start_main(fnMain, nArgc, ppArgv, fnInit, fnFini, fnRtld,
                             pStackEnd);
}

/* ---- soft deepen: thr_yield_np + crypt_r null-safe (unique) ------------- */

int thr_yield(void);

int
thr_yield_np(void)
{
    return thr_yield();
}

int __thr_yield_np(void) __attribute__((alias("thr_yield_np")));

/* crypt_r with explicit NULL-salt rejection helper name. */
char *crypt_r(const char *szKey, const char *szSalt, void *pData);

char *
crypt_r_null(const char *szKey, const char *szSalt, void *pData)
{
    if (szKey == NULL || szSalt == NULL || pData == NULL) {
        return NULL;
    }
    return crypt_r(szKey, szSalt, pData);
}

char *__crypt_r_null(const char *szKey, const char *szSalt, void *pData)
    __attribute__((alias("crypt_r_null")));
