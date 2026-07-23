/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25: underscored stdio/string/resolv aliases,
 * pthread cleanup macros surface, __libc_alloc_buffer_*, lll futex wait
 * stubs, NSS files/dns remainder (set/end/get*ent), small-string helpers,
 * RPC/auth helpers, printf_size hooks, inet exact helpers. Integer/pointer
 * only (no SSE doubles).
 */
#include <arpa/inet.h>
#include <errno.h>
#include <grp.h>
#include <locale.h>
#include <netdb.h>
#include <pwd.h>
#include <resolv.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

/* printf_info may be absent; keep opaque for fail-closed hooks */
struct printf_info;

int snprintf(char *szBuf, size_t cb, const char *szFmt, ...);
int vsnprintf(char *szBuf, size_t cb, const char *szFmt, va_list ap);
char *strerror_r(int nErr, char *szBuf, size_t cb);
size_t strftime_l(char *szBuf, size_t cb, const char *szFmt,
                  const struct tm *pTm, locale_t loc);
int iswctype(wint_t wc, wctype_t desc);
int getopt(int argc, char *const argv[], const char *szOptstring);
int res_search(const char *szDname, int nClass, int nType, unsigned char *pAns,
               int nAnslen);
int res_nclose(void *pState);
int res_hnok(const char *szDn);
int res_dnok(const char *szDn);
int res_mailok(const char *szDn);
int res_ownok(const char *szDn);
int ns_makecanon(const char *szSrc, char *szDst, size_t cbDst);
int ns_samename(const char *szA, const char *szB);
int dn_expand(const unsigned char *pMsg, const unsigned char *pEomorig,
              const unsigned char *pComp, char *szExp, int nLen);
int dn_skipname(const unsigned char *pComp, const unsigned char *pEom);
int inet_aton(const char *sz, struct in_addr *pAddr);
int inet_pton(int nAf, const char *szSrc, void *pDst);
void *mempcpy(void *pDst, const void *pSrc, size_t cb);
char *stpcpy(char *szDst, const char *szSrc);
char *strsep(char **ppString, const char *szDelim);
int getpwent_r(struct passwd *pPwd, char *szBuf, size_t cb,
               struct passwd **ppResult);
int getgrent_r(struct group *pGrp, char *szBuf, size_t cb,
               struct group **ppResult);
void setpwent(void);
void endpwent(void);
void setgrent(void);
void endgrent(void);
int gethostbyname_r(const char *szName, struct hostent *pResult, char *szBuf,
                    size_t cb, struct hostent **ppResult, int *pHErr);
int gethostbyname2_r(const char *szName, int nAf, struct hostent *pResult,
                     char *szBuf, size_t cb, struct hostent **ppResult,
                     int *pHErr);
int gethostbyaddr_r(const void *pAddr, socklen_t nLen, int nType,
                    struct hostent *pResult, char *szBuf, size_t cb,
                    struct hostent **ppResult, int *pHErr);

#ifndef NSS_STATUS_SUCCESS
#define NSS_STATUS_SUCCESS 1
#define NSS_STATUS_NOTFOUND 0
#define NSS_STATUS_UNAVAIL -1
#define NSS_STATUS_TRYAGAIN -2
#endif

/* ---- underscored stdio / string / locale aliases ------------------------ */

int
__snprintf(char *szBuf, size_t cb, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsnprintf(szBuf, cb, szFmt, ap);
    va_end(ap);
    return n;
}

int
__vsnprintf(char *szBuf, size_t cb, const char *szFmt, va_list ap)
{
    return vsnprintf(szBuf, cb, szFmt, ap);
}

char *
__strerror_r(int nErr, char *szBuf, size_t cb)
{
    return strerror_r(nErr, szBuf, cb);
}

size_t
__strftime_l(char *szBuf, size_t cb, const char *szFmt, const struct tm *pTm,
             locale_t loc)
{
    return strftime_l(szBuf, cb, szFmt, pTm, loc);
}

int
__iswctype(wint_t wc, wctype_t desc)
{
    return iswctype(wc, desc);
}

int
__posix_getopt(int argc, char *const argv[], const char *szOptstring)
{
    return getopt(argc, argv, szOptstring);
}

/* ---- resolv underscored / libc-prefixed aliases ------------------------- */

int
__res_search(const char *szDname, int nClass, int nType, unsigned char *pAns,
             int nAnslen)
{
    return res_search(szDname, nClass, nType, pAns, nAnslen);
}

int
__res_nclose(void *pState)
{
    return res_nclose(pState);
}

int
__res_mailok(const char *szDn)
{
    return res_mailok(szDn);
}

int
__res_ownok(const char *szDn)
{
    return res_ownok(szDn);
}

int
__libc_dn_expand(const unsigned char *pMsg, const unsigned char *pEomorig,
                 const unsigned char *pComp, char *szExp, int nLen)
{
    return dn_expand(pMsg, pEomorig, pComp, szExp, nLen);
}

int
__libc_dn_skipname(const unsigned char *pComp, const unsigned char *pEom)
{
    return dn_skipname(pComp, pEom);
}

int
__libc_res_hnok(const char *szDn)
{
    return res_hnok(szDn);
}

int
__libc_res_dnok(const char *szDn)
{
    return res_dnok(szDn);
}

int
__libc_res_mailok(const char *szDn)
{
    return res_mailok(szDn);
}

int
__libc_res_ownok(const char *szDn)
{
    return res_ownok(szDn);
}

int
__libc_ns_makecanon(const char *szSrc, char *szDst, size_t cbDst)
{
    return ns_makecanon(szSrc, szDst, cbDst);
}

int
__libc_ns_samename(const char *szA, const char *szB)
{
    return ns_samename(szA, szB);
}

int
__libc_res_nameinquery(const char *szName, int nClass, int nType,
                       const unsigned char *pBuf, const unsigned char *pEom)
{
    (void)szName;
    (void)nClass;
    (void)nType;
    (void)pBuf;
    (void)pEom;
    return 0;
}

int
__libc_res_queriesmatch(const unsigned char *pBuf1, const unsigned char *pEom1,
                        const unsigned char *pBuf2, const unsigned char *pEom2)
{
    (void)pBuf1;
    (void)pEom1;
    (void)pBuf2;
    (void)pEom2;
    return 0;
}

/* ---- small-string internal helpers -------------------------------------- */

void *
__mempcpy_small(void *pDst, const void *pSrc, size_t cb)
{
    return mempcpy(pDst, pSrc, cb);
}

char *
__stpcpy_small(char *szDst, const char *szSrc)
{
    return stpcpy(szDst, szSrc);
}

char *
__strcpy_small(char *szDst, const char *szSrc)
{
    return strcpy(szDst, szSrc);
}

char *
__strsep_g(char **ppString, const char *szDelim)
{
    return strsep(ppString, szDelim);
}

char *
__strsep_1c(char **ppString, char cDelim)
{
    char aDelim[2];

    aDelim[0] = cDelim;
    aDelim[1] = '\0';
    return strsep(ppString, aDelim);
}

char *
__strsep_2c(char **ppString, char c0, char c1)
{
    char aDelim[3];

    aDelim[0] = c0;
    aDelim[1] = c1;
    aDelim[2] = '\0';
    return strsep(ppString, aDelim);
}

char *
__strsep_3c(char **ppString, char c0, char c1, char c2)
{
    char aDelim[4];

    aDelim[0] = c0;
    aDelim[1] = c1;
    aDelim[2] = c2;
    aDelim[3] = '\0';
    return strsep(ppString, aDelim);
}

size_t
__strcspn_c1(const char *sz, char c0)
{
    char a[2];

    a[0] = c0;
    a[1] = '\0';
    return strcspn(sz, a);
}

size_t
__strcspn_c2(const char *sz, char c0, char c1)
{
    char a[3];

    a[0] = c0;
    a[1] = c1;
    a[2] = '\0';
    return strcspn(sz, a);
}

size_t
__strcspn_c3(const char *sz, char c0, char c1, char c2)
{
    char a[4];

    a[0] = c0;
    a[1] = c1;
    a[2] = c2;
    a[3] = '\0';
    return strcspn(sz, a);
}

size_t
__strpbrk_c2(const char *sz, char c0, char c1)
{
    char a[3];
    const char *p;

    a[0] = c0;
    a[1] = c1;
    a[2] = '\0';
    p = strpbrk(sz, a);
    return p == NULL ? 0 : (size_t)(p - sz);
}

size_t
__strpbrk_c3(const char *sz, char c0, char c1, char c2)
{
    char a[4];
    const char *p;

    a[0] = c0;
    a[1] = c1;
    a[2] = c2;
    a[3] = '\0';
    p = strpbrk(sz, a);
    return p == NULL ? 0 : (size_t)(p - sz);
}

/* ---- pthread cleanup surface (manual push/pop; no setjmp) --------------- */

struct b25_cleanup_frame {
    void (*fn)(void *);
    void *arg;
    struct b25_cleanup_frame *pNext;
};

static struct b25_cleanup_frame *s_pCleanupTop;

void
_pthread_cleanup_push(void (*fn)(void *), void *arg)
{
    static struct b25_cleanup_frame aFrames[8];
    static unsigned uDepth;
    struct b25_cleanup_frame *p;

    if (fn == NULL || uDepth >= 8u) {
        return;
    }
    p = &aFrames[uDepth++];
    p->fn = fn;
    p->arg = arg;
    p->pNext = s_pCleanupTop;
    s_pCleanupTop = p;
}

void
_pthread_cleanup_pop(int nExecute)
{
    struct b25_cleanup_frame *p = s_pCleanupTop;

    if (p == NULL) {
        return;
    }
    s_pCleanupTop = p->pNext;
    if (nExecute != 0 && p->fn != NULL) {
        p->fn(p->arg);
    }
}

void
_pthread_cleanup_push_defer(void (*fn)(void *), void *arg)
{
    _pthread_cleanup_push(fn, arg);
}

void
_pthread_cleanup_pop_restore(int nExecute)
{
    _pthread_cleanup_pop(nExecute);
}

/* low-level lock wait/wake — bring-up no-ops (host NPTL private shape) */
void
__lll_lock_wait_private(int *pFutex)
{
    (void)pFutex;
}

void
__lll_lock_wake_private(int *pFutex)
{
    (void)pFutex;
}

void
__nptl_create_event(void)
{
}

void
__nptl_death_event(void)
{
}

/* ---- __libc_alloc_buffer_* (bring-up over malloc) ----------------------- */

struct b25_alloc_buffer {
    char *pBase;
    char *pEnd;
    char *pCur;
};

void
__libc_alloc_buffer_allocate(struct b25_alloc_buffer *pBuf, size_t cb)
{
    if (pBuf == NULL) {
        return;
    }
    pBuf->pBase = (char *)malloc(cb == 0 ? 1 : cb);
    if (pBuf->pBase == NULL) {
        pBuf->pEnd = NULL;
        pBuf->pCur = NULL;
        return;
    }
    pBuf->pEnd = pBuf->pBase + (cb == 0 ? 1 : cb);
    pBuf->pCur = pBuf->pBase;
}

void *
__libc_alloc_buffer_alloc_array(struct b25_alloc_buffer *pBuf, size_t nElem,
                                size_t cbElem)
{
    size_t cb;
    char *p;

    if (pBuf == NULL || pBuf->pCur == NULL || cbElem == 0) {
        return NULL;
    }
    if (nElem > (size_t)-1 / cbElem) {
        return NULL;
    }
    cb = nElem * cbElem;
    if ((size_t)(pBuf->pEnd - pBuf->pCur) < cb) {
        return NULL;
    }
    p = pBuf->pCur;
    pBuf->pCur += cb;
    return p;
}

void *
__libc_alloc_buffer_copy_bytes(struct b25_alloc_buffer *pBuf, const void *pSrc,
                               size_t cb)
{
    void *p;

    p = __libc_alloc_buffer_alloc_array(pBuf, cb == 0 ? 1 : cb, 1);
    if (p == NULL) {
        return NULL;
    }
    if (cb > 0 && pSrc != NULL) {
        memcpy(p, pSrc, cb);
    }
    return p;
}

char *
__libc_alloc_buffer_copy_string(struct b25_alloc_buffer *pBuf,
                                const char *szSrc)
{
    size_t n;

    if (szSrc == NULL) {
        return NULL;
    }
    n = strlen(szSrc) + 1;
    return (char *)__libc_alloc_buffer_copy_bytes(pBuf, szSrc, n);
}

void
__libc_alloc_buffer_create_failure(struct b25_alloc_buffer *pBuf)
{
    if (pBuf != NULL) {
        free(pBuf->pBase);
        pBuf->pBase = NULL;
        pBuf->pEnd = NULL;
        pBuf->pCur = NULL;
    }
}

size_t
__libc_alloca_cutoff(size_t cb)
{
    /* classic glibc cutoff shape (~64 KiB); return min(cb, cutoff) policy */
    if (cb > 65536u) {
        return 65536u;
    }
    return cb;
}

void *
__libc_unwind_link_get(void)
{
    return NULL;
}

void
__libc_ifunc_impl_list(void)
{
}

/* ---- inet helpers ------------------------------------------------------- */

int
__inet_aton_exact(const char *sz, struct in_addr *pAddr)
{
    /* exact: reject trailing junk that inet_aton may accept on some paths */
    const char *p;
    char aTmp[64];
    size_t i = 0;

    if (sz == NULL) {
        return 0;
    }
    p = sz;
    while (*p != '\0' && i + 1 < sizeof(aTmp)) {
        if ((*p >= '0' && *p <= '9') || *p == '.') {
            aTmp[i++] = *p++;
        } else {
            return 0;
        }
    }
    if (*p != '\0') {
        return 0;
    }
    aTmp[i] = '\0';
    return inet_aton(aTmp, pAddr);
}

int
__inet_pton_length(int nAf, const char *szSrc, size_t cbSrc, void *pDst)
{
    char aTmp[128];
    size_t i;

    if (szSrc == NULL || pDst == NULL) {
        return 0;
    }
    if (cbSrc >= sizeof(aTmp)) {
        return 0;
    }
    for (i = 0; i < cbSrc; i++) {
        aTmp[i] = szSrc[i];
    }
    aTmp[cbSrc] = '\0';
    return inet_pton(nAf, aTmp, pDst);
}

int
__inet6_scopeid_pton(const char *sz, size_t cb, unsigned *pId)
{
    size_t i;
    unsigned u = 0;

    if (sz == NULL || pId == NULL || cb == 0) {
        return -1;
    }
    for (i = 0; i < cb; i++) {
        if (sz[i] < '0' || sz[i] > '9') {
            return -1;
        }
        u = u * 10u + (unsigned)(sz[i] - '0');
    }
    *pId = u;
    return 0;
}

/* ---- printf_size hooks (no FP formatting; fail-closed) ------------------ */

int
printf_size(FILE *pF, const struct printf_info *pInfo,
            const void *const *pArgs)
{
    (void)pF;
    (void)pInfo;
    (void)pArgs;
    errno = ENOSYS;
    return -1;
}

int
printf_size_info(const struct printf_info *pInfo, size_t n, int *pArgtypes)
{
    (void)pInfo;
    (void)n;
    if (pArgtypes != NULL) {
        pArgtypes[0] = 0;
    }
    return -1;
}

int
__printf_fp(FILE *pF, const struct printf_info *pInfo, const void *const *pArgs)
{
    (void)pF;
    (void)pInfo;
    (void)pArgs;
    errno = ENOSYS;
    return -1;
}

/* ---- profiling / shm name ----------------------------------------------- */

int
__profile_frequency(void)
{
    return 100; /* Hz bring-up */
}

char *
__shm_get_name(char *szBuf, size_t cb, const char *szName)
{
    size_t n;
    size_t i;
    size_t j;

    if (szBuf == NULL || cb == 0) {
        return NULL;
    }
    if (szName == NULL) {
        szName = "shm";
    }
    /* /dev/shm/ + name */
    const char *szPref = "/dev/shm/";
    n = 0;
    for (i = 0; szPref[i] != '\0' && n + 1 < cb; i++) {
        szBuf[n++] = szPref[i];
    }
    for (j = 0; szName[j] != '\0' && n + 1 < cb; j++) {
        if (szName[j] == '/') {
            continue;
        }
        szBuf[n++] = szName[j];
    }
    szBuf[n] = '\0';
    return szBuf;
}

int
__open_catalog(const char *szName, char **ppPath)
{
    (void)szName;
    if (ppPath != NULL) {
        *ppPath = NULL;
    }
    errno = ENOENT;
    return -1;
}

/* ---- RPC / auth helpers ------------------------------------------------- */

int
_rpc_dtablesize(void)
{
    return 256;
}

void
_seterr_reply(void *pReply, void *pError)
{
    (void)pReply;
    (void)pError;
}

int
_authenticate(void *pRqstp, void *pMsg)
{
    (void)pRqstp;
    (void)pMsg;
    return 0; /* AUTH_OK shape */
}

int
__ivaliduser(FILE *pF, unsigned long uAddr, const char *szRuser,
             const char *szLuser)
{
    (void)pF;
    (void)uAddr;
    (void)szRuser;
    (void)szLuser;
    return -1;
}

int
__libc_rpc_getport(void *pClnt, unsigned long uProg, unsigned long uVers)
{
    (void)pClnt;
    (void)uProg;
    (void)uVers;
    errno = ENOSYS;
    return 0;
}

void *
__libc_clntudp_bufcreate(void *pAddr, unsigned long uProg, unsigned long uVers,
                         struct timeval tv, int *pSockp, unsigned uSendsz,
                         unsigned uRecvsz)
{
    (void)pAddr;
    (void)uProg;
    (void)uVers;
    (void)tv;
    (void)pSockp;
    (void)uSendsz;
    (void)uRecvsz;
    errno = ENOSYS;
    return NULL;
}

/* ---- grp merge helpers -------------------------------------------------- */

int
__copy_grp(struct group *pDst, char *szBuf, size_t cb, const struct group *pSrc)
{
    size_t nName;
    size_t nPass;

    if (pDst == NULL || pSrc == NULL || szBuf == NULL || cb == 0) {
        return -1;
    }
    nName = pSrc->gr_name != NULL ? strlen(pSrc->gr_name) + 1 : 1;
    nPass = pSrc->gr_passwd != NULL ? strlen(pSrc->gr_passwd) + 1 : 1;
    if (nName + nPass > cb) {
        return -1;
    }
    pDst->gr_name = szBuf;
    if (pSrc->gr_name != NULL) {
        memcpy(szBuf, pSrc->gr_name, nName);
    } else {
        szBuf[0] = '\0';
    }
    pDst->gr_passwd = szBuf + nName;
    if (pSrc->gr_passwd != NULL) {
        memcpy(szBuf + nName, pSrc->gr_passwd, nPass);
    } else {
        szBuf[nName] = '\0';
    }
    pDst->gr_gid = pSrc->gr_gid;
    pDst->gr_mem = NULL;
    return 0;
}

int
__merge_grp(struct group *pDst, char *szBuf, size_t cb, const struct group *pA,
            const struct group *pB)
{
    (void)pB;
    return __copy_grp(pDst, szBuf, cb, pA);
}

/* ---- NSS files remainder ------------------------------------------------ */

static int
b25_nss_notfound_host(struct hostent **ppResult, int *pHErr)
{
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    if (pHErr != NULL) {
        *pHErr = 1;
    }
    return NSS_STATUS_TRYAGAIN;
}

int
_nss_files_init(void)
{
    return 0;
}

int
_nss_files_initgroups_dyn(const char *szUser, gid_t gid, long *pStart,
                          long *pSize, gid_t **ppGroups, long nLimit,
                          int *pErrnop)
{
    (void)szUser;
    (void)gid;
    (void)pStart;
    (void)pSize;
    (void)ppGroups;
    (void)nLimit;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

void
_nss_files_setpwent(int nStayopen)
{
    (void)nStayopen;
    setpwent();
}

void
_nss_files_endpwent(void)
{
    endpwent();
}

int
_nss_files_getpwent_r(struct passwd *pPwd, char *szBuf, size_t cb,
                      int *pErrnop)
{
    struct passwd *p = NULL;
    int n;

    n = getpwent_r(pPwd, szBuf, cb, &p);
    if (pErrnop != NULL) {
        *pErrnop = n != 0 ? errno : 0;
    }
    return p != NULL ? NSS_STATUS_SUCCESS : NSS_STATUS_NOTFOUND;
}

void
_nss_files_setgrent(int nStayopen)
{
    (void)nStayopen;
    setgrent();
}

void
_nss_files_endgrent(void)
{
    endgrent();
}

int
_nss_files_getgrent_r(struct group *pGrp, char *szBuf, size_t cb, int *pErrnop)
{
    struct group *p = NULL;
    int n;

    n = getgrent_r(pGrp, szBuf, cb, &p);
    if (pErrnop != NULL) {
        *pErrnop = n != 0 ? errno : 0;
    }
    return p != NULL ? NSS_STATUS_SUCCESS : NSS_STATUS_NOTFOUND;
}

#define B25_NSS_SETEND(name)                                                   \
    void _nss_files_set##name##ent(int nStayopen)                              \
    {                                                                          \
        (void)nStayopen;                                                       \
    }                                                                          \
    void _nss_files_end##name##ent(void)                                       \
    {                                                                          \
    }

B25_NSS_SETEND(host)
B25_NSS_SETEND(net)
B25_NSS_SETEND(proto)
B25_NSS_SETEND(serv)
B25_NSS_SETEND(rpc)
B25_NSS_SETEND(alias)
B25_NSS_SETEND(ether)
B25_NSS_SETEND(netgr)
B25_NSS_SETEND(sp)
B25_NSS_SETEND(sg)

#undef B25_NSS_SETEND

int
_nss_files_gethostent_r(struct hostent *pResult, char *szBuf, size_t cb,
                        int *pErrnop, int *pHErr)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return b25_nss_notfound_host(NULL, pHErr);
}

int
_nss_files_gethostbyname3_r(const char *szName, int nAf, struct hostent *pResult,
                            char *szBuf, size_t cb, int *pErrnop, int *pHErr,
                            int32_t *pTtl, char **ppCanon)
{
    struct hostent *p = NULL;
    int n;

    (void)pTtl;
    if (ppCanon != NULL) {
        *ppCanon = NULL;
    }
    n = gethostbyname2_r(szName, nAf, pResult, szBuf, cb, &p, pHErr);
    if (pErrnop != NULL) {
        *pErrnop = n != 0 ? errno : 0;
    }
    return p != NULL ? NSS_STATUS_SUCCESS : NSS_STATUS_NOTFOUND;
}

int
_nss_files_gethostbyname4_r(const char *szName, void *pPat, char *szBuf,
                            size_t cb, int *pErrnop, int *pHErr, int32_t *pTtl)
{
    (void)szName;
    (void)pPat;
    (void)szBuf;
    (void)cb;
    (void)pTtl;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    if (pHErr != NULL) {
        *pHErr = 1;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getnetbyname_r(const char *szName, void *pResult, char *szBuf,
                          size_t cb, int *pErrnop, int *pHErr)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    if (pHErr != NULL) {
        *pHErr = 1;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getnetbyaddr_r(uint32_t uNet, int nType, void *pResult, char *szBuf,
                          size_t cb, int *pErrnop, int *pHErr)
{
    (void)uNet;
    (void)nType;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    if (pHErr != NULL) {
        *pHErr = 1;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getnetent_r(void *pResult, char *szBuf, size_t cb, int *pErrnop,
                       int *pHErr)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    if (pHErr != NULL) {
        *pHErr = 1;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getprotobyname_r(const char *szName, void *pResult, char *szBuf,
                            size_t cb, int *pErrnop)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getprotobynumber_r(int nProto, void *pResult, char *szBuf, size_t cb,
                              int *pErrnop)
{
    (void)nProto;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getprotoent_r(void *pResult, char *szBuf, size_t cb, int *pErrnop)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getservbyname_r(const char *szName, const char *szProto,
                           void *pResult, char *szBuf, size_t cb, int *pErrnop)
{
    (void)szName;
    (void)szProto;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getservbyport_r(int nPort, const char *szProto, void *pResult,
                           char *szBuf, size_t cb, int *pErrnop)
{
    (void)nPort;
    (void)szProto;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getservent_r(void *pResult, char *szBuf, size_t cb, int *pErrnop)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getrpcbyname_r(const char *szName, void *pResult, char *szBuf,
                          size_t cb, int *pErrnop)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getrpcbynumber_r(int nNumber, void *pResult, char *szBuf, size_t cb,
                            int *pErrnop)
{
    (void)nNumber;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getrpcent_r(void *pResult, char *szBuf, size_t cb, int *pErrnop)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getaliasbyname_r(const char *szName, void *pResult, char *szBuf,
                            size_t cb, int *pErrnop)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getaliasent_r(void *pResult, char *szBuf, size_t cb, int *pErrnop)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getetherent_r(void *pResult, char *szBuf, size_t cb, int *pErrnop)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_gethostton_r(const char *szName, void *pResult, char *szBuf,
                        size_t cb, int *pErrnop)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getntohost_r(const void *pAddr, int nLen, void *pResult, char *szBuf,
                        size_t cb, int *pErrnop)
{
    (void)pAddr;
    (void)nLen;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getnetgrent_r(char **ppHost, char **ppUser, char **ppDomain,
                         char *szBuf, size_t cb, int *pErrnop)
{
    (void)szBuf;
    (void)cb;
    if (ppHost != NULL) {
        *ppHost = NULL;
    }
    if (ppUser != NULL) {
        *ppUser = NULL;
    }
    if (ppDomain != NULL) {
        *ppDomain = NULL;
    }
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getspent_r(void *pResult, char *szBuf, size_t cb, int *pErrnop)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getspnam_r(const char *szName, void *pResult, char *szBuf, size_t cb,
                      int *pErrnop)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getsgent_r(void *pResult, char *szBuf, size_t cb, int *pErrnop)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_files_getsgnam_r(const char *szName, void *pResult, char *szBuf, size_t cb,
                      int *pErrnop)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_NOTFOUND;
}

/* parse helpers — fail-closed (return UNAVAIL) */
int
_nss_files_parse_pwent(char *szLine, struct passwd *pPwd, int *pErrnop)
{
    (void)szLine;
    (void)pPwd;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_UNAVAIL;
}

int
_nss_files_parse_grent(char *szLine, struct group *pGrp, int *pErrnop)
{
    (void)szLine;
    (void)pGrp;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_UNAVAIL;
}

int
_nss_files_parse_spent(char *szLine, void *pSp, int *pErrnop)
{
    (void)szLine;
    (void)pSp;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_UNAVAIL;
}

int
_nss_files_parse_sgent(char *szLine, void *pSg, int *pErrnop)
{
    (void)szLine;
    (void)pSg;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_UNAVAIL;
}

int
_nss_files_parse_etherent(char *szLine, void *pEth, int *pErrnop)
{
    (void)szLine;
    (void)pEth;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_UNAVAIL;
}

int
_nss_files_parse_netent(char *szLine, void *pNet, int *pErrnop)
{
    (void)szLine;
    (void)pNet;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_UNAVAIL;
}

int
_nss_files_parse_protoent(char *szLine, void *pProto, int *pErrnop)
{
    (void)szLine;
    (void)pProto;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_UNAVAIL;
}

int
_nss_files_parse_servent(char *szLine, void *pServ, int *pErrnop)
{
    (void)szLine;
    (void)pServ;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_UNAVAIL;
}

int
_nss_files_parse_rpcent(char *szLine, void *pRpc, int *pErrnop)
{
    (void)szLine;
    (void)pRpc;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    return NSS_STATUS_UNAVAIL;
}

/* ---- NSS dns remainder -------------------------------------------------- */

int
_nss_dns_getcanonname_r(const char *szName, char *szBuf, size_t cb, char **ppOut,
                        int *pErrnop, int *pHErr)
{
    (void)szName;
    (void)szBuf;
    (void)cb;
    if (ppOut != NULL) {
        *ppOut = NULL;
    }
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    if (pHErr != NULL) {
        *pHErr = 1;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_dns_getnetbyname_r(const char *szName, void *pResult, char *szBuf,
                        size_t cb, int *pErrnop, int *pHErr)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    if (pHErr != NULL) {
        *pHErr = 1;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_dns_getnetbyaddr_r(uint32_t uNet, int nType, void *pResult, char *szBuf,
                        size_t cb, int *pErrnop, int *pHErr)
{
    (void)uNet;
    (void)nType;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (pErrnop != NULL) {
        *pErrnop = 0;
    }
    if (pHErr != NULL) {
        *pHErr = 1;
    }
    return NSS_STATUS_NOTFOUND;
}

int
_nss_netgroup_parseline(char **ppLine, char **ppHost, char **ppUser,
                        char **ppDomain)
{
    (void)ppLine;
    if (ppHost != NULL) {
        *ppHost = NULL;
    }
    if (ppUser != NULL) {
        *ppUser = NULL;
    }
    if (ppDomain != NULL) {
        *ppDomain = NULL;
    }
    return -1;
}

/* ---- resolv context stubs ----------------------------------------------- */

void *
__resolv_context_get(void)
{
    return NULL;
}

void *
__resolv_context_get_override(void)
{
    return NULL;
}

void *
__resolv_context_get_preinit(void)
{
    return NULL;
}

void
__resolv_context_put(void *pCtx)
{
    (void)pCtx;
}

void *
__res_get_nsaddr(void *pCtx, size_t n)
{
    (void)pCtx;
    (void)n;
    return NULL;
}

int
__res_nopt(void *pStatp, int nN0, unsigned char *pBuf, int nBuflen)
{
    (void)pStatp;
    (void)nN0;
    (void)pBuf;
    (void)nBuflen;
    return -1;
}

int
__res_context_hostalias(void *pCtx, const char *szName, char *szBuf, size_t cb)
{
    (void)pCtx;
    (void)szName;
    (void)szBuf;
    (void)cb;
    return 0;
}

int
__res_context_mkquery(void *pCtx, int nOp, const char *szDname, int nClass,
                      int nType, const unsigned char *pData, int nDatalen,
                      unsigned char *pBuf, int nBuflen)
{
    (void)pCtx;
    (void)nOp;
    (void)szDname;
    (void)nClass;
    (void)nType;
    (void)pData;
    (void)nDatalen;
    (void)pBuf;
    (void)nBuflen;
    errno = ENOSYS;
    return -1;
}

int
__res_context_query(void *pCtx, const char *szName, int nClass, int nType,
                    unsigned char *pAns, int nAnslen, unsigned char **ppAns,
                    int *pAnsp, int *pAnsp2, int *pNansp2, int *pResplen2)
{
    (void)pCtx;
    (void)szName;
    (void)nClass;
    (void)nType;
    (void)pAns;
    (void)nAnslen;
    if (ppAns != NULL) {
        *ppAns = NULL;
    }
    if (pAnsp != NULL) {
        *pAnsp = 0;
    }
    if (pAnsp2 != NULL) {
        *pAnsp2 = 0;
    }
    if (pNansp2 != NULL) {
        *pNansp2 = 0;
    }
    if (pResplen2 != NULL) {
        *pResplen2 = 0;
    }
    errno = ENOSYS;
    return -1;
}

int
__res_context_search(void *pCtx, const char *szName, int nClass, int nType,
                     unsigned char *pAns, int nAnslen, unsigned char **ppAns,
                     int *pAnsp, int *pAnsp2, int *pNansp2, int *pResplen2)
{
    return __res_context_query(pCtx, szName, nClass, nType, pAns, nAnslen,
                               ppAns, pAnsp, pAnsp2, pNansp2, pResplen2);
}

int
__res_context_send(void *pCtx, const unsigned char *pBuf, int nBuflen,
                   const unsigned char *pBuf2, int nBuflen2, unsigned char *pAns,
                   int nAnslen, int *pTerrno, int *pAnsp, int *pAnsp2,
                   int *pNansp2, int *pResplen2, int *pAnssizp2)
{
    (void)pCtx;
    (void)pBuf;
    (void)nBuflen;
    (void)pBuf2;
    (void)nBuflen2;
    (void)pAns;
    (void)nAnslen;
    if (pTerrno != NULL) {
        *pTerrno = ENOSYS;
    }
    if (pAnsp != NULL) {
        *pAnsp = 0;
    }
    if (pAnsp2 != NULL) {
        *pAnsp2 = 0;
    }
    if (pNansp2 != NULL) {
        *pNansp2 = 0;
    }
    if (pResplen2 != NULL) {
        *pResplen2 = 0;
    }
    if (pAnssizp2 != NULL) {
        *pAnssizp2 = 0;
    }
    errno = ENOSYS;
    return -1;
}

/* RPC thread locals — static bring-up cells */
void *
__rpc_thread_createerr(void)
{
    static int s_err;
    return &s_err;
}

void *
__rpc_thread_svc_fdset(void)
{
    return NULL;
}

int *
__rpc_thread_svc_max_pollfd(void)
{
    static int s_max;
    return &s_max;
}

void *
__rpc_thread_svc_pollfd(void)
{
    return NULL;
}

/* ---- fortify / pread nocancel alias shapes ------------------------------ */

ssize_t pread64(int nFd, void *pBuf, size_t cb, off_t nOff);

ssize_t
__pread64_nocancel(int nFd, void *pBuf, size_t cb, off_t nOff)
{
    return pread64(nFd, pBuf, cb, nOff);
}

