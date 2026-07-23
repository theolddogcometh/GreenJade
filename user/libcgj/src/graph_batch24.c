/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24: strtok, timespec_get*, setproctitle/setlogin,
 * BSD sigvec/sigstack/siginterrupt, res_nmkquery/nsend/querydomain,
 * remaining ONC RPC/XDR message surface, authdes/key_* remainder,
 * obsolete Linux module/ustat stubs, fgetln/strnstr/timingsafe_*,
 * getipnodeby*, C++ guard stubs, fortify wcrtomb. Integer/pointer only
 * (no SSE doubles).
 */
#include <errno.h>
#include <netdb.h>
#include <signal.h>
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

char *strtok_r(char *sz, const char *szDelim, char **ppSave);
int clock_gettime(clockid_t clk, struct timespec *pTs);
int clock_getres(clockid_t clk, struct timespec *pTs);
int res_mkquery(int nOp, const char *szDname, int nClass, int nType,
                const unsigned char *pData, int nDatalen,
                const unsigned char *pNew, unsigned char *pBuf, int nBuflen);
int res_send(const unsigned char *pMsg, int nMsglen, unsigned char *pAns,
             int nAnslen);
int res_query(const char *szDname, int nClass, int nType, unsigned char *pAns,
              int nAnslen);

#ifndef TIME_UTC
#define TIME_UTC 1
#endif

/* ---- strtok ------------------------------------------------------------- */

char *
strtok(char *sz, const char *szDelim)
{
    static char *s_pSave;

    return strtok_r(sz, szDelim, &s_pSave);
}

/* ---- timespec_get / timespec_getres (C11, integer timespec) ------------- */

int
timespec_get(struct timespec *pTs, int nBase)
{
    if (pTs == NULL) {
        return 0;
    }
    if (nBase != TIME_UTC) {
        return 0;
    }
    if (clock_gettime(CLOCK_REALTIME, pTs) != 0) {
        return 0;
    }
    return TIME_UTC;
}

int
timespec_getres(struct timespec *pTs, int nBase)
{
    if (pTs == NULL) {
        return 0;
    }
    if (nBase != TIME_UTC) {
        return 0;
    }
    if (clock_getres(CLOCK_REALTIME, pTs) != 0) {
        pTs->tv_sec = 0;
        pTs->tv_nsec = 1;
    }
    return TIME_UTC;
}

/* ---- setproctitle / setlogin -------------------------------------------- */

void
setproctitle(const char *szFmt, ...)
{
    va_list ap;

    (void)szFmt;
    va_start(ap, szFmt);
    va_end(ap);
    /* bring-up: no argv rewrite surface yet */
}

int
setlogin(const char *szName)
{
    (void)szName;
    errno = EPERM;
    return -1;
}

/* ---- BSD signal surface ------------------------------------------------- */

struct b24_sigvec {
    void (*sv_handler)(int);
    int sv_mask;
    int sv_flags;
};

struct b24_sigstack {
    void *ss_sp;
    int ss_onstack;
};

int
siginterrupt(int nSig, int nFlag)
{
    struct sigaction sa;

    if (sigaction(nSig, NULL, &sa) != 0) {
        return -1;
    }
    if (nFlag != 0) {
        sa.sa_flags &= ~SA_RESTART;
    } else {
        sa.sa_flags |= SA_RESTART;
    }
    return sigaction(nSig, &sa, NULL);
}

int
sigstack(struct b24_sigstack *pSs, struct b24_sigstack *pOss)
{
    (void)pSs;
    if (pOss != NULL) {
        pOss->ss_sp = NULL;
        pOss->ss_onstack = 0;
    }
    errno = ENOSYS;
    return -1;
}

int
sigvec(int nSig, struct b24_sigvec *pVec, struct b24_sigvec *pOvec)
{
    struct sigaction sa;
    struct sigaction osa;
    int n;

    (void)nSig;
    if (pOvec != NULL) {
        pOvec->sv_handler = NULL;
        pOvec->sv_mask = 0;
        pOvec->sv_flags = 0;
    }
    if (pVec == NULL) {
        if (sigaction(nSig, NULL, &osa) != 0) {
            return -1;
        }
        if (pOvec != NULL) {
            pOvec->sv_handler = osa.sa_handler;
            pOvec->sv_mask = 0;
            pOvec->sv_flags = osa.sa_flags;
        }
        return 0;
    }
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = pVec->sv_handler;
    sa.sa_flags = pVec->sv_flags;
    n = sigaction(nSig, &sa, &osa);
    if (n == 0 && pOvec != NULL) {
        pOvec->sv_handler = osa.sa_handler;
        pOvec->sv_mask = 0;
        pOvec->sv_flags = osa.sa_flags;
    }
    return n;
}

/* ---- resolv re-exports -------------------------------------------------- */

int
res_nmkquery(void *pState, int nOp, const char *szDname, int nClass, int nType,
             const unsigned char *pData, int nDatalen, unsigned char *pBuf,
             int nBuflen)
{
    (void)pState;
    return res_mkquery(nOp, szDname, nClass, nType, pData, nDatalen, NULL, pBuf,
                       nBuflen);
}

int
__res_nmkquery(void *pState, int nOp, const char *szDname, int nClass,
               int nType, const unsigned char *pData, int nDatalen,
               unsigned char *pBuf, int nBuflen)
{
    return res_nmkquery(pState, nOp, szDname, nClass, nType, pData, nDatalen,
                        pBuf, nBuflen);
}

int
res_nsend(void *pState, const unsigned char *pMsg, int nMsglen,
          unsigned char *pAns, int nAnslen)
{
    (void)pState;
    return res_send(pMsg, nMsglen, pAns, nAnslen);
}

int
__res_nsend(void *pState, const unsigned char *pMsg, int nMsglen,
            unsigned char *pAns, int nAnslen)
{
    return res_nsend(pState, pMsg, nMsglen, pAns, nAnslen);
}

int
res_querydomain(const char *szName, const char *szDomain, int nClass,
                int nType, unsigned char *pAns, int nAnslen)
{
    char aFqdn[256];
    size_t nName;
    size_t nDom;

    if (szName == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szDomain == NULL || szDomain[0] == '\0') {
        return res_query(szName, nClass, nType, pAns, nAnslen);
    }
    nName = 0;
    while (szName[nName] != '\0' && nName + 1u < sizeof(aFqdn)) {
        aFqdn[nName] = szName[nName];
        nName++;
    }
    if (nName > 0u && aFqdn[nName - 1u] != '.') {
        if (nName + 1u >= sizeof(aFqdn)) {
            errno = EINVAL;
            return -1;
        }
        aFqdn[nName++] = '.';
    }
    nDom = 0;
    while (szDomain[nDom] != '\0' && nName + nDom + 1u < sizeof(aFqdn)) {
        aFqdn[nName + nDom] = szDomain[nDom];
        nDom++;
    }
    aFqdn[nName + nDom] = '\0';
    if (szDomain[nDom] != '\0') {
        errno = EINVAL;
        return -1;
    }
    return res_query(aFqdn, nClass, nType, pAns, nAnslen);
}

int
__res_querydomain(const char *szName, const char *szDomain, int nClass,
                  int nType, unsigned char *pAns, int nAnslen)
{
    return res_querydomain(szName, szDomain, nClass, nType, pAns, nAnslen);
}

/* ---- XDR helpers (fail-closed; no float/double bodies) ------------------ */

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef int bool_t;

struct b24_xdr {
    int nOp;
    char *pBase;
    char *pCur;
    unsigned uHandy;
};

static bool_t
b24_xdr_fail2(struct b24_xdr *pXdr, void *p)
{
    (void)pXdr;
    (void)p;
    return FALSE;
}

bool_t
xdr_int8_t(struct b24_xdr *pXdr, int8_t *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_uint8_t(struct b24_xdr *pXdr, uint8_t *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_int16_t(struct b24_xdr *pXdr, int16_t *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_uint16_t(struct b24_xdr *pXdr, uint16_t *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_longlong_t(struct b24_xdr *pXdr, long long *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_u_longlong_t(struct b24_xdr *pXdr, unsigned long long *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_quad_t(struct b24_xdr *pXdr, long long *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_u_quad_t(struct b24_xdr *pXdr, unsigned long long *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_opaque_auth(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_des_block(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_accepted_reply(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_rejected_reply(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_replymsg(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_callhdr(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_callmsg(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_authunix_parms(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_authdes_cred(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_authdes_verf(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_pmap(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_pmaplist(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_rmtcall_args(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_rmtcallres(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_unixcred(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_netnamestr(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_keystatus(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_keybuf(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_cryptkeyarg(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_cryptkeyarg2(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_cryptkeyres(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_getcredres(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_key_netstarg(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdr_key_netstres(struct b24_xdr *pXdr, void *p)
{
    return b24_xdr_fail2(pXdr, p);
}

bool_t
xdrrec_endofrecord(struct b24_xdr *pXdr, int fSendNow)
{
    (void)pXdr;
    (void)fSendNow;
    return FALSE;
}

bool_t
xdrrec_eof(struct b24_xdr *pXdr)
{
    (void)pXdr;
    return TRUE;
}

bool_t
xdrrec_skiprecord(struct b24_xdr *pXdr)
{
    (void)pXdr;
    return FALSE;
}

int
xencrypt(char *szSecret, char *szPasswd)
{
    (void)szSecret;
    (void)szPasswd;
    errno = ENOSYS;
    return 0;
}

int
xdecrypt(char *szSecret, char *szPasswd)
{
    (void)szSecret;
    (void)szPasswd;
    errno = ENOSYS;
    return 0;
}

/* ---- more RPC client/server stubs --------------------------------------- */

void *
clnt_broadcast(unsigned long uProg, unsigned long uVers, unsigned long uProc,
               void *pfnXargs, void *pArgsp, void *pfnXresults, void *pResultsp,
               void *pfnEachresult)
{
    (void)uProg;
    (void)uVers;
    (void)uProc;
    (void)pfnXargs;
    (void)pArgsp;
    (void)pfnXresults;
    (void)pResultsp;
    (void)pfnEachresult;
    errno = ENOSYS;
    return NULL;
}

void *
clntunix_create(void *pAddr, unsigned long uProg, unsigned long uVers,
                int *pSockp, unsigned uSendsz, unsigned uRecvsz)
{
    (void)pAddr;
    (void)uProg;
    (void)uVers;
    (void)pSockp;
    (void)uSendsz;
    (void)uRecvsz;
    errno = ENOSYS;
    return NULL;
}

void *
authdes_create(const char *szServername, unsigned uWindow, void *pSyncaddr,
               void *pCkkey)
{
    (void)szServername;
    (void)uWindow;
    (void)pSyncaddr;
    (void)pCkkey;
    errno = ENOSYS;
    return NULL;
}

void *
authdes_pk_create(const char *szServername, void *pPkey, unsigned uWindow,
                  void *pSyncaddr, void *pCkkey)
{
    (void)szServername;
    (void)pPkey;
    (void)uWindow;
    (void)pSyncaddr;
    (void)pCkkey;
    errno = ENOSYS;
    return NULL;
}

int
authdes_getucred(void *pAdc, uid_t *pUid, gid_t *pGid, short *pGrouplen,
                 gid_t *aGroups)
{
    (void)pAdc;
    (void)pUid;
    (void)pGid;
    (void)pGrouplen;
    (void)aGroups;
    errno = ENOSYS;
    return 0;
}

int
registerrpc(unsigned long uPrognum, unsigned long uVersnum,
            unsigned long uProcnum, void *pfnProcname, void *pfnInproc,
            void *pfnOutproc)
{
    (void)uPrognum;
    (void)uVersnum;
    (void)uProcnum;
    (void)pfnProcname;
    (void)pfnInproc;
    (void)pfnOutproc;
    errno = ENOSYS;
    return -1;
}

int
pmap_rmtcall(void *pAddr, unsigned long uProg, unsigned long uVers,
             unsigned long uProc, void *pfnXargs, void *pArgsp,
             void *pfnXresults, void *pResultsp, struct timeval *pTout,
             unsigned long *pPortp)
{
    (void)pAddr;
    (void)uProg;
    (void)uVers;
    (void)uProc;
    (void)pfnXargs;
    (void)pArgsp;
    (void)pfnXresults;
    (void)pResultsp;
    (void)pTout;
    if (pPortp != NULL) {
        *pPortp = 0;
    }
    /* RPC_SYSTEMERROR shape */
    return 1;
}

int
netname2user(const char *szName, uid_t *pUidp, gid_t *pGidp, int *pLenp,
             gid_t *aGroups)
{
    (void)szName;
    if (pUidp != NULL) {
        *pUidp = (uid_t)-1;
    }
    if (pGidp != NULL) {
        *pGidp = (gid_t)-1;
    }
    if (pLenp != NULL) {
        *pLenp = 0;
    }
    (void)aGroups;
    return 0;
}

int
key_encryptsession_pk(char *szRemote, void *pPkey, void *pDeskey)
{
    (void)szRemote;
    (void)pPkey;
    (void)pDeskey;
    errno = ENOSYS;
    return -1;
}

int
key_decryptsession_pk(char *szRemote, void *pPkey, void *pDeskey)
{
    (void)szRemote;
    (void)pPkey;
    (void)pDeskey;
    errno = ENOSYS;
    return -1;
}

int
key_get_conv(char *szPname, void *pDeskey)
{
    (void)szPname;
    (void)pDeskey;
    errno = ENOSYS;
    return -1;
}

int
key_secretkey_is_set(void)
{
    return 0;
}

int
key_setnet(void *pArg)
{
    (void)pArg;
    errno = ENOSYS;
    return -1;
}

int
getpublickey(const char *szName, char *szKey)
{
    (void)szName;
    if (szKey != NULL) {
        szKey[0] = '\0';
    }
    return 0;
}

int
getsecretkey(const char *szName, char *szKey, char *szPasswd)
{
    (void)szName;
    (void)szPasswd;
    if (szKey != NULL) {
        szKey[0] = '\0';
    }
    return 0;
}

void
passwd2des(char *szPw, char *pKey)
{
    size_t i;

    if (pKey == NULL) {
        return;
    }
    for (i = 0; i < 8u; i++) {
        pKey[i] = 0;
    }
    if (szPw == NULL) {
        return;
    }
    for (i = 0; i < 8u && szPw[i] != '\0'; i++) {
        pKey[i] = szPw[i];
    }
}

int
pkey_set(const char *szKey, const char *szEncryptedkey)
{
    (void)szKey;
    (void)szEncryptedkey;
    errno = ENOSYS;
    return -1;
}

int
pkey_get(const char *szKey, char *szEncryptedkey)
{
    (void)szKey;
    if (szEncryptedkey != NULL) {
        szEncryptedkey[0] = '\0';
    }
    errno = ENOSYS;
    return -1;
}

/* ---- SVC remainder ------------------------------------------------------ */

void
svcerr_noprog(void *pXprt)
{
    (void)pXprt;
}

void
svcerr_progvers(void *pXprt, unsigned long uLow, unsigned long uHigh)
{
    (void)pXprt;
    (void)uLow;
    (void)uHigh;
}

void
svc_exit(void)
{
}

void *
svcfd_create(int nFd, unsigned uSendsize, unsigned uRecvsize)
{
    (void)nFd;
    (void)uSendsize;
    (void)uRecvsize;
    errno = ENOSYS;
    return NULL;
}

void
svc_getreq(int nRdfds)
{
    (void)nRdfds;
}

void
svc_getreq_common(int nFd)
{
    (void)nFd;
}

void
svc_getreq_poll(void *pPollfd, int nPollretval)
{
    (void)pPollfd;
    (void)nPollretval;
}

void *
svcraw_create(void)
{
    errno = ENOSYS;
    return NULL;
}

void *
svcudp_bufcreate(int nSock, unsigned uSendsz, unsigned uRecvsz)
{
    (void)nSock;
    (void)uSendsz;
    (void)uRecvsz;
    errno = ENOSYS;
    return NULL;
}

int
svcudp_enablecache(void *pXprt, unsigned long uSize)
{
    (void)pXprt;
    (void)uSize;
    return 0;
}

void *
svcunixfd_create(int nFd, unsigned uSendsize, unsigned uRecvsize)
{
    (void)nFd;
    (void)uSendsize;
    (void)uRecvsize;
    errno = ENOSYS;
    return NULL;
}

void
svc_unregister(unsigned long uProg, unsigned long uVers)
{
    (void)uProg;
    (void)uVers;
}

/* ---- obsolete / rare Linux ---------------------------------------------- */

int
bdflush(int nFunc, long nData)
{
    (void)nFunc;
    (void)nData;
    errno = ENOSYS;
    return -1;
}

int
create_module(const char *szName, size_t cb)
{
    (void)szName;
    (void)cb;
    errno = ENOSYS;
    return -1;
}

int
query_module(const char *szName, int nWhich, void *pBuf, size_t cb,
             size_t *pRet)
{
    (void)szName;
    (void)nWhich;
    (void)pBuf;
    (void)cb;
    if (pRet != NULL) {
        *pRet = 0;
    }
    errno = ENOSYS;
    return -1;
}

int
get_kernel_syms(void *pTable)
{
    (void)pTable;
    errno = ENOSYS;
    return -1;
}

int
uselib(const char *szLibrary)
{
    (void)szLibrary;
    errno = ENOSYS;
    return -1;
}

void *
sstk(int nInc)
{
    (void)nInc;
    errno = ENOSYS;
    return NULL;
}

int
ustat(dev_t dev, void *pUbuf)
{
    (void)dev;
    (void)pUbuf;
    errno = ENOSYS;
    return -1;
}

int
vlimit(int nResource, int nValue)
{
    (void)nResource;
    (void)nValue;
    errno = ENOSYS;
    return -1;
}

int
vtimes(void *pCurrent, void *pChild)
{
    (void)pCurrent;
    (void)pChild;
    errno = ENOSYS;
    return -1;
}

int
nfsservctl(int nCmd, void *pArgp, void *pResp)
{
    (void)nCmd;
    (void)pArgp;
    (void)pResp;
    errno = ENOSYS;
    return -1;
}

void
tr_break(void)
{
}

int
rtime(void *pAddrp, struct timeval *pTimep, struct timeval *pTimeout)
{
    (void)pAddrp;
    (void)pTimeout;
    if (pTimep != NULL) {
        pTimep->tv_sec = 0;
        pTimep->tv_usec = 0;
    }
    errno = ENOSYS;
    return -1;
}

int
ruserpass(const char *szHost, char **ppAname, char **ppApass)
{
    (void)szHost;
    (void)ppAname;
    (void)ppApass;
    errno = ENOSYS;
    return -1;
}

void
pthread_kill_other_threads_np(void)
{
    /* LinuxThreads legacy no-op under NPTL-shaped bring-up */
}

/* gshadow stream remainder */
void *
sgetsgent(const char *szLine)
{
    (void)szLine;
    return NULL;
}

/* ---- BSD/string + fortify + C++ guard stubs ---------------------------- */

void *reallocarray(void *p, size_t n, size_t cb);
size_t wcrtomb(char *pS, wchar_t wc, mbstate_t *pSt);
void __chk_fail(void);

void *
__reallocarray(void *p, size_t n, size_t cb)
{
    return reallocarray(p, n, cb);
}

char *
fgetln(FILE *pF, size_t *pcb)
{
    static char aLine[512];
    size_t n = 0;
    int c;

    if (pF == NULL) {
        errno = EINVAL;
        return NULL;
    }
    while (n + 1u < sizeof(aLine)) {
        c = fgetc(pF);
        if (c == EOF) {
            if (n == 0u) {
                if (pcb != NULL) {
                    *pcb = 0;
                }
                return NULL;
            }
            break;
        }
        aLine[n++] = (char)c;
        if (c == '\n') {
            break;
        }
    }
    aLine[n] = '\0';
    if (pcb != NULL) {
        *pcb = n;
    }
    return aLine;
}

char *
strnstr(const char *szHay, const char *szNeedle, size_t cb)
{
    size_t cbN;
    size_t i;

    if (szHay == NULL || szNeedle == NULL) {
        return NULL;
    }
    cbN = strlen(szNeedle);
    if (cbN == 0u) {
        return (char *)(uintptr_t)szHay;
    }
    if (cbN > cb) {
        return NULL;
    }
    for (i = 0; i + cbN <= cb; i++) {
        if (szHay[i] == '\0') {
            return NULL;
        }
        if (memcmp(szHay + i, szNeedle, cbN) == 0) {
            return (char *)(uintptr_t)(szHay + i);
        }
    }
    return NULL;
}

int
timingsafe_bcmp(const void *pA, const void *pB, size_t cb)
{
    const unsigned char *pX = (const unsigned char *)pA;
    const unsigned char *pY = (const unsigned char *)pB;
    unsigned char uDiff = 0;
    size_t i;

    for (i = 0; i < cb; i++) {
        uDiff = (unsigned char)(uDiff | (pX[i] ^ pY[i]));
    }
    return uDiff != 0;
}

int
timingsafe_memcmp(const void *pA, const void *pB, size_t cb)
{
    const unsigned char *pX = (const unsigned char *)pA;
    const unsigned char *pY = (const unsigned char *)pB;
    int nDone = 0;
    int nRes = 0;
    size_t i;

    for (i = 0; i < cb; i++) {
        int nLt = (pX[i] < pY[i]) ? -1 : 0;
        int nGt = (pX[i] > pY[i]) ? 1 : 0;
        int nCmp = nLt | nGt;

        nRes = (nDone == 0) ? nCmp : nRes;
        nDone = nDone | (nCmp != 0);
    }
    return nRes;
}

void
freehostent(struct hostent *pHe)
{
    (void)pHe;
    /* getipnodeby* bring-up returns static storage */
}

struct hostent *
getipnodebyname(const char *szName, int nAf, int nFlags, int *pError)
{
    (void)nFlags;
    if (pError != NULL) {
        *pError = 0;
    }
    if (szName == NULL) {
        if (pError != NULL) {
            *pError = HOST_NOT_FOUND;
        }
        return NULL;
    }
    /* numeric / localhost only via existing gethostbyname path shape */
    {
        struct hostent *p = gethostbyname(szName);

        (void)nAf;
        if (p == NULL && pError != NULL) {
            *pError = HOST_NOT_FOUND;
        }
        return p;
    }
}

struct hostent *
getipnodebyaddr(const void *pAddr, size_t cb, int nAf, int *pError)
{
    struct hostent *p;

    if (pError != NULL) {
        *pError = 0;
    }
    p = gethostbyaddr(pAddr, (socklen_t)cb, nAf);
    if (p == NULL && pError != NULL) {
        *pError = HOST_NOT_FOUND;
    }
    return p;
}

size_t
__wcrtomb_chk(char *pS, wchar_t wc, mbstate_t *pSt, size_t cbBuflen)
{
    char aTmp[8];
    size_t n;

    if (pS == NULL) {
        return wcrtomb(NULL, wc, pSt);
    }
    n = wcrtomb(aTmp, wc, pSt);
    if (n == (size_t)-1) {
        return n;
    }
    if (n > cbBuflen) {
        __chk_fail();
    }
    memcpy(pS, aTmp, n);
    return n;
}

/* Itanium C++ ABI guard + pure virtual (link stubs; no exception runtime). */
int
__cxa_guard_acquire(int64_t *pGuard)
{
    if (pGuard == NULL) {
        return 0;
    }
    if (((unsigned char *)pGuard)[0] != 0) {
        return 0; /* already init */
    }
    return 1; /* caller should init */
}

void
__cxa_guard_release(int64_t *pGuard)
{
    if (pGuard != NULL) {
        ((unsigned char *)pGuard)[0] = 1;
    }
}

void
__cxa_guard_abort(int64_t *pGuard)
{
    (void)pGuard;
}

void
__cxa_pure_virtual(void)
{
    abort();
}

void
__cxa_deleted_virtual(void)
{
    abort();
}

/* Legacy regex engine surface (step/advance) — fail-closed. */
int
step(const char *szString, const char *szExpbuf)
{
    (void)szString;
    (void)szExpbuf;
    return 0;
}

int
advance(const char *szString, const char *szExpbuf)
{
    (void)szString;
    (void)szExpbuf;
    return 0;
}

/* tmpnam64 alias shape (same as tmpnam bring-up). */
char *tmpnam(char *szBuf);

char *
tmpnam64(char *szBuf)
{
    return tmpnam(szBuf);
}
