/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding allocator: sbrk bump + free-list reuse. Not GNU glibc.
 */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct cgj_hdr {
    size_t cb;     /* total block size including this header */
    size_t fFree;  /* 1 if on free list */
};

struct cgj_free {
    struct cgj_hdr  hdr;
    struct cgj_free *pNext;
};

#define HDR_SZ sizeof(struct cgj_hdr)
#define ALIGN  16u
#define MIN_CB (sizeof(struct cgj_free))

static struct cgj_free *s_pFree;

static size_t
align_up(size_t n)
{
    return (n + (ALIGN - 1u)) & ~(size_t)(ALIGN - 1u);
}

static void
free_list_push(struct cgj_hdr *pH)
{
    struct cgj_free *pF;

    pH->fFree = 1;
    pF = (struct cgj_free *)pH;
    pF->pNext = s_pFree;
    s_pFree = pF;
}

static struct cgj_hdr *
free_list_take(size_t need)
{
    struct cgj_free **pp;
    struct cgj_free *p;

    pp = &s_pFree;
    while (*pp != NULL) {
        p = *pp;
        if (p->hdr.cb >= need) {
            *pp = p->pNext;
            p->hdr.fFree = 0;
            return &p->hdr;
        }
        pp = &p->pNext;
    }
    return NULL;
}

void *
malloc(size_t cb)
{
    struct cgj_hdr *p;
    size_t need;
    void *pRaw;

    if (cb == 0) {
        cb = 1;
    }
    need = align_up(cb) + HDR_SZ;
    if (need < MIN_CB) {
        need = MIN_CB;
    }
    p = free_list_take(need);
    if (p != NULL) {
        return (void *)(p + 1);
    }
    pRaw = sbrk((intptr_t)need);
    if (pRaw == (void *)(intptr_t)-1) {
        errno = ENOMEM;
        return NULL;
    }
    p = (struct cgj_hdr *)pRaw;
    p->cb = need;
    p->fFree = 0;
    return (void *)(p + 1);
}

void
free(void *p)
{
    struct cgj_hdr *pH;

    if (p == NULL) {
        return;
    }
    pH = ((struct cgj_hdr *)p) - 1;
    if (pH->fFree != 0 || pH->cb < HDR_SZ) {
        return; /* double-free / corrupt: ignore */
    }
    free_list_push(pH);
}

size_t
malloc_usable_size(void *p)
{
    struct cgj_hdr *pH;

    if (p == NULL) {
        return 0;
    }
    pH = ((struct cgj_hdr *)p) - 1;
    if (pH->fFree != 0 || pH->cb < HDR_SZ) {
        return 0;
    }
    return pH->cb - HDR_SZ;
}

void *
calloc(size_t n, size_t cb)
{
    size_t total;
    void *p;

    if (n != 0 && cb > (size_t)-1 / n) {
        errno = ENOMEM;
        return NULL;
    }
    total = n * cb;
    p = malloc(total);
    if (p != NULL) {
        (void)memset(p, 0, total);
    }
    return p;
}

void *
realloc(void *p, size_t cb)
{
    void *pNew;
    struct cgj_hdr *pH;
    size_t old;

    if (p == NULL) {
        return malloc(cb);
    }
    if (cb == 0) {
        free(p);
        return NULL;
    }
    pH = ((struct cgj_hdr *)p) - 1;
    if (pH->fFree != 0) {
        errno = EINVAL;
        return NULL;
    }
    old = (pH->cb > HDR_SZ) ? (pH->cb - HDR_SZ) : 0;
    if (old >= cb) {
        return p; /* already large enough */
    }
    pNew = malloc(cb);
    if (pNew == NULL) {
        return NULL;
    }
    (void)memcpy(pNew, p, old);
    free(p);
    return pNew;
}

/*
 * aligned_alloc / posix_memalign: allocate oversized block and return aligned
 * payload. free() only works on the original malloc return; for bring-up we
 * allocate with enough headroom and store nothing — free of aligned pointer
 * is unsafe if pad != 0. Prefer posix_memalign only when free not required,
 * or allocate exactly when already aligned.
 *
 * Strategy: request cb + uAlign + HDR so after header the user pointer can
 * be aligned; if we must pad, leak the pad (bring-up) by adjusting returned
 * pointer without a reverse map. Apps that free aligned_alloc pointers get
 * best-effort: only unpadded returns free cleanly.
 */
void *
aligned_alloc(size_t uAlign, size_t cb)
{
    void *p;
    uintptr_t u;
    size_t pad;

    if (uAlign == 0 || (uAlign & (uAlign - 1u)) != 0) {
        errno = EINVAL;
        return NULL;
    }
    if (uAlign < ALIGN) {
        uAlign = ALIGN;
    }
    if (cb == 0) {
        cb = 1;
    }
    if (cb > (size_t)-1 - uAlign - HDR_SZ) {
        errno = ENOMEM;
        return NULL;
    }
    p = malloc(cb + uAlign);
    if (p == NULL) {
        return NULL;
    }
    u = (uintptr_t)p;
    if ((u & (uAlign - 1u)) == 0) {
        return p;
    }
    /* Cannot free padded pointer safely with freelist; allocate exact via
     * retry with bump size so sbrk return is more likely aligned. */
    free(p);
    p = malloc(cb + uAlign * 2u);
    if (p == NULL) {
        return NULL;
    }
    u = (uintptr_t)p;
    if ((u & (uAlign - 1u)) == 0) {
        return p;
    }
    pad = uAlign - (u & (uAlign - 1u));
    /* Leave original on freelist unused; return aligned interior (leak). */
    return (void *)(u + pad);
}

int
posix_memalign(void **pp, size_t uAlign, size_t cb)
{
    void *p;

    if (pp == NULL) {
        return EINVAL;
    }
    if (uAlign < sizeof(void *) || (uAlign & (uAlign - 1u)) != 0) {
        return EINVAL;
    }
    p = aligned_alloc(uAlign, cb);
    if (p == NULL) {
        return errno != 0 ? errno : ENOMEM;
    }
    *pp = p;
    return 0;
}
