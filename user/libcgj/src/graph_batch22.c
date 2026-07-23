/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22: fpurge, sig2str/str2sig, b64_ntop/pton,
 * res_randomid / dn_count_labels, nss *_lookup2 stubs, affinity_new,
 * xattr underscored aliases, resolv p_type/p_class/putlong/putshort,
 * __libc_socket family, ns_format_ttl/parse_ttl/msg_getflag/name_rollback,
 * res_hostalias. Integer/pointer only (no SSE doubles).
 */
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <time.h>
#include <unistd.h>

void __fpurge(FILE *pF);
int sched_setaffinity(pid_t pid, size_t cbSet, const cpu_set_t *pSet);
int sched_getaffinity(pid_t pid, size_t cbSet, cpu_set_t *pSet);
int setxattr(const char *szPath, const char *szName, const void *pValue,
             size_t cb, int nFlags);
int lsetxattr(const char *szPath, const char *szName, const void *pValue,
              size_t cb, int nFlags);
int fsetxattr(int nFd, const char *szName, const void *pValue, size_t cb,
              int nFlags);
ssize_t getxattr(const char *szPath, const char *szName, void *pValue,
                 size_t cb);
ssize_t lgetxattr(const char *szPath, const char *szName, void *pValue,
                  size_t cb);
ssize_t fgetxattr(int nFd, const char *szName, void *pValue, size_t cb);
ssize_t listxattr(const char *szPath, char *szList, size_t cb);
ssize_t llistxattr(const char *szPath, char *szList, size_t cb);
ssize_t flistxattr(int nFd, char *szList, size_t cb);
int removexattr(const char *szPath, const char *szName);
int lremovexattr(const char *szPath, const char *szName);
int fremovexattr(int nFd, const char *szName);
int socket(int nDomain, int nType, int nProtocol);
int bind(int nFd, const struct sockaddr *pAddr, socklen_t nLen);
int connect(int nFd, const struct sockaddr *pAddr, socklen_t nLen);
int listen(int nFd, int nBacklog);
int accept(int nFd, struct sockaddr *pAddr, socklen_t *pLen);
ssize_t send(int nFd, const void *pBuf, size_t cb, int nFlags);
ssize_t recv(int nFd, void *pBuf, size_t cb, int nFlags);
ssize_t sendto(int nFd, const void *pBuf, size_t cb, int nFlags,
               const struct sockaddr *pAddr, socklen_t nLen);
ssize_t recvfrom(int nFd, void *pBuf, size_t cb, int nFlags,
                 struct sockaddr *pAddr, socklen_t *pLen);
ssize_t sendmsg(int nFd, const struct msghdr *pMsg, int nFlags);
ssize_t recvmsg(int nFd, struct msghdr *pMsg, int nFlags);
int shutdown(int nFd, int nHow);
int socketpair(int nDomain, int nType, int nProtocol, int aSv[2]);
unsigned ns_get16(const unsigned char *p);
unsigned long ns_get32(const unsigned char *p);
void ns_put16(unsigned u, unsigned char *p);
void ns_put32(unsigned long u, unsigned char *p);

#ifndef SIG2STR_MAX
#define SIG2STR_MAX 32
#endif

/* ---- fpurge ------------------------------------------------------------- */

void
fpurge(FILE *pF)
{
    __fpurge(pF);
}

/* ---- sig2str / str2sig -------------------------------------------------- */

struct b22_sig_name {
    int nSig;
    const char *szName;
};

static const struct b22_sig_name s_aSigNames[] = {
    { SIGHUP, "HUP" },   { SIGINT, "INT" },   { SIGQUIT, "QUIT" },
    { SIGILL, "ILL" },   { SIGTRAP, "TRAP" }, { SIGABRT, "ABRT" },
    { SIGBUS, "BUS" },   { SIGFPE, "FPE" },   { SIGKILL, "KILL" },
    { SIGUSR1, "USR1" }, { SIGSEGV, "SEGV" }, { SIGUSR2, "USR2" },
    { SIGPIPE, "PIPE" }, { SIGALRM, "ALRM" }, { SIGTERM, "TERM" },
    { SIGCHLD, "CHLD" }, { SIGCONT, "CONT" }, { SIGSTOP, "STOP" },
    { SIGTSTP, "TSTP" },
};

int
sig2str(int nSig, char *szBuf)
{
    size_t i;
    size_t n;
    unsigned u;
    char aDig[16];
    size_t nDig;

    if (szBuf == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < sizeof(s_aSigNames) / sizeof(s_aSigNames[0]); i++) {
        if (s_aSigNames[i].nSig == nSig) {
            const char *p = s_aSigNames[i].szName;

            n = 0;
            while (p[n] != '\0' && n + 1u < (size_t)SIG2STR_MAX) {
                szBuf[n] = p[n];
                n++;
            }
            szBuf[n] = '\0';
            return 0;
        }
    }
    if (nSig < 0) {
        errno = EINVAL;
        return -1;
    }
    u = (unsigned)nSig;
    nDig = 0;
    if (u == 0u) {
        aDig[nDig++] = '0';
    } else {
        while (u > 0u && nDig < sizeof(aDig)) {
            aDig[nDig++] = (char)('0' + (u % 10u));
            u /= 10u;
        }
    }
    n = 0;
    while (nDig > 0u && n + 1u < (size_t)SIG2STR_MAX) {
        nDig--;
        szBuf[n++] = aDig[nDig];
    }
    szBuf[n] = '\0';
    return 0;
}

int
str2sig(const char *szName, int *pSig)
{
    size_t i;
    int n;
    const char *p;

    if (szName == NULL || pSig == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = szName;
    if (p[0] == 'S' && p[1] == 'I' && p[2] == 'G') {
        p += 3;
    }
    for (i = 0; i < sizeof(s_aSigNames) / sizeof(s_aSigNames[0]); i++) {
        const char *q = s_aSigNames[i].szName;
        size_t j = 0;

        while (q[j] != '\0' && p[j] != '\0') {
            char cA = q[j];
            char cB = p[j];

            if (cA >= 'a' && cA <= 'z') {
                cA = (char)(cA - 'a' + 'A');
            }
            if (cB >= 'a' && cB <= 'z') {
                cB = (char)(cB - 'a' + 'A');
            }
            if (cA != cB) {
                break;
            }
            j++;
        }
        if (q[j] == '\0' && p[j] == '\0') {
            *pSig = s_aSigNames[i].nSig;
            return 0;
        }
    }
    n = 0;
    p = szName;
    if (*p == '\0') {
        errno = EINVAL;
        return -1;
    }
    while (*p >= '0' && *p <= '9') {
        n = n * 10 + (*p - '0');
        p++;
    }
    if (*p != '\0' || n <= 0) {
        errno = EINVAL;
        return -1;
    }
    *pSig = n;
    return 0;
}

/* ---- base64 (BIND/resolv shape) ----------------------------------------- */

static const char s_szB64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int
b64_ntop(const unsigned char *pSrc, size_t cbSrc, char *szDst, size_t cbDst)
{
    size_t i;
    size_t o = 0;

    if (pSrc == NULL || szDst == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < cbSrc; i += 3u) {
        unsigned b0 = pSrc[i];
        unsigned b1 = (i + 1u < cbSrc) ? pSrc[i + 1u] : 0u;
        unsigned b2 = (i + 2u < cbSrc) ? pSrc[i + 2u] : 0u;
        size_t nIn = cbSrc - i;

        if (o + 4u >= cbDst) {
            errno = ENOSPC;
            return -1;
        }
        szDst[o++] = s_szB64[(b0 >> 2) & 0x3fu];
        szDst[o++] = s_szB64[((b0 & 0x3u) << 4) | ((b1 >> 4) & 0xfu)];
        if (nIn == 1u) {
            szDst[o++] = '=';
            szDst[o++] = '=';
        } else if (nIn == 2u) {
            szDst[o++] = s_szB64[(b1 & 0xfu) << 2];
            szDst[o++] = '=';
        } else {
            szDst[o++] = s_szB64[((b1 & 0xfu) << 2) | ((b2 >> 6) & 0x3u)];
            szDst[o++] = s_szB64[b2 & 0x3fu];
        }
    }
    if (o >= cbDst) {
        errno = ENOSPC;
        return -1;
    }
    szDst[o] = '\0';
    return (int)o;
}

static int
b64_val(int ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A';
    }
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 26;
    }
    if (ch >= '0' && ch <= '9') {
        return ch - '0' + 52;
    }
    if (ch == '+') {
        return 62;
    }
    if (ch == '/') {
        return 63;
    }
    return -1;
}

int
b64_pton(const char *szSrc, unsigned char *pDst, size_t cbDst)
{
    size_t i = 0;
    size_t o = 0;
    int aV[4];
    int nV;
    int ch;

    if (szSrc == NULL || pDst == NULL) {
        errno = EINVAL;
        return -1;
    }
    while (szSrc[i] != '\0') {
        nV = 0;
        while (nV < 4 && szSrc[i] != '\0') {
            ch = (unsigned char)szSrc[i++];
            if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
                continue;
            }
            if (ch == '=') {
                aV[nV++] = -2;
                continue;
            }
            aV[nV] = b64_val(ch);
            if (aV[nV] < 0) {
                errno = EINVAL;
                return -1;
            }
            nV++;
        }
        if (nV == 0) {
            break;
        }
        if (nV < 4) {
            errno = EINVAL;
            return -1;
        }
        if (aV[0] < 0 || aV[1] < 0) {
            errno = EINVAL;
            return -1;
        }
        if (o >= cbDst) {
            errno = ENOSPC;
            return -1;
        }
        pDst[o++] = (unsigned char)((aV[0] << 2) | (aV[1] >> 4));
        if (aV[2] == -2) {
            break;
        }
        if (aV[2] < 0) {
            errno = EINVAL;
            return -1;
        }
        if (o >= cbDst) {
            errno = ENOSPC;
            return -1;
        }
        pDst[o++] = (unsigned char)(((aV[1] & 0xf) << 4) | (aV[2] >> 2));
        if (aV[3] == -2) {
            break;
        }
        if (aV[3] < 0) {
            errno = EINVAL;
            return -1;
        }
        if (o >= cbDst) {
            errno = ENOSPC;
            return -1;
        }
        pDst[o++] = (unsigned char)(((aV[2] & 0x3) << 6) | aV[3]);
    }
    return (int)o;
}

/* ---- resolv helpers ----------------------------------------------------- */

unsigned
res_randomid(void)
{
    /* Integer-only entropy mix; not cryptographic. */
    static unsigned uState = 0xA5A5u;
    struct timespec ts;
    unsigned u;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        uState ^= (unsigned)ts.tv_nsec;
        uState ^= (unsigned)ts.tv_sec * 0x9E37u;
    }
    uState = uState * 1664525u + 1013904223u;
    u = uState ^ (uState >> 16);
    return u & 0xffffu;
}

unsigned
__res_randomid(void)
{
    return res_randomid();
}

int
dn_count_labels(const char *szName)
{
    size_t i;
    int n = 0;
    int fIn = 0;

    if (szName == NULL || szName[0] == '\0' ||
        (szName[0] == '.' && szName[1] == '\0')) {
        return 0;
    }
    for (i = 0; szName[i] != '\0'; i++) {
        if (szName[i] == '.') {
            if (fIn) {
                n++;
                fIn = 0;
            }
        } else {
            fIn = 1;
        }
    }
    if (fIn) {
        n++;
    }
    return n;
}

int
__dn_count_labels(const char *szName)
{
    return dn_count_labels(szName);
}

const char *
p_type(int nType)
{
    switch (nType) {
    case 1:
        return "A";
    case 2:
        return "NS";
    case 5:
        return "CNAME";
    case 6:
        return "SOA";
    case 12:
        return "PTR";
    case 15:
        return "MX";
    case 16:
        return "TXT";
    case 28:
        return "AAAA";
    case 33:
        return "SRV";
    case 255:
        return "ANY";
    default:
        return "?";
    }
}

const char *
__p_type(int nType)
{
    return p_type(nType);
}

const char *
p_class(int nClass)
{
    switch (nClass) {
    case 1:
        return "IN";
    case 3:
        return "CH";
    case 4:
        return "HS";
    case 255:
        return "ANY";
    default:
        return "?";
    }
}

const char *
__p_class(int nClass)
{
    return p_class(nClass);
}

void
putshort(unsigned u, unsigned char *p)
{
    ns_put16(u, p);
}

void
putlong(unsigned long u, unsigned char *p)
{
    ns_put32(u, p);
}

unsigned
_getshort(const unsigned char *p)
{
    return ns_get16(p);
}

unsigned long
_getlong(const unsigned char *p)
{
    return ns_get32(p);
}

int
ns_format_ttl(unsigned long uTtl, char *szBuf, size_t cb)
{
    size_t n = 0;
    unsigned long u = uTtl;
    char aDig[16];
    size_t nDig;

    if (szBuf == NULL || cb == 0) {
        errno = EINVAL;
        return -1;
    }
    nDig = 0;
    if (u == 0ul) {
        aDig[nDig++] = '0';
    } else {
        while (u > 0ul && nDig < sizeof(aDig)) {
            aDig[nDig++] = (char)('0' + (unsigned)(u % 10ul));
            u /= 10ul;
        }
    }
    while (nDig > 0u && n + 1u < cb) {
        nDig--;
        szBuf[n++] = aDig[nDig];
    }
    szBuf[n] = '\0';
    return (int)n;
}

int
ns_parse_ttl(const char *sz, unsigned long *pTtl)
{
    unsigned long u = 0;
    const char *p;

    if (sz == NULL || pTtl == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = sz;
    if (*p == '\0') {
        errno = EINVAL;
        return -1;
    }
    while (*p >= '0' && *p <= '9') {
        u = u * 10ul + (unsigned long)(*p - '0');
        p++;
    }
    if (*p != '\0') {
        errno = EINVAL;
        return -1;
    }
    *pTtl = u;
    return 0;
}

int
ns_msg_getflag(void *pHandle, int nFlag)
{
    (void)pHandle;
    (void)nFlag;
    return 0;
}

int
ns_name_rollback(const unsigned char *pComp, const unsigned char **ppDnptrs,
                 const unsigned char **ppLastdnptr)
{
    (void)pComp;
    (void)ppDnptrs;
    (void)ppLastdnptr;
    return 0;
}

int
ns_sprintrr(void *pHandle, void *pRr, const char *szName, const char *szOrigin,
            char *szBuf, size_t cb)
{
    (void)pHandle;
    (void)pRr;
    (void)szName;
    (void)szOrigin;
    if (szBuf == NULL || cb == 0) {
        errno = EINVAL;
        return -1;
    }
    szBuf[0] = '\0';
    return 0;
}

const char *
res_hostalias(void *pState, const char *szName, char *szBuf, size_t cb)
{
    (void)pState;
    (void)szName;
    (void)szBuf;
    (void)cb;
    return NULL;
}

const char *
hostalias(const char *szName)
{
    (void)szName;
    return NULL;
}

void
__res_iclose(void *pState, int nFree)
{
    (void)pState;
    (void)nFree;
}

/* ---- nss *_lookup2 stubs ------------------------------------------------ */

static int
b22_nss_lookup2(void **ppNi, void **ppFct, void *pUnused, void *pUnused2)
{
    (void)pUnused;
    (void)pUnused2;
    if (ppNi != NULL) {
        *ppNi = NULL;
    }
    if (ppFct != NULL) {
        *ppFct = NULL;
    }
    return -1;
}

int
__nss_passwd_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_group_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_hosts_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_services_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_ethers_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_rpc_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_netgroup_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_protocols_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_networks_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_aliases_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_publickey_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_shadow_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

int
__nss_gshadow_lookup2(void **ppNi, void **ppFct, void *a, void *b)
{
    return b22_nss_lookup2(ppNi, ppFct, a, b);
}

/* ---- sched affinity "new" aliases --------------------------------------- */

int
__sched_setaffinity_new(pid_t pid, size_t cbSet, const cpu_set_t *pSet)
{
    return sched_setaffinity(pid, cbSet, pSet);
}

int
__sched_getaffinity_new(pid_t pid, size_t cbSet, cpu_set_t *pSet)
{
    return sched_getaffinity(pid, cbSet, pSet);
}

/* ---- xattr underscored -------------------------------------------------- */

int
__setxattr(const char *szPath, const char *szName, const void *pValue,
           size_t cb, int nFlags)
{
    return setxattr(szPath, szName, pValue, cb, nFlags);
}

int
__lsetxattr(const char *szPath, const char *szName, const void *pValue,
            size_t cb, int nFlags)
{
    return lsetxattr(szPath, szName, pValue, cb, nFlags);
}

int
__fsetxattr(int nFd, const char *szName, const void *pValue, size_t cb,
            int nFlags)
{
    return fsetxattr(nFd, szName, pValue, cb, nFlags);
}

ssize_t
__getxattr(const char *szPath, const char *szName, void *pValue, size_t cb)
{
    return getxattr(szPath, szName, pValue, cb);
}

ssize_t
__lgetxattr(const char *szPath, const char *szName, void *pValue, size_t cb)
{
    return lgetxattr(szPath, szName, pValue, cb);
}

ssize_t
__fgetxattr(int nFd, const char *szName, void *pValue, size_t cb)
{
    return fgetxattr(nFd, szName, pValue, cb);
}

ssize_t
__listxattr(const char *szPath, char *szList, size_t cb)
{
    return listxattr(szPath, szList, cb);
}

ssize_t
__llistxattr(const char *szPath, char *szList, size_t cb)
{
    return llistxattr(szPath, szList, cb);
}

ssize_t
__flistxattr(int nFd, char *szList, size_t cb)
{
    return flistxattr(nFd, szList, cb);
}

int
__removexattr(const char *szPath, const char *szName)
{
    return removexattr(szPath, szName);
}

int
__lremovexattr(const char *szPath, const char *szName)
{
    return lremovexattr(szPath, szName);
}

int
__fremovexattr(int nFd, const char *szName)
{
    return fremovexattr(nFd, szName);
}

/* ---- __libc_ socket family ---------------------------------------------- */

int
__libc_socket(int nDomain, int nType, int nProtocol)
{
    return socket(nDomain, nType, nProtocol);
}

int
__libc_bind(int nFd, const struct sockaddr *pAddr, socklen_t nLen)
{
    return bind(nFd, pAddr, nLen);
}

int
__libc_connect(int nFd, const struct sockaddr *pAddr, socklen_t nLen)
{
    return connect(nFd, pAddr, nLen);
}

int
__libc_listen(int nFd, int nBacklog)
{
    return listen(nFd, nBacklog);
}

int
__libc_accept(int nFd, struct sockaddr *pAddr, socklen_t *pLen)
{
    return accept(nFd, pAddr, pLen);
}

ssize_t
__libc_send(int nFd, const void *pBuf, size_t cb, int nFlags)
{
    return send(nFd, pBuf, cb, nFlags);
}

ssize_t
__libc_recv(int nFd, void *pBuf, size_t cb, int nFlags)
{
    return recv(nFd, pBuf, cb, nFlags);
}

ssize_t
__libc_sendto(int nFd, const void *pBuf, size_t cb, int nFlags,
              const struct sockaddr *pAddr, socklen_t nLen)
{
    return sendto(nFd, pBuf, cb, nFlags, pAddr, nLen);
}

ssize_t
__libc_recvfrom(int nFd, void *pBuf, size_t cb, int nFlags,
                struct sockaddr *pAddr, socklen_t *pLen)
{
    return recvfrom(nFd, pBuf, cb, nFlags, pAddr, pLen);
}

ssize_t
__libc_sendmsg(int nFd, const struct msghdr *pMsg, int nFlags)
{
    return sendmsg(nFd, pMsg, nFlags);
}

ssize_t
__libc_recvmsg(int nFd, struct msghdr *pMsg, int nFlags)
{
    return recvmsg(nFd, pMsg, nFlags);
}

int
__libc_shutdown(int nFd, int nHow)
{
    return shutdown(nFd, nHow);
}

int
__libc_socketpair(int nDomain, int nType, int nProtocol, int aSv[2])
{
    return socketpair(nDomain, nType, nProtocol, aSv);
}

/* ---- misc underscored --------------------------------------------------- */

int
__libc_pause(void)
{
    return pause();
}

int
__libc_nanosleep(const struct timespec *pReq, struct timespec *pRem)
{
    return nanosleep(pReq, pRem);
}

int
__libc_msync(void *pAddr, size_t cb, int nFlags)
{
    return msync(pAddr, cb, nFlags);
}
