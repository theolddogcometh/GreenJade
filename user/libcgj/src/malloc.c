/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding allocator: sbrk bump + free-list reuse. Not GNU glibc.
 * Soft deepen: freelist split, address-ordered coalesce, freeable
 * aligned_alloc interior headers, lightweight heap accounting.
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

/* Soft heap accounting for mallinfo / malloc_stats */
static size_t s_cbArena;   /* bytes obtained from sbrk */
static size_t s_cbUord;    /* bytes in live user payloads (excl. headers) */
static size_t s_cOrdblks;  /* free-list block count */

static size_t
align_up(size_t n)
{
    return (n + (ALIGN - 1u)) & ~(size_t)(ALIGN - 1u);
}

/* Address-ordered insert + adjacent coalesce (soft heap hygiene). */
static void
free_list_push(struct cgj_hdr *pH)
{
    struct cgj_free *pF;
    struct cgj_free **pp;
    struct cgj_free *pPrev;
    uintptr_t uThis;

    pH->fFree = 1;
    pF = (struct cgj_free *)pH;
    uThis = (uintptr_t)pH;
    pPrev = NULL;
    pp = &s_pFree;
    while (*pp != NULL && (uintptr_t)(*pp) < uThis) {
        pPrev = *pp;
        pp = &(*pp)->pNext;
    }
    pF->pNext = *pp;
    *pp = pF;
    s_cOrdblks++;

    /* Coalesce with next if adjacent. */
    if (pF->pNext != NULL &&
        (uintptr_t)pF + pF->hdr.cb == (uintptr_t)pF->pNext) {
        pF->hdr.cb += pF->pNext->hdr.cb;
        pF->pNext = pF->pNext->pNext;
        if (s_cOrdblks > 0) {
            s_cOrdblks--;
        }
    }

    /* Coalesce with previous if adjacent. */
    if (pPrev != NULL &&
        (uintptr_t)pPrev + pPrev->hdr.cb == (uintptr_t)pF) {
        pPrev->hdr.cb += pF->hdr.cb;
        pPrev->pNext = pF->pNext;
        if (s_cOrdblks > 0) {
            s_cOrdblks--;
        }
    }
}

static struct cgj_hdr *
free_list_take(size_t need)
{
    struct cgj_free **pp;
    struct cgj_free *p;
    size_t rem;

    pp = &s_pFree;
    while (*pp != NULL) {
        p = *pp;
        if (p->hdr.cb >= need) {
            rem = p->hdr.cb - need;
            /* Split when remainder can hold another free node. */
            if (rem >= MIN_CB + ALIGN) {
                struct cgj_free *pSplit;
                unsigned char *pBase = (unsigned char *)p;

                p->hdr.cb = need;
                pSplit = (struct cgj_free *)(void *)(pBase + need);
                pSplit->hdr.cb = rem;
                pSplit->hdr.fFree = 1;
                pSplit->pNext = p->pNext;
                *pp = pSplit;
                /* ordblks: removed p, added pSplit → count same */
            } else {
                *pp = p->pNext;
                if (s_cOrdblks > 0) {
                    s_cOrdblks--;
                }
            }
            p->hdr.fFree = 0;
            return &p->hdr;
        }
        pp = &p->pNext;
    }
    return NULL;
}

/* Soft query for mallinfo / malloc_stats (same TU linkage). */
void
_libcgj_heap_query(size_t *pcbArena, size_t *pcbUord, size_t *pcbFord,
                   size_t *pcOrdblks)
{
    struct cgj_free *p;
    size_t cbFord = 0;
    size_t cOrd = 0;

    for (p = s_pFree; p != NULL; p = p->pNext) {
        cbFord += p->hdr.cb;
        cOrd++;
    }
    s_cOrdblks = cOrd;
    if (pcbArena != NULL) {
        *pcbArena = s_cbArena;
    }
    if (pcbUord != NULL) {
        *pcbUord = s_cbUord;
    }
    if (pcbFord != NULL) {
        *pcbFord = cbFord;
    }
    if (pcOrdblks != NULL) {
        *pcOrdblks = cOrd;
    }
}

void *
malloc(size_t cb)
{
    struct cgj_hdr *p;
    size_t need;
    size_t cbUser;
    void *pRaw;

    if (cb == 0) {
        cb = 1;
    }
    cbUser = align_up(cb);
    need = cbUser + HDR_SZ;
    if (need < MIN_CB) {
        need = MIN_CB;
    }
    /* overflow guard */
    if (need < cbUser) {
        errno = ENOMEM;
        return NULL;
    }
    p = free_list_take(need);
    if (p != NULL) {
        s_cbUord += p->cb - HDR_SZ;
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
    s_cbArena += need;
    s_cbUord += need - HDR_SZ;
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
    if (s_cbUord >= pH->cb - HDR_SZ) {
        s_cbUord -= pH->cb - HDR_SZ;
    } else {
        s_cbUord = 0;
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
    size_t need;

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
    need = align_up(cb) + HDR_SZ;
    if (need < MIN_CB) {
        need = MIN_CB;
    }
    /* In-place grow only if next free block is adjacent (soft). */
    if (need > pH->cb) {
        struct cgj_free **pp = &s_pFree;
        uintptr_t uEnd = (uintptr_t)pH + pH->cb;

        while (*pp != NULL) {
            struct cgj_free *pF = *pp;

            if ((uintptr_t)pF == uEnd && pH->cb + pF->hdr.cb >= need) {
                size_t cbTake = pF->hdr.cb;
                size_t cbNew = pH->cb + cbTake;
                size_t rem = cbNew - need;

                *pp = pF->pNext;
                if (s_cOrdblks > 0) {
                    s_cOrdblks--;
                }
                if (rem >= MIN_CB + ALIGN) {
                    struct cgj_free *pSplit;
                    unsigned char *pBase = (unsigned char *)pH;

                    pH->cb = need;
                    pSplit = (struct cgj_free *)(void *)(pBase + need);
                    pSplit->hdr.cb = rem;
                    pSplit->hdr.fFree = 1;
                    free_list_push(&pSplit->hdr);
                } else {
                    pH->cb = cbNew;
                }
                s_cbUord += pH->cb - HDR_SZ - old;
                return p;
            }
            pp = &pF->pNext;
        }
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
 * aligned_alloc: place a real cgj_hdr immediately before the aligned user
 * pointer so free() works. Waste before that header is left unreclaimed
 * (soft; no free-list corruption).
 */
void *
aligned_alloc(size_t uAlign, size_t cb)
{
    size_t cbUser;
    size_t need;
    void *pRaw;
    uintptr_t uBase;
    uintptr_t uUser;
    struct cgj_hdr *pH;

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
    cbUser = align_up(cb);
    /* raw: pad + header + payload (pad ≤ uAlign-1 worst case) */
    if (cbUser > (size_t)-1 - HDR_SZ - uAlign) {
        errno = ENOMEM;
        return NULL;
    }
    need = cbUser + HDR_SZ + uAlign;
    /* Prefer freelist when naturally large enough and aligned enough. */
    {
        struct cgj_hdr *pTry = free_list_take(need);

        if (pTry != NULL) {
            uBase = (uintptr_t)pTry;
            uUser = (uBase + HDR_SZ + (uAlign - 1u)) & ~(uintptr_t)(uAlign - 1u);
            if (uUser + cbUser <= uBase + pTry->cb &&
                uUser >= uBase + HDR_SZ) {
                pH = (struct cgj_hdr *)(uUser - HDR_SZ);
                if (pH != pTry) {
                    /* Interior header: claim remainder of original block. */
                    pH->cb = (uBase + pTry->cb) - (uintptr_t)pH;
                    pH->fFree = 0;
                    /* Original pTry region before pH is abandoned soft waste. */
                } else {
                    pH = pTry;
                    pH->fFree = 0;
                }
                s_cbUord += pH->cb - HDR_SZ;
                return (void *)uUser;
            }
            /* Alignment fit failed: put block back and fall through. */
            free_list_push(pTry);
        }
    }
    pRaw = sbrk((intptr_t)need);
    if (pRaw == (void *)(intptr_t)-1) {
        errno = ENOMEM;
        return NULL;
    }
    s_cbArena += need;
    uBase = (uintptr_t)pRaw;
    uUser = (uBase + HDR_SZ + (uAlign - 1u)) & ~(uintptr_t)(uAlign - 1u);
    if (uUser + cbUser > uBase + need) {
        /* Should not happen with need formula; soft fail-safe. */
        errno = ENOMEM;
        return NULL;
    }
    pH = (struct cgj_hdr *)(uUser - HDR_SZ);
    pH->cb = (uBase + need) - (uintptr_t)pH;
    pH->fFree = 0;
    s_cbUord += pH->cb - HDR_SZ;
    return (void *)uUser;
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
