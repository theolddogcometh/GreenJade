/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Fault serialization + kernel-only map cookies + soft cluster coalesce
 * (SOLARIS_STYLE_REMAINING §7; CAP_ADDRESSING fault path).
 *
 * Cookies are single-use kernel secrets — never CNode caps (G-PTR / CAP).
 *
 * greppable: FAULT_MAP_COOKIE
 * greppable: FAULT_SERIALIZATION
 * greppable: FAULT_SERIALIZATION_STATS
 * greppable: FAULT_CLUSTER_COALESCE_SOFT
 * greppable: GJ_FAULT_CLUSTER_MAX
 */
#include <gj/config.h>
#include <gj/fault.h>
#include <gj/timer.h>

/* Simple global cookie table until per-CPU / slab. */
#define GJ_COOKIE_TAB 64u

static struct gj_map_cookie g_aCookies[GJ_COOKIE_TAB];

/* greppable: FAULT_SERIALIZATION_STATS */
static struct gj_fault_stats g_faultStats;

/*
 * PRNG for cookies. Seed mixes compile-time salt with mono clock when ready;
 * product can later inject arch entropy (RDRAND) without API change.
 * greppable: FAULT_MAP_COOKIE
 */
static u64 g_u64CookiePrng = 0xC0FFEEULL ^ 0x20260719ULL;
static int g_fCookiePrngSeeded;

static void
fault_stat_add(u64 *pu64Field, u64 u64Delta)
{
    if (pu64Field == NULL || u64Delta == 0) {
        return;
    }
    (void)__atomic_fetch_add(pu64Field, u64Delta, __ATOMIC_RELAXED);
}

static void
fault_stat_inc(u64 *pu64Field)
{
    fault_stat_add(pu64Field, 1ull);
}

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

/*
 * True if soft present-probe says the page is present (stop coalesce).
 * NULL probe ⇒ soft not-present (coalesce freely within caps).
 */
static int
cluster_page_present_soft(gj_fault_page_present_fn pfnPresent, void *pCtx,
                          u64 u64PageVa)
{
    if (pfnPresent == NULL) {
        return 0;
    }
    return pfnPresent(pCtx, u64PageVa) != 0 ? 1 : 0;
}

/*
 * Soft multi-page fault cluster coalesce.
 * greppable: FAULT_CLUSTER_COALESCE_SOFT
 * greppable: GJ_FAULT_CLUSTER_MAX
 */
gj_status_t
gj_fault_cluster_coalesce_soft(u64 u64FaultVa, u64 u64RegionLo,
                               u64 u64RegionHi, u32 u32Access,
                               gj_fault_page_present_fn pfnPresent,
                               void *pPresentCtx, u64 *pu64ClusterBase,
                               u32 *pu32NPages)
{
    u64 u64PageMask;
    u64 u64FaultPage;
    u64 u64Base;
    u64 u64End; /* exclusive end VA of cluster */
    u32 u32N;
    u32 u32CapHit;
    u32 u32PresentStop;
    int fHaveRegion;

    fault_stat_inc(&g_faultStats.u64ClusterSoftCalls);

    if (pu64ClusterBase == NULL || pu32NPages == NULL) {
        return GJ_ERR_INVAL;
    }
    /* Access must include at least one of R/W/X (same policy as cookie). */
    if ((u32Access & (GJ_FAULT_ACCESS_R | GJ_FAULT_ACCESS_W |
                      GJ_FAULT_ACCESS_X)) == 0) {
        return GJ_ERR_INVAL;
    }

    u64PageMask = (u64)GJ_PAGE_SIZE - 1ull;
    u64FaultPage = u64FaultVa & ~u64PageMask;

    fHaveRegion = (u64RegionLo != 0 || u64RegionHi != 0) ? 1 : 0;
    if (fHaveRegion) {
        if (u64RegionHi <= u64RegionLo) {
            return GJ_ERR_INVAL;
        }
        /* Region bounds should be page-aligned for clean clusters. */
        if ((u64RegionLo & u64PageMask) != 0 ||
            (u64RegionHi & u64PageMask) != 0) {
            return GJ_ERR_INVAL;
        }
        if (u64FaultPage < u64RegionLo || u64FaultPage >= u64RegionHi) {
            return GJ_ERR_INVAL;
        }
    }

    /*
     * Soft coalesce (C1): start at fault page, expand backward then forward
     * while pages are not present (or soft-assumed not present), stay inside
     * region, and never exceed GJ_FAULT_CLUSTER_MAX (C2 DoS bound).
     *
     * greppable: FAULT_CLUSTER_COALESCE_SOFT
     */
    u64Base = u64FaultPage;
    u64End = u64FaultPage + (u64)GJ_PAGE_SIZE;
    u32N = 1u;
    u32CapHit = 0;
    u32PresentStop = 0;

    /* Expand backward (lower addresses). */
    while (u32N < GJ_FAULT_CLUSTER_MAX) {
        u64 u64Prev;

        if (u64Base < (u64)GJ_PAGE_SIZE) {
            break; /* underflow guard */
        }
        u64Prev = u64Base - (u64)GJ_PAGE_SIZE;
        if (fHaveRegion && u64Prev < u64RegionLo) {
            break;
        }
        if (cluster_page_present_soft(pfnPresent, pPresentCtx, u64Prev)) {
            u32PresentStop = 1;
            break;
        }
        u64Base = u64Prev;
        u32N++;
    }
    if (u32N >= GJ_FAULT_CLUSTER_MAX) {
        u32CapHit = 1;
    }

    /* Expand forward (higher addresses) until cap. */
    while (u32N < GJ_FAULT_CLUSTER_MAX) {
        u64 u64Next = u64End;

        if (fHaveRegion && u64Next >= u64RegionHi) {
            break;
        }
        /* Guard wrap of end VA. */
        if (u64Next + (u64)GJ_PAGE_SIZE < u64Next) {
            break;
        }
        if (cluster_page_present_soft(pfnPresent, pPresentCtx, u64Next)) {
            u32PresentStop = 1;
            break;
        }
        u64End = u64Next + (u64)GJ_PAGE_SIZE;
        u32N++;
    }
    if (u32N >= GJ_FAULT_CLUSTER_MAX) {
        u32CapHit = 1;
    }

    /* Defend invariant: 1 .. GJ_FAULT_CLUSTER_MAX */
    if (u32N == 0 || u32N > GJ_FAULT_CLUSTER_MAX) {
        return GJ_ERR_INVAL;
    }

    *pu64ClusterBase = u64Base;
    *pu32NPages = u32N;

    fault_stat_add(&g_faultStats.u64ClusterSoftPages, (u64)u32N);
    if (u32N > 1u) {
        /* greppable: FAULT_CLUSTER_COALESCE_SOFT expand */
        fault_stat_inc(&g_faultStats.u64ClusterSoftExpand);
    }
    if (u32CapHit) {
        /* greppable: GJ_FAULT_CLUSTER_MAX cap hit */
        fault_stat_inc(&g_faultStats.u64ClusterSoftCapHit);
    }
    if (u32PresentStop) {
        fault_stat_inc(&g_faultStats.u64ClusterSoftPresent);
    }

    return GJ_OK;
}

/*
 * One lock per address space (L1–L4).
 * greppable: FAULT_SERIALIZATION
 */
gj_status_t
gj_space_fault_enter(struct gj_space_fault *pF)
{
    u32 u32Expected;

    if (pF == NULL) {
        fault_stat_inc(&g_faultStats.u64EnterInval);
        return GJ_ERR_INVAL;
    }

    u32Expected = 0;
    if (!__atomic_compare_exchange_n(&pF->u32FaultInProgress, &u32Expected, 1u,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        /*
         * Concurrent fault in same space — serialize (L3). Soft path: bump
         * waiters + return BUSY. Full impl: sleep on CV with mono timeout.
         * greppable: FAULT_SERIALIZATION busy
         */
        (void)__atomic_fetch_add(&pF->u32Waiters, 1u, __ATOMIC_RELAXED);
        fault_stat_inc(&g_faultStats.u64EnterBusy);
        fault_stat_inc(&g_faultStats.u64WaiterBump);
        return GJ_ERR_BUSY;
    }
    fault_stat_inc(&g_faultStats.u64EnterOk);
    return GJ_OK;
}

void
gj_space_fault_leave(struct gj_space_fault *pF)
{
    if (pF == NULL) {
        return;
    }
    __atomic_store_n(&pF->u32FaultInProgress, 0u, __ATOMIC_RELEASE);
    /* Full impl: wake waiters on fault CV. greppable: FAULT_SERIALIZATION leave */
    fault_stat_inc(&g_faultStats.u64Leave);
}

/*
 * greppable: FAULT_MAP_COOKIE
 */
gj_status_t
gj_map_cookie_create(struct gj_map_cookie *pOut, void *pSpace, void *pProc,
                     void *pThread, u64 u64ClusterBase, u32 u32NPages,
                     u32 u32Access, u64 u64DeadlineMono,
                     struct gj_fault_msg *pMsg)
{
    u32 iSlot;
    struct gj_map_cookie *pSlot;

    if (pOut == NULL || pMsg == NULL) {
        fault_stat_inc(&g_faultStats.u64CookieCreateFail);
        return GJ_ERR_INVAL;
    }
    /* greppable: GJ_FAULT_CLUSTER_MAX enforce on cookie mint */
    if (u32NPages == 0 || u32NPages > GJ_FAULT_CLUSTER_MAX) {
        fault_stat_inc(&g_faultStats.u64CookieCreateFail);
        return GJ_ERR_INVAL;
    }
    if ((u64ClusterBase & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        fault_stat_inc(&g_faultStats.u64CookieCreateFail);
        return GJ_ERR_INVAL;
    }
    /* Access must include at least one of R/W/X. */
    if ((u32Access & (GJ_FAULT_ACCESS_R | GJ_FAULT_ACCESS_W |
                      GJ_FAULT_ACCESS_X)) == 0) {
        fault_stat_inc(&g_faultStats.u64CookieCreateFail);
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
        fault_stat_inc(&g_faultStats.u64CookieCreateFail);
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
    pSlot->pMemObj = NULL; /* set via bind_memobj_soft or map path */
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

    fault_stat_inc(&g_faultStats.u64CookieCreateOk);
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
        fault_stat_inc(&g_faultStats.u64CookieConsumeFail);
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
            /* greppable: FAULT_MAP_COOKIE bound mismatch */
            fault_stat_inc(&g_faultStats.u64CookieConsumeFail);
            return GJ_ERR_PERM; /* bound mismatch */
        }
        if (pCookie->u8Used) {
            /* greppable: FAULT_MAP_COOKIE replay */
            fault_stat_inc(&g_faultStats.u64CookieReplay);
            fault_stat_inc(&g_faultStats.u64CookieConsumeFail);
            return GJ_ERR_STALE_CAP; /* replay */
        }
        /* Optional mono deadline (0 = none); skip if clock not ready. */
        if (pCookie->u64DeadlineMono != 0 && u64Now != 0 &&
            u64Now > pCookie->u64DeadlineMono) {
            pCookie->u8Live = 0;
            pCookie->u8Used = 0;
            /* greppable: FAULT_MAP_COOKIE timeout */
            fault_stat_inc(&g_faultStats.u64CookieTimeout);
            fault_stat_inc(&g_faultStats.u64CookieConsumeFail);
            return GJ_ERR_TIMEOUT;
        }

        pCookie->u8Used = 1;
        pCookie->u8Live = 0; /* single-use: dead after consume */
        if (pOutCopy != NULL) {
            *pOutCopy = *pCookie;
        }
        fault_stat_inc(&g_faultStats.u64CookieConsumeOk);
        return GJ_OK;
    }
    fault_stat_inc(&g_faultStats.u64CookieConsumeFail);
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
            /* greppable: FAULT_MAP_COOKIE invalidate */
            fault_stat_inc(&g_faultStats.u64CookieInvalidate);
            return;
        }
    }
}

/*
 * Soft bind of memory-object pointer on a live cookie (Apple §2 prep).
 * greppable: FAULT_MAP_COOKIE_MEMOBJ_SOFT
 */
gj_status_t
gj_map_cookie_bind_memobj_soft(u64 u64CookieLo, u64 u64CookieHi, void *pMemObj)
{
    u32 iSlot;

    if (u64CookieLo == 0 && u64CookieHi == 0) {
        return GJ_ERR_INVAL;
    }
    for (iSlot = 0; iSlot < GJ_COOKIE_TAB; iSlot++) {
        struct gj_map_cookie *pCookie = &g_aCookies[iSlot];

        if (pCookie->u8Live && pCookie->u64CookieLo == u64CookieLo &&
            pCookie->u64CookieHi == u64CookieHi) {
            pCookie->pMemObj = pMemObj;
            return GJ_OK;
        }
    }
    return GJ_ERR_NOENT;
}

u32
gj_map_cookie_live_count(void)
{
    u32 iSlot;
    u32 u32Live;

    u32Live = 0;
    for (iSlot = 0; iSlot < GJ_COOKIE_TAB; iSlot++) {
        if (g_aCookies[iSlot].u8Live) {
            u32Live++;
        }
    }
    return u32Live;
}

/*
 * greppable: FAULT_SERIALIZATION_STATS
 */
void
gj_fault_stats_get(struct gj_fault_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    /*
     * Relaxed snapshot — counters are independent; no need for a single
     * consistent total under v1 soft stats.
     */
    pOut->u64EnterOk =
        __atomic_load_n(&g_faultStats.u64EnterOk, __ATOMIC_RELAXED);
    pOut->u64EnterBusy =
        __atomic_load_n(&g_faultStats.u64EnterBusy, __ATOMIC_RELAXED);
    pOut->u64EnterInval =
        __atomic_load_n(&g_faultStats.u64EnterInval, __ATOMIC_RELAXED);
    pOut->u64Leave =
        __atomic_load_n(&g_faultStats.u64Leave, __ATOMIC_RELAXED);
    pOut->u64WaiterBump =
        __atomic_load_n(&g_faultStats.u64WaiterBump, __ATOMIC_RELAXED);
    pOut->u64CookieCreateOk =
        __atomic_load_n(&g_faultStats.u64CookieCreateOk, __ATOMIC_RELAXED);
    pOut->u64CookieCreateFail =
        __atomic_load_n(&g_faultStats.u64CookieCreateFail, __ATOMIC_RELAXED);
    pOut->u64CookieConsumeOk =
        __atomic_load_n(&g_faultStats.u64CookieConsumeOk, __ATOMIC_RELAXED);
    pOut->u64CookieConsumeFail =
        __atomic_load_n(&g_faultStats.u64CookieConsumeFail, __ATOMIC_RELAXED);
    pOut->u64CookieInvalidate =
        __atomic_load_n(&g_faultStats.u64CookieInvalidate, __ATOMIC_RELAXED);
    pOut->u64CookieTimeout =
        __atomic_load_n(&g_faultStats.u64CookieTimeout, __ATOMIC_RELAXED);
    pOut->u64CookieReplay =
        __atomic_load_n(&g_faultStats.u64CookieReplay, __ATOMIC_RELAXED);
    pOut->u64ClusterSoftCalls =
        __atomic_load_n(&g_faultStats.u64ClusterSoftCalls, __ATOMIC_RELAXED);
    pOut->u64ClusterSoftExpand =
        __atomic_load_n(&g_faultStats.u64ClusterSoftExpand, __ATOMIC_RELAXED);
    pOut->u64ClusterSoftPages =
        __atomic_load_n(&g_faultStats.u64ClusterSoftPages, __ATOMIC_RELAXED);
    pOut->u64ClusterSoftCapHit =
        __atomic_load_n(&g_faultStats.u64ClusterSoftCapHit, __ATOMIC_RELAXED);
    pOut->u64ClusterSoftPresent =
        __atomic_load_n(&g_faultStats.u64ClusterSoftPresent, __ATOMIC_RELAXED);
}

void
gj_fault_stats_reset(void)
{
    __atomic_store_n(&g_faultStats.u64EnterOk, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64EnterBusy, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64EnterInval, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64Leave, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64WaiterBump, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64CookieCreateOk, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64CookieCreateFail, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64CookieConsumeOk, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64CookieConsumeFail, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64CookieInvalidate, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64CookieTimeout, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64CookieReplay, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64ClusterSoftCalls, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64ClusterSoftExpand, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64ClusterSoftPages, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64ClusterSoftCapHit, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_faultStats.u64ClusterSoftPresent, 0, __ATOMIC_RELAXED);
}
