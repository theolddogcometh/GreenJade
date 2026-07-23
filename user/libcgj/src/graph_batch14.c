/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14: ucontext stubs, gmon/profil, catgets, strfmon
 * (integer/literal only), libaio + io_uring syscall wrappers, obstack, argp
 * bring-up, printf custom hooks, pthread clocklocks, sigqueue, get_nprocs*,
 * getpeereid, killpg, lchmod, malloc_trim, putpwent/putgrent, ttyent, futimesat,
 * fflush_unlocked, versionsort64, sysctl. Integer/pointer only (no SSE doubles).
 *
 * greppable: CGJ_GRAPH_BATCH14_SOFT_NULL
 * greppable: CGJ_GRAPH_BATCH14_SOFT_ARGS
 * greppable: CGJ_GRAPH_BATCH14_SOFT_EDGE
 *
 * Soft deepen: null/arg guards on user-facing graph nodes; edge
 * hardening only. No multi-def; no API break. Pure C integer/pointer.
 */
#include <argp.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <inttypes.h>
#include <libaio.h>
#include <locale.h>
#include <monetary.h>
#include <nl_types.h>
#include <obstack.h>
#include <printf.h>
#include <pthread.h>
#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/gmon.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <ucontext.h>
#include <unistd.h>
#include <wchar.h>

#include <linux/io_uring.h>

/* ---- local SYSCALL helpers (sys6 is static in syscall_linux.c) ----------- */

#define NR_rt_sigqueueinfo 129
#define NR_sysctl          156
#define NR_io_setup        206
#define NR_io_destroy      207
#define NR_io_getevents    208
#define NR_io_submit       209
#define NR_io_cancel       210
#define NR_io_uring_setup  425
#define NR_io_uring_enter  426
#define NR_io_uring_register 427

#ifndef SO_PEERCRED
#define SO_PEERCRED 17
#endif

struct ucred {
    pid_t pid;
    uid_t uid;
    gid_t gid;
};

static long
b14_sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;
    long ret;

    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory");
    return ret;
}

static long
b14_sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

/* ---- inttypes wide ------------------------------------------------------ */

intmax_t
wcstoimax(const wchar_t *pwcs, wchar_t **ppEnd, int nBase)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pwcs == NULL) {
        errno = EFAULT;
        return -1;
    }

    return (intmax_t)wcstoll(pwcs, ppEnd, nBase);
}

uintmax_t
wcstoumax(const wchar_t *pwcs, wchar_t **ppEnd, int nBase)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pwcs == NULL) {
        errno = EFAULT;
        return -1;
    }

    return (uintmax_t)wcstoull(pwcs, ppEnd, nBase);
}

/* ---- ucontext (bring-up stubs; real switch needs arch asm) -------------- */

int
getcontext(ucontext_t *pUc)
{
    if (pUc == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(pUc, 0, sizeof(*pUc));
    errno = ENOSYS;
    return -1;
}

int
setcontext(const ucontext_t *pUc)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pUc == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pUc;
    errno = ENOSYS;
    return -1;
}

void
makecontext(ucontext_t *pUc, void (*pfn)(void), int nArg, ...)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pUc == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pUc;
    (void)pfn;
    (void)nArg;
    /* No-op bring-up: cannot wire stack without arch glue */
}

int
swapcontext(ucontext_t *pOuc, const ucontext_t *pUc)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pOuc == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pUc == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pOuc;
    (void)pUc;
    errno = ENOSYS;
    return -1;
}

/* ---- gmon / profil ------------------------------------------------------ */

int
monstartup(unsigned long uLow, unsigned long uHigh)
{
    (void)uLow;
    (void)uHigh;
    return 0;
}

void
_mcleanup(void)
{
}

int
profil(unsigned short *pBuf, size_t cbBuf, size_t uOffset, unsigned uScale)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pBuf;
    (void)cbBuf;
    (void)uOffset;
    (void)uScale;
    return 0;
}

/* ---- message catalogs --------------------------------------------------- */

nl_catd
catopen(const char *szName, int nFlag)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)szName;
    (void)nFlag;
    /* Identity catalog: non-NULL handle, catgets returns default */
    return (nl_catd)(uintptr_t)1;
}

char *
catgets(nl_catd cat, int nSet, int nMsg, const char *szDefault)
{
    (void)cat;
    (void)nSet;
    (void)nMsg;
    return (char *)(szDefault != NULL ? szDefault : "");
}

int
catclose(nl_catd cat)
{
    (void)cat;
    return 0;
}

/* ---- strfmon: integer/literal only (no floating conversion) ------------- */

static ssize_t
strfmon_core(char *sz, size_t cbMax, const char *szFmt, va_list ap)
{
    size_t nOut = 0;
    const char *p;

    if (sz == NULL || szFmt == NULL) {
        errno = EINVAL;
        return (ssize_t)-1;
    }
    if (cbMax == 0) {
        errno = E2BIG;
        return (ssize_t)-1;
    }
    p = szFmt;
    while (*p != '\0') {
        if (*p != '%') {
            if (nOut + 1 >= cbMax) {
                errno = E2BIG;
                return (ssize_t)-1;
            }
            sz[nOut++] = *p++;
            continue;
        }
        p++; /* skip % */
        /* skip flags / width / precision lightly */
        while (*p == '=' || *p == '^' || *p == '+' || *p == '(' || *p == '!' ||
               *p == '-' || *p == ' ') {
            p++;
        }
        while (*p >= '0' && *p <= '9') {
            p++;
        }
        if (*p == '#') {
            p++;
            while (*p >= '0' && *p <= '9') {
                p++;
            }
        }
        if (*p == '.') {
            p++;
            while (*p >= '0' && *p <= '9') {
                p++;
            }
        }
        if (*p == 'i' || *p == 'n') {
            /* Consume a long long monetary amount as integer cents-ish */
            long long nVal = va_arg(ap, long long);
            char aDig[32];
            int nDig = 0;
            int fNeg = 0;
            unsigned long long u;
            int i;

            if (nVal < 0) {
                fNeg = 1;
                u = (unsigned long long)(-nVal);
            } else {
                u = (unsigned long long)nVal;
            }
            if (u == 0) {
                aDig[nDig++] = '0';
            } else {
                while (u > 0 && nDig < (int)sizeof(aDig)) {
                    aDig[nDig++] = (char)('0' + (u % 10ull));
                    u /= 10ull;
                }
            }
            if (fNeg) {
                if (nOut + 1 >= cbMax) {
                    errno = E2BIG;
                    return (ssize_t)-1;
                }
                sz[nOut++] = '-';
            }
            for (i = nDig - 1; i >= 0; i--) {
                if (nOut + 1 >= cbMax) {
                    errno = E2BIG;
                    return (ssize_t)-1;
                }
                sz[nOut++] = aDig[i];
            }
            p++;
        } else if (*p == '%') {
            if (nOut + 1 >= cbMax) {
                errno = E2BIG;
                return (ssize_t)-1;
            }
            sz[nOut++] = '%';
            p++;
        } else if (*p != '\0') {
            /* Unknown conversion: copy literally */
            if (nOut + 1 >= cbMax) {
                errno = E2BIG;
                return (ssize_t)-1;
            }
            sz[nOut++] = *p++;
        }
    }
    sz[nOut] = '\0';
    return (ssize_t)nOut;
}

ssize_t
strfmon(char *sz, size_t cbMax, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    ssize_t n;

    va_start(ap, szFmt);
    n = strfmon_core(sz, cbMax, szFmt, ap);
    va_end(ap);
    return n;
}

ssize_t
strfmon_l(char *sz, size_t cbMax, locale_t loc, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    ssize_t n;

    (void)loc;
    va_start(ap, szFmt);
    n = strfmon_core(sz, cbMax, szFmt, ap);
    va_end(ap);
    return n;
}

/* ---- libaio wrappers ---------------------------------------------------- */

int
io_setup(unsigned nNr, io_context_t *pCtx)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pCtx == NULL) {
        errno = EFAULT;
        return -1;
    }

    return (int)b14_sys_ret(b14_sys6(NR_io_setup, (long)nNr,
                                     (long)(uintptr_t)pCtx, 0, 0, 0, 0));
}

int
io_destroy(io_context_t ctx)
{
    return (int)b14_sys_ret(
        b14_sys6(NR_io_destroy, (long)(uintptr_t)ctx, 0, 0, 0, 0, 0));
}

int
io_submit(io_context_t ctx, long nNr, struct iocb **ppIocbs)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (ppIocbs == NULL) {
        errno = EFAULT;
        return -1;
    }

    return (int)b14_sys_ret(b14_sys6(NR_io_submit, (long)(uintptr_t)ctx, nNr,
                                     (long)(uintptr_t)ppIocbs, 0, 0, 0));
}

int
io_cancel(io_context_t ctx, struct iocb *pIocb, struct io_event *pEvent)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pIocb == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pEvent == NULL) {
        errno = EFAULT;
        return -1;
    }

    return (int)b14_sys_ret(b14_sys6(NR_io_cancel, (long)(uintptr_t)ctx,
                                     (long)(uintptr_t)pIocb,
                                     (long)(uintptr_t)pEvent, 0, 0, 0));
}

int
io_getevents(io_context_t ctx, long nMin, long nNr, struct io_event *pEvents,
             struct timespec *pTimeout)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pEvents == NULL) {
        errno = EFAULT;
        return -1;
    }

    return (int)b14_sys_ret(b14_sys6(NR_io_getevents, (long)(uintptr_t)ctx,
                                     nMin, nNr, (long)(uintptr_t)pEvents,
                                     (long)(uintptr_t)pTimeout, 0));
}

/* ---- io_uring syscall surface ------------------------------------------- */

int
io_uring_setup(unsigned nEntries, struct io_uring_params *pParams)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pParams == NULL) {
        errno = EFAULT;
        return -1;
    }

    return (int)b14_sys_ret(b14_sys6(NR_io_uring_setup, (long)nEntries,
                                     (long)(uintptr_t)pParams, 0, 0, 0, 0));
}

int
io_uring_enter(unsigned nFd, unsigned nToSubmit, unsigned nMinComplete,
               unsigned nFlags, void *pSig, size_t cbSig)
{
    return (int)b14_sys_ret(b14_sys6(NR_io_uring_enter, (long)nFd,
                                     (long)nToSubmit, (long)nMinComplete,
                                     (long)nFlags, (long)(uintptr_t)pSig,
                                     (long)cbSig));
}

int
io_uring_register(unsigned nFd, unsigned nOpcode, void *pArg, unsigned nNrArgs)
{
    return (int)b14_sys_ret(b14_sys6(NR_io_uring_register, (long)nFd,
                                     (long)nOpcode, (long)(uintptr_t)pArg,
                                     (long)nNrArgs, 0, 0));
}

/* ---- obstack (uses struct _obstack_chunk from obstack.h) ---------------- */

static void *
ob_default_alloc(long n)
{
    return malloc((size_t)n);
}

static void
ob_default_free(void *p)
{
    free(p);
}

static void *
ob_call_alloc(struct obstack *pH, long n)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pH == NULL) {
        return NULL;
    }

    if (pH->use_extra_arg) {
        return pH->chunkfun(pH->extra_arg, n);
    }
    return ((void *(*)(long))(uintptr_t)pH->chunkfun)(n);
}

static void
ob_call_free(struct obstack *pH, void *p)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pH == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (pH->use_extra_arg) {
        pH->freefun(pH->extra_arg, p);
    } else {
        ((void (*)(void *))(uintptr_t)pH->freefun)(p);
    }
}

int
_obstack_begin(struct obstack *pH, int nSize, int nAlign,
               void *(*pfnAlloc)(long), void (*pfnFree)(void *))
{
    if (pH == NULL) {
        return 0;
    }
    memset(pH, 0, sizeof(*pH));
    if (nSize <= 0) {
        nSize = 4096;
    }
    if (nAlign <= 0) {
        nAlign = (int)sizeof(void *);
    }
    pH->chunk_size = nSize;
    pH->alignment_mask = nAlign - 1;
    pH->chunkfun = (void *(*)(void *, long))(uintptr_t)(
        pfnAlloc != NULL ? pfnAlloc : ob_default_alloc);
    pH->freefun = (void (*)(void *, void *))(uintptr_t)(
        pfnFree != NULL ? pfnFree : ob_default_free);
    pH->use_extra_arg = 0;
    _obstack_newchunk(pH, 0);
    return pH->chunk != NULL;
}

int
_obstack_begin_1(struct obstack *pH, int nSize, int nAlign,
                 void *(*pfnAlloc)(void *, long),
                 void (*pfnFree)(void *, void *), void *pArg)
{
    if (pH == NULL) {
        return 0;
    }
    memset(pH, 0, sizeof(*pH));
    if (nSize <= 0) {
        nSize = 4096;
    }
    if (nAlign <= 0) {
        nAlign = (int)sizeof(void *);
    }
    pH->chunk_size = nSize;
    pH->alignment_mask = nAlign - 1;
    pH->chunkfun = pfnAlloc;
    pH->freefun = pfnFree;
    pH->extra_arg = pArg;
    pH->use_extra_arg = 1;
    _obstack_newchunk(pH, 0);
    return pH->chunk != NULL;
}

void
_obstack_newchunk(struct obstack *pH, int nLength)
{
    long nNeed;
    struct _obstack_chunk *pNew;
    size_t nObj;

    if (pH == NULL) {
        return;
    }
    nObj = 0;
    if (pH->object_base != NULL && pH->next_free != NULL) {
        nObj = (size_t)(pH->next_free - pH->object_base);
    }
    nNeed = pH->chunk_size;
    if (nNeed < (long)(nObj + (size_t)nLength + 64u)) {
        nNeed = (long)(nObj + (size_t)nLength + 64u);
    }
    pNew = (struct _obstack_chunk *)ob_call_alloc(pH, nNeed);
    if (pNew == NULL) {
        pH->alloc_failed = 1;
        return;
    }
    pNew->prev = pH->chunk;
    pNew->limit = (char *)pNew + nNeed;
    if (nObj > 0 && pH->object_base != NULL) {
        memcpy(pNew->contents, pH->object_base, nObj);
    }
    pH->chunk = pNew;
    pH->object_base = pNew->contents;
    pH->next_free = pNew->contents + nObj;
    pH->chunk_limit = pNew->limit;
}

/* Avoid recursive macro from obstack.h */
#undef obstack_free
void
obstack_free(struct obstack *pH, void *pObj)
{
    struct _obstack_chunk *p;
    struct _obstack_chunk *pPrev;

    if (pH == NULL) {
        return;
    }
    if (pObj == NULL) {
        p = pH->chunk;
        while (p != NULL) {
            pPrev = p->prev;
            ob_call_free(pH, p);
            p = pPrev;
        }
        pH->chunk = NULL;
        pH->object_base = NULL;
        pH->next_free = NULL;
        pH->chunk_limit = NULL;
        return;
    }
    /* Free chunks above object; leave object as next free point */
    p = pH->chunk;
    while (p != NULL) {
        char *pBase = (char *)p;
        char *pLim = p->limit;

        if ((char *)pObj >= pBase && (char *)pObj < pLim) {
            pH->object_base = (char *)pObj;
            pH->next_free = (char *)pObj;
            pH->chunk_limit = pLim;
            return;
        }
        pPrev = p->prev;
        ob_call_free(pH, p);
        pH->chunk = pPrev;
        p = pPrev;
    }
}

void *
obstack_finish(struct obstack *pH)
{
    void *p;

    if (pH == NULL) {
        return NULL;
    }
    p = (void *)pH->object_base;
    pH->object_base = pH->next_free;
    return p;
}

int
_obstack_allocated_p(struct obstack *pH, void *pObj)
{
    struct _obstack_chunk *p;

    if (pH == NULL || pObj == NULL) {
        return 0;
    }
    for (p = pH->chunk; p != NULL; p = p->prev) {
        if ((char *)pObj >= (char *)p && (char *)pObj < p->limit) {
            return 1;
        }
    }
    return 0;
}

int
_obstack_memory_used(struct obstack *pH)
{
    struct _obstack_chunk *p;
    int n = 0;

    if (pH == NULL) {
        return 0;
    }
    for (p = pH->chunk; p != NULL; p = p->prev) {
        n += (int)(p->limit - (char *)p);
    }
    return n;
}

/* ---- argp bring-up ------------------------------------------------------ */

error_t argp_err_exit_status = 1;
const char *argp_program_version;
const char *argp_program_bug_address;
void (*argp_program_version_hook)(FILE *pStream, struct argp_state *pState);

void
argp_help(const struct argp *pArgp, FILE *pStream, unsigned uFlags, char *szName)
{
    const struct argp_option *pOpt;

    (void)uFlags;
    if (pStream == NULL) {
        return;
    }
    if (szName != NULL) {
        (void)fprintf(pStream, "Usage: %s", szName);
        if (pArgp != NULL && pArgp->args_doc != NULL) {
            (void)fprintf(pStream, " %s", pArgp->args_doc);
        }
        (void)fprintf(pStream, "\n");
    }
    if (pArgp == NULL || pArgp->options == NULL) {
        return;
    }
    for (pOpt = pArgp->options; pOpt->name != NULL || pOpt->key != 0; pOpt++) {
        if ((pOpt->flags & OPTION_HIDDEN) != 0 || pOpt->name == NULL) {
            continue;
        }
        (void)fprintf(pStream, "  --%s", pOpt->name);
        if (pOpt->arg != NULL) {
            (void)fprintf(pStream, "=%s", pOpt->arg);
        }
        if (pOpt->doc != NULL) {
            (void)fprintf(pStream, "\t%s", pOpt->doc);
        }
        (void)fprintf(pStream, "\n");
    }
}

void
argp_state_help(const struct argp_state *pState, FILE *pStream, unsigned uFlags)
{
    if (pState == NULL) {
        return;
    }
    argp_help(pState->root_argp, pStream, uFlags, pState->name);
}

void
argp_usage(const struct argp_state *pState)
{
    FILE *p = (pState != NULL && pState->err_stream != NULL) ? pState->err_stream
                                                            : stderr;
    argp_state_help(pState, p, 0);
    if (pState == NULL || (pState->flags & ARGP_NO_EXIT) == 0) {
        _Exit(argp_err_exit_status != 0 ? argp_err_exit_status : 1);
    }
}

void
argp_error(const struct argp_state *pState, const char *szFmt, ...)
{
    va_list ap;
    FILE *p = (pState != NULL && pState->err_stream != NULL) ? pState->err_stream
                                                            : stderr;

    if (pState != NULL && pState->name != NULL) {
        (void)fprintf(p, "%s: ", pState->name);
    }
    if (szFmt != NULL) {
        va_start(ap, szFmt);
        (void)vfprintf(p, szFmt, ap);
        va_end(ap);
    }
    (void)fprintf(p, "\n");
    if (pState == NULL || (pState->flags & ARGP_NO_EXIT) == 0) {
        _Exit(argp_err_exit_status != 0 ? argp_err_exit_status : 1);
    }
}

void
argp_failure(const struct argp_state *pState, int nStatus, int nErrnum,
             const char *szFmt, ...)
{
    va_list ap;
    FILE *p = (pState != NULL && pState->err_stream != NULL) ? pState->err_stream
                                                            : stderr;

    if (pState != NULL && pState->name != NULL) {
        (void)fprintf(p, "%s: ", pState->name);
    }
    if (szFmt != NULL) {
        va_start(ap, szFmt);
        (void)vfprintf(p, szFmt, ap);
        va_end(ap);
    }
    if (nErrnum != 0) {
        (void)fprintf(p, ": %s", strerror(nErrnum));
    }
    (void)fprintf(p, "\n");
    if (nStatus != 0 &&
        (pState == NULL || (pState->flags & ARGP_NO_EXIT) == 0)) {
        _Exit(nStatus);
    }
}

error_t
argp_parse(const struct argp *pArgp, int nArgc, char **ppArgv, unsigned uFlags,
           int *pArgIndex, void *pInput)
{
    struct argp_state st;
    int i;
    error_t err = 0;

    memset(&st, 0, sizeof(st));
    st.root_argp = pArgp;
    st.argc = nArgc;
    st.argv = ppArgv;
    st.flags = uFlags;
    st.input = pInput;
    st.err_stream = stderr;
    st.out_stream = stdout;
    st.name = (ppArgv != NULL && nArgc > 0 && ppArgv[0] != NULL) ? ppArgv[0]
                                                                : (char *)"";
    st.next = ((uFlags & ARGP_PARSE_ARGV0) != 0) ? 0 : 1;

    if (pArgp != NULL && pArgp->parser != NULL) {
        err = pArgp->parser(0 /* ARGP_KEY_INIT shape: 0 */, NULL, &st);
        if (err != 0 && err != ARGP_ERR_UNKNOWN) {
            return err;
        }
    }

    for (i = st.next; i < nArgc; i++) {
        char *sz = ppArgv[i];
        const struct argp_option *pOpt;
        int fMatched = 0;

        if (sz == NULL) {
            continue;
        }
        if (sz[0] != '-' || sz[1] == '\0') {
            st.arg_num++;
            if (pArgp != NULL && pArgp->parser != NULL) {
                err = pArgp->parser(0 /* ARGP_KEY_ARG */, sz, &st);
                if (err == ARGP_ERR_UNKNOWN) {
                    err = 0;
                }
                if (err != 0) {
                    break;
                }
            }
            continue;
        }
        if (sz[1] == '-' && sz[2] == '\0') {
            /* end of options */
            i++;
            break;
        }
        if (sz[1] == '-') {
            const char *szName = sz + 2;
            char *szEq = strchr(szName, '=');
            char aName[64];
            char *szArg = NULL;
            size_t nName;

            if (szEq != NULL) {
                nName = (size_t)(szEq - szName);
                if (nName >= sizeof(aName)) {
                    nName = sizeof(aName) - 1;
                }
                memcpy(aName, szName, nName);
                aName[nName] = '\0';
                szArg = szEq + 1;
                szName = aName;
            }
            if (pArgp != NULL && pArgp->options != NULL) {
                for (pOpt = pArgp->options; pOpt->name != NULL || pOpt->key != 0;
                     pOpt++) {
                    if (pOpt->name != NULL && strcmp(pOpt->name, szName) == 0) {
                        fMatched = 1;
                        if (pOpt->arg != NULL && szArg == NULL &&
                            i + 1 < nArgc) {
                            szArg = ppArgv[++i];
                        }
                        if (pArgp->parser != NULL) {
                            err = pArgp->parser(pOpt->key, szArg, &st);
                            if (err == ARGP_ERR_UNKNOWN) {
                                err = 0;
                            }
                        }
                        break;
                    }
                }
            }
            if (!fMatched && (uFlags & ARGP_NO_ERRS) == 0) {
                (void)fprintf(stderr, "%s: unrecognized option '--%s'\n",
                              st.name, szName);
                if ((uFlags & ARGP_NO_EXIT) == 0) {
                    _Exit(argp_err_exit_status != 0 ? argp_err_exit_status : 1);
                }
                return EINVAL;
            }
        } else {
            /* short options cluster */
            const char *q = sz + 1;
            while (*q != '\0') {
                int nKey = (unsigned char)*q++;
                char *szArg = NULL;

                fMatched = 0;
                if (pArgp != NULL && pArgp->options != NULL) {
                    for (pOpt = pArgp->options;
                         pOpt->name != NULL || pOpt->key != 0; pOpt++) {
                        if (pOpt->key == nKey) {
                            fMatched = 1;
                            if (pOpt->arg != NULL) {
                                if (*q != '\0') {
                                    szArg = (char *)q;
                                    q += strlen(q);
                                } else if (i + 1 < nArgc) {
                                    szArg = ppArgv[++i];
                                }
                            }
                            if (pArgp->parser != NULL) {
                                err = pArgp->parser(nKey, szArg, &st);
                                if (err == ARGP_ERR_UNKNOWN) {
                                    err = 0;
                                }
                            }
                            break;
                        }
                    }
                }
                if (!fMatched && (uFlags & ARGP_NO_ERRS) == 0) {
                    (void)fprintf(stderr, "%s: invalid option -- '%c'\n",
                                  st.name, nKey);
                    if ((uFlags & ARGP_NO_EXIT) == 0) {
                        _Exit(argp_err_exit_status != 0 ? argp_err_exit_status
                                                       : 1);
                    }
                    return EINVAL;
                }
            }
        }
    }

    if (pArgIndex != NULL) {
        *pArgIndex = i;
    }
    if (pArgp != NULL && pArgp->parser != NULL && err == 0) {
        err = pArgp->parser(0 /* ARGP_KEY_END shape */, NULL, &st);
        if (err == ARGP_ERR_UNKNOWN) {
            err = 0;
        }
    }
    return err;
}

/* ---- printf custom hooks (registration only; printf path unchanged) ----- */

static printf_function g_aPrintfFn[256];
static printf_arginfo_function g_aPrintfInfo[256];
static printf_arginfo_size_function g_aPrintfInfoSz[256];

int
register_printf_function(int nSpec, printf_function pfn,
                         printf_arginfo_function pfnInfo)
{
    if (nSpec < 0 || nSpec > 255) {
        errno = EINVAL;
        return -1;
    }
    g_aPrintfFn[nSpec] = pfn;
    g_aPrintfInfo[nSpec] = pfnInfo;
    return 0;
}

int
register_printf_specifier(int nSpec, printf_function pfn,
                          printf_arginfo_size_function pfnInfo)
{
    if (nSpec < 0 || nSpec > 255) {
        errno = EINVAL;
        return -1;
    }
    g_aPrintfFn[nSpec] = pfn;
    g_aPrintfInfoSz[nSpec] = pfnInfo;
    return 0;
}

int
register_printf_modifier(const wchar_t *pStr)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pStr == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)pStr;
    return -1; /* no free user slots in bring-up */
}

int
register_printf_type(printf_va_arg_function pfn)
{
    (void)pfn;
    return -1;
}

size_t
parse_printf_format(const char *szFmt, size_t n, int *pArgtypes)
{
    size_t nArgs = 0;
    const char *p;

    if (szFmt == NULL) {
        return 0;
    }
    p = szFmt;
    while (*p != '\0') {
        int nType = PA_INT;
        if (*p != '%') {
            p++;
            continue;
        }
        p++;
        if (*p == '%') {
            p++;
            continue;
        }
        while (*p == '-' || *p == '+' || *p == ' ' || *p == '#' || *p == '0' ||
               *p == '\'') {
            p++;
        }
        while ((*p >= '0' && *p <= '9') || *p == '*' || *p == '.') {
            if (*p == '*') {
                if (nArgs < n && pArgtypes != NULL) {
                    pArgtypes[nArgs] = PA_INT;
                }
                nArgs++;
            }
            p++;
        }
        if (*p == 'l') {
            p++;
            if (*p == 'l') {
                p++;
                nType = PA_INT | PA_FLAG_LONG_LONG;
            } else {
                nType = PA_INT | PA_FLAG_LONG;
            }
        } else if (*p == 'h') {
            p++;
            nType = PA_INT | PA_FLAG_SHORT;
        } else if (*p == 'z' || *p == 't' || *p == 'j') {
            p++;
            nType = PA_INT | PA_FLAG_LONG_LONG;
        }
        switch (*p) {
        case 's':
            nType = PA_STRING;
            break;
        case 'c':
            nType = PA_CHAR;
            break;
        case 'p':
            nType = PA_POINTER;
            break;
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
            nType = PA_DOUBLE;
            break;
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
            break;
        default:
            break;
        }
        if (*p != '\0') {
            p++;
        }
        if (nArgs < n && pArgtypes != NULL) {
            pArgtypes[nArgs] = nType;
        }
        nArgs++;
        (void)g_aPrintfFn;
        (void)g_aPrintfInfo;
        (void)g_aPrintfInfoSz;
    }
    return nArgs;
}

/* ---- pthread clocklocks / default attr / sigqueue ----------------------- */

static pthread_attr_t g_defaultAttr;
static int g_fDefaultAttr;

int
pthread_mutex_clocklock(pthread_mutex_t *pM, clockid_t clk,
                        const struct timespec *pAbs)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pM == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAbs == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)clk;
    return pthread_mutex_timedlock(pM, pAbs);
}

int
pthread_cond_clockwait(pthread_cond_t *pC, pthread_mutex_t *pM, clockid_t clk,
                       const struct timespec *pAbs)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pC == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pM == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAbs == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)clk;
    return pthread_cond_timedwait(pC, pM, pAbs);
}

int
pthread_rwlock_clockrdlock(pthread_rwlock_t *pRw, clockid_t clk,
                           const struct timespec *pAbs)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pRw == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAbs == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)clk;
    return pthread_rwlock_timedrdlock(pRw, pAbs);
}

int
pthread_rwlock_clockwrlock(pthread_rwlock_t *pRw, clockid_t clk,
                           const struct timespec *pAbs)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pRw == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAbs == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)clk;
    return pthread_rwlock_timedwrlock(pRw, pAbs);
}

int
pthread_getattr_default_np(pthread_attr_t *pA)
{
    if (pA == NULL) {
        return EINVAL;
    }
    if (!g_fDefaultAttr) {
        (void)pthread_attr_init(&g_defaultAttr);
        g_fDefaultAttr = 1;
    }
    *pA = g_defaultAttr;
    return 0;
}

int
pthread_setattr_default_np(const pthread_attr_t *pA)
{
    if (pA == NULL) {
        return EINVAL;
    }
    g_defaultAttr = *pA;
    g_fDefaultAttr = 1;
    return 0;
}

int
sigqueue(pid_t nPid, int nSig, const union sigval value)
{
    siginfo_t info;

    memset(&info, 0, sizeof(info));
    info.si_signo = nSig;
    info.si_code = SI_QUEUE;
    info.si_pid = getpid();
    info.si_uid = getuid();
    info.si_value = value;
    return (int)b14_sys_ret(b14_sys6(NR_rt_sigqueueinfo, (long)nPid, (long)nSig,
                                     (long)(uintptr_t)&info, 0, 0, 0));
}

int
pthread_sigqueue(pthread_t tid, int nSig, const union sigval value)
{
    /* Bring-up: treat pthread_t as tid/pid-shaped */
    return sigqueue((pid_t)tid, nSig, value);
}

/* ---- sysconf-shaped helpers --------------------------------------------- */

int
get_nprocs(void)
{
    long n = sysconf(_SC_NPROCESSORS_ONLN);

    return (n > 0) ? (int)n : 1;
}

int
get_nprocs_conf(void)
{
    long n = sysconf(_SC_NPROCESSORS_CONF);

    return (n > 0) ? (int)n : 1;
}

long
get_phys_pages(void)
{
    struct sysinfo si;
    int nPage;

    if (sysinfo(&si) != 0) {
        return -1;
    }
    nPage = getpagesize();
    if (nPage <= 0) {
        nPage = 4096;
    }
    if (si.mem_unit == 0) {
        si.mem_unit = 1;
    }
    return (long)((si.totalram * (unsigned long)si.mem_unit) /
                  (unsigned long)nPage);
}

long
get_avphys_pages(void)
{
    struct sysinfo si;
    int nPage;

    if (sysinfo(&si) != 0) {
        return -1;
    }
    nPage = getpagesize();
    if (nPage <= 0) {
        nPage = 4096;
    }
    if (si.mem_unit == 0) {
        si.mem_unit = 1;
    }
    return (long)((si.freeram * (unsigned long)si.mem_unit) /
                  (unsigned long)nPage);
}

/* ---- credentials / process helpers -------------------------------------- */

int
getpeereid(int nFd, uid_t *pUid, gid_t *pGid)
{
    struct ucred cr;
    socklen_t cb = (socklen_t)sizeof(cr);

    memset(&cr, 0, sizeof(cr));
    if (getsockopt(nFd, SOL_SOCKET, SO_PEERCRED, &cr, &cb) != 0) {
        return -1;
    }
    if (pUid != NULL) {
        *pUid = cr.uid;
    }
    if (pGid != NULL) {
        *pGid = cr.gid;
    }
    return 0;
}

int
killpg(int nPgrp, int nSig)
{
    if (nPgrp < 0) {
        errno = EINVAL;
        return -1;
    }
    return kill(-nPgrp, nSig);
}

int
lchmod(const char *szPath, mode_t mode)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    /* Linux has no lchmod syscall; fchmodat with AT_SYMLINK_NOFOLLOW when avail.
     * Bring-up: chmod (follows links) — better than ENOSYS for portable apps. */
    return chmod(szPath, mode);
}

int
malloc_trim(size_t cbPad)
{
    (void)cbPad;
    /* Freelist allocator has no sbrk shrink yet */
    return 0;
}

int
putpwent(const struct passwd *pPwd, FILE *pF)
{
    if (pPwd == NULL || pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (fprintf(pF, "%s:%s:%u:%u:%s:%s:%s\n",
                pPwd->pw_name != NULL ? pPwd->pw_name : "",
                pPwd->pw_passwd != NULL ? pPwd->pw_passwd : "",
                (unsigned)pPwd->pw_uid, (unsigned)pPwd->pw_gid,
                pPwd->pw_gecos != NULL ? pPwd->pw_gecos : "",
                pPwd->pw_dir != NULL ? pPwd->pw_dir : "",
                pPwd->pw_shell != NULL ? pPwd->pw_shell : "") < 0) {
        return -1;
    }
    return 0;
}

int
putgrent(const struct group *pGrp, FILE *pF)
{
    char **pp;

    if (pGrp == NULL || pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (fprintf(pF, "%s:%s:%u:",
                pGrp->gr_name != NULL ? pGrp->gr_name : "",
                pGrp->gr_passwd != NULL ? pGrp->gr_passwd : "",
                (unsigned)pGrp->gr_gid) < 0) {
        return -1;
    }
    if (pGrp->gr_mem != NULL) {
        for (pp = pGrp->gr_mem; *pp != NULL; pp++) {
            if (pp != pGrp->gr_mem) {
                if (fputc(',', pF) == EOF) {
                    return -1;
                }
            }
            if (fputs(*pp, pF) == EOF) {
                return -1;
            }
        }
    }
    if (fputc('\n', pF) == EOF) {
        return -1;
    }
    return 0;
}

/* ---- ttyent bring-up (/etc/ttys optional) -------------------------------- */

struct ttyent {
    char *ty_name;
    char *ty_getty;
    char *ty_type;
    int   ty_status;
    char *ty_window;
    char *ty_comment;
};

#define TTY_ON     0x01
#define TTY_SECURE 0x02

static struct ttyent g_tty;
static char g_aTyName[] = "console";
static char g_aTyGetty[] = "/sbin/getty";
static char g_aTyType[] = "unknown";
static int g_fTtyOpen;
static int g_fTtyEof;

int
setttyent(void)
{
    g_fTtyOpen = 1;
    g_fTtyEof = 0;
    return 1;
}

int
endttyent(void)
{
    g_fTtyOpen = 0;
    g_fTtyEof = 0;
    return 1;
}

struct ttyent *
getttyent(void)
{
    if (!g_fTtyOpen) {
        (void)setttyent();
    }
    if (g_fTtyEof) {
        return NULL;
    }
    g_tty.ty_name = g_aTyName;
    g_tty.ty_getty = g_aTyGetty;
    g_tty.ty_type = g_aTyType;
    g_tty.ty_status = TTY_ON | TTY_SECURE;
    g_tty.ty_window = NULL;
    g_tty.ty_comment = NULL;
    g_fTtyEof = 1;
    return &g_tty;
}

struct ttyent *
getttynam(const char *szName)
{
    struct ttyent *p;

    if (szName == NULL) {
        return NULL;
    }
    (void)setttyent();
    while ((p = getttyent()) != NULL) {
        if (strcmp(p->ty_name, szName) == 0) {
            (void)endttyent();
            return p;
        }
    }
    (void)endttyent();
    return NULL;
}

int
ttyslot(void)
{
    return 1; /* bring-up: console slot */
}

/* ---- time / stdio / dirent extras --------------------------------------- */

int
futimesat(int nDfd, const char *szPath, const struct timeval *aTv)
{
    struct timespec aTs[2];

    if (aTv != NULL) {
        aTs[0].tv_sec = aTv[0].tv_sec;
        aTs[0].tv_nsec = aTv[0].tv_usec * 1000L;
        aTs[1].tv_sec = aTv[1].tv_sec;
        aTs[1].tv_nsec = aTv[1].tv_usec * 1000L;
        return utimensat(nDfd, szPath, aTs, 0);
    }
    return utimensat(nDfd, szPath, NULL, 0);
}

int
fflush_unlocked(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fflush(pF);
}

int
versionsort64(const struct dirent **ppA, const struct dirent **ppB)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (ppA == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppB == NULL) {
        errno = EFAULT;
        return -1;
    }

    return versionsort(ppA, ppB);
}

/* ---- sysctl (obsolete; ENOSYS on modern Linux) -------------------------- */

struct __b14_sysctl_args {
    int    *name;
    int     nlen;
    void   *oldval;
    size_t *oldlenp;
    void   *newval;
    size_t  newlen;
};

int
sysctl(int *pName, int nLen, void *pOld, size_t *pcbOld, void *pNew,
       size_t cbNew)
{
    /* greppable: CGJ_GRAPH_BATCH14_SOFT_NULL */
    if (pName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pcbOld == NULL) {
        errno = EFAULT;
        return -1;
    }

    struct __b14_sysctl_args args;

    args.name = pName;
    args.nlen = nLen;
    args.oldval = pOld;
    args.oldlenp = pcbOld;
    args.newval = pNew;
    args.newlen = cbNew;
    return (int)b14_sys_ret(
        b14_sys6(NR_sysctl, (long)(uintptr_t)&args, 0, 0, 0, 0, 0));
}
