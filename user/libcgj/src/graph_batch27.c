/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27: _IO_* FILE vtable surface, gconv stubs,
 * morecore hook, memcmpeq, isoc23 locale/wide integer converts, wchar
 * helpers, broader *_nocancel I/O. Integer/pointer only (no SSE doubles).
 */
#include <errno.h>
#include <fcntl.h>
#include <iconv.h>
#include <locale.h>
#include <malloc.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

void __chk_fail(void);
long strtol_l(const char *sz, char **ppEnd, int nBase, locale_t loc);
unsigned long strtoul_l(const char *sz, char **ppEnd, int nBase, locale_t loc);
long long strtoll_l(const char *sz, char **ppEnd, int nBase, locale_t loc);
unsigned long long strtoull_l(const char *sz, char **ppEnd, int nBase,
                              locale_t loc);
long wcstol(const wchar_t *sz, wchar_t **ppEnd, int nBase);
unsigned long wcstoul(const wchar_t *sz, wchar_t **ppEnd, int nBase);
long long wcstoll(const wchar_t *sz, wchar_t **ppEnd, int nBase);
unsigned long long wcstoull(const wchar_t *sz, wchar_t **ppEnd, int nBase);
size_t wcsnlen(const wchar_t *sz, size_t cMax);
char *stpncpy(char *pDst, const char *pSrc, size_t c);
int strcasecmp(const char *szA, const char *szB);
int strncasecmp(const char *szA, const char *szB, size_t n);
int openat(int nDirfd, const char *szPath, int nFlags, ...);
int openat64(int nDirfd, const char *szPath, int nFlags, ...);
int fcntl(int nFd, int nCmd, ...);
int poll(struct pollfd *pFds, nfds_t nFds, int nTimeout);
int ppoll(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTs,
          const void *pMask);
int accept(int nFd, struct sockaddr *pAddr, socklen_t *pcbAddr);
int accept4(int nFd, struct sockaddr *pAddr, socklen_t *pcbAddr, int nFlags);
int connect(int nFd, const struct sockaddr *pAddr, socklen_t cbAddr);
ssize_t send(int nFd, const void *pBuf, size_t cb, int nFlags);
ssize_t recv(int nFd, void *pBuf, size_t cb, int nFlags);
int pause(void);
int nanosleep(const struct timespec *pReq, struct timespec *pRem);
int sigsuspend(const sigset_t *pMask);
pid_t waitpid(pid_t nPid, int *pStatus, int nOptions);
int raise(int nSig);
int *__errno_location(void);
void *__default_morecore(ptrdiff_t nInc);

/* ---- string / mem ------------------------------------------------------- */

int
__memcmpeq(const void *pA, const void *pB, size_t cb)
{
    return memcmp(pA, pB, cb) != 0;
}

char *
__strchr_c(const char *sz, int c)
{
    return strchr(sz, c);
}

char *
__strrchr_c(const char *sz, int c)
{
    return strrchr(sz, c);
}

/* ---- wchar helpers ------------------------------------------------------ */

size_t
__wcsnlen(const wchar_t *sz, size_t cMax)
{
    return wcsnlen(sz, cMax);
}

wchar_t *
__wcschrnul(const wchar_t *sz, wchar_t wc)
{
    const wchar_t *p;

    if (sz == NULL) {
        return NULL;
    }
    p = sz;
    while (*p != L'\0' && *p != wc) {
        p++;
    }
    return (wchar_t *)p;
}

/* ---- isoc23 locale / wide integer converts ------------------------------ */

long
__isoc23_strtol_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    return strtol_l(sz, ppEnd, nBase, loc);
}

unsigned long
__isoc23_strtoul_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    return strtoul_l(sz, ppEnd, nBase, loc);
}

long long
__isoc23_strtoll_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    return strtoll_l(sz, ppEnd, nBase, loc);
}

unsigned long long
__isoc23_strtoull_l(const char *sz, char **ppEnd, int nBase, locale_t loc)
{
    return strtoull_l(sz, ppEnd, nBase, loc);
}

long
__isoc23_wcstol(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    return wcstol(sz, ppEnd, nBase);
}

unsigned long
__isoc23_wcstoul(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    return wcstoul(sz, ppEnd, nBase);
}

long long
__isoc23_wcstoll(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    return wcstoll(sz, ppEnd, nBase);
}

unsigned long long
__isoc23_wcstoull(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    return wcstoull(sz, ppEnd, nBase);
}

/* ---- morecore ----------------------------------------------------------- */

/* glibc malloc hook pointer — default to existing sbrk morecore. */
void *(*__morecore)(ptrdiff_t) = __default_morecore;

/* ---- nocancel I/O ------------------------------------------------------- */

int
__openat64_nocancel(int nDirfd, const char *szPath, int nFlags, ...)
{
    mode_t mode = 0;
    va_list ap;

    if ((nFlags & O_CREAT) != 0) {
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return openat64(nDirfd, szPath, nFlags, mode);
}

int
__fcntl_nocancel(int nFd, int nCmd, ...)
{
    va_list ap;
    void *pArg;
    int n;

    va_start(ap, nCmd);
    pArg = va_arg(ap, void *);
    va_end(ap);
    n = fcntl(nFd, nCmd, pArg);
    return n;
}

int
__libc_fcntl(int nFd, int nCmd, ...)
{
    va_list ap;
    void *pArg;

    va_start(ap, nCmd);
    pArg = va_arg(ap, void *);
    va_end(ap);
    return fcntl(nFd, nCmd, pArg);
}

int
__poll_nocancel(struct pollfd *pFds, nfds_t nFds, int nTimeout)
{
    return poll(pFds, nFds, nTimeout);
}

pid_t
__waitpid_nocancel(pid_t nPid, int *pStatus, int nOptions)
{
    return waitpid(nPid, pStatus, nOptions);
}

int
__pause_nocancel(void)
{
    return pause();
}

int
__nanosleep_nocancel(const struct timespec *pReq, struct timespec *pRem)
{
    return nanosleep(pReq, pRem);
}

int
__sigsuspend_nocancel(const sigset_t *pMask)
{
    return sigsuspend(pMask);
}

ssize_t
__recv_nocancel(int nFd, void *pBuf, size_t cb, int nFlags)
{
    return recv(nFd, pBuf, cb, nFlags);
}

ssize_t
__send_nocancel(int nFd, const void *pBuf, size_t cb, int nFlags)
{
    return send(nFd, pBuf, cb, nFlags);
}

int
__connect_nocancel(int nFd, const struct sockaddr *pAddr, socklen_t cbAddr)
{
    return connect(nFd, pAddr, cbAddr);
}

int
__accept_nocancel(int nFd, struct sockaddr *pAddr, socklen_t *pcbAddr)
{
    return accept(nFd, pAddr, pcbAddr);
}

/* ---- gconv / iconv internal stubs --------------------------------------- */

/* Opaque handle cookie used only for ABI surface. */
struct b27_gconv {
    int fOpen;
};

void *
__gconv_get_modules_db(void)
{
    return NULL;
}

void *
__gconv_get_cache(void)
{
    return NULL;
}

void *
__gconv_get_alias_db(void)
{
    return NULL;
}

int
__gconv_open(const char *szTo, const char *szFrom, void **ppHandle,
             int nFlags)
{
    struct b27_gconv *p;

    (void)nFlags;
    if (ppHandle == NULL || szTo == NULL || szFrom == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = (struct b27_gconv *)calloc(1, sizeof(*p));
    if (p == NULL) {
        return -1;
    }
    p->fOpen = 1;
    *ppHandle = p;
    (void)szTo;
    (void)szFrom;
    return 0;
}

int
__gconv_create_spec(void *pSpec, const char *szTo, const char *szFrom)
{
    (void)pSpec;
    (void)szTo;
    (void)szFrom;
    return 0;
}

void
__gconv_destroy_spec(void *pSpec)
{
    (void)pSpec;
}

int
__gconv_transliterate(void *pStep, void *pData, const unsigned char **ppIn,
                      const unsigned char *pInEnd, unsigned char **ppOut,
                      size_t *pIrrev)
{
    (void)pStep;
    (void)pData;
    (void)ppIn;
    (void)pInEnd;
    (void)ppOut;
    if (pIrrev != NULL) {
        *pIrrev = 0;
    }
    errno = EOPNOTSUPP;
    return -1;
}

iconv_t
__iconv_open(const char *szTo, const char *szFrom)
{
    return iconv_open(szTo, szFrom);
}

size_t
__iconv(iconv_t cd, char **ppIn, size_t *pcbIn, char **ppOut, size_t *pcbOut)
{
    return iconv(cd, ppIn, pcbIn, ppOut, pcbOut);
}

int
__iconv_close(iconv_t cd)
{
    return iconv_close(cd);
}

/* ---- _IO_* public aliases / fail-closed vtable surface ------------------ */

FILE *
_IO_fdopen(int nFd, const char *szMode)
{
    return fdopen(nFd, szMode);
}

FILE *
_IO_popen(const char *szCmd, const char *szMode)
{
    return popen(szCmd, szMode);
}

int
_IO_fgetpos(FILE *pF, fpos_t *pPos)
{
    return fgetpos(pF, pPos);
}

int
_IO_fgetpos64(FILE *pF, fpos_t *pPos)
{
    return fgetpos64(pF, pPos);
}

int
_IO_fsetpos(FILE *pF, const fpos_t *pPos)
{
    return fsetpos(pF, pPos);
}

int
_IO_fsetpos64(FILE *pF, const fpos_t *pPos)
{
    return fsetpos64(pF, pPos);
}

int
_IO_setvbuf(FILE *pF, char *pBuf, int nMode, size_t cb)
{
    return setvbuf(pF, pBuf, nMode, cb);
}

void
_IO_setbuffer(FILE *pF, char *pBuf, size_t cb)
{
    setbuffer(pF, pBuf, cb);
}

int
_IO_ungetc(int nCh, FILE *pF)
{
    return ungetc(nCh, pF);
}

int
_IO_sscanf(const char *sz, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsscanf(sz, szFmt, ap);
    va_end(ap);
    return n;
}

int
_IO_vfscanf(FILE *pF, const char *szFmt, va_list ap)
{
    return vfscanf(pF, szFmt, ap);
}

ssize_t
_IO_getline(char **ppLine, size_t *pcb, FILE *pF)
{
    return getline(ppLine, pcb, pF);
}

ssize_t
_IO_getline_info(FILE *pF, char **ppLine, size_t *pcb, int nDelim,
                 int nExtract, int *pEof)
{
    ssize_t n;

    (void)nExtract;
    if (pEof != NULL) {
        *pEof = 0;
    }
    if (nDelim == '\n') {
        n = getline(ppLine, pcb, pF);
    } else {
        n = getdelim(ppLine, pcb, nDelim, pF);
    }
    if (n < 0 && pEof != NULL && pF != NULL && feof(pF)) {
        *pEof = 1;
    }
    return n;
}

int
_IO_peekc_locked(FILE *pF)
{
    int nCh;

    if (pF == NULL) {
        return EOF;
    }
    nCh = getc(pF);
    if (nCh != EOF) {
        (void)ungetc(nCh, pF);
    }
    return nCh;
}

int
_IO_sputbackc(FILE *pF, int nCh)
{
    return ungetc(nCh, pF);
}

int
_IO_sungetc(FILE *pF)
{
    (void)pF;
    return EOF;
}

wint_t
_IO_sputbackwc(FILE *pF, wint_t wc)
{
    return ungetwc(wc, pF);
}

wint_t
_IO_sungetwc(FILE *pF)
{
    (void)pF;
    return (wint_t)WEOF;
}

/* Fail-closed vtable hooks — keep symbols for graph bind, no full libio. */

void
_IO_init(void *pFp, int nFlags)
{
    (void)pFp;
    (void)nFlags;
}

void
_IO_file_init(void *pFp)
{
    (void)pFp;
}

void *
_IO_file_open(void *pFp, const char *szPath, int nPosixMode, int nProt,
              int nReadWrite, int nIs32Not64)
{
    (void)pFp;
    (void)szPath;
    (void)nPosixMode;
    (void)nProt;
    (void)nReadWrite;
    (void)nIs32Not64;
    return NULL;
}

void *
_IO_file_fopen(void *pFp, const char *szPath, const char *szMode,
               int nIs32Not64)
{
    (void)pFp;
    (void)szPath;
    (void)szMode;
    (void)nIs32Not64;
    return NULL;
}

int
_IO_file_close(void *pFp)
{
    (void)pFp;
    return 0;
}

int
_IO_file_close_it(void *pFp)
{
    (void)pFp;
    return 0;
}

int
_IO_file_finish(void *pFp, int nDummy)
{
    (void)pFp;
    (void)nDummy;
    return 0;
}

int
_IO_file_attach(void *pFp, int nFd)
{
    (void)pFp;
    (void)nFd;
    return -1;
}

int
_IO_file_read(void *pFp, char *pBuf, int nCb)
{
    (void)pFp;
    (void)pBuf;
    (void)nCb;
    return 0;
}

int
_IO_file_write(void *pFp, const char *pBuf, int nCb)
{
    (void)pFp;
    (void)pBuf;
    (void)nCb;
    return 0;
}

long
_IO_file_seek(void *pFp, long nOff, int nWhence)
{
    (void)pFp;
    (void)nOff;
    (void)nWhence;
    return -1;
}

long
_IO_file_seekoff(void *pFp, long nOff, int nDir, int nMode)
{
    (void)pFp;
    (void)nOff;
    (void)nDir;
    (void)nMode;
    return -1;
}

int
_IO_file_stat(void *pFp, void *pSt)
{
    (void)pFp;
    (void)pSt;
    return -1;
}

int
_IO_file_sync(void *pFp)
{
    (void)pFp;
    return 0;
}

int
_IO_file_overflow(void *pFp, int nCh)
{
    (void)pFp;
    (void)nCh;
    return EOF;
}

int
_IO_file_underflow(void *pFp)
{
    (void)pFp;
    return EOF;
}

size_t
_IO_file_xsputn(void *pFp, const void *pData, size_t n)
{
    (void)pFp;
    (void)pData;
    (void)n;
    return 0;
}

void *
_IO_file_setbuf(void *pFp, char *pBuf, size_t cb)
{
    (void)pFp;
    (void)pBuf;
    (void)cb;
    return NULL;
}

int
_IO_file_doallocate(void *pFp)
{
    (void)pFp;
    return 0;
}

int
_IO_doallocbuf(void *pFp)
{
    (void)pFp;
    return 0;
}

int
_IO_do_write(void *pFp, const char *pData, size_t cb)
{
    (void)pFp;
    (void)pData;
    (void)cb;
    return 0;
}

int
_IO_default_uflow(void *pFp)
{
    (void)pFp;
    return EOF;
}

size_t
_IO_default_xsputn(void *pFp, const void *pData, size_t n)
{
    (void)pFp;
    (void)pData;
    (void)n;
    return 0;
}

size_t
_IO_default_xsgetn(void *pFp, void *pData, size_t n)
{
    (void)pFp;
    (void)pData;
    (void)n;
    return 0;
}

int
_IO_default_doallocate(void *pFp)
{
    (void)pFp;
    return 0;
}

void
_IO_default_finish(void *pFp, int nDummy)
{
    (void)pFp;
    (void)nDummy;
}

int
_IO_default_pbackfail(void *pFp, int nCh)
{
    (void)pFp;
    (void)nCh;
    return EOF;
}

void
_IO_switch_to_get_mode(void *pFp)
{
    (void)pFp;
}

void
_IO_setb(void *pFp, char *pBuf, char *pEnd, int nSet)
{
    (void)pFp;
    (void)pBuf;
    (void)pEnd;
    (void)nSet;
}

void
_IO_link_in(void *pFp)
{
    (void)pFp;
}

void
_IO_un_link(void *pFp)
{
    (void)pFp;
}

int
_IO_flush_all(void)
{
    return fflush(NULL);
}

void
_IO_flush_all_linebuffered(void)
{
    (void)fflush(NULL);
}

void
_IO_cleanup(void)
{
    (void)fflush(NULL);
}

void
_IO_enable_locks(void)
{
}

void
_IO_list_lock(void)
{
}

void
_IO_list_unlock(void)
{
}

void
_IO_list_resetlock(void)
{
}

int
_IO_adjust_column(int nStart, const char *szLine, int nCount)
{
    int nCol = nStart;
    int i;

    if (szLine == NULL || nCount <= 0) {
        return nStart;
    }
    for (i = 0; i < nCount; i++) {
        char c = szLine[i];

        if (c == '\n') {
            nCol = 0;
        } else if (c == '\t') {
            nCol = (nCol + 8) & ~7;
        } else if (c == '\b') {
            if (nCol > 0) {
                nCol--;
            }
        } else {
            nCol++;
        }
    }
    return nCol;
}

int
_IO_adjust_wcolumn(int nStart, const wchar_t *szLine, int nCount)
{
    int nCol = nStart;
    int i;

    if (szLine == NULL || nCount <= 0) {
        return nStart;
    }
    for (i = 0; i < nCount; i++) {
        wchar_t wc = szLine[i];

        if (wc == L'\n') {
            nCol = 0;
        } else if (wc == L'\t') {
            nCol = (nCol + 8) & ~7;
        } else if (wc == L'\b') {
            if (nCol > 0) {
                nCol--;
            }
        } else {
            nCol++;
        }
    }
    return nCol;
}

/* marker / iterator stubs */
void
_IO_init_marker(void *pMark, void *pFp)
{
    (void)pMark;
    (void)pFp;
}

void
_IO_init_wmarker(void *pMark, void *pFp)
{
    (void)pMark;
    (void)pFp;
}

void
_IO_remove_marker(void *pMark)
{
    (void)pMark;
}

int
_IO_marker_delta(void *pMark)
{
    (void)pMark;
    return 0;
}

int
_IO_marker_difference(void *pA, void *pB)
{
    (void)pA;
    (void)pB;
    return 0;
}

int
_IO_seekmark(void *pFp, void *pMark, int nDelta)
{
    (void)pFp;
    (void)pMark;
    (void)nDelta;
    return -1;
}

int
_IO_seekwmark(void *pFp, void *pMark, int nDelta)
{
    (void)pFp;
    (void)pMark;
    (void)nDelta;
    return -1;
}

int
_IO_least_wmarker(void *pFp, void *pMark)
{
    (void)pFp;
    (void)pMark;
    return 0;
}

void
_IO_unsave_markers(void *pFp)
{
    (void)pFp;
}

void
_IO_unsave_wmarkers(void *pFp)
{
    (void)pFp;
}

void *
_IO_iter_begin(void)
{
    return NULL;
}

void *
_IO_iter_end(void)
{
    return NULL;
}

void *
_IO_iter_next(void *pIter)
{
    (void)pIter;
    return NULL;
}

FILE *
_IO_iter_file(void *pIter)
{
    (void)pIter;
    return NULL;
}

/* string stream hooks */
void
_IO_str_init_static(void *pFp, char *pBuf, size_t cb, char *pEnd)
{
    (void)pFp;
    (void)pBuf;
    (void)cb;
    (void)pEnd;
}

void
_IO_str_init_readonly(void *pFp, const char *pBuf, int nCb)
{
    (void)pFp;
    (void)pBuf;
    (void)nCb;
}

int
_IO_str_overflow(void *pFp, int nCh)
{
    (void)pFp;
    (void)nCh;
    return EOF;
}

int
_IO_str_underflow(void *pFp)
{
    (void)pFp;
    return EOF;
}

int
_IO_str_pbackfail(void *pFp, int nCh)
{
    (void)pFp;
    (void)nCh;
    return EOF;
}

long
_IO_str_seekoff(void *pFp, long nOff, int nDir, int nMode)
{
    (void)pFp;
    (void)nOff;
    (void)nDir;
    (void)nMode;
    return -1;
}

/* wide file hooks (fail-closed) */
int
_IO_wdefault_uflow(void *pFp)
{
    (void)pFp;
    return (int)WEOF;
}

size_t
_IO_wdefault_xsputn(void *pFp, const void *pData, size_t n)
{
    (void)pFp;
    (void)pData;
    (void)n;
    return 0;
}

size_t
_IO_wdefault_xsgetn(void *pFp, void *pData, size_t n)
{
    (void)pFp;
    (void)pData;
    (void)n;
    return 0;
}

int
_IO_wdefault_doallocate(void *pFp)
{
    (void)pFp;
    return 0;
}

void
_IO_wdefault_finish(void *pFp, int nDummy)
{
    (void)pFp;
    (void)nDummy;
}

int
_IO_wdefault_pbackfail(void *pFp, int nCh)
{
    (void)pFp;
    (void)nCh;
    return (int)WEOF;
}

int
_IO_wdoallocbuf(void *pFp)
{
    (void)pFp;
    return 0;
}

int
_IO_wdo_write(void *pFp, const wchar_t *pData, size_t c)
{
    (void)pFp;
    (void)pData;
    (void)c;
    return 0;
}

int
_IO_wfile_overflow(void *pFp, int nCh)
{
    (void)pFp;
    (void)nCh;
    return (int)WEOF;
}

int
_IO_wfile_underflow(void *pFp)
{
    (void)pFp;
    return (int)WEOF;
}

long
_IO_wfile_seekoff(void *pFp, long nOff, int nDir, int nMode)
{
    (void)pFp;
    (void)nOff;
    (void)nDir;
    (void)nMode;
    return -1;
}

int
_IO_wfile_sync(void *pFp)
{
    (void)pFp;
    return 0;
}

size_t
_IO_wfile_xsputn(void *pFp, const void *pData, size_t n)
{
    (void)pFp;
    (void)pData;
    (void)n;
    return 0;
}

void
_IO_wsetb(void *pFp, wchar_t *pBuf, wchar_t *pEnd, int nSet)
{
    (void)pFp;
    (void)pBuf;
    (void)pEnd;
    (void)nSet;
}

int
_IO_wmarker_delta(void *pMark)
{
    (void)pMark;
    return 0;
}

void
_IO_free_wbackup_area(void *pFp)
{
    (void)pFp;
}

void
_IO_switch_to_wget_mode(void *pFp)
{
    (void)pFp;
}

void
_IO_switch_to_wbackup_area(void *pFp)
{
    (void)pFp;
}

void
_IO_switch_to_main_wget_area(void *pFp)
{
    (void)pFp;
}

/* proc open/close for popen graph shape */
void *
_IO_proc_open(void *pFp, const char *szCmd, const char *szMode)
{
    (void)pFp;
    (void)szCmd;
    (void)szMode;
    errno = ENOSYS;
    return NULL;
}

int
_IO_proc_close(void *pFp)
{
    (void)pFp;
    return -1;
}

/* printf internal (no FP) */
int
__printf_fphex(FILE *pF, const void *pInfo, const void *pArgs)
{
    (void)pF;
    (void)pInfo;
    (void)pArgs;
    errno = ENOSYS;
    return -1;
}

int
__vfscanf_internal(FILE *pF, const char *szFmt, va_list ap, unsigned nMode)
{
    (void)nMode;
    return vfscanf(pF, szFmt, ap);
}

int
__vfprintf_internal(FILE *pF, const char *szFmt, va_list ap, unsigned nMode)
{
    (void)nMode;
    return vfprintf(pF, szFmt, ap);
}

int
__vsnprintf_internal(char *sz, size_t cb, const char *szFmt, va_list ap,
                     unsigned nMode)
{
    (void)nMode;
    return vsnprintf(sz, cb, szFmt, ap);
}

int
__vsprintf_internal(char *sz, const char *szFmt, va_list ap, unsigned nMode)
{
    (void)nMode;
    return vsprintf(sz, szFmt, ap);
}

/* ---- map-planned remainder (batch27 surface) ---------------------------- */

size_t
__malloc_usable_size(void *p)
{
    return malloc_usable_size(p);
}

void
__fseterr(FILE *pF)
{
    /* Freestanding FILE has no public error-bit setter; graph stub. */
    (void)pF;
}

void
__libc_message(const char *szFmt, ...)
{
    va_list ap;

    if (szFmt == NULL) {
        return;
    }
    va_start(ap, szFmt);
    (void)vfprintf(stderr, szFmt, ap);
    va_end(ap);
}

void
__assert_fail_base(const char *szFmt, const char *szAssertion,
                   const char *szFile, unsigned int nLine,
                   const char *szFunction)
{
    (void)szFmt;
    (void)szAssertion;
    (void)szFile;
    (void)nLine;
    (void)szFunction;
    abort();
}

void
__tunables_init(char **ppEnv)
{
    (void)ppEnv;
}

void
__libc_print_version(void)
{
    (void)fputs("libcgj (GreenJade clean-room)\n", stderr);
}

void
__libc_print_banner(void)
{
    (void)fputs("GreenJade libcgj\n", stderr);
}

/* rseq shape (data) — zero means unavailable */
unsigned int __rseq_size;
ptrdiff_t __rseq_offset;
unsigned int __rseq_flags;

/* gettext catalog change counter */
int _nl_msg_cat_cntr;

/* Classic sys_errlist surface (small static table) */
const char *const sys_errlist[] = {
    "Success",
    "Operation not permitted",
    "No such file or directory",
    "No such process",
    "Interrupted system call",
    "Input/output error",
    "No such device or address",
    "Argument list too long",
    "Exec format error",
    "Bad file descriptor",
    "No child processes",
    "Resource temporarily unavailable",
    "Cannot allocate memory",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Device or resource busy",
    "File exists",
    "Invalid cross-device link",
    "No such device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "Too many open files in system",
    "Too many open files",
    "Inappropriate ioctl for device",
    "Text file busy",
    "File too large",
    "No space left on device",
    "Illegal seek",
    "Read-only file system",
    "Too many links",
    "Broken pipe",
    "Numerical argument out of domain",
    "Numerical result out of range",
    "Resource deadlock avoided",
    "File name too long",
    "No locks available",
    "Function not implemented",
    "Directory not empty",
    "Too many levels of symbolic links",
    "Unknown error",
};
const char *const *_sys_errlist = sys_errlist;
int sys_nerr = (int)(sizeof(sys_errlist) / sizeof(sys_errlist[0]));
int _sys_nerr = (int)(sizeof(sys_errlist) / sizeof(sys_errlist[0]));

char **__libc_argv;
int __libc_argc;
char **_dl_argv;
size_t _dl_pagesize = 4096u;
int __nptl_nthreads = 1;

char *
__strstr_g(const char *szHay, const char *szNeedle)
{
    return strstr(szHay, szNeedle);
}

char *
__strstr_cg(const char *szHay, const char *szNeedle)
{
    return strstr(szHay, szNeedle);
}

char *
__strpbrk_c1(const char *sz, char c0)
{
    char a[2];

    a[0] = c0;
    a[1] = '\0';
    return strpbrk(sz, a);
}

size_t
__strcspn_cg(const char *sz, const char *szReject)
{
    return strcspn(sz, szReject);
}

size_t
__strspn_cg(const char *sz, const char *szAccept)
{
    return strspn(sz, szAccept);
}

char *
__stpncpy(char *pDst, const char *pSrc, size_t c)
{
    return stpncpy(pDst, pSrc, c);
}

int
__strcasecmp(const char *szA, const char *szB)
{
    return strcasecmp(szA, szB);
}

int
__strncasecmp(const char *szA, const char *szB, size_t n)
{
    return strncasecmp(szA, szB, n);
}

int
__sigemptyset(sigset_t *pSet)
{
    return sigemptyset(pSet);
}

int
__sigfillset(sigset_t *pSet)
{
    return sigfillset(pSet);
}

int
__openat_nocancel(int nDirfd, const char *szPath, int nFlags, ...)
{
    mode_t mode = 0;
    va_list ap;

    if ((nFlags & O_CREAT) != 0) {
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return openat(nDirfd, szPath, nFlags, mode);
}

int
__ppoll_nocancel(struct pollfd *pFds, nfds_t nFds,
                 const struct timespec *pTs, const void *pMask)
{
    return ppoll(pFds, nFds, pTs, pMask);
}

int
__accept4_nocancel(int nFd, struct sockaddr *pAddr, socklen_t *pcbAddr,
                   int nFlags)
{
    return accept4(nFd, pAddr, pcbAddr, nFlags);
}

int
__GI_raise(int nSig)
{
    return raise(nSig);
}

int *
__GI___errno_location(void)
{
    return __errno_location();
}

/* Keep argv/pagesize in sync once libc_start runs (best-effort). */
__attribute__((constructor)) static void
b27_ctor(void)
{
    if (_dl_pagesize == 0) {
        _dl_pagesize = 4096u;
    }
    if (__nptl_nthreads < 1) {
        __nptl_nthreads = 1;
    }
}
