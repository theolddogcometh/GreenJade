/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21: underscored aliases for path/cred/sys/ipc
 * helpers, wchar fortify, obstack printf fortify, nss/resolv stubs,
 * exit_group/sigprocmask aliases. Integer/pointer only (no SSE doubles).
 */
#include <errno.h>
#include <fcntl.h>
#include <obstack.h>
#include <sched.h>
#include <signal.h>
#include <spawn.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

void __chk_fail(void);

/* Bases not always visible from headers (graph-local decls). */
void *memrchr(const void *p, int c, size_t cb);
char *strchrnul(const char *sz, int ch);
char *strsep(char **ppString, const char *szDelim);
ssize_t copy_file_range(int nFdin, off_t *pOffIn, int nFdout, off_t *pOffOut,
                        size_t cb, unsigned nFlags);
int close_range(unsigned nFirst, unsigned nLast, int nFlags);
int getentropy(void *pBuf, size_t cb);
ssize_t pread64(int nFd, void *pBuf, size_t cb, off_t off);
ssize_t pwrite64(int nFd, const void *pBuf, size_t cb, off_t off);
off_t lseek64(int nFd, off_t off, int nWhence);
int euidaccess(const char *szPath, int nMode);
int tkill(int nTid, int nSig);
int tgkill(int nTgid, int nTid, int nSig);
void exit_group(int nStatus);
int res_mkquery(int nOp, const char *szDname, int nClass, int nType,
                const unsigned char *pData, int nDatalen,
                const unsigned char *pNew, unsigned char *pBuf, int nBuflen);
int res_send(const unsigned char *pMsg, int nMsglen, unsigned char *pAns,
             int nAnslen);
int res_nquery(void *pState, const char *szDname, int nClass, int nType,
               unsigned char *pAns, int nAnslen);
char *getwd(char *szBuf);

/* ---- string underscored ------------------------------------------------- */

void *
__memrchr(const void *p, int c, size_t cb)
{
    return memrchr(p, c, cb);
}

char *
__strchrnul(const char *sz, int ch)
{
    return strchrnul(sz, ch);
}

char *
__strsep(char **ppString, const char *szDelim)
{
    return strsep(ppString, szDelim);
}

/* ---- path / fd / mm underscored ---------------------------------------- */

ssize_t
__copy_file_range(int nFdin, off_t *pOffIn, int nFdout, off_t *pOffOut,
                  size_t cb, unsigned nFlags)
{
    return copy_file_range(nFdin, pOffIn, nFdout, pOffOut, cb, nFlags);
}

int
__close_range(unsigned nFirst, unsigned nLast, int nFlags)
{
    return close_range(nFirst, nLast, nFlags);
}

int
__getentropy(void *pBuf, size_t cb)
{
    return getentropy(pBuf, cb);
}

ssize_t
__getrandom(void *pBuf, size_t cb, unsigned nFlags)
{
    return getrandom(pBuf, cb, nFlags);
}

ssize_t
__pread64(int nFd, void *pBuf, size_t cb, off_t off)
{
    return pread64(nFd, pBuf, cb, off);
}

ssize_t
__pwrite64(int nFd, const void *pBuf, size_t cb, off_t off)
{
    return pwrite64(nFd, pBuf, cb, off);
}

off_t
__lseek(int nFd, off_t off, int nWhence)
{
    return lseek(nFd, off, nWhence);
}

off_t
__lseek64(int nFd, off_t off, int nWhence)
{
    return lseek64(nFd, off, nWhence);
}

int
__euidaccess(const char *szPath, int nMode)
{
    return euidaccess(szPath, nMode);
}

int
__chmod(const char *szPath, mode_t mode)
{
    return chmod(szPath, mode);
}

int
__fchmod(int nFd, mode_t mode)
{
    return fchmod(nFd, mode);
}

int
__chown(const char *szPath, uid_t uid, gid_t gid)
{
    return chown(szPath, uid, gid);
}

int
__fchown(int nFd, uid_t uid, gid_t gid)
{
    return fchown(nFd, uid, gid);
}

int
__link(const char *szOld, const char *szNew)
{
    return link(szOld, szNew);
}

int
__symlink(const char *szTarget, const char *szLink)
{
    return symlink(szTarget, szLink);
}

void *
__mremap(void *pOld, size_t cbOld, size_t cbNew, int nFlags, ...)
{
    va_list ap;
    void *pNew = NULL;

    if ((nFlags & MREMAP_FIXED) != 0) {
        va_start(ap, nFlags);
        pNew = va_arg(ap, void *);
        va_end(ap);
        return mremap(pOld, cbOld, cbNew, nFlags, pNew);
    }
    return mremap(pOld, cbOld, cbNew, nFlags);
}

int
__msync(void *pAddr, size_t cb, int nFlags)
{
    return msync(pAddr, cb, nFlags);
}

int
__mlock(const void *pAddr, size_t cb)
{
    return mlock(pAddr, cb);
}

int
__munlock(const void *pAddr, size_t cb)
{
    return munlock(pAddr, cb);
}

int
__mlockall(int nFlags)
{
    return mlockall(nFlags);
}

int
__munlockall(void)
{
    return munlockall();
}

int
__mincore(void *pAddr, size_t cb, unsigned char *pVec)
{
    return mincore(pAddr, cb, pVec);
}

/* ---- credentials / process --------------------------------------------- */

pid_t
__getppid(void)
{
    return getppid();
}

uid_t
__getuid(void)
{
    return getuid();
}

gid_t
__getgid(void)
{
    return getgid();
}

uid_t
__geteuid(void)
{
    return geteuid();
}

gid_t
__getegid(void)
{
    return getegid();
}

int
__setuid(uid_t uid)
{
    return setuid(uid);
}

int
__setgid(gid_t gid)
{
    return setgid(gid);
}

int
__kill(pid_t pid, int nSig)
{
    return kill(pid, nSig);
}

int
__tkill(int nTid, int nSig)
{
    return tkill(nTid, nSig);
}

int
__tgkill(int nTgid, int nTid, int nSig)
{
    return tgkill(nTgid, nTid, nSig);
}

int
__raise(int nSig)
{
    return raise(nSig);
}

int
__sigprocmask(int nHow, const sigset_t *pSet, sigset_t *pOld)
{
    return sigprocmask(nHow, pSet, pOld);
}

void
__exit_group(int nStatus)
{
    exit_group(nStatus);
}

void
__exit(int nStatus)
{
    _exit(nStatus);
}

int
__posix_spawn(pid_t *pPid, const char *szPath,
              const posix_spawn_file_actions_t *pFa,
              const posix_spawnattr_t *pAttr, char *const aArgv[],
              char *const aEnvp[])
{
    return posix_spawn(pPid, szPath, pFa, pAttr, aArgv, aEnvp);
}

int
__posix_spawnp(pid_t *pPid, const char *szFile,
               const posix_spawn_file_actions_t *pFa,
               const posix_spawnattr_t *pAttr, char *const aArgv[],
               char *const aEnvp[])
{
    return posix_spawnp(pPid, szFile, pFa, pAttr, aArgv, aEnvp);
}

int
__execvpe(const char *szFile, char *const aArgv[], char *const aEnvp[])
{
    return execvpe(szFile, aArgv, aEnvp);
}

/* ---- time / fs / resource ---------------------------------------------- */

clock_t
__times(struct tms *pBuf)
{
    return times(pBuf);
}

int
__sysinfo(struct sysinfo *pInfo)
{
    return sysinfo(pInfo);
}

int
__mount(const char *szSrc, const char *szTarget, const char *szType,
        unsigned long uFlags, const void *pData)
{
    return mount(szSrc, szTarget, szType, uFlags, pData);
}

int
__umount(const char *szTarget)
{
    return umount(szTarget);
}

int
__umount2(const char *szTarget, int nFlags)
{
    return umount2(szTarget, nFlags);
}

void
__sync(void)
{
    sync();
}

int
__fsync(int nFd)
{
    return fsync(nFd);
}

int
__fdatasync(int nFd)
{
    return fdatasync(nFd);
}

int
__truncate(const char *szPath, off_t cb)
{
    return truncate(szPath, cb);
}

int
__ftruncate(int nFd, off_t cb)
{
    return ftruncate(nFd, cb);
}

int
__flock(int nFd, int nOp)
{
    return flock(nFd, nOp);
}

int
__setrlimit(int nResource, const struct rlimit *pLim)
{
    return setrlimit(nResource, pLim);
}

int
__getrusage(int nWho, struct rusage *pUsage)
{
    return getrusage(nWho, pUsage);
}

int
__getpriority(int nWhich, id_t nWho)
{
    return getpriority(nWhich, nWho);
}

int
__setpriority(int nWhich, id_t nWho, int nPrio)
{
    return setpriority(nWhich, nWho, nPrio);
}

int
__sched_setaffinity(pid_t pid, size_t cbSet, const cpu_set_t *pSet)
{
    return sched_setaffinity(pid, cbSet, pSet);
}

int
__sched_getaffinity(pid_t pid, size_t cbSet, cpu_set_t *pSet)
{
    return sched_getaffinity(pid, cbSet, pSet);
}

size_t
__confstr(int nName, char *szBuf, size_t cb)
{
    return confstr(nName, szBuf, cb);
}

long
__pathconf(const char *szPath, int nName)
{
    return pathconf(szPath, nName);
}

long
__fpathconf(int nFd, int nName)
{
    return fpathconf(nFd, nName);
}

/* ---- env / login ------------------------------------------------------- */

int
__setenv(const char *szName, const char *szValue, int nOverwrite)
{
    return setenv(szName, szValue, nOverwrite);
}

int
__unsetenv(const char *szName)
{
    return unsetenv(szName);
}

int
__putenv(char *szString)
{
    return putenv(szString);
}

int
__clearenv(void)
{
    return clearenv();
}

int
__getlogin_r(char *szBuf, size_t cb)
{
    return getlogin_r(szBuf, cb);
}

/* ---- wchar fortify ----------------------------------------------------- */

wchar_t *
__wcscpy_chk(wchar_t *pDst, const wchar_t *szSrc, size_t cDst)
{
    size_t n;

    if (szSrc == NULL) {
        return wcscpy(pDst, szSrc);
    }
    n = wcsnlen(szSrc, (cDst == (size_t)-1) ? (size_t)-1 : cDst) + 1;
    if (cDst != (size_t)-1 && n > cDst) {
        __chk_fail();
    }
    return wcscpy(pDst, szSrc);
}

wchar_t *
__wcsncpy_chk(wchar_t *pDst, const wchar_t *szSrc, size_t c, size_t cDst)
{
    if (cDst != (size_t)-1 && c > cDst) {
        __chk_fail();
    }
    return wcsncpy(pDst, szSrc, c);
}

wchar_t *
__wcscat_chk(wchar_t *pDst, const wchar_t *szSrc, size_t cDst)
{
    size_t nDst;
    size_t nSrc;

    if (pDst == NULL || szSrc == NULL) {
        return wcscat(pDst, szSrc);
    }
    nDst = wcsnlen(pDst, (cDst == (size_t)-1) ? (size_t)-1 : cDst);
    nSrc = wcsnlen(szSrc, (size_t)-1);
    if (cDst != (size_t)-1 && nDst + nSrc + 1 > cDst) {
        __chk_fail();
    }
    return wcscat(pDst, szSrc);
}

wchar_t *
__wcsncat_chk(wchar_t *pDst, const wchar_t *szSrc, size_t c, size_t cDst)
{
    size_t nDst;

    if (pDst == NULL) {
        return wcsncat(pDst, szSrc, c);
    }
    nDst = wcsnlen(pDst, (cDst == (size_t)-1) ? (size_t)-1 : cDst);
    if (cDst != (size_t)-1 && nDst + c + 1 > cDst) {
        __chk_fail();
    }
    return wcsncat(pDst, szSrc, c);
}

size_t
__wcsrtombs_chk(char *pDst, const wchar_t **ppSrc, size_t cb, mbstate_t *pSt,
                size_t cbDst)
{
    if (pDst != NULL && cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return wcsrtombs(pDst, ppSrc, cb, pSt);
}

size_t
__wcstombs_chk(char *pDst, const wchar_t *szSrc, size_t cb, size_t cbDst)
{
    if (pDst != NULL && cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return wcstombs(pDst, szSrc, cb);
}

/* ---- obstack printf (fortify + plain) ---------------------------------- */

int
obstack_vprintf(struct obstack *pH, const char *szFmt, va_list ap)
{
    char aTmp[256];
    int n;
    va_list ap2;

    if (pH == NULL || szFmt == NULL) {
        errno = EINVAL;
        return -1;
    }
    va_copy(ap2, ap);
    n = vsnprintf(aTmp, sizeof(aTmp), szFmt, ap2);
    va_end(ap2);
    if (n < 0) {
        return -1;
    }
    if ((size_t)n < sizeof(aTmp)) {
        obstack_grow(pH, aTmp, (size_t)n);
        return n;
    }
    {
        char *pBig = (char *)malloc((size_t)n + 1u);

        if (pBig == NULL) {
            return -1;
        }
        n = vsnprintf(pBig, (size_t)n + 1u, szFmt, ap);
        if (n > 0) {
            obstack_grow(pH, pBig, (size_t)n);
        }
        free(pBig);
        return n;
    }
}

int
obstack_printf(struct obstack *pH, const char *szFmt, ...)
{
    va_list ap;
    int r;

    va_start(ap, szFmt);
    r = obstack_vprintf(pH, szFmt, ap);
    va_end(ap);
    return r;
}

int
__obstack_vprintf_chk(int nFlag, struct obstack *pH, const char *szFmt,
                      va_list ap)
{
    (void)nFlag;
    return obstack_vprintf(pH, szFmt, ap);
}

int
__obstack_printf_chk(int nFlag, struct obstack *pH, const char *szFmt, ...)
{
    va_list ap;
    int r;

    (void)nFlag;
    va_start(ap, szFmt);
    r = obstack_vprintf(pH, szFmt, ap);
    va_end(ap);
    return r;
}

/* ---- resolv underscored / domain query --------------------------------- */

int
__res_mkquery(int nOp, const char *szDname, int nClass, int nType,
              const unsigned char *pData, int nDatalen,
              const unsigned char *pNew, unsigned char *pBuf, int nBuflen)
{
    return res_mkquery(nOp, szDname, nClass, nType, pData, nDatalen, pNew,
                       pBuf, nBuflen);
}

int
__res_send(const unsigned char *pMsg, int nMsglen, unsigned char *pAns,
           int nAnslen)
{
    return res_send(pMsg, nMsglen, pAns, nAnslen);
}

int
res_nquerydomain(void *pState, const char *szName, const char *szDomain,
                 int nClass, int nType, unsigned char *pAns, int nAnslen)
{
    char aFqdn[256];
    size_t nName;
    size_t nDom;
    size_t i;

    (void)pState;
    if (szName == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szDomain == NULL || szDomain[0] == '\0') {
        return res_nquery(pState, szName, nClass, nType, pAns, nAnslen);
    }
    nName = 0;
    while (szName[nName] != '\0' && nName + 1u < sizeof(aFqdn)) {
        nName++;
    }
    nDom = 0;
    while (szDomain[nDom] != '\0') {
        nDom++;
    }
    if (nName + 1u + nDom + 1u > sizeof(aFqdn)) {
        errno = ENAMETOOLONG;
        return -1;
    }
    for (i = 0; i < nName; i++) {
        aFqdn[i] = szName[i];
    }
    aFqdn[nName] = '.';
    for (i = 0; i < nDom; i++) {
        aFqdn[nName + 1u + i] = szDomain[i];
    }
    aFqdn[nName + 1u + nDom] = '\0';
    return res_nquery(pState, aFqdn, nClass, nType, pAns, nAnslen);
}

int
__res_nquerydomain(void *pState, const char *szName, const char *szDomain,
                   int nClass, int nType, unsigned char *pAns, int nAnslen)
{
    return res_nquerydomain(pState, szName, szDomain, nClass, nType, pAns,
                            nAnslen);
}

/* ---- nss bring-up stubs (desktop graph nodes) -------------------------- */

int
__nss_configure_lookup(const char *szDb, const char *szService)
{
    (void)szDb;
    (void)szService;
    errno = ENOSYS;
    return -1;
}

int
__nss_database_lookup(const char *szDb, const char *szAlternate,
                      const char *szDefconfig, char **ppStatus)
{
    (void)szDb;
    (void)szAlternate;
    (void)szDefconfig;
    if (ppStatus != NULL) {
        *ppStatus = NULL;
    }
    errno = ENOSYS;
    return -1;
}

int
__nss_hostname_digits_dots(const char *szName, void *pRes, char **ppBuf,
                           size_t *pBuflen, int *pErrnop, int *pH_errnop,
                           int nFlags, void *pAf)
{
    (void)szName;
    (void)pRes;
    (void)ppBuf;
    (void)pBuflen;
    (void)pErrnop;
    (void)pH_errnop;
    (void)nFlags;
    (void)pAf;
    return 0; /* not digits-dots; continue NSS */
}

int
__nss_next(char **ppStatus, char **ppStatus2, void **ppFct, int nStatus,
           int nAll)
{
    (void)ppStatus;
    (void)ppStatus2;
    (void)ppFct;
    (void)nStatus;
    (void)nAll;
    return -1;
}

int
__nss_next2(char **ppStatus, const char *szStatus2, const char *szStatus3,
            void **ppFct, int nStatus, int nAll, int nFlags)
{
    (void)ppStatus;
    (void)szStatus2;
    (void)szStatus3;
    (void)ppFct;
    (void)nStatus;
    (void)nAll;
    (void)nFlags;
    return -1;
}

void *
__nss_lookup_function(void *pNi, const char *szFct)
{
    (void)pNi;
    (void)szFct;
    return NULL;
}

/* ---- DNS name helpers (graph) ------------------------------------------ */

int
ns_samedomain(const char *szA, const char *szB)
{
    size_t nA;
    size_t nB;
    size_t i;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    nA = 0;
    while (szA[nA] != '\0') {
        nA++;
    }
    nB = 0;
    while (szB[nB] != '\0') {
        nB++;
    }
    while (nA > 0 && szA[nA - 1] == '.') {
        nA--;
    }
    while (nB > 0 && szB[nB - 1] == '.') {
        nB--;
    }
    if (nB == 0) {
        return 1;
    }
    if (nA < nB) {
        return 0;
    }
    if (nA > nB && szA[nA - nB - 1] != '.') {
        return 0;
    }
    for (i = 0; i < nB; i++) {
        char cA = szA[nA - nB + i];
        char cB = szB[i];

        if (cA >= 'A' && cA <= 'Z') {
            cA = (char)(cA - 'A' + 'a');
        }
        if (cB >= 'A' && cB <= 'Z') {
            cB = (char)(cB - 'A' + 'a');
        }
        if (cA != cB) {
            return 0;
        }
    }
    return 1;
}

int
ns_subdomain(const char *szA, const char *szB)
{
    size_t nA;
    size_t nB;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    if (!ns_samedomain(szA, szB)) {
        return 0;
    }
    nA = 0;
    while (szA[nA] != '\0') {
        nA++;
    }
    nB = 0;
    while (szB[nB] != '\0') {
        nB++;
    }
    while (nA > 0 && szA[nA - 1] == '.') {
        nA--;
    }
    while (nB > 0 && szB[nB - 1] == '.') {
        nB--;
    }
    return (nA > nB) ? 1 : 0;
}

int
ns_makecanon(const char *szSrc, char *szDst, size_t cbDst)
{
    size_t n;
    size_t i;

    if (szSrc == NULL || szDst == NULL || cbDst == 0) {
        errno = EINVAL;
        return -1;
    }
    n = 0;
    while (szSrc[n] != '\0') {
        n++;
    }
    if (n + 2u > cbDst) {
        errno = ENOSPC;
        return -1;
    }
    for (i = 0; i < n; i++) {
        szDst[i] = szSrc[i];
    }
    if (n == 0 || szSrc[n - 1] != '.') {
        szDst[n++] = '.';
    }
    szDst[n] = '\0';
    return 0;
}

int
ns_samename(const char *szA, const char *szB)
{
    size_t i;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    for (i = 0;; i++) {
        char cA = szA[i];
        char cB = szB[i];

        if (cA >= 'A' && cA <= 'Z') {
            cA = (char)(cA - 'A' + 'a');
        }
        if (cB >= 'A' && cB <= 'Z') {
            cB = (char)(cB - 'A' + 'a');
        }
        if (cA == '.') {
            cA = '\0';
        }
        if (cB == '.') {
            cB = '\0';
        }
        if (cA != cB) {
            return 0;
        }
        if (cA == '\0') {
            return 1;
        }
    }
}

/* ---- getwd underscored (dangerous API surface) ------------------------- */

char *
__getwd(char *szBuf)
{
    return getwd(szBuf);
}
