/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped stdlib (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void  *malloc(size_t cb);
void   free(void *p);
void   cfree(void *p);
void  *__libc_malloc(size_t cb);
void  *__libc_calloc(size_t n, size_t cb);
void  *__libc_realloc(void *p, size_t cb);
void   __libc_free(void *p);
void  *calloc(size_t n, size_t cb);
void  *realloc(void *p, size_t cb);
const char *gnu_get_libc_version(void);
const char *gnu_get_libc_release(void);
size_t malloc_usable_size(void *p);
void  *reallocarray(void *p, size_t n, size_t cb);
void  *recallocarray(void *p, size_t nOld, size_t nNew, size_t cb);
void   freezero(void *p, size_t cb);
void  *reallocf(void *p, size_t cb);
long long strtonum(const char *sz, long long nMin, long long nMax,
                   const char **ppErr);
const char *getprogname(void);
void   setprogname(const char *sz);
int    heapsort(void *pBase, size_t nNel, size_t cbWidth,
                int (*pfnCmp)(const void *, const void *));
int    mergesort(void *pBase, size_t nNel, size_t cbWidth,
                 int (*pfnCmp)(const void *, const void *));
void  *aligned_alloc(size_t uAlign, size_t cb);
void  *memalign(size_t uAlign, size_t cb);
void  *valloc(size_t cb);
void  *pvalloc(size_t cb);
int    posix_memalign(void **pp, size_t uAlign, size_t cb);
void   abort(void) __attribute__((noreturn));
void   exit(int nCode) __attribute__((noreturn));
void   _exit(int nCode) __attribute__((noreturn));
void   _Exit(int nCode) __attribute__((noreturn));
void   quick_exit(int nCode) __attribute__((noreturn));
int    atexit(void (*pfn)(void));
int    at_quick_exit(void (*pfn)(void));
int    on_exit(void (*pfn)(int, void *), void *pArg);
int    dysize(int nYear);
int    abs(int n);
long   labs(long n);
long long llabs(long long n);
long   a64l(const char *sz);
char  *l64a(long n);

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
char  *tmpnam(char *szBuf);
char  *tmpnam_r(char *szBuf);
char  *tempnam(const char *szDir, const char *szPfx);
int    mkstemps(char *szTemplate, int nSuffixLen);
int    mkstemps64(char *szTemplate, int nSuffixLen);
int    mkostemps(char *szTemplate, int nSuffixLen, int nFlags);
long   strtoq(const char *sz, char **ppEnd, int nBase);
unsigned long strtouq(const char *sz, char **ppEnd, int nBase);
int    l64a_r(long n, char *szBuf, int nBuflen);
int    rpmatch(const char *sz);
const char *gnu_get_libc_version(void);
const char *gnu_get_libc_release(void);
/* gnu_get_libc_* implemented in graph_batch12.c */

/* POSIX rand48 (double APIs return +0.0 bit pattern — no SSE) */
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
int    system(const char *szCmd);
char  *get_current_dir_name(void);
int    atoi(const char *sz);
long   atol(const char *sz);
long long atoll(const char *sz);
long   strtol(const char *sz, char **ppEnd, int nBase);
unsigned long strtoul(const char *sz, char **ppEnd, int nBase);
long long strtoll(const char *sz, char **ppEnd, int nBase);
unsigned long long strtoull(const char *sz, char **ppEnd, int nBase);
long   __isoc23_strtol(const char *sz, char **ppEnd, int nBase);
unsigned long __isoc23_strtoul(const char *sz, char **ppEnd, int nBase);
long long __isoc23_strtoll(const char *sz, char **ppEnd, int nBase);
unsigned long long __isoc23_strtoull(const char *sz, char **ppEnd, int nBase);
extern char **environ;
char  *getenv(const char *szName);
char  *secure_getenv(const char *szName);
int    setenv(const char *szName, const char *szVal, int nOverwrite);
int    unsetenv(const char *szName);
int    clearenv(void);
int    putenv(char *szString);
char  *realpath(const char *szPath, char *szResolved);
int    mkstemp(char *szTemplate);
int    mkostemp(char *szTemplate, int nFlags);
char  *mkdtemp(char *szTemplate);

/* OpenBSD/glibc arc4random family (getrandom-backed) */
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
void   qsort(void *pBase, size_t n, size_t cb,
             int (*pfnCmp)(const void *, const void *));
void   qsort_r(void *pBase, size_t n, size_t cb,
               int (*pfnCmp)(const void *, const void *, void *), void *pArg);
void  *bsearch(const void *pKey, const void *pBase, size_t n, size_t cb,
               int (*pfnCmp)(const void *, const void *));
int    daemon(int nNochdir, int nNoclose);
int    group_member(gid_t gid);
char  *getwd(char *szBuf);
int    mkstemp64(char *szTemplate);
int    mkostemp64(char *szTemplate, int nFlags);

#ifdef __cplusplus
}
#endif
