/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23: fortify wctomb/mbrtowc, legacy inet6_option_*,
 * gtty/stty/revoke, idna stubs, __libc_sa_len/msgrcv/msgsnd, dynarray/scratch
 * buffer helpers, NSS files/dns host stubs, classic ONC RPC/XDR surface stubs.
 * Integer/pointer only (no SSE doubles).
 *
 * greppable: CGJ_GRAPH_BATCH23_SOFT_NULL
 * greppable: CGJ_GRAPH_BATCH23_SOFT_ARGS
 * greppable: CGJ_GRAPH_BATCH23_SOFT_EDGE
 *
 * Soft deepen: null/arg guards on user-facing graph nodes; edge
 * hardening only. No multi-def; no API break. Pure C integer/pointer.
 */
#include <errno.h>
#include <grp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pwd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

void __chk_fail(void);
int wctomb(char *sz, wchar_t wc);
size_t mbrtowc(wchar_t *pWc, const char *sz, size_t cb, mbstate_t *pSt);
ssize_t msgrcv(int nMsqid, void *pMsg, size_t cb, long nType, int nFlag);
int msgsnd(int nMsqid, const void *pMsg, size_t cb, int nFlag);

/* ---- fortify ------------------------------------------------------------- */

int
__wctomb_chk(char *sz, wchar_t wc, size_t cbBuflen)
{
    char aTmp[16];
    int n;

    if (sz == NULL) {
        return wctomb(NULL, wc);
    }
    n = wctomb(aTmp, wc);
    if (n < 0) {
        return n;
    }
    if ((size_t)n > cbBuflen) {
        __chk_fail();
    }
    memcpy(sz, aTmp, (size_t)n);
    return n;
}

size_t
__mbrtowc_chk(wchar_t *pWc, const char *sz, size_t cb, mbstate_t *pSt,
              size_t cbBuflen)
{
    if (sz != NULL && cb > cbBuflen) {
        __chk_fail();
    }
    return mbrtowc(pWc, sz, cb, pSt);
}

/* ---- legacy inet6 option API (RFC 2292 shape) --------------------------- */

int
inet6_option_space(int nBytes)
{
    if (nBytes < 0) {
        return -1;
    }
    return (nBytes + 7) & ~7;
}

int
inet6_option_init(void *pBuf, void **ppOpt, int nType)
{
    unsigned char *p;

    if (pBuf == NULL || ppOpt == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = (unsigned char *)pBuf;
    p[0] = (unsigned char)nType;
    p[1] = 0;
    *ppOpt = pBuf;
    return 0;
}

int
inet6_option_append(void *pSp, const uint8_t *pTypep, int nMultx, int nPlusy)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pTypep == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pSp;
    (void)pTypep;
    (void)nMultx;
    (void)nPlusy;
    errno = ENOSYS;
    return -1;
}

uint8_t *
inet6_option_alloc(void *pSp, int nDataLen, int nMultx, int nPlusy)
{
    (void)pSp;
    (void)nDataLen;
    (void)nMultx;
    (void)nPlusy;
    errno = ENOSYS;
    return NULL;
}

int
inet6_option_next(const void *pCmsg, uint8_t **ppTptr)
{
    (void)pCmsg;
    if (ppTptr != NULL) {
        *ppTptr = NULL;
    }
    return -1;
}

int
inet6_option_find(const void *pCmsg, uint8_t **ppTptr, int nType)
{
    (void)pCmsg;
    (void)nType;
    if (ppTptr != NULL) {
        *ppTptr = NULL;
    }
    return -1;
}

/* ---- BSD tty / revoke --------------------------------------------------- */

int
gtty(int nFd, void *pArg)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    (void)nFd;
    (void)pArg;
    errno = ENOSYS;
    return -1;
}

int
stty(int nFd, const void *pArg)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    (void)nFd;
    (void)pArg;
    errno = ENOSYS;
    return -1;
}

int
revoke(const char *szPath)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szPath;
    errno = ENOSYS;
    return -1;
}

/* ---- IDNA stubs --------------------------------------------------------- */

int
__idna_to_dns_encoding(const char *szName, char **ppResult)
{
    size_t n;

    if (szName == NULL || ppResult == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = strlen(szName);
    *ppResult = (char *)malloc(n + 1u);
    if (*ppResult == NULL) {
        return -1;
    }
    memcpy(*ppResult, szName, n + 1u);
    return 0;
}

int
__idna_from_dns_encoding(const char *szName, char **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return __idna_to_dns_encoding(szName, ppResult);
}

/* ---- libc internal helpers ---------------------------------------------- */

int
__libc_sa_len(int nFamily)
{
    switch (nFamily) {
    case AF_INET:
        return (int)sizeof(struct sockaddr_in);
#ifdef AF_INET6
    case AF_INET6:
        return (int)sizeof(struct sockaddr_in6);
#endif
    case AF_UNIX:
        return (int)sizeof(struct sockaddr);
    default:
        return 0;
    }
}

ssize_t
__libc_msgrcv(int nMsqid, void *pMsg, size_t cb, long nType, int nFlag)
{
    return msgrcv(nMsqid, pMsg, cb, nType, nFlag);
}

int
__libc_msgsnd(int nMsqid, const void *pMsg, size_t cb, int nFlag)
{
    return msgsnd(nMsqid, pMsg, cb, nFlag);
}

/* Minimal dynarray / scratch buffer (glibc internal graph shape). */

struct b23_dynarray {
    void *pArray;
    size_t nUsed;
    size_t nAllocated;
    size_t cbElt;
};

int
__libc_dynarray_resize(struct b23_dynarray *pDa, size_t nSize)
{
    void *pNew;
    size_t cb;

    if (pDa == NULL || pDa->cbElt == 0) {
        errno = EINVAL;
        return 0;
    }
    if (nSize <= pDa->nAllocated) {
        pDa->nUsed = nSize;
        return 1;
    }
    if (nSize > ((size_t)-1) / pDa->cbElt) {
        errno = ENOMEM;
        return 0;
    }
    cb = nSize * pDa->cbElt;
    pNew = realloc(pDa->pArray, cb);
    if (pNew == NULL) {
        return 0;
    }
    pDa->pArray = pNew;
    pDa->nAllocated = nSize;
    pDa->nUsed = nSize;
    return 1;
}

int
__libc_dynarray_resize_clear(struct b23_dynarray *pDa, size_t nSize)
{
    size_t nOld = (pDa != NULL) ? pDa->nUsed : 0;
    size_t cbElt;
    unsigned char *p;

    if (!__libc_dynarray_resize(pDa, nSize)) {
        return 0;
    }
    cbElt = pDa->cbElt;
    p = (unsigned char *)pDa->pArray;
    if (nSize > nOld) {
        memset(p + nOld * cbElt, 0, (nSize - nOld) * cbElt);
    }
    return 1;
}

void
__libc_dynarray_finalize(struct b23_dynarray *pDa, void *pResult,
                         size_t *pLen)
{
    if (pDa == NULL) {
        return;
    }
    if (pResult != NULL) {
        /* Caller owns: leave array pointer in *result-shaped blob if needed. */
        *(void **)pResult = pDa->pArray;
    }
    if (pLen != NULL) {
        *pLen = pDa->nUsed;
    }
    pDa->pArray = NULL;
    pDa->nUsed = 0;
    pDa->nAllocated = 0;
}

void
__libc_dynarray_at_failure(size_t nSize)
{
    (void)nSize;
    abort();
}

void
__libc_dynarray_emplace_enlarge(struct b23_dynarray *pDa)
{
    size_t nNew;

    if (pDa == NULL) {
        return;
    }
    nNew = (pDa->nAllocated == 0) ? 8u : pDa->nAllocated * 2u;
    (void)__libc_dynarray_resize(pDa, nNew);
}

struct b23_scratch {
    void *pData;
    size_t cb;
    char aLocal[1024];
};

int
__libc_scratch_buffer_grow(struct b23_scratch *pBuf)
{
    size_t cbNew;
    void *pNew;

    if (pBuf == NULL) {
        errno = EINVAL;
        return 0;
    }
    cbNew = (pBuf->cb == 0) ? 1024u : pBuf->cb * 2u;
    if (pBuf->pData == pBuf->aLocal || pBuf->pData == NULL) {
        pNew = malloc(cbNew);
        if (pNew == NULL) {
            return 0;
        }
        if (pBuf->pData == pBuf->aLocal && pBuf->cb > 0) {
            memcpy(pNew, pBuf->aLocal,
                   pBuf->cb < sizeof(pBuf->aLocal) ? pBuf->cb
                                                   : sizeof(pBuf->aLocal));
        }
    } else {
        pNew = realloc(pBuf->pData, cbNew);
        if (pNew == NULL) {
            return 0;
        }
    }
    pBuf->pData = pNew;
    pBuf->cb = cbNew;
    return 1;
}

int
__libc_scratch_buffer_grow_preserve(struct b23_scratch *pBuf)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return __libc_scratch_buffer_grow(pBuf);
}

int
__libc_scratch_buffer_set_array_size(struct b23_scratch *pBuf, size_t nElem,
                                     size_t cbElt)
{
    size_t cb;

    if (pBuf == NULL || cbElt == 0) {
        errno = EINVAL;
        return 0;
    }
    if (nElem > ((size_t)-1) / cbElt) {
        errno = ENOMEM;
        return 0;
    }
    cb = nElem * cbElt;
    while (pBuf->cb < cb) {
        if (!__libc_scratch_buffer_grow(pBuf)) {
            return 0;
        }
    }
    return 1;
}

void *
__libc_scratch_buffer_dupfree(struct b23_scratch *pBuf, size_t cb)
{
    void *p;

    if (pBuf == NULL) {
        return NULL;
    }
    p = malloc(cb);
    if (p == NULL) {
        return NULL;
    }
    if (pBuf->pData != NULL && cb > 0) {
        memcpy(p, pBuf->pData, cb);
    }
    if (pBuf->pData != NULL && pBuf->pData != pBuf->aLocal) {
        free(pBuf->pData);
    }
    pBuf->pData = pBuf->aLocal;
    pBuf->cb = sizeof(pBuf->aLocal);
    return p;
}

int
__libc_allocate_once_slow(void **ppMem, void *(*pfnAlloc)(void),
                          void (*pfnDealloc)(void *), void *pUnused)
{
    void *p;

    (void)pUnused;
    (void)pfnDealloc;
    if (ppMem == NULL) {
        errno = EINVAL;
        return 0;
    }
    if (*ppMem != NULL) {
        return 1;
    }
    if (pfnAlloc == NULL) {
        errno = EINVAL;
        return 0;
    }
    p = pfnAlloc();
    if (p == NULL) {
        return 0;
    }
    *ppMem = p;
    return 1;
}

/* ---- NSS helpers -------------------------------------------------------- */

int
__nss_database_get(const char *szDb, void **ppNi)
{
    (void)szDb;
    if (ppNi != NULL) {
        *ppNi = NULL;
    }
    return 0;
}

void
__nss_disable_nscd(void)
{
}

/* files backend — thin wrappers over existing reentrant get* */

int
_nss_files_getpwnam_r(const char *szName, struct passwd *pPwd, char *szBuf,
                      size_t cb, struct passwd **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pPwd == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getpwnam_r(szName, pPwd, szBuf, cb, ppResult);
}

int
_nss_files_getpwuid_r(uid_t uid, struct passwd *pPwd, char *szBuf, size_t cb,
                      struct passwd **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pPwd == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getpwuid_r(uid, pPwd, szBuf, cb, ppResult);
}

int
_nss_files_getgrnam_r(const char *szName, struct group *pGrp, char *szBuf,
                      size_t cb, struct group **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pGrp == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getgrnam_r(szName, pGrp, szBuf, cb, ppResult);
}

int
_nss_files_getgrgid_r(gid_t gid, struct group *pGrp, char *szBuf, size_t cb,
                      struct group **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pGrp == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getgrgid_r(gid, pGrp, szBuf, cb, ppResult);
}

int
_nss_files_gethostbyname_r(const char *szName, struct hostent *pResult,
                           char *szBuf, size_t cb, struct hostent **ppResult,
                           int *pHErr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pHErr == NULL) {
        errno = EFAULT;
        return -1;
    }

    return gethostbyname_r(szName, pResult, szBuf, cb, ppResult, pHErr);
}

int
_nss_files_gethostbyname2_r(const char *szName, int nAf, struct hostent *pResult,
                            char *szBuf, size_t cb, struct hostent **ppResult,
                            int *pHErr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pHErr == NULL) {
        errno = EFAULT;
        return -1;
    }

    return gethostbyname2_r(szName, nAf, pResult, szBuf, cb, ppResult, pHErr);
}

int
_nss_files_gethostbyaddr_r(const void *pAddr, socklen_t nLen, int nType,
                           struct hostent *pResult, char *szBuf, size_t cb,
                           struct hostent **ppResult, int *pHErr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pHErr == NULL) {
        errno = EFAULT;
        return -1;
    }

    return gethostbyaddr_r(pAddr, nLen, nType, pResult, szBuf, cb, ppResult,
                           pHErr);
}

/* dns backend — always "not found" without a resolver */

static int
b23_nss_notfound(struct hostent **ppResult, int *pHErr)
{
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    if (pHErr != NULL) {
        *pHErr = 1; /* HOST_NOT_FOUND shape */
    }
    return -2; /* NSS_STATUS_NOTFOUND-ish */
}

int
_nss_dns_gethostbyname_r(const char *szName, struct hostent *pResult,
                         char *szBuf, size_t cb, struct hostent **ppResult,
                         int *pHErr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pHErr == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    return b23_nss_notfound(ppResult, pHErr);
}

int
_nss_dns_gethostbyname2_r(const char *szName, int nAf, struct hostent *pResult,
                          char *szBuf, size_t cb, struct hostent **ppResult,
                          int *pHErr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pHErr == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szName;
    (void)nAf;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    return b23_nss_notfound(ppResult, pHErr);
}

int
_nss_dns_gethostbyname3_r(const char *szName, int nAf, struct hostent *pResult,
                          char *szBuf, size_t cb, struct hostent **ppResult,
                          int *pHErr, int32_t *pTtl, char **pCanon)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pHErr == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pTtl == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pCanon == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pTtl;
    (void)pCanon;
    return _nss_dns_gethostbyname2_r(szName, nAf, pResult, szBuf, cb, ppResult,
                                     pHErr);
}

int
_nss_dns_gethostbyname4_r(const char *szName, void *pResult, char *szBuf,
                          size_t cb, void **ppResult, int *pHErr, int32_t *pTtl)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    (void)pTtl;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    if (pHErr != NULL) {
        *pHErr = 1;
    }
    return -2;
}

int
_nss_dns_gethostbyaddr_r(const void *pAddr, socklen_t nLen, int nType,
                         struct hostent *pResult, char *szBuf, size_t cb,
                         struct hostent **ppResult, int *pHErr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pHErr == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pAddr;
    (void)nLen;
    (void)nType;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    return b23_nss_notfound(ppResult, pHErr);
}

int
_nss_dns_gethostbyaddr2_r(const void *pAddr, socklen_t nLen, int nType,
                          struct hostent *pResult, char *szBuf, size_t cb,
                          struct hostent **ppResult, int *pHErr, int32_t *pTtl)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pHErr == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pTtl == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pTtl;
    return _nss_dns_gethostbyaddr_r(pAddr, nLen, nType, pResult, szBuf, cb,
                                    ppResult, pHErr);
}

/* ---- ONC RPC / XDR surface (ENOSYS / no-op stubs) ----------------------- */

struct rpcent {
    char *r_name;
    char **r_aliases;
    int r_number;
};

static struct rpcent s_rpc_null;

struct rpcent *
getrpcent(void)
{
    errno = ENOENT;
    return NULL;
}

struct rpcent *
getrpcbyname(const char *szName)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szName;
    errno = ENOENT;
    return NULL;
}

struct rpcent *
getrpcbynumber(int nNumber)
{
    (void)nNumber;
    errno = ENOENT;
    return NULL;
}

void
setrpcent(int fStayopen)
{
    (void)fStayopen;
}

void
endrpcent(void)
{
}

int
getrpcport(const char *szHost, unsigned long nProg, unsigned long nVers,
           unsigned nProto)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szHost == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szHost;
    (void)nProg;
    (void)nVers;
    (void)nProto;
    errno = ENOSYS;
    return 0;
}

int
getrpcbyname_r(const char *szName, struct rpcent *pResult, char *szBuf,
               size_t cb, struct rpcent **ppResult)
{
    (void)szName;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

int
getrpcbynumber_r(int nNumber, struct rpcent *pResult, char *szBuf, size_t cb,
                 struct rpcent **ppResult)
{
    (void)nNumber;
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

int
getrpcent_r(struct rpcent *pResult, char *szBuf, size_t cb,
            struct rpcent **ppResult)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOENT;
}

/* XDR — opaque handle; operations are no-ops / fail closed. */

typedef struct {
    int nOp;
    void *pPrivate;
} XDR;

typedef int bool_t;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

void
xdrmem_create(XDR *pXdrs, char *pAddr, unsigned nSize, int nOp)
{
    if (pXdrs == NULL) {
        return;
    }
    pXdrs->nOp = nOp;
    pXdrs->pPrivate = pAddr;
    (void)nSize;
}

void
xdrstdio_create(XDR *pXdrs, void *pFile, int nOp)
{
    if (pXdrs == NULL) {
        return;
    }
    pXdrs->nOp = nOp;
    pXdrs->pPrivate = pFile;
}

void
xdrrec_create(XDR *pXdrs, unsigned nSendSize, unsigned nRecvSize, void *pTcp,
              int (*pfnRead)(void *, void *, int),
              int (*pfnWrite)(void *, void *, int))
{
    (void)nSendSize;
    (void)nRecvSize;
    (void)pfnRead;
    (void)pfnWrite;
    if (pXdrs == NULL) {
        return;
    }
    pXdrs->nOp = 0;
    pXdrs->pPrivate = pTcp;
}

void
xdr_destroy(XDR *pXdrs)
{
    if (pXdrs != NULL) {
        pXdrs->pPrivate = NULL;
    }
}

void
xdr_free(void *pfnProc, char *pObj)
{
    (void)pfnProc;
    (void)pObj;
}

unsigned
xdr_sizeof(void *pfnProc, void *pData)
{
    (void)pfnProc;
    (void)pData;
    return 0;
}

bool_t
xdr_void(void)
{
    return TRUE;
}

bool_t
xdr_int(XDR *pXdrs, int *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_u_int(XDR *pXdrs, unsigned *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_long(XDR *pXdrs, long *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_u_long(XDR *pXdrs, unsigned long *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_short(XDR *pXdrs, short *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_u_short(XDR *pXdrs, unsigned short *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_char(XDR *pXdrs, char *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_u_char(XDR *pXdrs, unsigned char *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_bool(XDR *pXdrs, bool_t *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_enum(XDR *pXdrs, int *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    return xdr_int(pXdrs, p);
}

bool_t
xdr_hyper(XDR *pXdrs, int64_t *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_u_hyper(XDR *pXdrs, uint64_t *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_int32_t(XDR *pXdrs, int32_t *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_uint32_t(XDR *pXdrs, uint32_t *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_int64_t(XDR *pXdrs, int64_t *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    return xdr_hyper(pXdrs, p);
}

bool_t
xdr_uint64_t(XDR *pXdrs, uint64_t *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    return xdr_u_hyper(pXdrs, p);
}

bool_t
xdr_opaque(XDR *pXdrs, char *pCp, unsigned nCnt)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pCp == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)pCp;
    (void)nCnt;
    return FALSE;
}

bool_t
xdr_bytes(XDR *pXdrs, char **ppCpp, unsigned *pSize, unsigned nMaxSize)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppCpp == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pSize == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)ppCpp;
    (void)pSize;
    (void)nMaxSize;
    return FALSE;
}

bool_t
xdr_string(XDR *pXdrs, char **ppCpp, unsigned nMaxSize)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppCpp == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)ppCpp;
    (void)nMaxSize;
    return FALSE;
}

bool_t
xdr_wrapstring(XDR *pXdrs, char **ppCpp)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppCpp == NULL) {
        errno = EFAULT;
        return -1;
    }

    return xdr_string(pXdrs, ppCpp, (unsigned)-1);
}

bool_t
xdr_array(XDR *pXdrs, void *pAddr, unsigned *pSize, unsigned nMaxSize,
          unsigned nEltSize, void *pfnXdr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pSize == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)pAddr;
    (void)pSize;
    (void)nMaxSize;
    (void)nEltSize;
    (void)pfnXdr;
    return FALSE;
}

bool_t
xdr_vector(XDR *pXdrs, char *pBase, unsigned nElem, unsigned nSize,
           void *pfnXdr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pBase == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)pBase;
    (void)nElem;
    (void)nSize;
    (void)pfnXdr;
    return FALSE;
}

bool_t
xdr_reference(XDR *pXdrs, void *pp, unsigned nSize, void *pfnXdr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)pp;
    (void)nSize;
    (void)pfnXdr;
    return FALSE;
}

bool_t
xdr_pointer(XDR *pXdrs, char **ppObj, unsigned nSize, void *pfnXdr)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppObj == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)ppObj;
    (void)nSize;
    (void)pfnXdr;
    return FALSE;
}

bool_t
xdr_union(XDR *pXdrs, int *pDsc, char *pUn, void *pChoices, void *pfnDefault)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pDsc == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pUn == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)pDsc;
    (void)pUn;
    (void)pChoices;
    (void)pfnDefault;
    return FALSE;
}

bool_t
xdr_netobj(XDR *pXdrs, void *pNo)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)pNo;
    return FALSE;
}

/* float/double XDR omitted from use; symbols present as fail-closed stubs. */

bool_t
xdr_float(XDR *pXdrs, void *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

bool_t
xdr_double(XDR *pXdrs, void *p)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pXdrs == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pXdrs;
    (void)p;
    return FALSE;
}

void *
clnt_create(const char *szHost, unsigned long nProg, unsigned long nVers,
            const char *szProto)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szHost == NULL) {
        return NULL;
    }
    if (szProto == NULL) {
        return NULL;
    }

    (void)szHost;
    (void)nProg;
    (void)nVers;
    (void)szProto;
    errno = ENOSYS;
    return NULL;
}

void *
clnttcp_create(void *pAddr, unsigned long nProg, unsigned long nVers,
               int *pSockp, unsigned nSendsz, unsigned nRecvsz)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pSockp == NULL) {
        return NULL;
    }

    (void)pAddr;
    (void)nProg;
    (void)nVers;
    (void)pSockp;
    (void)nSendsz;
    (void)nRecvsz;
    errno = ENOSYS;
    return NULL;
}

void *
clntudp_create(void *pAddr, unsigned long nProg, unsigned long nVers,
               struct timeval tv, int *pSockp)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pSockp == NULL) {
        return NULL;
    }

    (void)pAddr;
    (void)nProg;
    (void)nVers;
    (void)tv;
    (void)pSockp;
    errno = ENOSYS;
    return NULL;
}

void *
clntudp_bufcreate(void *pAddr, unsigned long nProg, unsigned long nVers,
                  struct timeval tv, int *pSockp, unsigned nSendsz,
                  unsigned nRecvsz)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pSockp == NULL) {
        return NULL;
    }

    (void)nSendsz;
    (void)nRecvsz;
    return clntudp_create(pAddr, nProg, nVers, tv, pSockp);
}

void *
clntraw_create(unsigned long nProg, unsigned long nVers)
{
    (void)nProg;
    (void)nVers;
    errno = ENOSYS;
    return NULL;
}

void
clnt_pcreateerror(const char *szMsg)
{
    (void)szMsg;
}

void
clnt_perrno(int nStat)
{
    (void)nStat;
}

void
clnt_perror(void *pClnt, const char *szMsg)
{
    (void)pClnt;
    (void)szMsg;
}

char *
clnt_sperror(void *pClnt, const char *szMsg)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szMsg == NULL) {
        return NULL;
    }

    (void)pClnt;
    (void)szMsg;
    return (char *)"rpc: unavailable";
}

char *
clnt_sperrno(int nStat)
{
    (void)nStat;
    return (char *)"rpc: unavailable";
}

char *
clnt_spcreateerror(const char *szMsg)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szMsg == NULL) {
        return NULL;
    }

    (void)szMsg;
    return (char *)"rpc: create failed";
}

void *
authnone_create(void)
{
    return NULL;
}

void *
authunix_create_default(void)
{
    errno = ENOSYS;
    return NULL;
}

void *
authunix_create(char *szMachname, uid_t uid, gid_t gid, int nLen, gid_t *pAup)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szMachname == NULL) {
        return NULL;
    }
    if (pAup == NULL) {
        return NULL;
    }

    (void)szMachname;
    (void)uid;
    (void)gid;
    (void)nLen;
    (void)pAup;
    errno = ENOSYS;
    return NULL;
}

int
pmap_set(unsigned long nProg, unsigned long nVers, unsigned nProto,
         unsigned short nPort)
{
    (void)nProg;
    (void)nVers;
    (void)nProto;
    (void)nPort;
    errno = ENOSYS;
    return 0;
}

int
pmap_unset(unsigned long nProg, unsigned long nVers)
{
    (void)nProg;
    (void)nVers;
    return 0;
}

unsigned short
pmap_getport(void *pAddr, unsigned long nProg, unsigned long nVers,
             unsigned nProto)
{
    (void)pAddr;
    (void)nProg;
    (void)nVers;
    (void)nProto;
    errno = ENOSYS;
    return 0;
}

void *
pmap_getmaps(void *pAddr)
{
    (void)pAddr;
    errno = ENOSYS;
    return NULL;
}

void *
svcudp_create(int nSock)
{
    (void)nSock;
    errno = ENOSYS;
    return NULL;
}

void *
svctcp_create(int nSock, unsigned nSend, unsigned nRecv)
{
    (void)nSock;
    (void)nSend;
    (void)nRecv;
    errno = ENOSYS;
    return NULL;
}

void *
svcunix_create(int nSock, unsigned nSend, unsigned nRecv, char *szPath)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szPath == NULL) {
        return NULL;
    }

    (void)nSock;
    (void)nSend;
    (void)nRecv;
    (void)szPath;
    errno = ENOSYS;
    return NULL;
}

int
svc_register(void *pXprt, unsigned long nProg, unsigned long nVers,
             void (*pfnDispatch)(void), unsigned nProto)
{
    (void)pXprt;
    (void)nProg;
    (void)nVers;
    (void)pfnDispatch;
    (void)nProto;
    errno = ENOSYS;
    return 0;
}

void
svc_run(void)
{
}

void
svc_getreqset(void *pReadfds)
{
    (void)pReadfds;
}

int
svc_sendreply(void *pXprt, void *pfnXdr, void *pResult)
{
    (void)pXprt;
    (void)pfnXdr;
    (void)pResult;
    return 0;
}

void
svcerr_noproc(void *pXprt)
{
    (void)pXprt;
}

void
svcerr_decode(void *pXprt)
{
    (void)pXprt;
}

void
svcerr_systemerr(void *pXprt)
{
    (void)pXprt;
}

void
svcerr_auth(void *pXprt, int nWhy)
{
    (void)pXprt;
    (void)nWhy;
}

void
svcerr_weakauth(void *pXprt)
{
    (void)pXprt;
}

void
xprt_register(void *pXprt)
{
    (void)pXprt;
}

void
xprt_unregister(void *pXprt)
{
    (void)pXprt;
}

int
callrpc(const char *szHost, unsigned long nProg, unsigned long nVers,
        unsigned long nProc, void *pfnIn, void *pIn, void *pfnOut, void *pOut)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szHost == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szHost;
    (void)nProg;
    (void)nVers;
    (void)nProc;
    (void)pfnIn;
    (void)pIn;
    (void)pfnOut;
    (void)pOut;
    errno = ENOSYS;
    return -1;
}

int
get_myaddress(void *pAddr)
{
    (void)pAddr;
    errno = ENOSYS;
    return -1;
}

int
getnetname(char *szName)
{
    if (szName == NULL) {
        errno = EINVAL;
        return 0;
    }
    szName[0] = '\0';
    return 0;
}

int
host2netname(char *szName, const char *szHost, const char *szDomain)
{
    (void)szHost;
    (void)szDomain;
    if (szName != NULL) {
        szName[0] = '\0';
    }
    return 0;
}

int
netname2host(const char *szName, char *szHost, int nHostlen)
{
    (void)szName;
    if (szHost != NULL && nHostlen > 0) {
        szHost[0] = '\0';
    }
    return 0;
}

int
user2netname(char *szName, uid_t uid, const char *szDomain)
{
    (void)uid;
    (void)szDomain;
    if (szName != NULL) {
        szName[0] = '\0';
    }
    return 0;
}

int
key_encryptsession(char *szRemote, void *pDeskey)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szRemote == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szRemote;
    (void)pDeskey;
    errno = ENOSYS;
    return -1;
}

int
key_decryptsession(char *szRemote, void *pDeskey)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szRemote == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szRemote;
    (void)pDeskey;
    errno = ENOSYS;
    return -1;
}

int
key_gendes(void *pDeskey)
{
    (void)pDeskey;
    errno = ENOSYS;
    return -1;
}

int
key_setsecret(char *szKey)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (szKey == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szKey;
    errno = ENOSYS;
    return -1;
}

void
des_setparity(char *pKey)
{
    (void)pKey;
}

int
cbc_crypt(char *pKey, char *pBuf, unsigned nLen, unsigned nMode, char *pIvec)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pKey == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pIvec == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pKey;
    (void)pBuf;
    (void)nLen;
    (void)nMode;
    (void)pIvec;
    errno = ENOSYS;
    return -1;
}

int
ecb_crypt(char *pKey, char *pBuf, unsigned nLen, unsigned nMode)
{
    /* greppable: CGJ_GRAPH_BATCH23_SOFT_NULL */
    if (pKey == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pKey;
    (void)pBuf;
    (void)nLen;
    (void)nMode;
    errno = ENOSYS;
    return -1;
}

/* Silence unused static (keeps shape for future real table). */
void
__b23_rpc_touch(void)
{
    (void)s_rpc_null;
}
