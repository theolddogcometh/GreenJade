/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <stdio.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Buffered FILE streams: printf/scanf families, fopen/fdopen/fmemopen/
 * open_memstream, flockfile + unlocked I/O, fortify printf, and a subset of
 * libio _IO_* aliases for dynlink graphs. Wide stdio is declared in <wchar.h>
 * (open_wmemstream lives there).
 *
 * Design notes
 * ------------
 * FILE is an opaque-to-apps layout implemented as struct gj_cgj_file: fd-
 * backed, memstream, and cookie streams share one struct. Buffering modes
 * match ISO C (_IOFBF/_IOLBF/_IONBF). fpos_t is long on this bring-up (LFS
 * *64 aliases are identity on LP64).
 *
 * Non-goals
 * ---------
 * Full glibc libio ABI binary layout compatibility with host libio objects;
 * we export symbols, not the historical FILE memory layout for foreign libc.
 *
 * See docs/GLIBC_COMPAT.md (stdio bar).
 */
#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EOF     (-1)
#define BUFSIZ  1024
#define _IOFBF  0  /* fully buffered */
#define _IOLBF  1  /* line buffered */
#define _IONBF  2  /* unbuffered */

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef long fpos_t;

/**
 * libcgj stream object (apps treat FILE as opaque).
 *
 * nFd is the backing descriptor (-1 for pure memory/cookie streams).
 * Memstream fields track user-visible buffer pointers updated on flush/seek.
 * Cookie function pointers implement fopencookie custom I/O.
 */
typedef struct gj_cgj_file {
    int            nFd;
    int            nFlags;   /* r/w/ownbuf/memstream/wmemstream */
    int            nErr;
    int            nEof;
    int            nBufMode; /* _IOFBF / _IOLBF / _IONBF */
    int            nUnget;   /* -1 or pushed char */
    int            nWide;    /* 0 unset, >0 wide, <0 byte oriented */
    size_t         cbBuf;
    size_t         cbWpos;   /* bytes pending in write buffer */
    size_t         cbRlen;   /* valid read bytes in buffer */
    size_t         cbRpos;   /* next read index */
    unsigned char *pBuf;
    unsigned char  aSmall[256];
    /* open_memstream / open_wmemstream */
    char         **ppMemUser;
    void         **ppWMemUser; /* wchar_t ** when F_WMEMSTREAM */
    size_t        *pcbMemUser;
    char          *pMemData;
    size_t         cbMemCap; /* bytes of pMemData capacity */
    size_t         cbMemLen; /* bytes used (narrow) or wchar count (wide) */
    off_t          offMem;
    /* fopencookie */
    void          *pCookie;
    ssize_t      (*pfnCkRead)(void *, char *, size_t);
    ssize_t      (*pfnCkWrite)(void *, const char *, size_t);
    int          (*pfnCkSeek)(void *, off_t *, int);
    int          (*pfnCkClose)(void *);
} FILE;

/** Callback table for fopencookie (GNU extension). NULL members may ENOSYS. */
typedef struct {
    ssize_t (*read)(void *pCookie, char *pBuf, size_t cb);
    ssize_t (*write)(void *pCookie, const char *pBuf, size_t cb);
    int     (*seek)(void *pCookie, off_t *pOff, int nWhence);
    int     (*close)(void *pCookie);
} cookie_io_functions_t;

FILE *fopencookie(void *pCookie, const char *szMode, cookie_io_functions_t io);

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

/* ---- Formatted I/O ------------------------------------------------------ */

int    printf(const char *szFmt, ...);
int    fprintf(FILE *pF, const char *szFmt, ...);
int    vfprintf(FILE *pF, const char *szFmt, va_list ap);
int    vprintf(const char *szFmt, va_list ap);
int    sprintf(char *szBuf, const char *szFmt, ...);
int    vsprintf(char *szBuf, const char *szFmt, va_list ap);
int    snprintf(char *szBuf, size_t cb, const char *szFmt, ...);
int    vsnprintf(char *szBuf, size_t cb, const char *szFmt, va_list ap);
int    asprintf(char **ppBuf, const char *szFmt, ...);  /* malloc *ppBuf */
int    __asprintf(char **ppBuf, const char *szFmt, ...);
int    vasprintf(char **ppBuf, const char *szFmt, va_list ap);
int    fcloseall(void);
FILE  *fopen64(const char *szPath, const char *szMode); /* LFS alias */
int    fgetpos64(FILE *pF, fpos_t *pPos);
int    fsetpos64(FILE *pF, const fpos_t *pPos);
char  *ctermid(char *sz);
char  *cuserid(char *sz);

/* ---- Scanf family ------------------------------------------------------- */

int    scanf(const char *szFmt, ...);
int    fscanf(FILE *pF, const char *szFmt, ...);
int    sscanf(const char *szBuf, const char *szFmt, ...);
int    vscanf(const char *szFmt, va_list ap);
int    vfscanf(FILE *pF, const char *szFmt, va_list ap);
int    vsscanf(const char *szBuf, const char *szFmt, va_list ap);

/* ---- Character / string / buffer I/O ------------------------------------ */

int    puts(const char *sz);
int    fputs(const char *sz, FILE *pF);
int    putchar(int ch);
int    fputc(int ch, FILE *pF);
int    putc(int ch, FILE *pF);
int    getchar(void);
int    fgetc(FILE *pF);
int    getc(FILE *pF);
int    ungetc(int ch, FILE *pF);
char  *fgets(char *sz, int n, FILE *pF);
int    fflush(FILE *pF);
int    fflush_unlocked(FILE *pF);
int    fileno(FILE *pF);
int    setvbuf(FILE *pF, char *pBuf, int nMode, size_t cb);
void   setbuf(FILE *pF, char *pBuf);
void   setbuffer(FILE *pF, char *pBuf, size_t cb);
void   setlinebuf(FILE *pF);
size_t fwrite(const void *p, size_t cb, size_t n, FILE *pF);
size_t fread(void *p, size_t cb, size_t n, FILE *pF);

/* ---- Open / close / seek / status --------------------------------------- */

FILE  *fopen(const char *szPath, const char *szMode);
FILE  *fdopen(int nFd, const char *szMode);
FILE  *freopen(const char *szPath, const char *szMode, FILE *pF);
FILE  *freopen64(const char *szPath, const char *szMode, FILE *pF);
FILE  *fmemopen(void *pBuf, size_t cb, const char *szMode);
FILE  *open_memstream(char **ppBuf, size_t *pcb); /* updates *ppBuf on flush */
/* open_wmemstream declared in wchar.h */
int    fclose(FILE *pF);
int    fseek(FILE *pF, long off, int nWhence);
int    fseeko(FILE *pF, off_t off, int nWhence);
long   ftell(FILE *pF);
off_t  ftello(FILE *pF);
void   rewind(FILE *pF);
void   clearerr(FILE *pF);
int    ferror(FILE *pF);
int    feof(FILE *pF);
void   perror(const char *sz);
int    remove(const char *szPath);
int    rename(const char *szOld, const char *szNew);
ssize_t getline(char **ppLine, size_t *pcb, FILE *pF);
ssize_t getdelim(char **ppLine, size_t *pcb, int nDelim, FILE *pF);
int     dprintf(int nFd, const char *szFmt, ...);
int     vdprintf(int nFd, const char *szFmt, va_list ap);
int     fgetpos(FILE *pF, fpos_t *pPos);
int     fsetpos(FILE *pF, const fpos_t *pPos);

/* ---- Stream locking + unlocked I/O (POSIX / glibc) ---------------------- */

void    flockfile(FILE *pF);
void    funlockfile(FILE *pF);
int     ftrylockfile(FILE *pF);
int     getc_unlocked(FILE *pF);
int     putc_unlocked(int ch, FILE *pF);
int     getchar_unlocked(void);
int     putchar_unlocked(int ch);
size_t  fread_unlocked(void *p, size_t cb, size_t n, FILE *pF);
size_t  fwrite_unlocked(const void *p, size_t cb, size_t n, FILE *pF);
int     fputc_unlocked(int ch, FILE *pF);
int     fputs_unlocked(const char *sz, FILE *pF);
int     fgetc_unlocked(FILE *pF);
char   *fgets_unlocked(char *sz, int n, FILE *pF);
void    clearerr_unlocked(FILE *pF);
int     feof_unlocked(FILE *pF);
int     ferror_unlocked(FILE *pF);
int     fileno_unlocked(FILE *pF);
FILE   *tmpfile(void);
FILE   *popen(const char *szCmd, const char *szMode);
int     pclose(FILE *pF);

/* C99 scanf aliases (glibc graph) */
int     __isoc99_scanf(const char *szFmt, ...);
int     __isoc99_fscanf(FILE *pF, const char *szFmt, ...);
int     __isoc99_sscanf(const char *szBuf, const char *szFmt, ...);
int     __isoc99_vscanf(const char *szFmt, va_list ap);
int     __isoc99_vfscanf(FILE *pF, const char *szFmt, va_list ap);
int     __isoc99_vsscanf(const char *szBuf, const char *szFmt, va_list ap);
/* C23 binary-compatible scanf aliases */
int     __isoc23_scanf(const char *szFmt, ...);
int     __isoc23_fscanf(FILE *pF, const char *szFmt, ...);
int     __isoc23_sscanf(const char *szBuf, const char *szFmt, ...);
int     __isoc23_vscanf(const char *szFmt, va_list ap);
int     __isoc23_vfscanf(FILE *pF, const char *szFmt, va_list ap);
int     __isoc23_vsscanf(const char *szBuf, const char *szFmt, va_list ap);

/* ---- libio-shaped aliases (symbol graph; not host FILE layout) ---------- */

int     _IO_getc(FILE *pF);
int     _IO_putc(int ch, FILE *pF);
int     _IO_feof(FILE *pF);
int     _IO_ferror(FILE *pF);
int     _IO_printf(const char *szFmt, ...);
int     _IO_fprintf(FILE *pF, const char *szFmt, ...);
int     _IO_vfprintf(FILE *pF, const char *szFmt, va_list ap);
int     _IO_sprintf(char *szBuf, const char *szFmt, ...);
int     _IO_vsprintf(char *szBuf, const char *szFmt, va_list ap);
int     _IO_puts(const char *sz);
int     _IO_fputs(const char *sz, FILE *pF);
char   *_IO_fgets(char *sz, int n, FILE *pF);
size_t  _IO_fread(void *p, size_t cb, size_t n, FILE *pF);
size_t  _IO_fwrite(const void *p, size_t cb, size_t n, FILE *pF);
FILE   *_IO_fopen(const char *szPath, const char *szMode);
int     _IO_fclose(FILE *pF);
int     _IO_fflush(FILE *pF);
long    _IO_padn(FILE *pF, int ch, long n);
size_t  _IO_sgetn(FILE *pF, void *pBuf, size_t cb);
int     __uflow(FILE *pF);              /* underflow helper */
int     __overflow(FILE *pF, int ch);   /* overflow helper */

/* Fortify printf family (subset; nFlag / cbDst from compiler) */
int     __printf_chk(int nFlag, const char *szFmt, ...);
int     __fprintf_chk(FILE *pF, int nFlag, const char *szFmt, ...);
int     __sprintf_chk(char *szBuf, int nFlag, size_t cbDst, const char *szFmt,
                      ...);
int     __snprintf_chk(char *szBuf, size_t cb, int nFlag, size_t cbDst,
                       const char *szFmt, ...);
int     __vsnprintf_chk(char *szBuf, size_t cb, int nFlag, size_t cbDst,
                        const char *szFmt, va_list ap);
int     __vsprintf_chk(char *szBuf, int nFlag, size_t cbDst, const char *szFmt,
                       va_list ap);
int     fseeko64(FILE *pF, off_t off, int nWhence);
off_t   ftello64(FILE *pF);
FILE   *tmpfile64(void);

#ifdef __cplusplus
}
#endif
