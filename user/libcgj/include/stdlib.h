/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <stdlib.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Heap (malloc family + aligned/posix_memalign), process exit/atexit,
 * env (environ/getenv/setenv), integer conversion (strto*), sorting/search,
 * PRNG (rand/random/rand48/arc4random), temp files (mkstemp*), and a few
 * BSD/GNU extras needed by desktop graphs.
 *
 * Design notes
 * ------------
 * Heap is freelist + sbrk/mmap-shaped bring-up (see docs/GLIBC_COMPAT.md).
 * gnu_get_libc_version/release report libcgj identity strings for apps that
 * probe "glibc" version without linking the real library.
 * arc4random* is getrandom-backed (not RC4).
 *
 * Non-goals
 * ---------
 * Floating strtod/strtof (SSE/ABI policy may omit doubles on some paths);
 * full multibyte conversion (see <wchar.h>). drand48/erand48 omitted when
 * double returns need SSE under SysV — integer rand48 remains.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Heap --------------------------------------------------------------- */

void  *malloc(size_t cb);
void   free(void *p);
void   cfree(void *p); /* historical alias of free */
void  *__libc_malloc(size_t cb);
void  *__libc_calloc(size_t n, size_t cb);
void  *__libc_realloc(void *p, size_t cb);
void   __libc_free(void *p);
void  *calloc(size_t n, size_t cb);
void  *realloc(void *p, size_t cb);
const char *gnu_get_libc_version(void);
const char *gnu_get_libc_release(void);
size_t malloc_usable_size(void *p); /* bytes available in block, or 0 */
void  *reallocarray(void *p, size_t n, size_t cb); /* overflow-checked */
void  *recallocarray(void *p, size_t nOld, size_t nNew, size_t cb); /* BSD */
void   freezero(void *p, size_t cb);  /* explicit_bzero then free */
void  *reallocf(void *p, size_t cb);  /* free p on realloc failure */
long long strtonum(const char *sz, long long nMin, long long nMax,
                   const char **ppErr); /* OpenBSD bounded parse */
const char *getprogname(void);
void   setprogname(const char *sz);
int    heapsort(void *pBase, size_t nNel, size_t cbWidth,
                int (*pfnCmp)(const void *, const void *));
int    mergesort(void *pBase, size_t nNel, size_t cbWidth,
                 int (*pfnCmp)(const void *, const void *));
void  *aligned_alloc(size_t uAlign, size_t cb); /* C11; align power of two */
void  *memalign(size_t uAlign, size_t cb);
void  *valloc(size_t cb);   /* page-aligned */
void  *pvalloc(size_t cb);  /* round up to pages */
int    posix_memalign(void **pp, size_t uAlign, size_t cb);

/* ---- Process lifetime --------------------------------------------------- */

void   abort(void) __attribute__((noreturn));
void   exit(int nCode) __attribute__((noreturn));
void   _exit(int nCode) __attribute__((noreturn));  /* no atexit */
void   _Exit(int nCode) __attribute__((noreturn));  /* ISO: no atexit */
void   quick_exit(int nCode) __attribute__((noreturn));
int    atexit(void (*pfn)(void));
int    at_quick_exit(void (*pfn)(void));
int    on_exit(void (*pfn)(int, void *), void *pArg); /* glibc: status + arg */

/* ---- Integer helpers ---------------------------------------------------- */

int    dysize(int nYear); /* days in year (leap-aware) */
int    abs(int n);
long   labs(long n);
long long llabs(long long n);
long   a64l(const char *sz); /* radix-64 string → long */
char  *l64a(long n);         /* long → static radix-64 string */

typedef struct {
    int quot;
    int rem;
} div_t;
typedef struct {
    long quot;
    long rem;
} ldiv_t;
typedef struct {
    long long quot;
    long long rem;
} lldiv_t;

div_t   div(int nNumer, int nDenom);
ldiv_t  ldiv(long nNumer, long nDenom);
lldiv_t lldiv(long long nNumer, long long nDenom);

/* ---- PRNG (not for crypto — use arc4random / getrandom) ----------------- */

void   srand(unsigned int uSeed);
int    rand(void);
int    rand_r(unsigned int *pSeed);
void   srandom(unsigned int uSeed);
long   random(void);
struct random_data;
int    srandom_r(unsigned int uSeed, struct random_data *pBuf);
int    random_r(struct random_data *pBuf, int32_t *pResult);
char  *initstate(unsigned int uSeed, char *szState, size_t n);
char  *setstate(char *szState);
int    initstate_r(unsigned int uSeed, char *szState, size_t n,
                   struct random_data *pBuf);
int    setstate_r(char *szState, struct random_data *pBuf);

/* ---- Temporary names / files (prefer mkstemp over tmpnam) --------------- */

char  *tmpnam(char *szBuf);
char  *tmpnam_r(char *szBuf);
char  *tempnam(const char *szDir, const char *szPfx);
int    mkstemps(char *szTemplate, int nSuffixLen);
int    mkstemps64(char *szTemplate, int nSuffixLen);
int    mkostemps(char *szTemplate, int nSuffixLen, int nFlags);
long   strtoq(const char *sz, char **ppEnd, int nBase);   /* ≈ strtoll */
unsigned long strtouq(const char *sz, char **ppEnd, int nBase);
int    l64a_r(long n, char *szBuf, int nBuflen);
int    rpmatch(const char *sz); /* yes/no locale match; C: y/Y vs n/N */
const char *gnu_get_libc_version(void);
const char *gnu_get_libc_release(void);
/* gnu_get_libc_* implemented in graph_batch12.c */

/*
 * POSIX rand48 integer APIs. drand48/erand48 omitted: double return needs
 * SSE under SysV ABI on some freestanding configs.
 */
struct drand48_data;
void   srand48(long nSeed);
unsigned short *seed48(unsigned short aSeed16v[3]);
void   lcong48(unsigned short aParam[7]);
long   lrand48(void);
long   nrand48(unsigned short aXsubi[3]);
long   mrand48(void);
long   jrand48(unsigned short aXsubi[3]);
/* drand48/erand48 omitted: double return needs SSE under SysV ABI */
int    srand48_r(long nSeed, struct drand48_data *pBuf);
int    seed48_r(unsigned short aSeed16v[3], struct drand48_data *pBuf);
int    lcong48_r(unsigned short aParam[7], struct drand48_data *pBuf);
int    lrand48_r(struct drand48_data *pBuf, long *pResult);
int    nrand48_r(unsigned short aXsubi[3], struct drand48_data *pBuf,
                 long *pResult);
int    mrand48_r(struct drand48_data *pBuf, long *pResult);
int    jrand48_r(unsigned short aXsubi[3], struct drand48_data *pBuf,
                 long *pResult);

/* ---- Shell / paths / conversion / environment --------------------------- */

int    system(const char *szCmd);
char  *get_current_dir_name(void); /* malloc'd cwd; free() when done */
int    atoi(const char *sz);
long   atol(const char *sz);
long long atoll(const char *sz);
long   strtol(const char *sz, char **ppEnd, int nBase);
unsigned long strtoul(const char *sz, char **ppEnd, int nBase);
long long strtoll(const char *sz, char **ppEnd, int nBase);
unsigned long long strtoull(const char *sz, char **ppEnd, int nBase);
/* C23 binary-compatible aliases (same semantics as strto* above) */
long   __isoc23_strtol(const char *sz, char **ppEnd, int nBase);
unsigned long __isoc23_strtoul(const char *sz, char **ppEnd, int nBase);
long long __isoc23_strtoll(const char *sz, char **ppEnd, int nBase);
unsigned long long __isoc23_strtoull(const char *sz, char **ppEnd, int nBase);
extern char **environ;
char  *getenv(const char *szName);
char  *secure_getenv(const char *szName); /* NULL if issetugid / elevated */
int    setenv(const char *szName, const char *szVal, int nOverwrite);
int    unsetenv(const char *szName);
int    clearenv(void);
int    putenv(char *szString); /* takes ownership of "NAME=val" string */
char  *realpath(const char *szPath, char *szResolved);
int    mkstemp(char *szTemplate);
int    mkostemp(char *szTemplate, int nFlags);
char  *mkdtemp(char *szTemplate);

/* OpenBSD/glibc arc4random family (getrandom-backed; not RC4 crypto) */
uint32_t arc4random(void);
void     arc4random_buf(void *pBuf, size_t cb);
uint32_t arc4random_uniform(uint32_t uUpper);
int      getsubopt(char **ppOptionp, char *const *ppTokens, char **ppValuep);

/* PTY helpers (glibc also exposes these via stdlib.h) */
int      posix_openpt(int nFlags);
int      grantpt(int nFd);
int      unlockpt(int nFd);
char    *ptsname(int nFd);
int      ptsname_r(int nFd, char *szBuf, size_t cb);

/* Internal: seed environ from aux stack (called by __libc_start_main). */
void   _libcgj_env_init(char **envp);

/* ---- Sort / search / misc ----------------------------------------------- */

void   qsort(void *pBase, size_t n, size_t cb,
             int (*pfnCmp)(const void *, const void *));
void   qsort_r(void *pBase, size_t n, size_t cb,
               int (*pfnCmp)(const void *, const void *, void *), void *pArg);
void  *bsearch(const void *pKey, const void *pBase, size_t n, size_t cb,
               int (*pfnCmp)(const void *, const void *));
int    daemon(int nNochdir, int nNoclose);
int    group_member(gid_t gid);
char  *getwd(char *szBuf); /* obsolete; prefer getcwd */
int    mkstemp64(char *szTemplate);
int    mkostemp64(char *szTemplate, int nFlags);

#ifdef __cplusplus
}
#endif
