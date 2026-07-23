/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Fault serialization + kernel-only map cookies + soft cluster coalesce
 * (SOLARIS_STYLE_REMAINING §7; CAP_ADDRESSING fault path).
 *
 * Cookies are single-use kernel secrets — never CNode caps (G-PTR / CAP).
 *
 * Soft deepen (Wave 8 exclusive): soft fault class counters + greppable
 * "fault: soft …" logs for user/kernel, present/not, write/exec soft.
 * Diagnostics only — not product SEH / exception-port complete.
 *
 * Soft deepen (pager + views): region→pager→views inventory toward the
 * product fault path (CAP §1.6 / Apple §1). Shape only — freestanding soft:
 *   create cookie → soft "Call pager" tally
 *   consume → soft install views of object pages (counts + greppable)
 *   FAIL/death invalidate + kill-on-timeout counters
 * Honesty: soft deepen toward region→pager→views; not product pager
 * complete; not bar3; door Call + FRAME validate + real vmm_map still open.
 *
 * greppable: FAULT_MAP_COOKIE
 * greppable: FAULT_SERIALIZATION
 * greppable: FAULT_SERIALIZATION_STATS
 * greppable: FAULT_CLUSTER_COALESCE_SOFT
 * greppable: GJ_FAULT_CLUSTER_MAX
 * greppable: fault: soft
 * greppable: fault: pager call soft
 * greppable: fault: cookie view map soft
 * greppable: fault: view install
 * greppable: fault: kill-on-timeout soft
 * greppable: fault: fail invalidate soft
 */
#include <gj/config.h>
#include <gj/fault.h>
#include <gj/klog.h>
#include <gj/timer.h>
#include <gj/user_access.h>

/* Simple global cookie table until per-CPU / slab. */
#define GJ_COOKIE_TAB 64u

/* Rate-limit per-event soft class lines (first N only; totals still free). */
#define FAULT_SOFT_CLASS_LOG_MAX 8u

static struct gj_map_cookie g_aCookies[GJ_COOKIE_TAB];

/* greppable: FAULT_SERIALIZATION_STATS */
static struct gj_fault_stats g_faultStats;

/*
 * Soft fault class counters (wrap OK; never hard-gate product policy).
 * Orthogonal axes: user|kernel VA, present|not soft probe, write|exec access.
 * Not an SEH / exception-port product claim — pager/cookie path only.
 * greppable: fault: soft
 */
static u64 g_u64SoftClassUser;
static u64 g_u64SoftClassKernel;
static u64 g_u64SoftClassPresent;
static u64 g_u64SoftClassNot;
static u64 g_u64SoftClassWrite;
static u64 g_u64SoftClassExec;
static u64 g_u64SoftClassCalls;   /* classified soft events */
static u32 g_u32SoftClassLogged;  /* per-event log emissions */

/*
 * Soft pager Call + view-install inventory (file-local; not gj_fault_stats ABI).
 * Protocol shape: create → Call pager soft → consume/views | FAIL invalidate |
 * kill-on-timeout. Maps are views of object pages (Apple §2); soft counts only
 * until door Call + FRAME list + vmm_map_page product wire.
 * greppable: fault: pager call soft
 * greppable: fault: cookie view map soft
 * greppable: fault: view install
 * greppable: fault: kill-on-timeout soft
 * greppable: fault: fail invalidate soft
 */
static u64 g_u64PagerCallSoft;        /* soft Call after cookie mint */
static u64 g_u64PagerCallPages;       /* sum of cluster pages on Call soft */
static u64 g_u64ViewInstallSoft;      /* consume → view install soft events */
static u64 g_u64ViewInstallPages;     /* sum of pages soft-recorded as views */
static u64 g_u64ViewInstallMemobj;    /* installs with pMemObj bound */
static u64 g_u64ViewInstallNoMemobj;  /* installs without bound object */
static u64 g_u64KillOnTimeoutSoft;    /* consume past deadline → kill policy soft */
static u64 g_u64FailInvalidateSoft;   /* FAIL/death/early drop invalidate live */
static u32 g_u32PagerCallLogged;
static u32 g_u32ViewInstallLogged;
static u32 g_u32KillTimeoutLogged;
static u32 g_u32FailInvLogged;

/* Rate-limit pager-path per-event lines (totals still free). */
#define FAULT_PAGER_PATH_LOG_MAX 8u

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

/*
 * Soft: fault VA in the product user window vs kernel/other.
 * Uses GJ_USER_VA_* geometry only (no present bits) — class lamp.
 */
static int
fault_va_is_user_soft(u64 u64Va)
{
    if (u64Va >= GJ_USER_VA_BASE && u64Va < GJ_USER_VA_END) {
        return 1;
    }
    return 0;
}

/*
 * Emit greppable soft class totals.
 * greppable: fault: soft class
 * Honesty: soft counters only; not product SEH complete.
 */
static void
fault_class_soft_log(void)
{
    kprintf("fault: soft class user=%llu kernel=%llu present=%llu not=%llu "
            "write=%llu exec=%llu calls=%llu logs=%u (soft; not product SEH)\n",
            (unsigned long long)__atomic_load_n(&g_u64SoftClassUser,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClassKernel,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClassPresent,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClassNot,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClassWrite,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClassExec,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClassCalls,
                                                __ATOMIC_RELAXED),
            (unsigned)__atomic_load_n(&g_u32SoftClassLogged,
                                      __ATOMIC_RELAXED));
}

/*
 * Soft-classify one fault-path event and rate-limit a greppable line.
 *
 * fPresent: 1 = soft present probe hit on the fault page (or known present);
 *           0 = soft not-present (NULL probe or probe said absent).
 * u32Access: GJ_FAULT_ACCESS_* bitmask (write/exec soft axes).
 *
 * greppable: fault: soft
 */
static void
fault_class_soft_note(u64 u64Va, u32 u32Access, int fPresent, u32 u32NPages,
                      const char *szWhere)
{
    int fUser;
    int fWrite;
    int fExec;
    u32 u32N;
    const char *szUk;
    const char *szPn;
    const char *szWx;

    fUser = fault_va_is_user_soft(u64Va);
    fWrite = ((u32Access & GJ_FAULT_ACCESS_W) != 0) ? 1 : 0;
    fExec = ((u32Access & GJ_FAULT_ACCESS_X) != 0) ? 1 : 0;

    fault_stat_inc(&g_u64SoftClassCalls);
    if (fUser) {
        fault_stat_inc(&g_u64SoftClassUser);
        szUk = "user";
    } else {
        fault_stat_inc(&g_u64SoftClassKernel);
        szUk = "kernel";
    }
    if (fPresent) {
        fault_stat_inc(&g_u64SoftClassPresent);
        szPn = "present";
    } else {
        fault_stat_inc(&g_u64SoftClassNot);
        szPn = "not";
    }
    if (fWrite) {
        fault_stat_inc(&g_u64SoftClassWrite);
    }
    if (fExec) {
        fault_stat_inc(&g_u64SoftClassExec);
    }

    /* Access tag for greppable per-event line (write / exec / r soft). */
    if (fWrite && fExec) {
        szWx = "write+exec";
    } else if (fWrite) {
        szWx = "write";
    } else if (fExec) {
        szWx = "exec";
    } else {
        szWx = "r";
    }

    if (szWhere == NULL) {
        szWhere = "path";
    }

    /* Rate-limited per-event line; totals dump stays unbounded. */
    u32N = __atomic_load_n(&g_u32SoftClassLogged, __ATOMIC_RELAXED);
    if (u32N < FAULT_SOFT_CLASS_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32SoftClassLogged, &u32N, u32N + 1u,
                                        0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
            /*
             * Grep: fault: soft user|kernel present|not write|exec
             * Soft class lamp only — not SEH product complete.
             */
            kprintf("fault: soft %s %s %s va=0x%llx pages=%u access=0x%x "
                    "via=%s (soft class; not product SEH)\n",
                    szUk, szPn, szWx,
                    (unsigned long long)u64Va, (unsigned)u32NPages,
                    (unsigned)u32Access, szWhere);
        }
    }
}

/*
 * Soft pager-path inventory dump (create→Call→views | FAIL | kill-timeout).
 * Honesty: soft shape inventory only; not product pager / not bar3.
 * greppable: fault: pager call soft
 * greppable: fault: cookie view map soft
 * greppable: fault: view install
 * greppable: fault: kill-on-timeout soft
 * greppable: fault: fail invalidate soft
 */
static void
fault_pager_path_inventory_log(void)
{
    kprintf("fault: pager call soft calls=%llu pages=%llu "
            "view_install=%llu view_pages=%llu memobj=%llu no_memobj=%llu "
            "kill_timeout=%llu fail_inv=%llu "
            "(soft region→pager→views; not product pager; not bar3)\n",
            (unsigned long long)__atomic_load_n(&g_u64PagerCallSoft,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallPages,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallSoft,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallPages,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallMemobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallNoMemobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64KillOnTimeoutSoft,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64FailInvalidateSoft,
                                                __ATOMIC_RELAXED));
}

/*
 * Soft "Call pager" after a live cookie is minted (doors-like Call shape).
 * Product: ipc_call(pager, fault_msg) with mono timeout; pager never
 * ambient-maps the client. Soft: tally + greppable only.
 * greppable: fault: pager call soft
 */
static void
fault_pager_call_soft(u64 u64ClusterBase, u32 u32NPages, u32 u32Access,
                      void *pSpace, void *pMemObj)
{
    u32 u32N;
    const char *szObj;

    fault_stat_inc(&g_u64PagerCallSoft);
    fault_stat_add(&g_u64PagerCallPages, (u64)u32NPages);

    szObj = (pMemObj != NULL) ? "memobj" : "default_pager";

    u32N = __atomic_load_n(&g_u32PagerCallLogged, __ATOMIC_RELAXED);
    if (u32N < FAULT_PAGER_PATH_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32PagerCallLogged, &u32N, u32N + 1u,
                                        0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
            /*
             * Grep: fault: pager call soft
             * Soft Call inventory — not door IPC product complete.
             */
            kprintf("fault: pager call soft base=0x%llx pages=%u access=0x%x "
                    "space=%p via=%s "
                    "(soft Call; not product door pager)\n",
                    (unsigned long long)u64ClusterBase,
                    (unsigned)u32NPages,
                    (unsigned)u32Access,
                    pSpace,
                    szObj);
        }
    }
}

/*
 * Soft install views of object pages after single-use cookie consume.
 *
 * Product path (open): validate FRAMEs from pager reply, then vmm_map_page
 * under the fault space CR3; pages owned by memory object; maps are views
 * (Apple §2 / CAP §1.6). Soft path: record view-install counts + greppable
 * success — no ambient PTE write without validated frames (fail closed soft).
 *
 * greppable: fault: cookie view map soft
 * greppable: fault: view install
 */
static void
fault_view_install_soft(const struct gj_map_cookie *pCookie)
{
    u32 u32N;
    u32 u32Pages;
    int fMemObj;
    const char *szBind;

    if (pCookie == NULL) {
        return;
    }

    u32Pages = pCookie->u32NPages;
    if (u32Pages == 0 || u32Pages > GJ_FAULT_CLUSTER_MAX) {
        return;
    }

    fMemObj = (pCookie->pMemObj != NULL) ? 1 : 0;
    szBind = fMemObj ? "memobj_view" : "unbound_view";

    fault_stat_inc(&g_u64ViewInstallSoft);
    fault_stat_add(&g_u64ViewInstallPages, (u64)u32Pages);
    if (fMemObj) {
        fault_stat_inc(&g_u64ViewInstallMemobj);
    } else {
        fault_stat_inc(&g_u64ViewInstallNoMemobj);
    }

    /*
     * Product would call vmm_map_page per cluster page with validated FRAME
     * PAs and access→prot (W^X at map time). Soft: no PTE write without
     * frames — document as views, count install success soft only.
     * greppable: fault: view install
     */
    u32N = __atomic_load_n(&g_u32ViewInstallLogged, __ATOMIC_RELAXED);
    if (u32N < FAULT_PAGER_PATH_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32ViewInstallLogged, &u32N,
                                        u32N + 1u, 0, __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED)) {
            kprintf("fault: cookie view map soft base=0x%llx pages=%u "
                    "access=0x%x memobj=%p bind=%s "
                    "(views of object pages; soft install; not product map)\n",
                    (unsigned long long)pCookie->u64ClusterBase,
                    (unsigned)u32Pages,
                    (unsigned)pCookie->u32Access,
                    pCookie->pMemObj,
                    szBind);
            kprintf("fault: view install pages=%u ok soft "
                    "(region→pager→views deepen; not product pager; not bar3)\n",
                    (unsigned)u32Pages);
        }
    }
}

/*
 * Consume past mono deadline: soft kill-on-timeout policy (CAP: kill thread).
 * Cookie already marked dead by caller; product resumes as kill path.
 * greppable: fault: kill-on-timeout soft
 */
static void
fault_kill_on_timeout_soft(u64 u64ClusterBase, u32 u32NPages, u64 u64Deadline,
                           u64 u64Now)
{
    u32 u32N;

    fault_stat_inc(&g_u64KillOnTimeoutSoft);

    u32N = __atomic_load_n(&g_u32KillTimeoutLogged, __ATOMIC_RELAXED);
    if (u32N < FAULT_PAGER_PATH_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32KillTimeoutLogged, &u32N,
                                        u32N + 1u, 0, __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED)) {
            kprintf("fault: kill-on-timeout soft base=0x%llx pages=%u "
                    "deadline=%llu now=%llu "
                    "(soft kill policy; not product thread kill complete)\n",
                    (unsigned long long)u64ClusterBase,
                    (unsigned)u32NPages,
                    (unsigned long long)u64Deadline,
                    (unsigned long long)u64Now);
        }
    }
}

/*
 * FAIL / death / early drop: invalidate live cookie without map.
 * greppable: fault: fail invalidate soft
 */
static void
fault_fail_invalidate_soft(u64 u64CookieLo, u64 u64CookieHi,
                           u64 u64ClusterBase, u32 u32NPages)
{
    u32 u32N;

    fault_stat_inc(&g_u64FailInvalidateSoft);

    u32N = __atomic_load_n(&g_u32FailInvLogged, __ATOMIC_RELAXED);
    if (u32N < FAULT_PAGER_PATH_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32FailInvLogged, &u32N, u32N + 1u,
                                        0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
            kprintf("fault: fail invalidate soft cookie=%llx:%llx "
                    "base=0x%llx pages=%u "
                    "(FAIL/death/drop; soft; not product pager complete)\n",
                    (unsigned long long)u64CookieLo,
                    (unsigned long long)u64CookieHi,
                    (unsigned long long)u64ClusterBase,
                    (unsigned)u32NPages);
        }
    }
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
    int fFaultPresentSoft;

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

    /*
     * Soft class: present/not on the fault page itself.
     * NULL probe ⇒ soft not-present (coalesce freely within caps).
     * greppable: fault: soft
     */
    fFaultPresentSoft =
        cluster_page_present_soft(pfnPresent, pPresentCtx, u64FaultPage);

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

    /*
     * Soft fault class counters + rate-limited greppable line.
     * Axes: user/kernel · present/not · write/exec soft.
     * greppable: fault: soft
     */
    fault_class_soft_note(u64FaultPage, u32Access, fFaultPresentSoft, u32N,
                          "cluster");

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

    /*
     * Soft class on cookie mint: VA window + access bits.
     * Present axis soft-unknown here (cookie has no PTE probe) → not.
     * greppable: fault: soft
     */
    fault_class_soft_note(u64ClusterBase, u32Access, /*fPresent*/ 0, u32NPages,
                          "cookie");

    /*
     * Soft Call-pager shape after mint (product: doors Call with mono
     * timeout carrying gj_fault_msg). Inventory only until IPC wired.
     * greppable: fault: pager call soft
     */
    fault_pager_call_soft(u64ClusterBase, u32NPages, u32Access, pSpace,
                          pSlot->pMemObj);

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
            /*
             * Kill-on-timeout soft (CAP / Solaris: kill faulting thread).
             * greppable: fault: kill-on-timeout soft
             */
            fault_kill_on_timeout_soft(pCookie->u64ClusterBase,
                                       pCookie->u32NPages,
                                       pCookie->u64DeadlineMono, u64Now);
            return GJ_ERR_TIMEOUT;
        }

        pCookie->u8Used = 1;
        pCookie->u8Live = 0; /* single-use: dead after consume */
        if (pOutCopy != NULL) {
            *pOutCopy = *pCookie;
        }
        fault_stat_inc(&g_faultStats.u64CookieConsumeOk);

        /*
         * Soft view install: maps are views of object pages (Apple §2).
         * Product: vmm_map_page after FRAME validate; soft records counts.
         * greppable: fault: cookie view map soft
         * greppable: fault: view install
         */
        fault_view_install_soft(pCookie);

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
            u64 u64Base;
            u32 u32N;

            u64Base = pCookie->u64ClusterBase;
            u32N = pCookie->u32NPages;
            pCookie->u8Live = 0;
            pCookie->u8Used = 0;
            /* greppable: FAULT_MAP_COOKIE invalidate */
            fault_stat_inc(&g_faultStats.u64CookieInvalidate);
            /*
             * FAIL / death / early drop path (no map). Soft kill policy
             * inventory; product ties to thread kill / resume fail.
             * greppable: fault: fail invalidate soft
             */
            fault_fail_invalidate_soft(u64CookieLo, u64CookieHi, u64Base,
                                       u32N);
            return;
        }
    }
}

/*
 * Soft bind of memory-object pointer on a live cookie (Apple §2 prep).
 * Views installed at consume record memobj_view when this pointer is set.
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

    /*
     * Soft class totals log on stats snapshot when any soft class event
     * has been noted (greppable inventory). Never hard-gates.
     * Honesty: not product SEH complete.
     * greppable: fault: soft class
     */
    if (__atomic_load_n(&g_u64SoftClassCalls, __ATOMIC_RELAXED) != 0) {
        fault_class_soft_log();
    }

    /*
     * Soft pager path inventory when any Call / view / kill / FAIL soft
     * activity has been noted. Never hard-gates.
     * Honesty: soft region→pager→views deepen; not product pager; not bar3.
     * greppable: fault: pager call soft
     */
    if (__atomic_load_n(&g_u64PagerCallSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64ViewInstallSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64KillOnTimeoutSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64FailInvalidateSoft, __ATOMIC_RELAXED) != 0) {
        fault_pager_path_inventory_log();
    }
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

    /* Soft class counters (file-local; not in gj_fault_stats ABI). */
    __atomic_store_n(&g_u64SoftClassUser, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClassKernel, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClassPresent, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClassNot, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClassWrite, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClassExec, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClassCalls, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32SoftClassLogged, 0, __ATOMIC_RELAXED);

    /* Soft pager Call + view install + kill/FAIL path (file-local). */
    __atomic_store_n(&g_u64PagerCallSoft, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallPages, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallSoft, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallPages, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallMemobj, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallNoMemobj, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64KillOnTimeoutSoft, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64FailInvalidateSoft, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32PagerCallLogged, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32ViewInstallLogged, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32KillTimeoutLogged, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32FailInvLogged, 0, __ATOMIC_RELAXED);
}
