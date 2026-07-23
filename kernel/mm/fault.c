/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Fault serialization + kernel-only map cookies
 * (SOLARIS_STYLE_REMAINING §7; CAP_ADDRESSING fault path).
 *
 * Cookies are single-use kernel secrets — never CNode caps (G-PTR / CAP).
 */
#include <gj/config.h>
#include <gj/fault.h>
#include <gj/timer.h>

/* Simple global cookie table until per-CPU / slab. */
#define GJ_COOKIE_TAB 64u

static struct gj_map_cookie g_aCookies[GJ_COOKIE_TAB];

/*
 * PRNG for cookies. Seed mixes compile-time salt with mono clock when ready;
 * product can later inject arch entropy (RDRAND) without API change.
 */
static u64 g_u64CookiePrng = 0xC0FFEEULL ^ 0x20260719ULL;
static int g_fCookiePrngSeeded;

static u64
cookie_next(void)
{
    /* xorshift64* */
    u64 u64X;

    if (!g_fCookiePrngSeeded) {
        if (timer_ready()) {
            u64 u64Mono = timer_mono_nsec();

            g_u64CookiePrng ^= u64Mono;
            g_u64CookiePrng ^= (u64Mono << 17) | (u64Mono >> 47);
        }
        g_fCookiePrngSeeded = 1;
    }

    u64X = g_u64CookiePrng;
    u64X ^= u64X >> 12;
    u64X ^= u64X << 25;
    u64X ^= u64X >> 27;
    g_u64CookiePrng = u64X;
    u64X *= 0x2545F4914F6CDD1DULL;
    /* Never publish an all-zero half (null cookie). */
    if (u64X == 0) {
        u64X = 1;
    }
    return u64X;
}

gj_status_t
gj_space_fault_enter(struct gj_space_fault *pF)
{
    u32 u32Expected;

    if (pF == NULL) {
        return GJ_ERR_INVAL;
    }

    u32Expected = 0;
    if (!__atomic_compare_exchange_n(&pF->u32FaultInProgress, &u32Expected, 1u,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        return GJ_ERR_BUSY;
    }
    return GJ_OK;
}

void
gj_space_fault_leave(struct gj_space_fault *pF)
{
    if (pF == NULL) {
        return;
    }
    __atomic_store_n(&pF->u32FaultInProgress, 0u, __ATOMIC_RELEASE);
    /* Full impl: wake waiters on fault CV */
}

gj_status_t
gj_map_cookie_create(struct gj_map_cookie *pOut, void *pSpace, void *pProc,
                     void *pThread, u64 u64ClusterBase, u32 u32NPages,
                     u32 u32Access, u64 u64DeadlineMono,
                     struct gj_fault_msg *pMsg)
{
    u32 iSlot;
    struct gj_map_cookie *pSlot;

    if (pOut == NULL || pMsg == NULL) {
        return GJ_ERR_INVAL;
    }
    if (u32NPages == 0 || u32NPages > GJ_FAULT_CLUSTER_MAX) {
        return GJ_ERR_INVAL;
    }
    if ((u64ClusterBase & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        return GJ_ERR_INVAL;
    }
    /* Access must include at least one of R/W/X. */
    if ((u32Access & (GJ_FAULT_ACCESS_R | GJ_FAULT_ACCESS_W |
                      GJ_FAULT_ACCESS_X)) == 0) {
        return GJ_ERR_INVAL;
    }

    pSlot = NULL;
    for (iSlot = 0; iSlot < GJ_COOKIE_TAB; iSlot++) {
        if (!g_aCookies[iSlot].u8Live) {
            pSlot = &g_aCookies[iSlot];
            break;
        }
    }
    if (pSlot == NULL) {
        return GJ_ERR_AGAIN;
    }

    pSlot->u64CookieLo = cookie_next();
    pSlot->u64CookieHi = cookie_next();
    /* Guarantee 128-bit non-null. */
    if (pSlot->u64CookieLo == 0) {
        pSlot->u64CookieLo = 1;
    }
    if (pSlot->u64CookieHi == 0) {
        pSlot->u64CookieHi = 1;
    }
    pSlot->u64ClusterBase = u64ClusterBase;
    pSlot->u32NPages = u32NPages;
    pSlot->u32Access = u32Access;
    pSlot->pSpace = pSpace;
    pSlot->pMemObj = NULL; /* set when region/object path is wired */
    pSlot->pProc = pProc;
    pSlot->pThread = pThread;
    pSlot->u64DeadlineMono = u64DeadlineMono;
    pSlot->u8Live = 1;
    pSlot->u8Used = 0;

    *pOut = *pSlot;

    pMsg->u64ClusterBase = u64ClusterBase;
    pMsg->u32NPages = u32NPages;
    pMsg->u32Access = u32Access;
    pMsg->u64CookieLo = pSlot->u64CookieLo;
    pMsg->u64CookieHi = pSlot->u64CookieHi;
    pMsg->u32Flags = 0;
    pMsg->u32Pad = 0;

    return GJ_OK;
}

gj_status_t
gj_map_cookie_consume(u64 u64CookieLo, u64 u64CookieHi, u64 u64ClusterBase,
                      u32 u32NPages, struct gj_map_cookie *pOutCopy)
{
    u32 iSlot;
    u64 u64Now;

    /* Null cookie never matches a live entry. */
    if (u64CookieLo == 0 && u64CookieHi == 0) {
        return GJ_ERR_INVAL;
    }

    u64Now = timer_ready() ? timer_mono_nsec() : 0;

    for (iSlot = 0; iSlot < GJ_COOKIE_TAB; iSlot++) {
        struct gj_map_cookie *pCookie = &g_aCookies[iSlot];

        if (!pCookie->u8Live) {
            continue;
        }
        if (pCookie->u64CookieLo != u64CookieLo ||
            pCookie->u64CookieHi != u64CookieHi) {
            continue;
        }
        if (pCookie->u64ClusterBase != u64ClusterBase ||
            pCookie->u32NPages != u32NPages) {
            return GJ_ERR_PERM; /* bound mismatch */
        }
        if (pCookie->u8Used) {
            return GJ_ERR_STALE_CAP; /* replay */
        }
        /* Optional mono deadline (0 = none); skip if clock not ready. */
        if (pCookie->u64DeadlineMono != 0 && u64Now != 0 &&
            u64Now > pCookie->u64DeadlineMono) {
            pCookie->u8Live = 0;
            pCookie->u8Used = 0;
            return GJ_ERR_TIMEOUT;
        }

        pCookie->u8Used = 1;
        pCookie->u8Live = 0; /* single-use: dead after consume */
        if (pOutCopy != NULL) {
            *pOutCopy = *pCookie;
        }
        return GJ_OK;
    }
    return GJ_ERR_NOENT; /* unknown / already invalidated */
}

void
gj_map_cookie_invalidate(u64 u64CookieLo, u64 u64CookieHi)
{
    u32 iSlot;

    if (u64CookieLo == 0 && u64CookieHi == 0) {
        return;
    }
    for (iSlot = 0; iSlot < GJ_COOKIE_TAB; iSlot++) {
        struct gj_map_cookie *pCookie = &g_aCookies[iSlot];

        if (pCookie->u8Live && pCookie->u64CookieLo == u64CookieLo &&
            pCookie->u64CookieHi == u64CookieHi) {
            pCookie->u8Live = 0;
            pCookie->u8Used = 0;
            return;
        }
    }
}
