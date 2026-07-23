/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17: more underscored libc/pthread/sched/sig
 * aliases, fortify path helpers, LFS statvfs/fseek aliases, fts64,
 * fget*ent_r, getpw/gets, fstab surface, wchar unlocked I/O, _IO_* stdio
 * aliases, freezero, morecore/tls/ctype init stubs.
 * Integer/pointer only (no SSE doubles).
 *
 * greppable: CGJ_GRAPH_BATCH17_SOFT_NULL
 * greppable: CGJ_GRAPH_BATCH17_SOFT_ARGS
 * greppable: CGJ_GRAPH_BATCH17_SOFT_EDGE
 *
 * Soft deepen: null/arg guards on user-facing graph nodes; edge
 * hardening only. No multi-def; no API break. Pure C integer/pointer.
 */
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <grp.h>
#include <locale.h>
#include <malloc.h>
#include <mntent.h>
#include <pthread.h>
#include <pwd.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

void __chk_fail(void);
void __fortify_fail(const char *szMsg);

/* Not always declared in freestanding headers */
int strcoll_l(const char *szA, const char *szB, locale_t loc);
size_t strxfrm_l(char *szDst, const char *szSrc, size_t cb, locale_t loc);
int strcasecmp_l(const char *szA, const char *szB, locale_t loc);
int strncasecmp_l(const char *szA, const char *szB, size_t n, locale_t loc);
int wcscoll_l(const wchar_t *a, const wchar_t *b, locale_t loc);
size_t wcsxfrm_l(wchar_t *pDst, const wchar_t *szSrc, size_t c, locale_t loc);
char *strcasestr(const char *szHay, const char *szNeedle);
char *gets(char *sz);
int fseeko64(FILE *pF, off_t off, int nWhence);
off_t ftello64(FILE *pF);
struct mallinfo mallinfo(void);
int mallopt(int nParam, int nVal);
void *memalign(size_t uAlign, size_t cb);
void *valloc(size_t cb);
void *pvalloc(size_t cb);
char *secure_getenv(const char *szName);

/* fstab-shaped bring-up (no fstab.h required for dyn export) */
struct fstab {
    char *fs_spec;
    char *fs_file;
    char *fs_vfstype;
    char *fs_mntops;
    char *fs_type;
    int   fs_freq;
    int   fs_passno;
};

/* ---- LFS / underscored stdio seek / stat -------------------------------- */

int
__fseeko64(FILE *pF, off_t off, int nWhence)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fseeko64(pF, off, nWhence);
}

off_t
__ftello64(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return ftello64(pF);
}

int
__fstat64(int nFd, struct stat *pSt)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pSt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fstat(nFd, pSt);
}

int
statvfs64(const char *szPath, struct statvfs *pBuf)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return statvfs(szPath, pBuf);
}

int
fstatvfs64(int nFd, struct statvfs *pBuf)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fstatvfs(nFd, pBuf);
}

/* ---- fts64 aliases (LP64: same as fts_*) -------------------------------- */

FTS *
fts64_open(char *const *ppPathv, int nOptions,
           int (*pfnCompar)(const FTSENT **, const FTSENT **))
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (ppPathv == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fts_open(ppPathv, nOptions, pfnCompar);
}

FTSENT *
fts64_read(FTS *pFts)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pFts == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fts_read(pFts);
}

int
fts64_close(FTS *pFts)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pFts == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fts_close(pFts);
}

int
fts64_set(FTS *pFts, FTSENT *pEnt, int nInstr)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pFts == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pEnt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fts_set(pFts, pEnt, nInstr);
}

FTSENT *
fts64_children(FTS *pFts, int nOptions)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pFts == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fts_children(pFts, nOptions);
}

/* ---- passwd / group stream reentrant + getpw / gets --------------------- */

int
fgetpwent_r(FILE *pF, struct passwd *pPwd, char *szBuf, size_t cb,
            struct passwd **ppResult)
{
    struct passwd *p;
    size_t nNeed;
    size_t nName;
    size_t nPass;
    size_t nGecos;
    size_t nDir;
    size_t nShell;
    char *pCur;

    if (pPwd == NULL || ppResult == NULL) {
        return EINVAL;
    }
    p = fgetpwent(pF);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    if (szBuf == NULL) {
        return ERANGE;
    }
    nName = strlen(p->pw_name) + 1;
    nPass = strlen(p->pw_passwd) + 1;
    nGecos = strlen(p->pw_gecos) + 1;
    nDir = strlen(p->pw_dir) + 1;
    nShell = strlen(p->pw_shell) + 1;
    nNeed = nName + nPass + nGecos + nDir + nShell;
    if (nNeed > cb) {
        *ppResult = NULL;
        return ERANGE;
    }
    pCur = szBuf;
    memcpy(pCur, p->pw_name, nName);
    pPwd->pw_name = pCur;
    pCur += nName;
    memcpy(pCur, p->pw_passwd, nPass);
    pPwd->pw_passwd = pCur;
    pCur += nPass;
    memcpy(pCur, p->pw_gecos, nGecos);
    pPwd->pw_gecos = pCur;
    pCur += nGecos;
    memcpy(pCur, p->pw_dir, nDir);
    pPwd->pw_dir = pCur;
    pCur += nDir;
    memcpy(pCur, p->pw_shell, nShell);
    pPwd->pw_shell = pCur;
    pPwd->pw_uid = p->pw_uid;
    pPwd->pw_gid = p->pw_gid;
    *ppResult = pPwd;
    return 0;
}

int
fgetgrent_r(FILE *pF, struct group *pGrp, char *szBuf, size_t cb,
            struct group **ppResult)
{
    struct group *p;
    size_t nName;
    size_t nPass;
    size_t nNeed;
    char *pCur;
    char **ppMem;

    if (pGrp == NULL || ppResult == NULL) {
        return EINVAL;
    }
    p = fgetgrent(pF);
    if (p == NULL) {
        *ppResult = NULL;
        return 0;
    }
    if (szBuf == NULL) {
        return ERANGE;
    }
    nName = strlen(p->gr_name) + 1;
    nPass = strlen(p->gr_passwd) + 1;
    /* one NULL member pointer + strings */
    nNeed = nName + nPass + sizeof(char *);
    if (nNeed > cb) {
        *ppResult = NULL;
        return ERANGE;
    }
    pCur = szBuf;
    memcpy(pCur, p->gr_name, nName);
    pGrp->gr_name = pCur;
    pCur += nName;
    memcpy(pCur, p->gr_passwd, nPass);
    pGrp->gr_passwd = pCur;
    pCur += nPass;
    /* align pointer storage roughly */
    while (((uintptr_t)pCur & (sizeof(void *) - 1u)) != 0) {
        if ((size_t)(pCur - szBuf) >= cb) {
            *ppResult = NULL;
            return ERANGE;
        }
        *pCur++ = '\0';
    }
    if ((size_t)(pCur - szBuf) + sizeof(char *) > cb) {
        *ppResult = NULL;
        return ERANGE;
    }
    ppMem = (char **)(void *)pCur;
    ppMem[0] = NULL;
    pGrp->gr_mem = ppMem;
    pGrp->gr_gid = p->gr_gid;
    *ppResult = pGrp;
    return 0;
}

int
getpw(uid_t uid, char *szBuf)
{
    struct passwd *p;
    int n;

    if (szBuf == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = getpwuid(uid);
    if (p == NULL) {
        return -1;
    }
    n = snprintf(szBuf, 512,
                 "%s:%s:%u:%u:%s:%s:%s",
                 p->pw_name != NULL ? p->pw_name : "",
                 p->pw_passwd != NULL ? p->pw_passwd : "",
                 (unsigned)p->pw_uid, (unsigned)p->pw_gid,
                 p->pw_gecos != NULL ? p->pw_gecos : "",
                 p->pw_dir != NULL ? p->pw_dir : "",
                 p->pw_shell != NULL ? p->pw_shell : "");
    if (n < 0) {
        return -1;
    }
    return 0;
}

/* Deprecated gets(3) — unbounded stdin line (graph export only). */
char *
gets(char *sz)
{
    size_t i = 0;
    int ch;

    if (sz == NULL) {
        return NULL;
    }
    for (;;) {
        ch = getchar();
        if (ch == EOF) {
            if (i == 0) {
                return NULL;
            }
            break;
        }
        if (ch == '\n') {
            break;
        }
        sz[i++] = (char)ch;
    }
    sz[i] = '\0';
    return sz;
}

char *
__gets_chk(char *sz, size_t cbDst)
{
    size_t i = 0;
    int ch;

    if (sz == NULL) {
        return NULL;
    }
    if (cbDst == 0) {
        __chk_fail();
    }
    for (;;) {
        ch = getchar();
        if (ch == EOF) {
            if (i == 0) {
                return NULL;
            }
            break;
        }
        if (ch == '\n') {
            break;
        }
        if (i + 1 >= cbDst) {
            __chk_fail();
        }
        sz[i++] = (char)ch;
    }
    sz[i] = '\0';
    return sz;
}

/* ---- fstab bring-up table ----------------------------------------------- */

static struct fstab s_aFstab[3];
static char s_aSpec0[] = "/dev/root";
static char s_aFile0[] = "/";
static char s_aType0[] = "ramfs";
static char s_aOpts0[] = "rw";
static char s_aFsType0[] = "rw";
static char s_aSpec1[] = "proc";
static char s_aFile1[] = "/proc";
static char s_aType1[] = "proc";
static char s_aOpts1[] = "rw";
static char s_aFsType1[] = "rw";
static int s_nFstabIdx = -1;
static int s_fFstabInit;

static void
fstab_init(void)
{
    if (s_fFstabInit) {
        return;
    }
    s_aFstab[0].fs_spec = s_aSpec0;
    s_aFstab[0].fs_file = s_aFile0;
    s_aFstab[0].fs_vfstype = s_aType0;
    s_aFstab[0].fs_mntops = s_aOpts0;
    s_aFstab[0].fs_type = s_aFsType0;
    s_aFstab[0].fs_freq = 0;
    s_aFstab[0].fs_passno = 1;
    s_aFstab[1].fs_spec = s_aSpec1;
    s_aFstab[1].fs_file = s_aFile1;
    s_aFstab[1].fs_vfstype = s_aType1;
    s_aFstab[1].fs_mntops = s_aOpts1;
    s_aFstab[1].fs_type = s_aFsType1;
    s_aFstab[1].fs_freq = 0;
    s_aFstab[1].fs_passno = 0;
    s_aFstab[2].fs_spec = NULL;
    s_fFstabInit = 1;
}

int
setfsent(void)
{
    fstab_init();
    s_nFstabIdx = 0;
    return 1;
}

void
endfsent(void)
{
    s_nFstabIdx = -1;
}

struct fstab *
getfsent(void)
{
    fstab_init();
    if (s_nFstabIdx < 0) {
        s_nFstabIdx = 0;
    }
    if (s_nFstabIdx >= 2) {
        return NULL;
    }
    return &s_aFstab[s_nFstabIdx++];
}

struct fstab *
getfsspec(const char *szSpec)
{
    int i;

    fstab_init();
    if (szSpec == NULL) {
        return NULL;
    }
    for (i = 0; i < 2; i++) {
        if (strcmp(s_aFstab[i].fs_spec, szSpec) == 0) {
            return &s_aFstab[i];
        }
    }
    return NULL;
}

struct fstab *
getfsfile(const char *szFile)
{
    int i;

    fstab_init();
    if (szFile == NULL) {
        return NULL;
    }
    for (i = 0; i < 2; i++) {
        if (strcmp(s_aFstab[i].fs_file, szFile) == 0) {
            return &s_aFstab[i];
        }
    }
    return NULL;
}

/* ---- stdio helpers / _IO_* aliases -------------------------------------- */

void
_flushlbf(void)
{
    (void)fflush(stdout);
    (void)fflush(stderr);
}

int
_IO_fclose(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fclose(pF);
}

FILE *
_IO_fopen(const char *szPath, const char *szMode)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szPath == NULL) {
        return NULL;
    }
    if (szMode == NULL) {
        return NULL;
    }

    return fopen(szPath, szMode);
}

int
_IO_fflush(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fflush(pF);
}

size_t
_IO_fread(void *p, size_t cb, size_t n, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fread(p, cb, n, pF);
}

size_t
_IO_fwrite(const void *p, size_t cb, size_t n, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fwrite(p, cb, n, pF);
}

int
_IO_fputs(const char *sz, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (sz == NULL) {
        return EOF;
    }
    if (pF == NULL) {
        return EOF;
    }

    return fputs(sz, pF);
}

char *
_IO_fgets(char *sz, int n, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (sz == NULL) {
        return EOF;
    }
    if (pF == NULL) {
        return EOF;
    }

    return fgets(sz, n, pF);
}

void
_IO_flockfile(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    flockfile(pF);
}

void
_IO_funlockfile(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    funlockfile(pF);
}

int
_IO_ftrylockfile(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return ftrylockfile(pF);
}

long
_IO_ftell(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return ftell(pF);
}

/* ---- wchar unlocked I/O (lockless bring-up) ----------------------------- */

wint_t
fgetwc_unlocked(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return fgetwc(pF);
}

wint_t
fputwc_unlocked(wchar_t wc, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return fputwc(wc, pF);
}

wchar_t *
fgetws_unlocked(wchar_t *sz, int n, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return fgetws(sz, n, pF);
}

int
fputws_unlocked(const wchar_t *sz, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return fputws(sz, pF);
}

wint_t
getwc_unlocked(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return fgetwc(pF);
}

wint_t
getwchar_unlocked(void)
{
    return fgetwc(stdin);
}

wint_t
putwc_unlocked(wchar_t wc, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return fputwc(wc, pF);
}

wint_t
putwchar_unlocked(wchar_t wc)
{
    return fputwc(wc, stdout);
}

/* ---- fortify / path helpers --------------------------------------------- */

ssize_t
__readlink_chk(const char *szPath, char *szBuf, size_t cb, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return readlink(szPath, szBuf, cb);
}

ssize_t
__readlinkat_chk(int nDfd, const char *szPath, char *szBuf, size_t cb,
                 size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nDfd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return readlinkat(nDfd, szPath, szBuf, cb);
}

size_t
__confstr_chk(int nName, char *szBuf, size_t cb, size_t cbDst)
{
    if (szBuf != NULL && cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return confstr(nName, szBuf, cb);
}

char *
__getwd_chk(char *szBuf, size_t cbDst)
{
    if (szBuf == NULL || cbDst == 0) {
        __chk_fail();
    }
    if (getcwd(szBuf, cbDst) == NULL) {
        return NULL;
    }
    return szBuf;
}

int
__getdomainname_chk(char *szName, size_t cb, size_t cbDst)
{
    if (szName != NULL && cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return getdomainname(szName, cb);
}

size_t
__fread_unlocked_chk(void *p, size_t cbSize, size_t n, size_t cbDst, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    size_t cb;

    if (cbSize != 0 && n > cbDst / cbSize) {
        __chk_fail();
    }
    cb = cbSize * n;
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return fread_unlocked(p, cbSize, n, pF);
}

char *
__fgets_unlocked_chk(char *sz, size_t cbDst, int n, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (sz == NULL) {
        return EOF;
    }
    if (pF == NULL) {
        return EOF;
    }

    if (n > 0 && cbDst != (size_t)-1 && (size_t)n > cbDst) {
        __chk_fail();
    }
    return fgets_unlocked(sz, n, pF);
}

int
__fwprintf_chk(FILE *pF, int nFlag, const wchar_t *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int r;

    (void)nFlag;
    va_start(ap, szFmt);
    r = vfwprintf(pF, szFmt, ap);
    va_end(ap);
    return r;
}

int
__openat_2(int nDfd, const char *szPath, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nDfd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    if ((nFlags & O_CREAT) != 0) {
        __fortify_fail("invalid openat call: O_CREAT without mode");
    }
    return openat(nDfd, szPath, nFlags);
}

int
__open_nocancel(const char *szPath, int nFlags, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

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
__open64_nocancel(const char *szPath, int nFlags, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    mode_t mode = 0;
    va_list ap;

    if ((nFlags & O_CREAT) != 0) {
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return open(szPath, nFlags, mode);
}

ssize_t
__read_nocancel(int nFd, void *pBuf, size_t cb)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return read(nFd, pBuf, cb);
}

/* ---- libc private-ish aliases ------------------------------------------- */

void *
__libc_memalign(size_t uAlign, size_t cb)
{
    return memalign(uAlign, cb);
}

void *
__libc_valloc(size_t cb)
{
    return valloc(cb);
}

void *
__libc_pvalloc(size_t cb)
{
    return pvalloc(cb);
}

struct mallinfo
__libc_mallinfo(void)
{
    return mallinfo();
}

int
__libc_mallopt(int nParam, int nVal)
{
    return mallopt(nParam, nVal);
}

char *
__libc_secure_getenv(const char *szName)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szName == NULL) {
        return NULL;
    }

    return secure_getenv(szName);
}

int
__libc_sigaction(int nSig, const struct sigaction *pAct,
                 struct sigaction *pOld)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pAct == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sigaction(nSig, pAct, pOld);
}

void
__libc_fatal(const char *szMsg)
{
    const char *p = (szMsg != NULL) ? szMsg : "fatal\n";

    while (*p) {
        (void)write(2, p, 1);
        p++;
    }
    abort();
}

void
__libc_early_init(int nInitial)
{
    (void)nInitial;
}

void
__ctype_init(void)
{
}

void
__call_tls_dtors(void)
{
}

void *
__default_morecore(ptrdiff_t nInc)
{
    void *p;

    p = sbrk(nInc);
    if (p == (void *)(intptr_t)-1) {
        return NULL;
    }
    return p;
}

/* ---- underscored string / setmnt / setpgid / sig / sched ---------------- */

char *
__strcasestr(const char *szHay, const char *szNeedle)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szHay == NULL) {
        return NULL;
    }
    if (szNeedle == NULL) {
        return NULL;
    }

    return strcasestr(szHay, szNeedle);
}

int
__strcoll_l(const char *szA, const char *szB, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szA == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szB == NULL) {
        errno = EFAULT;
        return -1;
    }

    return strcoll_l(szA, szB, loc);
}

int
__setpgid(pid_t pid, pid_t pgid)
{
    return setpgid(pid, pgid);
}

FILE *
__setmntent(const char *szFile, const char *szType)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szFile == NULL) {
        return NULL;
    }
    if (szType == NULL) {
        return NULL;
    }

    return setmntent(szFile, szType);
}

int
__sigaddset(sigset_t *pSet, int nSig)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pSet == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sigaddset(pSet, nSig);
}

int
__sigdelset(sigset_t *pSet, int nSig)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pSet == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sigdelset(pSet, nSig);
}

int
__sigismember(const sigset_t *pSet, int nSig)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pSet == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sigismember(pSet, nSig);
}

int
__sigsuspend(const sigset_t *pSet)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pSet == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sigsuspend(pSet);
}

int
__sigtimedwait(const sigset_t *pSet, siginfo_t *pInfo,
               const struct timespec *pTs)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pSet == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sigtimedwait(pSet, pInfo, pTs);
}

int
__sched_getparam(pid_t pid, struct sched_param *pParam)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pParam == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sched_getparam(pid, pParam);
}

int
__sched_setscheduler(pid_t pid, int nPolicy, const struct sched_param *pParam)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pParam == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sched_setscheduler(pid, nPolicy, pParam);
}

int
__sched_getscheduler(pid_t pid)
{
    return sched_getscheduler(pid);
}

int
__sched_get_priority_max(int nPolicy)
{
    return sched_get_priority_max(nPolicy);
}

int
__sched_get_priority_min(int nPolicy)
{
    return sched_get_priority_min(nPolicy);
}

/* ---- pthread underscored more ------------------------------------------- */

int
__pthread_mutex_init(pthread_mutex_t *pM, const pthread_mutexattr_t *pA)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pM == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_mutex_init(pM, pA);
}

int
__pthread_mutex_destroy(pthread_mutex_t *pM)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pM == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_mutex_destroy(pM);
}

int
__pthread_mutexattr_init(pthread_mutexattr_t *pA)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_mutexattr_init(pA);
}

int
__pthread_mutexattr_destroy(pthread_mutexattr_t *pA)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_mutexattr_destroy(pA);
}

int
__pthread_mutexattr_settype(pthread_mutexattr_t *pA, int nType)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_mutexattr_settype(pA, nType);
}

int
__pthread_rwlock_init(pthread_rwlock_t *pRw, const pthread_rwlockattr_t *pA)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pRw == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pA == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_rwlock_init(pRw, pA);
}

int
__pthread_rwlock_destroy(pthread_rwlock_t *pRw)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pRw == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_rwlock_destroy(pRw);
}

int
__pthread_rwlock_tryrdlock(pthread_rwlock_t *pRw)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pRw == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_rwlock_tryrdlock(pRw);
}

int
__pthread_rwlock_trywrlock(pthread_rwlock_t *pRw)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pRw == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_rwlock_trywrlock(pRw);
}

void
__pthread_cleanup_routine(void *pArg)
{
    /* cleanup buffer layout is implementation-defined; no-op bring-up */
    (void)pArg;
}

size_t
__pthread_get_minstack(const pthread_attr_t *pAttr)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pAttr == NULL) {
        return 0;
    }

    (void)pAttr;
    return (size_t)16384;
}

void
__pthread_register_cancel_defer(void *pBuf)
{
    (void)pBuf;
}

void
__pthread_unregister_cancel_restore(void *pBuf)
{
    (void)pBuf;
}

void
__pthread_unwind_next(void *pBuf)
{
    (void)pBuf;
    /* cannot unwind without full cancellation machinery */
}

/* ---- misc graph nodes --------------------------------------------------- */

void
freezero(void *p, size_t cb)
{
    if (p == NULL) {
        return;
    }
    if (cb > 0) {
        explicit_bzero(p, cb);
    }
    free(p);
}

time_t
timegm_r(struct tm *pTm, time_t *pOut)
{
    time_t t;

    if (pTm == NULL) {
        errno = EINVAL;
        return (time_t)-1;
    }
    t = timegm(pTm);
    if (pOut != NULL) {
        *pOut = t;
    }
    return t;
}

wchar_t *
__wmemcpy_chk(wchar_t *pDst, const wchar_t *pSrc, size_t n, size_t nDst)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pDst == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pSrc == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (nDst != (size_t)-1 && n > nDst) {
        __chk_fail();
    }
    return wmemcpy(pDst, pSrc, n);
}

wchar_t *
__wmemmove_chk(wchar_t *pDst, const wchar_t *pSrc, size_t n, size_t nDst)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pDst == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pSrc == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (nDst != (size_t)-1 && n > nDst) {
        __chk_fail();
    }
    return wmemmove(pDst, pSrc, n);
}

wchar_t *
__wmemset_chk(wchar_t *pDst, wchar_t wc, size_t n, size_t nDst)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pDst == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (nDst != (size_t)-1 && n > nDst) {
        __chk_fail();
    }
    return wmemset(pDst, wc, n);
}

int
__isoc99_fwprintf(FILE *pF, const wchar_t *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int r;

    va_start(ap, szFmt);
    r = vfwprintf(pF, szFmt, ap);
    va_end(ap);
    return r;
}

int
__isoc99_vfwprintf(FILE *pF, const wchar_t *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vfwprintf(pF, szFmt, ap);
}

/* ---- batch17b: memccpy, _IO printf/padn, locale underscores, isw*_l ---- */

void *
memccpy(void *pDst, const void *pSrc, int c, size_t n)
{
    unsigned char *d = (unsigned char *)pDst;
    const unsigned char *s = (const unsigned char *)pSrc;
    unsigned char ch = (unsigned char)c;
    size_t i;

    if (pDst == NULL || pSrc == NULL) {
        return NULL;
    }
    for (i = 0; i < n; i++) {
        d[i] = s[i];
        if (s[i] == ch) {
            return d + i + 1;
        }
    }
    return NULL;
}

int
_IO_printf(const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vprintf(szFmt, ap);
    va_end(ap);
    return n;
}

int
_IO_fprintf(FILE *pF, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfprintf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
_IO_vfprintf(FILE *pF, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vfprintf(pF, szFmt, ap);
}

int
_IO_sprintf(char *szBuf, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsprintf(szBuf, szFmt, ap);
    va_end(ap);
    return n;
}

int
_IO_vsprintf(char *szBuf, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vsprintf(szBuf, szFmt, ap);
}

int
_IO_puts(const char *sz)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    return puts(sz);
}

char *
_IO_gets(char *sz)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (sz == NULL) {
        return NULL;
    }

    return gets(sz);
}

long
_IO_padn(FILE *pF, int ch, long n)
{
    long i;

    if (pF == NULL || n <= 0) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        if (fputc(ch, pF) == EOF) {
            return i;
        }
    }
    return n;
}

size_t
_IO_sgetn(FILE *pF, void *pBuf, size_t cb)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return fread(pBuf, 1, cb, pF);
}

long
_IO_seekoff(FILE *pF, long off, int nDir, int nMode)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    (void)nMode;
    if (fseek(pF, off, nDir) != 0) {
        return -1;
    }
    return ftell(pF);
}

long
_IO_seekpos(FILE *pF, long pos, int nMode)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    (void)nMode;
    if (fseek(pF, pos, SEEK_SET) != 0) {
        return -1;
    }
    return ftell(pF);
}

void
_IO_free_backup_area(FILE *pF)
{
    (void)pF;
}

void *_IO_list_all = NULL;

locale_t
__newlocale(int nMask, const char *szLocale, locale_t base)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szLocale == NULL) {
        errno = EFAULT;
        return -1;
    }

    return newlocale(nMask, szLocale, base);
}

locale_t
__uselocale(locale_t loc)
{
    return uselocale(loc);
}

int
__strcasecmp_l(const char *szA, const char *szB, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szA == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szB == NULL) {
        errno = EFAULT;
        return -1;
    }

    return strcasecmp_l(szA, szB, loc);
}

int
__strncasecmp_l(const char *szA, const char *szB, size_t n, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szA == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szB == NULL) {
        errno = EFAULT;
        return -1;
    }

    return strncasecmp_l(szA, szB, n, loc);
}

size_t
__strxfrm_l(char *szDst, const char *szSrc, size_t cb, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szDst == NULL) {
        return 0;
    }
    if (szSrc == NULL) {
        return 0;
    }

    return strxfrm_l(szDst, szSrc, cb, loc);
}

int
__wcscoll_l(const wchar_t *a, const wchar_t *b, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (a == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (b == NULL) {
        errno = EFAULT;
        return -1;
    }

    return wcscoll_l(a, b, loc);
}

size_t
__wcsxfrm_l(wchar_t *pDst, const wchar_t *szSrc, size_t c, locale_t loc)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pDst == NULL) {
        return 0;
    }
    if (szSrc == NULL) {
        return 0;
    }

    return wcsxfrm_l(pDst, szSrc, c, loc);
}

int
iswalnum_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswalnum(wc);
}

int
iswalpha_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswalpha(wc);
}

int
iswblank_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswblank(wc);
}

int
iswcntrl_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswcntrl(wc);
}

int
iswdigit_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswdigit(wc);
}

int
iswgraph_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswgraph(wc);
}

int
iswlower_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswlower(wc);
}

int
iswprint_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswprint(wc);
}

int
iswpunct_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswpunct(wc);
}

int
iswspace_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswspace(wc);
}

int
iswupper_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswupper(wc);
}

int
iswxdigit_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return iswxdigit(wc);
}

int
iswctype_l(wint_t wc, wctype_t desc, locale_t loc)
{
    (void)loc;
    return iswctype(wc, desc);
}

wint_t
towlower_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return towlower(wc);
}

wint_t
towupper_l(wint_t wc, locale_t loc)
{
    (void)loc;
    return towupper(wc);
}

int
__iswalnum_l(wint_t wc, locale_t loc)
{
    return iswalnum_l(wc, loc);
}

int
__iswalpha_l(wint_t wc, locale_t loc)
{
    return iswalpha_l(wc, loc);
}

int
__iswblank_l(wint_t wc, locale_t loc)
{
    return iswblank_l(wc, loc);
}

int
__iswcntrl_l(wint_t wc, locale_t loc)
{
    return iswcntrl_l(wc, loc);
}

int
__iswdigit_l(wint_t wc, locale_t loc)
{
    return iswdigit_l(wc, loc);
}

int
__iswgraph_l(wint_t wc, locale_t loc)
{
    return iswgraph_l(wc, loc);
}

int
__iswlower_l(wint_t wc, locale_t loc)
{
    return iswlower_l(wc, loc);
}

int
__iswprint_l(wint_t wc, locale_t loc)
{
    return iswprint_l(wc, loc);
}

int
__iswpunct_l(wint_t wc, locale_t loc)
{
    return iswpunct_l(wc, loc);
}

int
__iswspace_l(wint_t wc, locale_t loc)
{
    return iswspace_l(wc, loc);
}

int
__iswupper_l(wint_t wc, locale_t loc)
{
    return iswupper_l(wc, loc);
}

int
__iswxdigit_l(wint_t wc, locale_t loc)
{
    return iswxdigit_l(wc, loc);
}

int
__iswctype_l(wint_t wc, wctype_t desc, locale_t loc)
{
    return iswctype_l(wc, desc, loc);
}

wint_t
__towlower_l(wint_t wc, locale_t loc)
{
    return towlower_l(wc, loc);
}

wint_t
__towupper_l(wint_t wc, locale_t loc)
{
    return towupper_l(wc, loc);
}

/* ctype _l underscored aliases (wrappers over graph_batch15) */
int isalnum_l(int c, locale_t loc);
int isalpha_l(int c, locale_t loc);
int isblank_l(int c, locale_t loc);
int iscntrl_l(int c, locale_t loc);
int isdigit_l(int c, locale_t loc);
int isgraph_l(int c, locale_t loc);
int islower_l(int c, locale_t loc);
int isprint_l(int c, locale_t loc);
int ispunct_l(int c, locale_t loc);
int isspace_l(int c, locale_t loc);
int isupper_l(int c, locale_t loc);
int isxdigit_l(int c, locale_t loc);
int tolower_l(int c, locale_t loc);
int toupper_l(int c, locale_t loc);

int
__isalnum_l(int c, locale_t loc)
{
    return isalnum_l(c, loc);
}

int
__isalpha_l(int c, locale_t loc)
{
    return isalpha_l(c, loc);
}

int
__isblank_l(int c, locale_t loc)
{
    return isblank_l(c, loc);
}

int
__iscntrl_l(int c, locale_t loc)
{
    return iscntrl_l(c, loc);
}

int
__isdigit_l(int c, locale_t loc)
{
    return isdigit_l(c, loc);
}

int
__isgraph_l(int c, locale_t loc)
{
    return isgraph_l(c, loc);
}

int
__islower_l(int c, locale_t loc)
{
    return islower_l(c, loc);
}

int
__isprint_l(int c, locale_t loc)
{
    return isprint_l(c, loc);
}

int
__ispunct_l(int c, locale_t loc)
{
    return ispunct_l(c, loc);
}

int
__isspace_l(int c, locale_t loc)
{
    return isspace_l(c, loc);
}

int
__isupper_l(int c, locale_t loc)
{
    return isupper_l(c, loc);
}

int
__isxdigit_l(int c, locale_t loc)
{
    return isxdigit_l(c, loc);
}

int
__tolower_l(int c, locale_t loc)
{
    return tolower_l(c, loc);
}

int
__toupper_l(int c, locale_t loc)
{
    return toupper_l(c, loc);
}

/* ---- shadow reentrant + regex + spawn + dynlinker ----------------------- */

#include <regex.h>
#include <shadow.h>
#include <spawn.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int
getspent_r(struct spwd *pSp, char *szBuf, size_t cb, struct spwd **ppResult)
{
    (void)pSp;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOSYS;
}

int
fgetspent_r(FILE *pF, struct spwd *pSp, char *szBuf, size_t cb,
            struct spwd **ppResult)
{
    (void)pF;
    (void)pSp;
    (void)szBuf;
    (void)cb;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return ENOSYS;
}

typedef unsigned long reg_syntax_t;
static reg_syntax_t g_reSyntax = 0;

reg_syntax_t
re_set_syntax(reg_syntax_t nSyntax)
{
    reg_syntax_t old = g_reSyntax;

    g_reSyntax = nSyntax;
    return old;
}

int
re_compile_fastmap(void *pBuf)
{
    (void)pBuf;
    return 0;
}

int
regncomp(regex_t *pPreg, const char *szPattern, size_t nLen, int nCflags)
{
    char aTmp[512];
    size_t n = nLen;

    if (pPreg == NULL || szPattern == NULL) {
        return REG_BADPAT;
    }
    if (n >= sizeof(aTmp)) {
        n = sizeof(aTmp) - 1;
    }
    memcpy(aTmp, szPattern, n);
    aTmp[n] = '\0';
    return regcomp(pPreg, aTmp, nCflags);
}

int
regnexec(const regex_t *pPreg, const char *szString, size_t nLen, size_t nMatch,
         regmatch_t pMatch[], int nEflags)
{
    char aTmp[512];
    size_t n = nLen;

    if (szString == NULL) {
        return REG_NOMATCH;
    }
    if (n >= sizeof(aTmp)) {
        n = sizeof(aTmp) - 1;
    }
    memcpy(aTmp, szString, n);
    aTmp[n] = '\0';
    return regexec(pPreg, aTmp, nMatch, pMatch, nEflags);
}

int
posix_spawn_file_actions_addtcsetpgrp_np(posix_spawn_file_actions_t *pFa,
                                         int nFd)
{
    if (pFa == NULL || pFa->__actions == NULL) {
        return EINVAL;
    }
    (void)nFd;
    return 0;
}

int __libc_enable_secure = 0;

int
__libc_allocate_rtsig(int nHigh)
{
    static int s_nNext = 34;

    (void)nHigh;
    if (s_nNext > 64) {
        return -1;
    }
    return s_nNext++;
}

/* _dl_find_object lives in graph_batch18 */

/* ---- inet_net_* + netgroup + rcmd + STREAMS ----------------------------- */

char *
inet_net_ntop(int nAf, const void *pSrc, int nBits, char *szDst, size_t cbDst)
{
    const unsigned char *p;
    char aTmp[64];
    int n;

    if (nAf != AF_INET || pSrc == NULL || szDst == NULL || cbDst == 0) {
        errno = EAFNOSUPPORT;
        return NULL;
    }
    if (nBits < 0 || nBits > 32) {
        errno = EINVAL;
        return NULL;
    }
    p = (const unsigned char *)pSrc;
    n = snprintf(aTmp, sizeof(aTmp), "%u.%u.%u.%u/%d", (unsigned)p[0],
                 (unsigned)p[1], (unsigned)p[2], (unsigned)p[3], nBits);
    if (n < 0 || (size_t)n >= cbDst) {
        errno = ENOSPC;
        return NULL;
    }
    memcpy(szDst, aTmp, (size_t)n + 1);
    return szDst;
}

int
inet_net_pton(int nAf, const char *szSrc, void *pDst, size_t cbDst)
{
    unsigned b0, b1, b2, b3;
    int nBits = 32;
    unsigned char *p;
    int nScan;

    if (nAf != AF_INET || szSrc == NULL || pDst == NULL || cbDst < 4) {
        errno = EAFNOSUPPORT;
        return -1;
    }
    nScan = sscanf(szSrc, "%u.%u.%u.%u/%d", &b0, &b1, &b2, &b3, &nBits);
    if (nScan < 4) {
        nScan = sscanf(szSrc, "%u.%u.%u.%u", &b0, &b1, &b2, &b3);
        if (nScan != 4) {
            errno = EINVAL;
            return -1;
        }
        nBits = 32;
    }
    if (b0 > 255 || b1 > 255 || b2 > 255 || b3 > 255 || nBits < 0 ||
        nBits > 32) {
        errno = EINVAL;
        return -1;
    }
    p = (unsigned char *)pDst;
    p[0] = (unsigned char)b0;
    p[1] = (unsigned char)b1;
    p[2] = (unsigned char)b2;
    p[3] = (unsigned char)b3;
    return nBits;
}

void
setnetgrent(const char *szNetgroup)
{
    (void)szNetgroup;
}

void
endnetgrent(void)
{
}

int
getnetgrent(char **ppHost, char **ppUser, char **ppDomain)
{
    if (ppHost != NULL) {
        *ppHost = NULL;
    }
    if (ppUser != NULL) {
        *ppUser = NULL;
    }
    if (ppDomain != NULL) {
        *ppDomain = NULL;
    }
    return 0;
}

int
getnetgrent_r(char **ppHost, char **ppUser, char **ppDomain, char *szBuf,
              size_t cb)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (ppHost == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppUser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppDomain == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szBuf;
    (void)cb;
    return getnetgrent(ppHost, ppUser, ppDomain);
}

int
innetgr(const char *szNetgroup, const char *szHost, const char *szUser,
        const char *szDomain)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szNetgroup == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szHost == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szUser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szDomain == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szNetgroup;
    (void)szHost;
    (void)szUser;
    (void)szDomain;
    return 0;
}

int
rcmd(char **pAhost, unsigned short nPort, const char *szLocuser,
     const char *szRemuser, const char *szCmd, int *pFd2p)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pAhost == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szLocuser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szRemuser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szCmd == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pFd2p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pAhost;
    (void)nPort;
    (void)szLocuser;
    (void)szRemuser;
    (void)szCmd;
    (void)pFd2p;
    errno = ENOSYS;
    return -1;
}

int
rcmd_af(char **pAhost, unsigned short nPort, const char *szLocuser,
        const char *szRemuser, const char *szCmd, int *pFd2p, int nAf)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pAhost == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szLocuser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szRemuser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szCmd == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pFd2p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nAf;
    return rcmd(pAhost, nPort, szLocuser, szRemuser, szCmd, pFd2p);
}

int
rexec(char **pAhost, int nPort, const char *szUser, const char *szPass,
      const char *szCmd, int *pFd2p)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pAhost == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szUser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szPass == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szCmd == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pFd2p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pAhost;
    (void)nPort;
    (void)szUser;
    (void)szPass;
    (void)szCmd;
    (void)pFd2p;
    errno = ENOSYS;
    return -1;
}

int
rexec_af(char **pAhost, int nPort, const char *szUser, const char *szPass,
         const char *szCmd, int *pFd2p, int nAf)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pAhost == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szUser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szPass == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szCmd == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pFd2p == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nAf;
    return rexec(pAhost, nPort, szUser, szPass, szCmd, pFd2p);
}

int
rresvport(int *pPort)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pPort == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pPort;
    errno = ENOSYS;
    return -1;
}

int
rresvport_af(int *pPort, int nAf)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pPort == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pPort;
    (void)nAf;
    errno = ENOSYS;
    return -1;
}

int
ruserok(const char *szRhost, int nSuperuser, const char *szRuser,
        const char *szLuser)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szRhost == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szRuser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szLuser == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szRhost;
    (void)nSuperuser;
    (void)szRuser;
    (void)szLuser;
    return -1;
}

int
ruserok_af(const char *szRhost, int nSuperuser, const char *szRuser,
           const char *szLuser, int nAf)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szRhost == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szRuser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szLuser == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nAf;
    return ruserok(szRhost, nSuperuser, szRuser, szLuser);
}

int
iruserok(uint32_t nAddr, int nSuperuser, const char *szRuser,
         const char *szLuser)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szRuser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szLuser == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nAddr;
    (void)nSuperuser;
    (void)szRuser;
    (void)szLuser;
    return -1;
}

int
iruserok_af(const void *pAddr, int nSuperuser, const char *szRuser,
            const char *szLuser, int nAf)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szRuser == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szLuser == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pAddr;
    (void)nSuperuser;
    (void)szRuser;
    (void)szLuser;
    (void)nAf;
    return -1;
}

int
isastream(int nFd)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    (void)nFd;
    return 0;
}

int
getmsg(int nFd, void *pCtl, void *pDat, int *pFlags)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pFlags == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFd;
    (void)pCtl;
    (void)pDat;
    (void)pFlags;
    errno = ENOSYS;
    return -1;
}

int
putmsg(int nFd, const void *pCtl, const void *pDat, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    (void)nFd;
    (void)pCtl;
    (void)pDat;
    (void)nFlags;
    errno = ENOSYS;
    return -1;
}

int
getpmsg(int nFd, void *pCtl, void *pDat, int *pBand, int *pFlags)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pBand == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pFlags == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFd;
    (void)pCtl;
    (void)pDat;
    (void)pBand;
    (void)pFlags;
    errno = ENOSYS;
    return -1;
}

int
putpmsg(int nFd, const void *pCtl, const void *pDat, int nBand, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    (void)nFd;
    (void)pCtl;
    (void)pDat;
    (void)nBand;
    (void)nFlags;
    errno = ENOSYS;
    return -1;
}

int
fattach(int nFd, const char *szPath)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFd;
    (void)szPath;
    errno = ENOSYS;
    return -1;
}

int
fdetach(const char *szPath)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szPath;
    errno = ENOSYS;
    return -1;
}

int
getsourcefilter(int nFd, uint32_t nIface, const struct sockaddr *pGroup,
                socklen_t nGroupLen, uint32_t *pFmode, uint32_t *pNumsrc,
                void *pSlist)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pGroup == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pFmode == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pNumsrc == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFd;
    (void)nIface;
    (void)pGroup;
    (void)nGroupLen;
    (void)pFmode;
    (void)pNumsrc;
    (void)pSlist;
    errno = ENOSYS;
    return -1;
}

int
setsourcefilter(int nFd, uint32_t nIface, const struct sockaddr *pGroup,
                socklen_t nGroupLen, uint32_t nFmode, uint32_t nNumsrc,
                const void *pSlist)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pGroup == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFd;
    (void)nIface;
    (void)pGroup;
    (void)nGroupLen;
    (void)nFmode;
    (void)nNumsrc;
    (void)pSlist;
    errno = ENOSYS;
    return -1;
}

int
inet6_opt_init(void *pExtbuf, socklen_t nExtlen)
{
    (void)pExtbuf;
    (void)nExtlen;
    errno = ENOSYS;
    return -1;
}

void *
inet6_rth_init(void *pBp, socklen_t nBpLen, int nType, int nSegments)
{
    (void)pBp;
    (void)nBpLen;
    (void)nType;
    (void)nSegments;
    return NULL;
}

int
__isoc99_fwscanf(FILE *pF, const wchar_t *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfwscanf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_wscanf(const wchar_t *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vwscanf(szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_swscanf(const wchar_t *szBuf, const wchar_t *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vswscanf(szBuf, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_vfwscanf(FILE *pF, const wchar_t *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vfwscanf(pF, szFmt, ap);
}

int
__isoc99_vwscanf(const wchar_t *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vwscanf(szFmt, ap);
}

int
__isoc99_vswscanf(const wchar_t *szBuf, const wchar_t *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vswscanf(szBuf, szFmt, ap);
}

/* gshadow / aliases empty streams */
struct sgrp {
    char *sg_namp;
    char *sg_passwd;
    char **sg_adm;
    char **sg_mem;
};

void
setsgent(void)
{
}

void
endsgent(void)
{
}

struct sgrp *
getsgent(void)
{
    return NULL;
}

struct sgrp *
getsgnam(const char *szName)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szName;
    return NULL;
}

struct sgrp *
fgetsgent(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    (void)pF;
    return NULL;
}

int
putsgent(const struct sgrp *pSg, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (pSg == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    (void)pSg;
    (void)pF;
    errno = ENOSYS;
    return -1;
}

void
setaliasent(void)
{
}

void
endaliasent(void)
{
}

struct aliasent {
    char *alias_name;
    size_t alias_members_len;
    char **alias_members;
    int alias_local;
};

struct aliasent *
getaliasent(void)
{
    return NULL;
}

struct aliasent *
getaliasbyname(const char *szName)
{
    /* greppable: CGJ_GRAPH_BATCH17_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szName;
    return NULL;
}
