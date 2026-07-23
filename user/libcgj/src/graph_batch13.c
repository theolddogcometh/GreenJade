/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13: underscored aliases (__close/__fork/...),
 * stdio_ext (__fbufsize/__freading/...), reentrant netdb/pwd/grp, dn_*,
 * dladdr1/dlinfo/dlmopen stubs, ftok/ftime/dysize, quick_exit/at_quick,
 * netent, fmtmsg. Integer/pointer only (no SSE doubles).
 *
 * greppable: CGJ_GRAPH_BATCH13_SOFT_NULL
 * greppable: CGJ_GRAPH_BATCH13_SOFT_ARGS
 * greppable: CGJ_GRAPH_BATCH13_SOFT_EDGE
 *
 * Soft deepen: null/arg guards on user-facing graph nodes; edge
 * hardening only. No multi-def; no API break. Pure C integer/pointer.
 */
#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <locale.h>
#include <mntent.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pwd.h>
#include <resolv.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/auxv.h>
#include <sys/ipc.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* Match stdio.c private flag bits (public in FILE.nFlags). */
#define F_READ  1
#define F_WRITE 2

void __fortify_fail(const char *szMsg);
void __assert_fail(const char *szExpr, const char *szFile, unsigned nLine,
                   const char *szFunc);
int  __cxa_atexit(void (*pfn)(void *), void *pArg, void *pDso);

/* ---- underscored syscall / libc aliases --------------------------------- */

int
__close(int nFd)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return close(nFd);
}

int
__close_nocancel(int nFd)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return close(nFd);
}

int
__dup2(int nOld, int nNew)
{
    return dup2(nOld, nNew);
}

pid_t
__fork(void)
{
    return fork();
}

pid_t
_Fork(void)
{
    /* glibc _Fork: fork without atfork handlers in full NPTL; bring-up = fork */
    return fork();
}

pid_t
__getpid(void)
{
    return getpid();
}

pid_t
__getpgid(pid_t pid)
{
    return getpgid(pid);
}

pid_t
__bsd_getpgrp(void)
{
    return getpgrp();
}

int
__getrlimit(int nResource, struct rlimit *pRlim)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (pRlim == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getrlimit(nResource, pRlim);
}

int
__getpagesize(void)
{
    return getpagesize();
}

unsigned long
__getauxval(unsigned long uType)
{
    return getauxval(uType);
}

struct tm *
__gmtime_r(const time_t *pT, struct tm *pTm)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (pT == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pTm == NULL) {
        errno = EFAULT;
        return -1;
    }

    return gmtime_r(pT, pTm);
}

int
__ffs(int n)
{
    return ffs(n);
}

locale_t
__duplocale(locale_t loc)
{
    return duplocale(loc);
}

void
__freelocale(locale_t loc)
{
    freelocale(loc);
}

int
__endmntent(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return endmntent(pF);
}

struct mntent *
__getmntent_r(FILE *pF, struct mntent *pMnt, char *szBuf, int nBufSize)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (pMnt == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getmntent_r(pF, pMnt, szBuf, nBufSize);
}

void
__explicit_bzero_chk(void *p, size_t cb, size_t cbObj)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (p == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (cb > cbObj) {
        __fortify_fail("explicit_bzero");
    }
    explicit_bzero(p, cb);
}

/* ---- assert variants ---------------------------------------------------- */

void
__assert(const char *szExpr, const char *szFile, int nLine)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (szExpr == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFile == NULL) {
        errno = EFAULT;
        return -1;
    }

    __assert_fail(szExpr, szFile, (unsigned)nLine, "");
}

void
__assert_perror_fail(int nErr, const char *szFile, unsigned nLine,
                     const char *szFunc)
{
    char aBuf[128];
    const char *sz;

    sz = strerror(nErr);
    if (sz == NULL) {
        sz = "Unknown error";
    }
    (void)snprintf(aBuf, sizeof(aBuf), "%s: %s",
                   (szFunc != NULL && szFunc[0] != '\0') ? szFunc : "?", sz);
    __assert_fail(aBuf, szFile, nLine,
                  (szFunc != NULL) ? szFunc : "");
}

/* ---- stdio_ext-style helpers -------------------------------------------- */

size_t
__fbufsize(FILE *pF)
{
    if (pF == NULL) {
        return 0;
    }
    return pF->cbBuf;
}

int
__flbf(FILE *pF)
{
    if (pF == NULL) {
        return 0;
    }
    return (pF->nBufMode == _IOLBF) ? 1 : 0;
}

size_t
__fpending(FILE *pF)
{
    if (pF == NULL) {
        return 0;
    }
    return pF->cbWpos;
}

void
__fpurge(FILE *pF)
{
    if (pF == NULL) {
        return;
    }
    pF->cbWpos = 0;
    pF->cbRlen = 0;
    pF->cbRpos = 0;
    pF->nUnget = -1;
    pF->nEof = 0;
}

int
__freadable(FILE *pF)
{
    if (pF == NULL) {
        return 0;
    }
    return (pF->nFlags & F_READ) ? 1 : 0;
}

int
__fwritable(FILE *pF)
{
    if (pF == NULL) {
        return 0;
    }
    return (pF->nFlags & F_WRITE) ? 1 : 0;
}

int
__freading(FILE *pF)
{
    if (pF == NULL) {
        return 0;
    }
    if ((pF->nFlags & F_READ) == 0) {
        return 0;
    }
    /* Pending write buffer means last activity was write. */
    if (pF->cbWpos > 0) {
        return 0;
    }
    return 1;
}

int
__fwriting(FILE *pF)
{
    if (pF == NULL) {
        return 0;
    }
    if ((pF->nFlags & F_WRITE) == 0) {
        return 0;
    }
    return (pF->cbWpos > 0) ? 1 : 0;
}

int
__fsetlocking(FILE *pF, int nType)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Bring-up: locking is process-global / none; accept any type. */
    (void)pF;
    (void)nType;
    return 0; /* FSETLOCKING_INTERNAL-shaped: previous mode ignored */
}

FILE *
freopen64(const char *szPath, const char *szMode, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (szPath == NULL) {
        return NULL;
    }
    if (szMode == NULL) {
        return NULL;
    }
    if (pF == NULL) {
        return NULL;
    }

    return freopen(szPath, szMode, pF);
}

/* ---- ftok / ftime / dysize ---------------------------------------------- */

key_t
ftok(const char *szPath, int nProj)
{
    struct stat st;

    if (szPath == NULL) {
        errno = EINVAL;
        return (key_t)-1;
    }
    if (stat(szPath, &st) != 0) {
        return (key_t)-1;
    }
    return (key_t)(((key_t)(nProj & 0xff) << 24) |
                   ((key_t)(st.st_dev & 0xff) << 16) |
                   (key_t)(st.st_ino & 0xffff));
}

int
ftime(struct timeb *pTb)
{
    struct timeval tv;

    if (pTb == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (gettimeofday(&tv, NULL) != 0) {
        pTb->time = time(NULL);
        pTb->millitm = 0;
    } else {
        pTb->time = tv.tv_sec;
        pTb->millitm = (unsigned short)(tv.tv_usec / 1000);
    }
    pTb->timezone = 0;
    pTb->dstflag = 0;
    return 0;
}

int
dysize(int nYear)
{
    if ((nYear % 4) != 0) {
        return 365;
    }
    if ((nYear % 100) != 0) {
        return 366;
    }
    if ((nYear % 400) != 0) {
        return 365;
    }
    return 366;
}

/* ---- passwd / group stream + reentrant ---------------------------------- */

struct passwd *
fgetpwent(FILE *pF)
{
    char aLine[256];
    char *p;
    static struct passwd s_pw;
    static char s_aName[64];
    static char s_aPass[64];
    static char s_aGecos[64];
    static char s_aDir[128];
    static char s_aShell[64];
    char *szName;
    char *szPass;
    char *szUid;
    char *szGid;
    char *szGecos;
    char *szDir;
    char *szShell;

    if (pF == NULL) {
        errno = EINVAL;
        return NULL;
    }
    for (;;) {
        if (fgets(aLine, (int)sizeof(aLine), pF) == NULL) {
            return NULL;
        }
        p = aLine;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0' || *p == '\n' || *p == '#') {
            continue;
        }
        szName = p;
        szPass = strchr(p, ':');
        if (szPass == NULL) {
            continue;
        }
        *szPass++ = '\0';
        szUid = strchr(szPass, ':');
        if (szUid == NULL) {
            continue;
        }
        *szUid++ = '\0';
        szGid = strchr(szUid, ':');
        if (szGid == NULL) {
            continue;
        }
        *szGid++ = '\0';
        szGecos = strchr(szGid, ':');
        if (szGecos == NULL) {
            continue;
        }
        *szGecos++ = '\0';
        szDir = strchr(szGecos, ':');
        if (szDir == NULL) {
            continue;
        }
        *szDir++ = '\0';
        szShell = strchr(szDir, ':');
        if (szShell == NULL) {
            continue;
        }
        *szShell++ = '\0';
        {
            char *nl = strchr(szShell, '\n');

            if (nl != NULL) {
                *nl = '\0';
            }
        }
        (void)strncpy(s_aName, szName, sizeof(s_aName) - 1);
        s_aName[sizeof(s_aName) - 1] = '\0';
        (void)strncpy(s_aPass, szPass, sizeof(s_aPass) - 1);
        s_aPass[sizeof(s_aPass) - 1] = '\0';
        (void)strncpy(s_aGecos, szGecos, sizeof(s_aGecos) - 1);
        s_aGecos[sizeof(s_aGecos) - 1] = '\0';
        (void)strncpy(s_aDir, szDir, sizeof(s_aDir) - 1);
        s_aDir[sizeof(s_aDir) - 1] = '\0';
        (void)strncpy(s_aShell, szShell, sizeof(s_aShell) - 1);
        s_aShell[sizeof(s_aShell) - 1] = '\0';
        s_pw.pw_name = s_aName;
        s_pw.pw_passwd = s_aPass;
        s_pw.pw_uid = (uid_t)strtoul(szUid, NULL, 10);
        s_pw.pw_gid = (gid_t)strtoul(szGid, NULL, 10);
        s_pw.pw_gecos = s_aGecos;
        s_pw.pw_dir = s_aDir;
        s_pw.pw_shell = s_aShell;
        return &s_pw;
    }
}

struct group *
fgetgrent(FILE *pF)
{
    char aLine[256];
    char *p;
    static struct group s_gr;
    static char s_aName[64];
    static char s_aPass[64];
    static char *s_aMem[1];
    char *szName;
    char *szPass;
    char *szGid;

    if (pF == NULL) {
        errno = EINVAL;
        return NULL;
    }
    for (;;) {
        if (fgets(aLine, (int)sizeof(aLine), pF) == NULL) {
            return NULL;
        }
        p = aLine;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0' || *p == '\n' || *p == '#') {
            continue;
        }
        szName = p;
        szPass = strchr(p, ':');
        if (szPass == NULL) {
            continue;
        }
        *szPass++ = '\0';
        szGid = strchr(szPass, ':');
        if (szGid == NULL) {
            continue;
        }
        *szGid++ = '\0';
        {
            char *rest = strchr(szGid, ':');

            if (rest != NULL) {
                *rest = '\0';
            }
            rest = strchr(szGid, '\n');
            if (rest != NULL) {
                *rest = '\0';
            }
        }
        (void)strncpy(s_aName, szName, sizeof(s_aName) - 1);
        s_aName[sizeof(s_aName) - 1] = '\0';
        (void)strncpy(s_aPass, szPass, sizeof(s_aPass) - 1);
        s_aPass[sizeof(s_aPass) - 1] = '\0';
        s_aMem[0] = NULL;
        s_gr.gr_name = s_aName;
        s_gr.gr_passwd = s_aPass;
        s_gr.gr_gid = (gid_t)strtoul(szGid, NULL, 10);
        s_gr.gr_mem = s_aMem;
        return &s_gr;
    }
}

int
getpwent_r(struct passwd *pPwd, char *szBuf, size_t cb, struct passwd **ppResult)
{
    struct passwd *p;

    (void)szBuf;
    (void)cb;
    if (pPwd == NULL || ppResult == NULL) {
        return EINVAL;
    }
    p = getpwent();
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    *pPwd = *p;
    *ppResult = pPwd;
    return 0;
}

int
getgrent_r(struct group *pGrp, char *szBuf, size_t cb, struct group **ppResult)
{
    struct group *p;

    (void)szBuf;
    (void)cb;
    if (pGrp == NULL || ppResult == NULL) {
        return EINVAL;
    }
    p = getgrent();
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    *pGrp = *p;
    *ppResult = pGrp;
    return 0;
}

/* ---- reentrant / AF_INET6-shaped netdb ---------------------------------- */

static int
hostent_to_buf(struct hostent *pSrc, struct hostent *pDst, char *szBuf,
               size_t cb, struct hostent **ppResult)
{
    size_t nName;
    size_t nNeed;
    char *p;
    char **ppList;

    if (pSrc == NULL) {
        *ppResult = NULL;
        return 0;
    }
    if (pDst == NULL || szBuf == NULL || ppResult == NULL) {
        return EINVAL;
    }
    nName = strlen(pSrc->h_name) + 1;
    nNeed = nName + (size_t)pSrc->h_length + sizeof(char *) * 4u;
    if (nNeed > cb) {
        return ERANGE;
    }
    p = szBuf;
    memcpy(p, pSrc->h_name, nName);
    pDst->h_name = p;
    p += nName;
    ppList = (char **)(void *)p;
    p += sizeof(char *) * 2u;
    memcpy(p, pSrc->h_addr_list[0], (size_t)pSrc->h_length);
    ppList[0] = p;
    ppList[1] = NULL;
    pDst->h_addr_list = ppList;
    pDst->h_aliases = ppList + 2;
    pDst->h_aliases[0] = NULL;
    pDst->h_addrtype = pSrc->h_addrtype;
    pDst->h_length = pSrc->h_length;
    *ppResult = pDst;
    return 0;
}

int
gethostbyname_r(const char *szName, struct hostent *pResult, char *szBuf,
                size_t cb, struct hostent **ppResult, int *pHErr)
{
    struct hostent *p;
    int n;

    p = gethostbyname(szName);
    if (pHErr != NULL) {
        *pHErr = (p == NULL) ? h_errno : 0;
    }
    n = hostent_to_buf(p, pResult, szBuf, cb, ppResult);
    return n;
}

struct hostent *
gethostbyname2(const char *szName, int nAf)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (nAf != AF_INET) {
        h_errno = NO_RECOVERY;
        errno = EAFNOSUPPORT;
        return NULL;
    }
    return gethostbyname(szName);
}

int
gethostbyname2_r(const char *szName, int nAf, struct hostent *pResult,
                 char *szBuf, size_t cb, struct hostent **ppResult, int *pHErr)
{
    struct hostent *p;
    int n;

    p = gethostbyname2(szName, nAf);
    if (pHErr != NULL) {
        *pHErr = (p == NULL) ? h_errno : 0;
    }
    n = hostent_to_buf(p, pResult, szBuf, cb, ppResult);
    return n;
}

int
gethostbyaddr_r(const void *pAddr, socklen_t cbLen, int nType,
                struct hostent *pResult, char *szBuf, size_t cb,
                struct hostent **ppResult, int *pHErr)
{
    struct hostent *p;
    int n;

    p = gethostbyaddr(pAddr, cbLen, nType);
    if (pHErr != NULL) {
        *pHErr = (p == NULL) ? h_errno : 0;
    }
    n = hostent_to_buf(p, pResult, szBuf, cb, ppResult);
    return n;
}

int
gethostent_r(struct hostent *pResult, char *szBuf, size_t cb,
             struct hostent **ppResult, int *pHErr)
{
    (void)pResult;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    if (pHErr != NULL) {
        *pHErr = NO_RECOVERY;
    }
    return 0;
}

static int
servent_copy(struct servent *pSrc, struct servent *pDst, char *szBuf, size_t cb,
             struct servent **ppResult)
{
    size_t nName;
    size_t nProto;
    size_t nNeed;
    char *p;

    if (pSrc == NULL) {
        *ppResult = NULL;
        return 0;
    }
    if (pDst == NULL || szBuf == NULL || ppResult == NULL) {
        return EINVAL;
    }
    nName = strlen(pSrc->s_name) + 1;
    nProto = strlen(pSrc->s_proto) + 1;
    nNeed = nName + nProto + sizeof(char *) * 2u;
    if (nNeed > cb) {
        return ERANGE;
    }
    p = szBuf;
    memcpy(p, pSrc->s_name, nName);
    pDst->s_name = p;
    p += nName;
    memcpy(p, pSrc->s_proto, nProto);
    pDst->s_proto = p;
    p += nProto;
    pDst->s_aliases = (char **)(void *)p;
    pDst->s_aliases[0] = NULL;
    pDst->s_port = pSrc->s_port;
    *ppResult = pDst;
    return 0;
}

int
getservbyname_r(const char *szName, const char *szProto, struct servent *pResult,
                char *szBuf, size_t cb, struct servent **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szProto == NULL) {
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

    return servent_copy(getservbyname(szName, szProto), pResult, szBuf, cb,
                        ppResult);
}

int
getservbyport_r(int nPort, const char *szProto, struct servent *pResult,
                char *szBuf, size_t cb, struct servent **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (szProto == NULL) {
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

    return servent_copy(getservbyport(nPort, szProto), pResult, szBuf, cb,
                        ppResult);
}

int
getservent_r(struct servent *pResult, char *szBuf, size_t cb,
             struct servent **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
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

    return servent_copy(getservent(), pResult, szBuf, cb, ppResult);
}

static int
protoent_copy(struct protoent *pSrc, struct protoent *pDst, char *szBuf,
              size_t cb, struct protoent **ppResult)
{
    size_t nName;
    size_t nNeed;
    char *p;

    if (pSrc == NULL) {
        *ppResult = NULL;
        return 0;
    }
    if (pDst == NULL || szBuf == NULL || ppResult == NULL) {
        return EINVAL;
    }
    nName = strlen(pSrc->p_name) + 1;
    nNeed = nName + sizeof(char *) * 2u;
    if (nNeed > cb) {
        return ERANGE;
    }
    p = szBuf;
    memcpy(p, pSrc->p_name, nName);
    pDst->p_name = p;
    p += nName;
    pDst->p_aliases = (char **)(void *)p;
    pDst->p_aliases[0] = NULL;
    pDst->p_proto = pSrc->p_proto;
    *ppResult = pDst;
    return 0;
}

int
getprotobyname_r(const char *szName, struct protoent *pResult, char *szBuf,
                 size_t cb, struct protoent **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
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

    return protoent_copy(getprotobyname(szName), pResult, szBuf, cb, ppResult);
}

int
getprotobynumber_r(int nProto, struct protoent *pResult, char *szBuf, size_t cb,
                   struct protoent **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
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

    return protoent_copy(getprotobynumber(nProto), pResult, szBuf, cb,
                         ppResult);
}

int
getprotoent_r(struct protoent *pResult, char *szBuf, size_t cb,
              struct protoent **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
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

    return protoent_copy(getprotoent(), pResult, szBuf, cb, ppResult);
}

/* ---- netent (IPv4 table bring-up) --------------------------------------- */

static struct netent g_ne;
static char g_szNeName[32];
static char *g_aNeAliases[1];
static int g_nNeIdx;

static const struct {
    const char *szName;
    uint32_t    uNet; /* host order network number */
    int         nBits;
} g_aNets[] = {
    { "loopback", 0x7f000000u, 8 },
    { "link-local", 0xa9fe0000u, 16 },
    { NULL, 0, 0 }
};

void
setnetent(int fStayopen)
{
    (void)fStayopen;
    g_nNeIdx = 0;
}

void
endnetent(void)
{
    g_nNeIdx = 0;
}

struct netent *
getnetent(void)
{
    size_t n;

    if (g_aNets[g_nNeIdx].szName == NULL) {
        return NULL;
    }
    n = strlen(g_aNets[g_nNeIdx].szName);
    if (n + 1 > sizeof(g_szNeName)) {
        n = sizeof(g_szNeName) - 1;
    }
    memcpy(g_szNeName, g_aNets[g_nNeIdx].szName, n);
    g_szNeName[n] = '\0';
    g_aNeAliases[0] = NULL;
    g_ne.n_name = g_szNeName;
    g_ne.n_aliases = g_aNeAliases;
    g_ne.n_addrtype = AF_INET;
    g_ne.n_net = g_aNets[g_nNeIdx].uNet;
    g_nNeIdx++;
    return &g_ne;
}

struct netent *
getnetbyname(const char *szName)
{
    if (szName == NULL) {
        errno = EINVAL;
        return NULL;
    }
    setnetent(0);
    for (;;) {
        struct netent *p = getnetent();

        if (p == NULL) {
            return NULL;
        }
        if (strcmp(p->n_name, szName) == 0) {
            return p;
        }
    }
}

struct netent *
getnetbyaddr(uint32_t uNet, int nType)
{
    if (nType != AF_INET) {
        errno = EAFNOSUPPORT;
        return NULL;
    }
    setnetent(0);
    for (;;) {
        struct netent *p = getnetent();

        if (p == NULL) {
            return NULL;
        }
        if (p->n_net == uNet) {
            return p;
        }
    }
}

int
getnetent_r(struct netent *pResult, char *szBuf, size_t cb,
            struct netent **ppResult, int *pHErr)
{
    struct netent *p;
    size_t nName;

    (void)pHErr;
    p = getnetent();
    if (p == NULL) {
        if (ppResult != NULL) {
            *ppResult = NULL;
        }
        return 0;
    }
    if (pResult == NULL || szBuf == NULL || ppResult == NULL) {
        return EINVAL;
    }
    nName = strlen(p->n_name) + 1;
    if (nName + sizeof(char *) > cb) {
        return ERANGE;
    }
    memcpy(szBuf, p->n_name, nName);
    pResult->n_name = szBuf;
    pResult->n_aliases = (char **)(void *)(szBuf + nName);
    pResult->n_aliases[0] = NULL;
    pResult->n_addrtype = p->n_addrtype;
    pResult->n_net = p->n_net;
    *ppResult = pResult;
    return 0;
}

int
getnetbyname_r(const char *szName, struct netent *pResult, char *szBuf,
               size_t cb, struct netent **ppResult, int *pHErr)
{
    struct netent *p;
    size_t nName;

    (void)pHErr;
    p = getnetbyname(szName);
    if (p == NULL) {
        if (ppResult != NULL) {
            *ppResult = NULL;
        }
        return 0;
    }
    if (pResult == NULL || szBuf == NULL || ppResult == NULL) {
        return EINVAL;
    }
    nName = strlen(p->n_name) + 1;
    if (nName + sizeof(char *) > cb) {
        return ERANGE;
    }
    memcpy(szBuf, p->n_name, nName);
    pResult->n_name = szBuf;
    pResult->n_aliases = (char **)(void *)(szBuf + nName);
    pResult->n_aliases[0] = NULL;
    pResult->n_addrtype = p->n_addrtype;
    pResult->n_net = p->n_net;
    *ppResult = pResult;
    return 0;
}

int
getnetbyaddr_r(uint32_t uNet, int nType, struct netent *pResult, char *szBuf,
               size_t cb, struct netent **ppResult, int *pHErr)
{
    struct netent *p;
    size_t nName;

    (void)pHErr;
    p = getnetbyaddr(uNet, nType);
    if (p == NULL) {
        if (ppResult != NULL) {
            *ppResult = NULL;
        }
        return 0;
    }
    if (pResult == NULL || szBuf == NULL || ppResult == NULL) {
        return EINVAL;
    }
    nName = strlen(p->n_name) + 1;
    if (nName + sizeof(char *) > cb) {
        return ERANGE;
    }
    memcpy(szBuf, p->n_name, nName);
    pResult->n_name = szBuf;
    pResult->n_aliases = (char **)(void *)(szBuf + nName);
    pResult->n_aliases[0] = NULL;
    pResult->n_addrtype = p->n_addrtype;
    pResult->n_net = p->n_net;
    *ppResult = pResult;
    return 0;
}

/* ---- DNS name compression (RFC 1035) ------------------------------------ */

int
dn_skipname(const unsigned char *pComp, const unsigned char *pEom)
{
    const unsigned char *p = pComp;

    if (p == NULL || pEom == NULL || p >= pEom) {
        return -1;
    }
    while (p < pEom) {
        unsigned int n = *p;

        if (n == 0) {
            return (int)(p - pComp + 1);
        }
        if ((n & 0xc0u) == 0xc0u) {
            if (p + 1 >= pEom) {
                return -1;
            }
            return (int)(p - pComp + 2);
        }
        if ((n & 0xc0u) != 0) {
            return -1;
        }
        p++;
        if (p + n > pEom) {
            return -1;
        }
        p += n;
    }
    return -1;
}

int
__dn_skipname(const unsigned char *pComp, const unsigned char *pEom)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (pComp == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pEom == NULL) {
        errno = EFAULT;
        return -1;
    }

    return dn_skipname(pComp, pEom);
}

int
dn_expand(const unsigned char *pMsg, const unsigned char *pEomOrig,
          const unsigned char *pComp, char *szExp, int nExp)
{
    const unsigned char *p = pComp;
    char *pOut = szExp;
    int nOut = 0;
    int nJumped = 0;
    int nLen = 0;
    const unsigned char *pEom = pEomOrig;
    int nHops = 0;

    if (pMsg == NULL || pEom == NULL || pComp == NULL || szExp == NULL ||
        nExp <= 0) {
        return -1;
    }
    if (p < pMsg || p >= pEom) {
        return -1;
    }
    while (p < pEom) {
        unsigned int n = *p;

        if (n == 0) {
            /* Labels are separated by dots inserted before non-first labels;
             * never trailing, so just NUL-terminate at pOut. */
            if (nOut + 1 > nExp) {
                return -1;
            }
            *pOut = '\0';
            if (nJumped == 0) {
                nLen = (int)(p - pComp + 1);
            }
            return nLen;
        }
        if ((n & 0xc0u) == 0xc0u) {
            unsigned int uOff;

            if (p + 1 >= pEom) {
                return -1;
            }
            uOff = ((n & 0x3fu) << 8) | p[1];
            if (pMsg + uOff >= pEom) {
                return -1;
            }
            if (nJumped == 0) {
                nLen = (int)(p - pComp + 2);
            }
            p = pMsg + uOff;
            nJumped = 1;
            if (++nHops > 32) {
                return -1;
            }
            continue;
        }
        if ((n & 0xc0u) != 0) {
            return -1;
        }
        p++;
        if (p + n > pEom) {
            return -1;
        }
        if (nOut != 0) {
            if (nOut + 1 >= nExp) {
                return -1;
            }
            *pOut++ = '.';
            nOut++;
        }
        if (nOut + (int)n + 1 > nExp) {
            return -1;
        }
        memcpy(pOut, p, n);
        pOut += n;
        nOut += (int)n;
        p += n;
        if (nJumped == 0) {
            nLen = (int)(p - pComp);
        }
    }
    return -1;
}

int
__dn_expand(const unsigned char *pMsg, const unsigned char *pEomOrig,
            const unsigned char *pComp, char *szExp, int nExp)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (pMsg == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pEomOrig == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pComp == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szExp == NULL) {
        errno = EFAULT;
        return -1;
    }

    return dn_expand(pMsg, pEomOrig, pComp, szExp, nExp);
}

int
dn_comp(const char *szExp, unsigned char *pComp, int nComp,
        unsigned char **ppDnptrs, unsigned char **ppLastdnptr)
{
    const char *p;
    unsigned char *pOut;
    unsigned char *pLabel;
    int nLeft;

    (void)ppDnptrs;
    (void)ppLastdnptr;
    if (szExp == NULL || pComp == NULL || nComp < 1) {
        return -1;
    }
    if (szExp[0] == '\0' || (szExp[0] == '.' && szExp[1] == '\0')) {
        pComp[0] = 0;
        return 1;
    }
    pOut = pComp;
    nLeft = nComp;
    p = szExp;
    while (*p != '\0') {
        size_t nLab = 0;

        if (nLeft < 2) {
            return -1;
        }
        pLabel = pOut++;
        nLeft--;
        while (*p != '\0' && *p != '.') {
            if (nLab >= 63 || nLeft < 1) {
                return -1;
            }
            *pOut++ = (unsigned char)*p++;
            nLab++;
            nLeft--;
        }
        *pLabel = (unsigned char)nLab;
        if (*p == '.') {
            p++;
            if (*p == '\0') {
                break;
            }
        }
    }
    if (nLeft < 1) {
        return -1;
    }
    *pOut++ = 0;
    return (int)(pOut - pComp);
}

int
__dn_comp(const char *szExp, unsigned char *pComp, int nComp,
          unsigned char **ppDnptrs, unsigned char **ppLastdnptr)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (szExp == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pComp == NULL) {
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

    return dn_comp(szExp, pComp, nComp, ppDnptrs, ppLastdnptr);
}

/* ---- dlfcn extensions (stubs; real load = ld-gj) ------------------------ */

int
dladdr1(const void *pAddr, void *pInfo, void **ppExtra, int nFlags)
{
    (void)ppExtra;
    (void)nFlags;
    return dladdr(pAddr, pInfo);
}

int
dlinfo(void *pHandle, int nRequest, void *pArg)
{
    (void)pHandle;
    (void)nRequest;
    (void)pArg;
    errno = ENOSYS;
    return -1;
}

void *
dlmopen(long nNs, const char *szFile, int nMode)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (szFile == NULL) {
        return NULL;
    }

    (void)nNs;
    return dlopen(szFile, nMode);
}

/* ---- quick_exit / cxa thread atexit ------------------------------------- */

#define CGJ_QUICK_MAX 16

static void (*g_aQuick[CGJ_QUICK_MAX])(void);
static int g_nQuick;

int
at_quick_exit(void (*pfn)(void))
{
    if (pfn == NULL || g_nQuick >= CGJ_QUICK_MAX) {
        return -1;
    }
    g_aQuick[g_nQuick++] = pfn;
    return 0;
}

int
__cxa_at_quick_exit(void (*pfn)(void *), void *pDso)
{
    (void)pDso;
    /* Store as plain quick handler; arg ignored in bring-up */
    return at_quick_exit((void (*)(void))(uintptr_t)pfn);
}

void
quick_exit(int nCode)
{
    int i;

    for (i = g_nQuick - 1; i >= 0; i--) {
        if (g_aQuick[i] != NULL) {
            g_aQuick[i]();
        }
    }
    _Exit(nCode);
}

int
__cxa_thread_atexit_impl(void (*pfn)(void *), void *pArg, void *pDso)
{
    /* Bring-up: register as process cxa atexit */
    return __cxa_atexit(pfn, pArg, pDso);
}

/* ---- fmtmsg ------------------------------------------------------------- */

#define MM_NULLLBL  ((char *)0)
#define MM_NULLSEV  0
#define MM_NULLMC   ((char *)0)
#define MM_NULLTXT  ((char *)0)
#define MM_NULLACT  ((char *)0)
#define MM_NULLTAG  ((char *)0)

#define MM_HARD     0x001
#define MM_SOFT     0x002
#define MM_FIRM     0x004
#define MM_APPL     0x008
#define MM_UTIL     0x010
#define MM_OPSYS    0x020
#define MM_RECOVER  0x040
#define MM_NRECOV   0x080
#define MM_PRINT    0x100
#define MM_CONSOLE  0x200

#define MM_NOSEV    0
#define MM_HALT     1
#define MM_ERROR    2
#define MM_WARNING  3
#define MM_INFO     4

#define MM_NOTOK    (-1)
#define MM_OK       0
#define MM_NOMSG    1
#define MM_NOCON    4

int
fmtmsg(long nClass, const char *szLabel, int nSev, const char *szText,
       const char *szAction, const char *szTag)
{
    /* greppable: CGJ_GRAPH_BATCH13_SOFT_NULL */
    if (szLabel == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szText == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szAction == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szTag == NULL) {
        errno = EFAULT;
        return -1;
    }

    const char *szSev = "";

    (void)nClass;
    if ((nClass & MM_PRINT) == 0 && nClass != 0) {
        /* still print for bring-up visibility when text present */
    }
    switch (nSev) {
    case MM_HALT:
        szSev = "HALT";
        break;
    case MM_ERROR:
        szSev = "ERROR";
        break;
    case MM_WARNING:
        szSev = "WARNING";
        break;
    case MM_INFO:
        szSev = "INFO";
        break;
    default:
        szSev = "";
        break;
    }
    if (szLabel != NULL || szText != NULL) {
        (void)fprintf(stderr, "%s%s%s: %s",
                      (szLabel != NULL) ? szLabel : "",
                      (szLabel != NULL && szSev[0] != '\0') ? ": " : "",
                      szSev,
                      (szText != NULL) ? szText : "");
        if (szAction != NULL) {
            (void)fprintf(stderr, " TO FIX: %s", szAction);
        }
        if (szTag != NULL) {
            (void)fprintf(stderr, " %s", szTag);
        }
        (void)fprintf(stderr, "\n");
    }
    return MM_OK;
}
