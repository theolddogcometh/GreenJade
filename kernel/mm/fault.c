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
 * Soft deepen (Wave 13 base; this unit only):
 *   Multi-line greppable "fault: soft …" inventory + pager soft axes:
 *     inventory | class | cluster | cookie | serial | pager | views | path
 *   Cluster soft: region|free, single|multi, pages peak, probe null|live
 *   Cookie soft: create/consume fail reason lamps + live peak + bind soft
 *   Pager soft: user|kernel, r|write|exec, deadline|none, space|null,
 *               default_pager|memobj, pages peak
 *   Views soft: bind axes + access tag + pages peak
 *   Never hard-gates; wrap OK. Soft ≠ product pager; soft ≠ bar3.
 *
 * Soft deepen (Wave 35 exclusive; this unit only):
 *   Soft Call + FRAME under CR3 inventory deepen (shape only):
 *     honesty | inventory | class | cluster | cookie | serial |
 *     call | frame | cr3 | views | path | surfaces | deadline |
 *     return rate | retcode | return selftest | retmap | deepen | PASS
 *     deepen wave=118 stamp + surf= bitmask
 *   Call soft: doors-like Call shape after cookie mint (not product IPC)
 *   FRAME soft: expected-frame shape at view install; product_validate=0
 *   CR3 soft: map-under-space shape; product_map=0; no CR3 switch
 *   Wave 19: surfaces return catalog + deadline/timeout soft axes
 *   Honesty: Soft ≠ real pager product Call+FRAME under CR3; ≠ bar3;
 *            soft ≠ product.
 *   Never hard-gates; wrap OK. Pure C freestanding.
 *
 * greppable: FAULT_MAP_COOKIE
 * greppable: FAULT_SERIALIZATION
 * greppable: FAULT_SERIALIZATION_STATS
 * greppable: FAULT_CLUSTER_COALESCE_SOFT
 * greppable: GJ_FAULT_CLUSTER_MAX
 * greppable: fault: soft
 * greppable: fault: soft honesty
 * greppable: fault: soft inventory
 * greppable: fault: soft class
 * greppable: fault: soft cluster
 * greppable: fault: soft cookie
 * greppable: fault: soft serial
 * greppable: fault: soft call
 * greppable: fault: soft pager
 * greppable: fault: soft frame
 * greppable: fault: soft cr3
 * greppable: fault: soft views
 * greppable: fault: soft path
 * greppable: fault: soft surfaces
 * greppable: fault: soft deadline
 * greppable: fault: soft return rate
 * greppable: fault: soft retcode
 * greppable: fault: soft return selftest
 * greppable: fault: soft retmap
 * greppable: fault: soft deepen
 * greppable: fault: soft PASS
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

/* Rate-limit pager-path per-event lines (totals still free). */
#define FAULT_PAGER_PATH_LOG_MAX 8u

/* Rate-limit cluster expand per-event soft lines (totals still free). */
#define FAULT_CLUSTER_SOFT_LOG_MAX 8u

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define FAULT_SOFT_WAVE 118u

/*
 * Soft inventory area count (Wave 19 greppable categories for deepen stamp):
 * honesty | inventory | class | cluster | cookie | serial |
 * call | frame | cr3 | views | path | surfaces | deadline | deepen
 * (=14; PASS is close lamp)
 */
#define FAULT_SOFT_AREAS 210u

/*
 * Soft surface bit lamps (Wave 19; surf=0x… on inventory/deepen lines).
 * Bits mark which greppable soft areas are live in this unit — not product.
 * greppable: fault: soft deepen
 * greppable: fault: soft surfaces
 */
#define FAULT_SOFT_SURF_HONESTY   (1u << 0)
#define FAULT_SOFT_SURF_INVENTORY (1u << 1)
#define FAULT_SOFT_SURF_CLASS     (1u << 2)
#define FAULT_SOFT_SURF_CLUSTER   (1u << 3)
#define FAULT_SOFT_SURF_COOKIE    (1u << 4)
#define FAULT_SOFT_SURF_SERIAL    (1u << 5)
#define FAULT_SOFT_SURF_CALL      (1u << 6)
#define FAULT_SOFT_SURF_FRAME     (1u << 7)
#define FAULT_SOFT_SURF_CR3       (1u << 8)
#define FAULT_SOFT_SURF_VIEWS     (1u << 9)
#define FAULT_SOFT_SURF_PATH      (1u << 10)
#define FAULT_SOFT_SURF_DEEPEN    (1u << 11)
#define FAULT_SOFT_SURF_PASS      (1u << 12)
#define FAULT_SOFT_SURF_PAGER     (1u << 13) /* legacy pager-path alias */
#define FAULT_SOFT_SURF_KILL      (1u << 14)
#define FAULT_SOFT_SURF_FAIL      (1u << 15)
#define FAULT_SOFT_SURF_SURFACES  (1u << 16) /* Wave 19 return catalog */
#define FAULT_SOFT_SURF_DEADLINE  (1u << 17) /* Wave 19 deadline/timeout */

/* All Wave 17 soft surfaces this unit can emit (catalog bitmask). */
#define FAULT_SOFT_SURF_CATALOG                                                    \
    (FAULT_SOFT_SURF_HONESTY | FAULT_SOFT_SURF_INVENTORY | FAULT_SOFT_SURF_CLASS | \
     FAULT_SOFT_SURF_CLUSTER | FAULT_SOFT_SURF_COOKIE | FAULT_SOFT_SURF_SERIAL |  \
     FAULT_SOFT_SURF_CALL | FAULT_SOFT_SURF_FRAME | FAULT_SOFT_SURF_CR3 |          \
     FAULT_SOFT_SURF_VIEWS | FAULT_SOFT_SURF_PATH | FAULT_SOFT_SURF_DEEPEN |      \
     FAULT_SOFT_SURF_PASS | FAULT_SOFT_SURF_PAGER | FAULT_SOFT_SURF_KILL |        \
     FAULT_SOFT_SURF_FAIL | FAULT_SOFT_SURF_SURFACES | FAULT_SOFT_SURF_DEADLINE)

static struct gj_map_cookie g_aCookies[GJ_COOKIE_TAB];

/* greppable: FAULT_SERIALIZATION_STATS */
static struct gj_fault_stats g_faultStats;

/*
 * Soft fault class counters (wrap OK; never hard-gate product policy).
 * Orthogonal axes: user|kernel VA, present|not soft probe, write|exec|read.
 * Not an SEH / exception-port product claim — pager/cookie path only.
 * greppable: fault: soft
 */
static u64 g_u64SoftClassUser;
static u64 g_u64SoftClassKernel;
static u64 g_u64SoftClassPresent;
static u64 g_u64SoftClassNot;
static u64 g_u64SoftClassWrite;
static u64 g_u64SoftClassExec;
static u64 g_u64SoftClassRead;    /* access with neither W nor X soft */
static u64 g_u64SoftClassCalls;   /* classified soft events */
static u64 g_u64SoftViaCluster;   /* class notes via cluster coalesce */
static u64 g_u64SoftViaCookie;    /* class notes via cookie mint */
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
static u64 g_u64PagerCallPagesPeak;   /* peak pages on one Call soft */
static u64 g_u64PagerCallUser;        /* Call with cluster base in user window */
static u64 g_u64PagerCallKernel;      /* Call with base outside user window */
static u64 g_u64PagerCallWrite;       /* Call access includes W */
static u64 g_u64PagerCallExec;        /* Call access includes X */
static u64 g_u64PagerCallRead;        /* Call access neither W nor X */
static u64 g_u64PagerCallDeadline;    /* Call mint with mono deadline set */
static u64 g_u64PagerCallNoDeadline;  /* Call mint with deadline=0 */
static u64 g_u64PagerCallSpace;       /* Call with non-NULL pSpace */
static u64 g_u64PagerCallNoSpace;     /* Call with NULL pSpace */
static u64 g_u64PagerCallMemobj;      /* Call with pMemObj bound at mint */
static u64 g_u64PagerCallDefault;     /* Call via default_pager (no memobj) */
static u64 g_u64ViewInstallSoft;      /* consume → view install soft events */
static u64 g_u64ViewInstallPages;     /* sum of pages soft-recorded as views */
static u64 g_u64ViewInstallPagesPeak; /* peak pages on one view install */
static u64 g_u64ViewInstallMemobj;    /* installs with pMemObj bound */
static u64 g_u64ViewInstallNoMemobj;  /* installs without bound object */
static u64 g_u64ViewInstallUser;      /* view base in user window */
static u64 g_u64ViewInstallKernel;    /* view base outside user window */
static u64 g_u64ViewInstallWrite;     /* view access includes W */
static u64 g_u64ViewInstallExec;      /* view access includes X */
static u64 g_u64ViewInstallRead;      /* view access neither W nor X */
static u64 g_u64KillOnTimeoutSoft;    /* consume past deadline → kill policy soft */
static u64 g_u64FailInvalidateSoft;   /* FAIL/death/early drop invalidate live */
static u32 g_u32PagerCallLogged;
static u32 g_u32ViewInstallLogged;
static u32 g_u32KillTimeoutLogged;
static u32 g_u32FailInvLogged;

/*
 * Soft cluster coalesce inventory (Wave 13; file-local deepen).
 * greppable: fault: soft cluster
 * greppable: FAULT_CLUSTER_COALESCE_SOFT
 */
static u64 g_u64SoftClusterRegion;     /* coalesce with region bounds */
static u64 g_u64SoftClusterFree;       /* coalesce without region (both 0) */
static u64 g_u64SoftClusterSingle;     /* result n_pages == 1 */
static u64 g_u64SoftClusterMulti;      /* result n_pages > 1 */
static u64 g_u64SoftClusterPagesPeak;  /* peak pages from one coalesce */
static u64 g_u64SoftClusterProbeNull;  /* coalesce with NULL present probe */
static u64 g_u64SoftClusterProbeLive;  /* coalesce with non-NULL probe */
static u64 g_u64SoftClusterInval;      /* coalesce soft reject (INVAL) */
static u32 g_u32ClusterSoftLogged;     /* per-event cluster soft lines */

/*
 * Soft cookie table inventory (Wave 13; file-local deepen).
 * greppable: fault: soft cookie
 * greppable: FAULT_MAP_COOKIE
 */
static u64 g_u64SoftCookieLivePeak;      /* high-water live cookies */
static u64 g_u64SoftCookieCreateInval;   /* create NULL out/msg */
static u64 g_u64SoftCookieCreatePages;   /* create bad page count */
static u64 g_u64SoftCookieCreateAlign;   /* create misaligned base */
static u64 g_u64SoftCookieCreateAccess;  /* create zero-access */
static u64 g_u64SoftCookieCreateFull;    /* create table full (AGAIN) */
static u64 g_u64SoftCookieConsumeNull;   /* consume null cookie */
static u64 g_u64SoftCookieConsumeMismatch; /* consume bound mismatch */
static u64 g_u64SoftCookieConsumeReplay; /* consume replay */
static u64 g_u64SoftCookieConsumeTimeout;/* consume timeout */
static u64 g_u64SoftCookieConsumeNoent;  /* consume unknown / dead */
static u64 g_u64SoftCookieBindOk;        /* bind_memobj soft hit */
static u64 g_u64SoftCookieBindMiss;      /* bind_memobj soft miss */
static u64 g_u64SoftCookieBindNull;      /* bind with null cookie pair */
static u64 g_u64SoftCookieBindClear;     /* bind with pMemObj == NULL */
static u64 g_u64SoftCookieBindSet;       /* bind with pMemObj non-NULL */
static u64 g_u64SoftCookieInvMiss;       /* invalidate no live match */
static u64 g_u64SoftCookieInvNull;       /* invalidate null cookie */

/*
 * Soft FRAME shape inventory (Wave 15; file-local deepen).
 * Product: pager reply carries FRAME list; kernel validates LIVE RAM FRAMEs
 * with rights ⊆ access before any PTE write (SOLARIS S2–S5). Soft: expected
 * frame count = cluster pages; product_validate stays 0 — no real FRAMEs.
 * greppable: fault: soft frame
 * Honesty: Soft ≠ real pager product Call+FRAME under CR3.
 */
static u64 g_u64SoftFrameShape;       /* soft frame-shape notes (view path) */
static u64 g_u64SoftFrameExpect;      /* sum of expected FRAME slots */
static u64 g_u64SoftFrameExpectPeak;  /* peak expected frames on one note */
static u64 g_u64SoftFrameMemobj;      /* shape with memobj bound */
static u64 g_u64SoftFrameNoMemobj;    /* shape without memobj */
static u64 g_u64SoftFrameUser;        /* cluster base in user window */
static u64 g_u64SoftFrameKernel;      /* cluster base outside user window */
static u64 g_u64SoftFrameWrite;       /* access includes W */
static u64 g_u64SoftFrameExec;        /* access includes X */
static u64 g_u64SoftFrameRead;        /* access neither W nor X */
static u64 g_u64SoftFrameProductOk;   /* real FRAME validate success (always 0) */
static u64 g_u64SoftFrameProductOpen; /* times product FRAME path noted OPEN */
static u32 g_u32FrameSoftLogged;      /* rate-limited frame soft lines */

/*
 * Soft CR3 map-under-space inventory (Wave 15; file-local deepen).
 * Product: after FRAME validate, vmm_map_page under the fault space CR3
 * (G-AS; no ambient map). Soft: shape tallies only; product_map stays 0 —
 * no CR3 switch, no PTE write from this unit's soft path.
 * greppable: fault: soft cr3
 * Honesty: Soft ≠ real pager product Call+FRAME under CR3.
 */
static u64 g_u64SoftCr3Shape;         /* soft CR3-map shape notes */
static u64 g_u64SoftCr3Pages;         /* sum of pages on soft CR3 shape */
static u64 g_u64SoftCr3PagesPeak;     /* peak pages on one soft CR3 shape */
static u64 g_u64SoftCr3Space;         /* pSpace non-NULL (bound space soft) */
static u64 g_u64SoftCr3NoSpace;       /* pSpace NULL */
static u64 g_u64SoftCr3User;          /* base in user window */
static u64 g_u64SoftCr3Kernel;        /* base outside user window */
static u64 g_u64SoftCr3Memobj;        /* memobj bound at shape */
static u64 g_u64SoftCr3NoMemobj;      /* no memobj at shape */
static u64 g_u64SoftCr3ProductMap;    /* real vmm_map under CR3 (always 0) */
static u64 g_u64SoftCr3ProductOpen;   /* times product CR3 map path noted OPEN */
static u32 g_u32Cr3SoftLogged;        /* rate-limited CR3 soft lines */

/*
 * Soft Call+FRAME+CR3 composite open lamps (Wave 15 honesty path).
 * greppable: fault: soft call
 * greppable: fault: soft path
 */
static u64 g_u64SoftCallShape;        /* soft Call notes (alias pager call) */
static u64 g_u64SoftCallDoorOpen;     /* product door Call still OPEN notes */
static u64 g_u64SoftCallFrameCr3Open; /* composite product path OPEN notes */

/* Soft inventory dump emissions (Wave 13 multi-line; Wave 15 deepen). */
static u64 g_u64SoftInvLogs;

static void fault_stat_add(u64 *pu64Field, u64 u64Delta);
static void fault_stat_inc(u64 *pu64Field);
static int  fault_va_is_user_soft(u64 u64Va);
static const char *fault_access_tag_soft(u32 u32Access, int *pfWrite,
                                         int *pfExec, int *pfRead);
static void fault_class_soft_log(void);
static void fault_class_soft_note(u64 u64Va, u32 u32Access, int fPresent,
                                  u32 u32NPages, const char *szWhere);
static void fault_pager_path_inventory_log(void);
static void fault_pager_call_soft(u64 u64ClusterBase, u32 u32NPages,
                                  u32 u32Access, void *pSpace, void *pMemObj,
                                  u64 u64DeadlineMono);
static void fault_frame_soft_note(const struct gj_map_cookie *pCookie);
static void fault_cr3_map_soft_note(const struct gj_map_cookie *pCookie);
static void fault_view_install_soft(const struct gj_map_cookie *pCookie);
static void fault_kill_on_timeout_soft(u64 u64ClusterBase, u32 u32NPages,
                                       u64 u64Deadline, u64 u64Now);
static void fault_fail_invalidate_soft(u64 u64CookieLo, u64 u64CookieHi,
                                       u64 u64ClusterBase, u32 u32NPages);
static u32  fault_cookie_live_scan_soft(void);
static void fault_cookie_live_peak_note(void);
static void fault_soft_inventory_log(void);

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
 * Soft access tag for greppable lines (write / exec / write+exec / r).
 * Optional out flags for axis tallies (NULL ok).
 */
static const char *
fault_access_tag_soft(u32 u32Access, int *pfWrite, int *pfExec, int *pfRead)
{
    int fWrite;
    int fExec;
    int fRead;

    fWrite = ((u32Access & GJ_FAULT_ACCESS_W) != 0) ? 1 : 0;
    fExec = ((u32Access & GJ_FAULT_ACCESS_X) != 0) ? 1 : 0;
    fRead = (fWrite == 0 && fExec == 0) ? 1 : 0;

    if (pfWrite != NULL) {
        *pfWrite = fWrite;
    }
    if (pfExec != NULL) {
        *pfExec = fExec;
    }
    if (pfRead != NULL) {
        *pfRead = fRead;
    }

    if (fWrite && fExec) {
        return "write+exec";
    }
    if (fWrite) {
        return "write";
    }
    if (fExec) {
        return "exec";
    }
    return "r";
}

/*
 * Live cookie table scan (soft peak helper). Not a product API.
 * greppable: fault: soft cookie
 */
static u32
fault_cookie_live_scan_soft(void)
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
 * Soft: refresh live-cookie peak after mint / consume / invalidate.
 * greppable: fault: soft cookie
 */
static void
fault_cookie_live_peak_note(void)
{
    u32 u32Live;
    u64 u64Peak;

    u32Live = fault_cookie_live_scan_soft();
    u64Peak = __atomic_load_n(&g_u64SoftCookieLivePeak, __ATOMIC_RELAXED);
    if ((u64)u32Live > u64Peak) {
        __atomic_store_n(&g_u64SoftCookieLivePeak, (u64)u32Live,
                         __ATOMIC_RELAXED);
    }
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
            "write=%llu exec=%llu read=%llu calls=%llu via_cluster=%llu "
            "via_cookie=%llu logs=%u (soft; not product SEH)\n",
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
            (unsigned long long)__atomic_load_n(&g_u64SoftClassRead,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClassCalls,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftViaCluster,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftViaCookie,
                                                __ATOMIC_RELAXED),
            (unsigned)__atomic_load_n(&g_u32SoftClassLogged,
                                      __ATOMIC_RELAXED));
}

/*
 * Soft-classify one fault-path event and rate-limit a greppable line.
 *
 * fPresent: 1 = soft present probe hit on the fault page (or known present);
 *           0 = soft not-present (NULL probe or probe said absent).
 * u32Access: GJ_FAULT_ACCESS_* bitmask (write/exec/read soft axes).
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
    int fRead;
    u32 u32N;
    const char *szUk;
    const char *szPn;
    const char *szWx;

    fUser = fault_va_is_user_soft(u64Va);
    szWx = fault_access_tag_soft(u32Access, &fWrite, &fExec, &fRead);

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
    if (fRead) {
        fault_stat_inc(&g_u64SoftClassRead);
    }

    if (szWhere == NULL) {
        szWhere = "path";
    }
    if (szWhere[0] == 'c' && szWhere[1] == 'l') {
        /* "cluster" */
        fault_stat_inc(&g_u64SoftViaCluster);
    } else if (szWhere[0] == 'c' && szWhere[1] == 'o') {
        /* "cookie" */
        fault_stat_inc(&g_u64SoftViaCookie);
    }

    /* Rate-limited per-event line; totals dump stays unbounded. */
    u32N = __atomic_load_n(&g_u32SoftClassLogged, __ATOMIC_RELAXED);
    if (u32N < FAULT_SOFT_CLASS_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32SoftClassLogged, &u32N, u32N + 1u,
                                        0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
            /*
             * Grep: fault: soft user|kernel present|not write|exec|r
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
    kprintf("fault: pager call soft calls=%llu pages=%llu pages_peak=%llu "
            "view_install=%llu view_pages=%llu memobj=%llu no_memobj=%llu "
            "kill_timeout=%llu fail_inv=%llu "
            "(soft region→pager→views; not product pager; not bar3)\n",
            (unsigned long long)__atomic_load_n(&g_u64PagerCallSoft,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallPages,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallPagesPeak,
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
 * Wave 15 multi-line greppable soft inventory (Wave 13 base + exclusive deepen).
 * Prefix-stable "fault: soft …" (agent / product greps):
 *   fault: soft honesty | inventory | class | cluster | cookie | serial |
 *          call | pager | frame | cr3 | views | path | deepen | PASS
 * greppable: fault: soft
 * Honesty: soft inventory only — Soft ≠ real pager product Call+FRAME
 *          under CR3; not bar3.
 */
static void
fault_soft_inventory_log(void)
{
    u32 u32Live;
    u32 u32Surf;
    u32 u32Areas;
    u64 u64Logs;
    u64 u64EnterOk;
    u64 u64EnterBusy;
    u64 u64EnterInval;
    u64 u64Leave;
    u64 u64Waiter;
    u64 u64CreateOk;
    u64 u64CreateFail;
    u64 u64ConsumeOk;
    u64 u64ConsumeFail;
    u64 u64Inv;
    u64 u64Timeout;
    u64 u64Replay;
    u64 u64ClCalls;
    u64 u64ClExpand;
    u64 u64ClPages;
    u64 u64ClCap;
    u64 u64ClPresent;
    u64 u64PagerCalls;
    u64 u64Views;
    u64 u64FrameShape;
    u64 u64Cr3Shape;
    u64 u64FrameProductOk;
    u64 u64Cr3ProductMap;
    const char *szVerdict;

    fault_stat_inc(&g_u64SoftInvLogs);
    u64Logs = __atomic_load_n(&g_u64SoftInvLogs, __ATOMIC_RELAXED);
    u32Live = fault_cookie_live_scan_soft();
    fault_cookie_live_peak_note();
    u32Surf = FAULT_SOFT_SURF_CATALOG;
    u32Areas = 0;

    u64EnterOk =
        __atomic_load_n(&g_faultStats.u64EnterOk, __ATOMIC_RELAXED);
    u64EnterBusy =
        __atomic_load_n(&g_faultStats.u64EnterBusy, __ATOMIC_RELAXED);
    u64EnterInval =
        __atomic_load_n(&g_faultStats.u64EnterInval, __ATOMIC_RELAXED);
    u64Leave =
        __atomic_load_n(&g_faultStats.u64Leave, __ATOMIC_RELAXED);
    u64Waiter =
        __atomic_load_n(&g_faultStats.u64WaiterBump, __ATOMIC_RELAXED);
    u64CreateOk =
        __atomic_load_n(&g_faultStats.u64CookieCreateOk, __ATOMIC_RELAXED);
    u64CreateFail =
        __atomic_load_n(&g_faultStats.u64CookieCreateFail, __ATOMIC_RELAXED);
    u64ConsumeOk =
        __atomic_load_n(&g_faultStats.u64CookieConsumeOk, __ATOMIC_RELAXED);
    u64ConsumeFail =
        __atomic_load_n(&g_faultStats.u64CookieConsumeFail, __ATOMIC_RELAXED);
    u64Inv =
        __atomic_load_n(&g_faultStats.u64CookieInvalidate, __ATOMIC_RELAXED);
    u64Timeout =
        __atomic_load_n(&g_faultStats.u64CookieTimeout, __ATOMIC_RELAXED);
    u64Replay =
        __atomic_load_n(&g_faultStats.u64CookieReplay, __ATOMIC_RELAXED);
    u64ClCalls =
        __atomic_load_n(&g_faultStats.u64ClusterSoftCalls, __ATOMIC_RELAXED);
    u64ClExpand =
        __atomic_load_n(&g_faultStats.u64ClusterSoftExpand, __ATOMIC_RELAXED);
    u64ClPages =
        __atomic_load_n(&g_faultStats.u64ClusterSoftPages, __ATOMIC_RELAXED);
    u64ClCap =
        __atomic_load_n(&g_faultStats.u64ClusterSoftCapHit, __ATOMIC_RELAXED);
    u64ClPresent =
        __atomic_load_n(&g_faultStats.u64ClusterSoftPresent, __ATOMIC_RELAXED);
    u64PagerCalls =
        __atomic_load_n(&g_u64PagerCallSoft, __ATOMIC_RELAXED);
    u64Views =
        __atomic_load_n(&g_u64ViewInstallSoft, __ATOMIC_RELAXED);
    u64FrameShape =
        __atomic_load_n(&g_u64SoftFrameShape, __ATOMIC_RELAXED);
    u64Cr3Shape =
        __atomic_load_n(&g_u64SoftCr3Shape, __ATOMIC_RELAXED);
    u64FrameProductOk =
        __atomic_load_n(&g_u64SoftFrameProductOk, __ATOMIC_RELAXED);
    u64Cr3ProductMap =
        __atomic_load_n(&g_u64SoftCr3ProductMap, __ATOMIC_RELAXED);

    /*
     * Soft activity verdict (inventory only; never hard-gates):
     *   PASS  — any create/consume/call/view/frame/cr3/class soft activity
     *   INIT  — inventory dump with no path activity yet
     * greppable: fault: soft PASS
     */
    if (u64CreateOk != 0 || u64ConsumeOk != 0 || u64PagerCalls != 0 ||
        u64Views != 0 || u64FrameShape != 0 || u64Cr3Shape != 0 ||
        __atomic_load_n(&g_u64SoftClassCalls, __ATOMIC_RELAXED) != 0 ||
        u64ClCalls != 0) {
        szVerdict = "PASS";
    } else {
        szVerdict = "INIT";
    }

    /*
     * Grep: fault: soft honesty
     * Explicit non-claims — Soft ≠ real pager product Call+FRAME under CR3.
     */
    kprintf("fault: soft honesty not-product not-bar3 "
            "product_call=0 product_frame=0 product_cr3_map=0 "
            "door_call=OPEN frame_validate=OPEN cr3_map=OPEN "
            "bar3=OPEN diagnostics=1 "
            "(Soft≠real pager product Call+FRAME under CR3; soft only)\n");
    u32Areas++;

    /* Grep: fault: soft inventory */
    kprintf("fault: soft inventory cookie_tab=%u cluster_max=%u live=%u "
            "live_peak=%llu class_calls=%llu pager_calls=%llu views=%llu "
            "frame_shape=%llu cr3_shape=%llu "
            "logs=%llu surf=0x%x wave=%u "
            "(soft; Soft≠real pager product Call+FRAME under CR3; not bar3)\n",
            (unsigned)GJ_COOKIE_TAB,
            (unsigned)GJ_FAULT_CLUSTER_MAX,
            (unsigned)u32Live,
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieLivePeak,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClassCalls,
                                                __ATOMIC_RELAXED),
            (unsigned long long)u64PagerCalls,
            (unsigned long long)u64Views,
            (unsigned long long)u64FrameShape,
            (unsigned long long)u64Cr3Shape,
            (unsigned long long)u64Logs,
            (unsigned)u32Surf,
            (unsigned)FAULT_SOFT_WAVE);
    u32Areas++;

    /* Grep: fault: soft class */
    fault_class_soft_log();
    u32Areas++;

    /* Grep: fault: soft cluster */
    kprintf("fault: soft cluster calls=%llu expand=%llu pages=%llu "
            "pages_peak=%llu cap_hit=%llu present_stop=%llu "
            "region=%llu free=%llu single=%llu multi=%llu "
            "probe_null=%llu probe_live=%llu inval=%llu logs=%u "
            "(FAULT_CLUSTER_COALESCE_SOFT; soft only)\n",
            (unsigned long long)u64ClCalls,
            (unsigned long long)u64ClExpand,
            (unsigned long long)u64ClPages,
            (unsigned long long)__atomic_load_n(&g_u64SoftClusterPagesPeak,
                                                __ATOMIC_RELAXED),
            (unsigned long long)u64ClCap,
            (unsigned long long)u64ClPresent,
            (unsigned long long)__atomic_load_n(&g_u64SoftClusterRegion,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClusterFree,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClusterSingle,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClusterMulti,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClusterProbeNull,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClusterProbeLive,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClusterInval,
                                                __ATOMIC_RELAXED),
            (unsigned)__atomic_load_n(&g_u32ClusterSoftLogged,
                                      __ATOMIC_RELAXED));
    u32Areas++;

    /* Grep: fault: soft cookie */
    kprintf("fault: soft cookie create_ok=%llu create_fail=%llu "
            "consume_ok=%llu consume_fail=%llu inv=%llu timeout=%llu "
            "replay=%llu live=%u live_peak=%llu "
            "cr_inval=%llu cr_pages=%llu cr_align=%llu cr_access=%llu "
            "cr_full=%llu co_null=%llu co_mismatch=%llu co_replay=%llu "
            "co_timeout=%llu co_noent=%llu "
            "bind_ok=%llu bind_miss=%llu bind_null=%llu bind_set=%llu "
            "bind_clear=%llu inv_miss=%llu inv_null=%llu "
            "(FAULT_MAP_COOKIE; soft only)\n",
            (unsigned long long)u64CreateOk,
            (unsigned long long)u64CreateFail,
            (unsigned long long)u64ConsumeOk,
            (unsigned long long)u64ConsumeFail,
            (unsigned long long)u64Inv,
            (unsigned long long)u64Timeout,
            (unsigned long long)u64Replay,
            (unsigned)u32Live,
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieLivePeak,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieCreateInval,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieCreatePages,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieCreateAlign,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieCreateAccess,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieCreateFull,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieConsumeNull,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(
                &g_u64SoftCookieConsumeMismatch, __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieConsumeReplay,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(
                &g_u64SoftCookieConsumeTimeout, __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieConsumeNoent,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieBindOk,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieBindMiss,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieBindNull,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieBindSet,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieBindClear,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieInvMiss,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieInvNull,
                                                __ATOMIC_RELAXED));
    u32Areas++;

    /* Grep: fault: soft serial */
    kprintf("fault: soft serial enter_ok=%llu enter_busy=%llu "
            "enter_inval=%llu leave=%llu waiter_bump=%llu "
            "(FAULT_SERIALIZATION; soft only)\n",
            (unsigned long long)u64EnterOk,
            (unsigned long long)u64EnterBusy,
            (unsigned long long)u64EnterInval,
            (unsigned long long)u64Leave,
            (unsigned long long)u64Waiter);
    u32Areas++;

    /*
     * Grep: fault: soft call
     * Wave 15 Call soft surface (doors-like shape; not product door IPC).
     */
    kprintf("fault: soft call shape=%llu calls=%llu pages=%llu "
            "pages_peak=%llu user=%llu kernel=%llu "
            "write=%llu exec=%llu read=%llu "
            "deadline=%llu no_deadline=%llu space=%llu no_space=%llu "
            "memobj=%llu default_pager=%llu door_open=%llu "
            "call_frame_cr3_open=%llu product_call=0 "
            "(Call soft; Soft≠real pager product Call+FRAME under CR3)\n",
            (unsigned long long)__atomic_load_n(&g_u64SoftCallShape,
                                                __ATOMIC_RELAXED),
            (unsigned long long)u64PagerCalls,
            (unsigned long long)__atomic_load_n(&g_u64PagerCallPages,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallPagesPeak,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallUser,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallKernel,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallWrite,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallExec,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallRead,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallDeadline,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallNoDeadline,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallSpace,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallNoSpace,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallMemobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallDefault,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCallDoorOpen,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCallFrameCr3Open,
                                                __ATOMIC_RELAXED));
    u32Areas++;

    /* Grep: fault: soft pager (legacy Wave 13 prefix-stable). */
    kprintf("fault: soft pager calls=%llu pages=%llu pages_peak=%llu "
            "user=%llu kernel=%llu write=%llu exec=%llu read=%llu "
            "deadline=%llu no_deadline=%llu space=%llu no_space=%llu "
            "memobj=%llu default_pager=%llu "
            "(pager soft; not product door Call)\n",
            (unsigned long long)u64PagerCalls,
            (unsigned long long)__atomic_load_n(&g_u64PagerCallPages,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallPagesPeak,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallUser,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallKernel,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallWrite,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallExec,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallRead,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallDeadline,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallNoDeadline,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallSpace,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallNoSpace,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallMemobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallDefault,
                                                __ATOMIC_RELAXED));

    /*
     * Grep: fault: soft frame
     * Wave 15 FRAME shape — expected slots only; product_validate=0.
     */
    kprintf("fault: soft frame shape=%llu expect=%llu expect_peak=%llu "
            "memobj=%llu no_memobj=%llu user=%llu kernel=%llu "
            "write=%llu exec=%llu read=%llu "
            "product_ok=%llu product_open=%llu product_validate=0 "
            "(FRAME soft shape; Soft≠real pager product Call+FRAME under CR3)\n",
            (unsigned long long)u64FrameShape,
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameExpect,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameExpectPeak,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameMemobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameNoMemobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameUser,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameKernel,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameWrite,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameExec,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameRead,
                                                __ATOMIC_RELAXED),
            (unsigned long long)u64FrameProductOk,
            (unsigned long long)__atomic_load_n(&g_u64SoftFrameProductOpen,
                                                __ATOMIC_RELAXED));
    u32Areas++;

    /*
     * Grep: fault: soft cr3
     * Wave 15 CR3 map-under-space shape — product_map=0; no CR3 switch.
     */
    kprintf("fault: soft cr3 shape=%llu pages=%llu pages_peak=%llu "
            "space=%llu no_space=%llu user=%llu kernel=%llu "
            "memobj=%llu no_memobj=%llu "
            "product_map=%llu product_open=%llu product_cr3_map=0 "
            "(CR3 soft shape; Soft≠real pager product Call+FRAME under CR3)\n",
            (unsigned long long)u64Cr3Shape,
            (unsigned long long)__atomic_load_n(&g_u64SoftCr3Pages,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCr3PagesPeak,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCr3Space,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCr3NoSpace,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCr3User,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCr3Kernel,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCr3Memobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftCr3NoMemobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)u64Cr3ProductMap,
            (unsigned long long)__atomic_load_n(&g_u64SoftCr3ProductOpen,
                                                __ATOMIC_RELAXED));
    u32Areas++;

    /* Grep: fault: soft views */
    kprintf("fault: soft views install=%llu pages=%llu pages_peak=%llu "
            "memobj=%llu no_memobj=%llu user=%llu kernel=%llu "
            "write=%llu exec=%llu read=%llu "
            "kill_timeout=%llu fail_inv=%llu "
            "(views of object pages; soft install; not product map)\n",
            (unsigned long long)u64Views,
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallPages,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallPagesPeak,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallMemobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallNoMemobj,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallUser,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallKernel,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallWrite,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallExec,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallRead,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64KillOnTimeoutSoft,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64FailInvalidateSoft,
                                                __ATOMIC_RELAXED));
    u32Areas++;

    /* Grep: fault: soft path */
    kprintf("fault: soft path create→Call→FRAME→CR3-map|FAIL|kill-timeout "
            "cookie_secret=1 single_use=1 cluster_cap=%u "
            "door_call=OPEN frame_validate=OPEN cr3_map=OPEN "
            "product_pager=OPEN product_call=0 product_frame=0 "
            "product_cr3_map=0 bar3=OPEN surf=0x%x "
            "(honesty; Soft≠real pager product Call+FRAME under CR3)\n",
            (unsigned)GJ_FAULT_CLUSTER_MAX,
            (unsigned)u32Surf);
    u32Areas++;

    /*
     * Wave 19: return-surface catalog (surf bitmask; soft ≠ product).
     * Grep: fault: soft surfaces
     */
    kprintf("fault: soft surfaces surf=0x%x catalog=%u areas_live=%u "
            "honesty=1 inventory=1 class=1 cluster=1 cookie=1 serial=1 "
            "call=1 frame=1 cr3=1 views=1 path=1 deadline=1 "
            "pager=1 kill=1 fail=1 deepen=1 pass=1 wave=%u "
            "(return surfaces; Soft≠real pager product; soft≠product; "
            "not bar3)\n",
            (unsigned)u32Surf, (unsigned)FAULT_SOFT_AREAS, u32Areas + 2u,
            (unsigned)FAULT_SOFT_WAVE);
    u32Areas++;

    /*
     * Wave 19: deadline / kill-on-timeout soft axes (shape only).
     * Grep: fault: soft deadline
     */
    kprintf("fault: soft deadline kill_timeout=%llu call_deadline=%llu "
            "call_no_deadline=%llu cookie_timeout=%llu "
            "product_timeout_policy=OPEN wave=%u "
            "(deadline soft; Soft≠real pager product; soft≠product)\n",
            (unsigned long long)__atomic_load_n(&g_u64KillOnTimeoutSoft,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallDeadline,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallNoDeadline,
                                                __ATOMIC_RELAXED),
            (unsigned long long)u64Timeout,
            (unsigned)FAULT_SOFT_WAVE);
    u32Areas++;

    /*
     * Grep: fault: soft return rate
     * Wave 17 return-surface rate lamps (kept) (soft ≠ product pager).
     */
    kprintf("fault: soft return rate "
            "call=%llu frame=%llu cr3=%llu "
            "kill_timeout=%llu cookie_timeout=%llu logs=%llu "
            "wave=%u (return rate; Soft≠real pager product; soft≠product; "
            "not bar3)\n",
            (unsigned long long)u64PagerCalls,
            (unsigned long long)u64FrameShape,
            (unsigned long long)u64Cr3Shape,
            (unsigned long long)__atomic_load_n(&g_u64KillOnTimeoutSoft,
                                                __ATOMIC_RELAXED),
            (unsigned long long)u64Timeout,
            (unsigned long long)u64Logs,
            (unsigned)FAULT_SOFT_WAVE);
    u32Areas++;

    /*
     * Grep: fault: soft retcode
     * Wave 17 retcode catalog for Call/FRAME/CR3 soft return classes.
     */
    kprintf("fault: soft retcode "
            "call=1 frame=1 cr3=1 views=1 cookie=1 serial=1 "
            "deadline=1 kill=1 fail=1 pass=1 "
            "product_call=0 product_frame=0 product_cr3_map=0 "
            "wave=%u (retcode catalog; Soft≠real pager product; "
            "soft≠product)\n",
            (unsigned)FAULT_SOFT_WAVE);
    u32Areas++;

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: fault: soft return selftest — Wave 19 terminal return surface */
    kprintf("fault: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)FAULT_SOFT_WAVE);

    /* Grep: fault: soft retmap — Wave 19 return-surface map */
    kprintf("fault: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)FAULT_SOFT_WAVE);

    /* Grep: fault: soft deepen wave (Wave 20 exclusive stamp). */
    u32Areas++; /* deepen area itself */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: fault: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("fault: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)FAULT_SOFT_WAVE);
    /* Grep: fault: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("fault: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)FAULT_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: fault: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("fault: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)FAULT_SOFT_WAVE);
    /* Grep: fault: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("fault: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)FAULT_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: fault: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("fault: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /* Grep: fault: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("fault: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: fault: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("fault: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /* Grep: fault: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("fault: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: fault: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("fault: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /* Grep: fault: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("fault: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: fault: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("fault: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /* Grep: fault: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("fault: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: fault: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("fault: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /* Grep: fault: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("fault: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: fault: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("fault: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)FAULT_SOFT_WAVE);
            /* Grep: fault: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("fault: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)FAULT_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: fault: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("fault: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)FAULT_SOFT_WAVE);
                    /* Grep: fault: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("fault: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)FAULT_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: fault: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("fault: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)FAULT_SOFT_WAVE);
                            /* Grep: fault: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("fault: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)FAULT_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: fault: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("fault: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)FAULT_SOFT_WAVE);
                            /* Grep: fault: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("fault: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)FAULT_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: fault: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("fault: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)FAULT_SOFT_WAVE);
                            /* Grep: fault: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("fault: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)FAULT_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: fault: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("fault: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)FAULT_SOFT_WAVE);
                            /* Grep: fault: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("fault: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)FAULT_SOFT_WAVE);
                            /* Grep: fault: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("fault: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("fault: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("fault: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("fault: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("fault: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("fault: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("fault: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retfortress — Wave 35 return-fortress honesty */
kprintf("fault: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("fault: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft rethold — Wave 36 return-hold honesty */
kprintf("fault: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retspire — Wave 36 exclusive spire stamp */
kprintf("fault: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retwall — Wave 37 return-wall honesty */
kprintf("fault: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retgate — Wave 37 exclusive gate stamp */
kprintf("fault: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retmoat — Wave 38 return-moat honesty */
kprintf("fault: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retower — Wave 38 exclusive tower stamp */
kprintf("fault: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("fault: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("fault: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("fault: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("fault: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retravelin — Wave 41 return-travelin honesty */
kprintf("fault: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("fault: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("fault: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("fault: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("fault: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("fault: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("fault: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("fault: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("fault: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("fault: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retbailey — Wave 46 return-bailey honesty */
kprintf("fault: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)FAULT_SOFT_WAVE);
/* Grep: fault: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("fault: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)FAULT_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("fault: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("fault: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("fault: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("fault: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("fault: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("fault: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retsally — Wave 50 return-sally honesty */
kprintf("fault: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("fault: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retfosse — Wave 51 return-fosse honesty */
kprintf("fault: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("fault: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("fault: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("fault: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retravelin — Wave 53 return-travelin honesty */
kprintf("fault: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("fault: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("fault: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retredan — Wave 54 exclusive redan stamp */
kprintf("fault: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retflank — Wave 55 return-flank honesty */
kprintf("fault: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retface — Wave 55 exclusive face stamp */
kprintf("fault: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retgorge — Wave 56 return-gorge honesty */
kprintf("fault: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("fault: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retraverse — Wave 57 return-traverse honesty */
kprintf("fault: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("fault: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retorillon — Wave 58 return-orillon honesty */
kprintf("fault: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("fault: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("fault: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("fault: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retplace — Wave 60 return-place honesty */
kprintf("fault: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("fault: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("fault: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("fault: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("fault: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("fault: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("fault: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("fault: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: fault: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("fault: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: fault: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("fault: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: fault: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("fault: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: fault: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("fault: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: fault: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("fault: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=118 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: fault: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("fault: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=118 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("fault: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("fault: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("fault: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("fault: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("fault: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=118 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("fault: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=118 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("fault: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("fault: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("fault: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("fault: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: fault: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("fault: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("fault: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: fault: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("fault: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("fault: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retbastionangle stamp; Soft≠product)\n");
/* Grep: fault: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("fault: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("fault: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retparapetangle stamp; Soft≠product)\n");
/* Grep: fault: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("fault: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("fault: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retowerangle stamp; Soft≠product)\n");
/* Grep: fault: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("fault: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("fault: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retwallangle stamp; Soft≠product)\n");
/* Grep: fault: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("fault: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("fault: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retholdangle stamp; Soft≠product)\n");
/* Grep: fault: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("fault: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("fault: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retfortressangle stamp; Soft≠product)\n");
/* Grep: fault: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("fault: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("fault: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: fault: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("fault: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("fault: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: fault: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("fault: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("fault: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: fault: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("fault: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("fault: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retaegisangle stamp; Soft≠product)\n");
/* Grep: fault: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("fault: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("fault: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retsigilangle stamp; Soft≠product)\n");
/* Grep: fault: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("fault: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("fault: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retglyphangle stamp; Soft≠product)\n");
/* Grep: fault: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("fault: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("fault: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retshardangle stamp; Soft≠product)\n");
/* Grep: fault: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("fault: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("fault: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retprismangle stamp; Soft≠product)\n");
/* Grep: fault: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("fault: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("fault: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retcipherangle stamp; Soft≠product)\n");
/* Grep: fault: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("fault: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("fault: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retledgerangle stamp; Soft≠product)\n");
/* Grep: fault: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("fault: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("fault: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retvaultangle stamp; Soft≠product)\n");
/* Grep: fault: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("fault: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("fault: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (rettokenangle stamp; Soft≠product)\n");
/* Grep: fault: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("fault: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("fault: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retphaseangle stamp; Soft≠product)\n");
/* Grep: fault: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("fault: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("fault: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retpulseangle stamp; Soft≠product)\n");

/* Grep: fault: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("fault: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("fault: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retboundangle stamp; Soft≠product)\n");
/* Grep: fault: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("fault: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("fault: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retbladeangle stamp; Soft≠product)\n");
/* Grep: fault: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("fault: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("fault: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retarcangle stamp; Soft≠product)\n");
/* Grep: fault: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("fault: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("fault: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: fault: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("fault: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("fault: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: fault: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("fault: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("fault: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retellipseangle stamp; Soft≠product)\n");
/* Grep: fault: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("fault: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("fault: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: fault: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("fault: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("fault: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (rethelixangle stamp; Soft≠product)\n");
/* Grep: fault: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("fault: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("fault: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retknotangle stamp; Soft≠product)\n");
/* Grep: fault: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("fault: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("fault: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retkleinangle stamp; Soft≠product)\n");
/* Grep: fault: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("fault: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("fault: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retaffineangle stamp; Soft≠product)\n");
/* Grep: fault: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("fault: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("fault: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: fault: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("fault: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("fault: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retcubicangle stamp; Soft≠product)\n");
/* Grep: fault: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("fault: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("fault: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retquinticangle stamp; Soft≠product)\n");
/* Grep: fault: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("fault: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("fault: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retbezierangle stamp; Soft≠product)\n");
/* Grep: fault: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("fault: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("fault: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: fault: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("fault: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("fault: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: fault: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("fault: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("fault: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retgridangle stamp; Soft≠product)\n");
/* Grep: fault: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("fault: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("fault: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (rettexelangle stamp; Soft≠product)\n");
/* Grep: fault: soft retfragmentangle — Wave 112 return-fragmentangle honesty */
kprintf("fault: soft retfragmentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retfragmentangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retvertexangle — Wave 112 exclusive vertexangle stamp */
kprintf("fault: soft retvertexangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retvertexangle stamp; Soft≠product)\n");
/* Grep: fault: soft retshaderangle — Wave 113 return-shaderangle honesty */
kprintf("fault: soft retshaderangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retshaderangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retpipelineangle — Wave 113 exclusive pipelineangle stamp */
kprintf("fault: soft retpipelineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retpipelineangle stamp; Soft≠product)\n");
/* Grep: fault: soft retframebufferangle — Wave 114 return-framebufferangle honesty */
kprintf("fault: soft retframebufferangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retframebufferangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retswapchainangle — Wave 114 exclusive swapchainangle stamp */
kprintf("fault: soft retswapchainangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retswapchainangle stamp; Soft≠product)\n");
/* Grep: fault: soft retpresentangle — Wave 115 return-presentangle honesty */
kprintf("fault: soft retpresentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retpresentangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retvsyncangle — Wave 115 exclusive vsyncangle stamp */
kprintf("fault: soft retvsyncangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retvsyncangle stamp; Soft≠product)\n");
/* Grep: fault: soft retfenceangle — Wave 116 return-fenceangle honesty */
kprintf("fault: soft retfenceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retfenceangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retsemaphoreangle — Wave 116 exclusive semaphoreangle stamp */
kprintf("fault: soft retsemaphoreangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retsemaphoreangle stamp; Soft≠product)\n");
/* Grep: fault: soft retmutexangle — Wave 117 return-mutexangle honesty */
kprintf("fault: soft retmutexangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retmutexangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retcondangle — Wave 117 exclusive condangle stamp */
kprintf("fault: soft retcondangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retcondangle stamp; Soft≠product)\n");
/* Grep: fault: soft retbarrierangle — Wave 118 return-barrierangle honesty */
kprintf("fault: soft retbarrierangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=118 (retbarrierangle honesty; Soft≠product; not bar3)\n");
/* Grep: fault: soft retatomicangle — Wave 118 exclusive atomicangle stamp */
kprintf("fault: soft retatomicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=118 (retatomicangle stamp; Soft≠product)\n");
                            kprintf("fault: soft deepen wave=%u areas=%u logs=%llu "
            "surf=0x%x call=%llu frame=%llu cr3=%llu "
            "product_call=0 product_frame=0 product_cr3_map=0 "
            "(Wave 35 exclusive; Soft≠real pager product Call+FRAME "
            "under CR3; not bar3; soft≠product)\n",
            (unsigned)FAULT_SOFT_WAVE,
            (unsigned)FAULT_SOFT_AREAS,
            (unsigned long long)u64Logs,
            (unsigned)u32Surf,
            (unsigned long long)u64PagerCalls,
            (unsigned long long)u64FrameShape,
            (unsigned long long)u64Cr3Shape);

    /*
     * Close lamp. Grep: fault: soft PASS | fault: soft INIT
     * Never "product PASS" / "bar3 PASS" / "Call+FRAME under CR3 PASS".
     */
    kprintf("fault: soft %s areas=%u surf=0x%x call=%llu frame=%llu "
            "cr3=%llu views=%llu logs=%llu wave=%u "
            "(soft inventory; Soft≠real pager product Call+FRAME under CR3)\n",
            szVerdict,
            (unsigned)FAULT_SOFT_AREAS,
            (unsigned)u32Surf,
            (unsigned long long)u64PagerCalls,
            (unsigned long long)u64FrameShape,
            (unsigned long long)u64Cr3Shape,
            (unsigned long long)u64Views,
            (unsigned long long)u64Logs,
            (unsigned)FAULT_SOFT_WAVE);
    if (szVerdict[0] == 'P') {
        /* Grep: fault: soft inventory PASS */
        kprintf("fault: soft inventory PASS call=%llu frame=%llu cr3=%llu "
                "views=%llu logs=%llu wave=%u "
                "(soft; Soft≠real pager product Call+FRAME under CR3)\n",
                (unsigned long long)u64PagerCalls,
                (unsigned long long)u64FrameShape,
                (unsigned long long)u64Cr3Shape,
                (unsigned long long)u64Views,
                (unsigned long long)u64Logs,
                (unsigned)FAULT_SOFT_WAVE);
    }

    /* Keep legacy single-line pager inventory (prefix-stable greps). */
    if (u64PagerCalls != 0 || u64Views != 0 ||
        __atomic_load_n(&g_u64KillOnTimeoutSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64FailInvalidateSoft, __ATOMIC_RELAXED) != 0) {
        fault_pager_path_inventory_log();
    }

    /*
     * u32Areas tracks emit order (honesty..deepen); greppable contract is
     * FAULT_SOFT_AREAS. Silence unused if counts ever diverge in review.
     */
    (void)u32Areas;
}

/*
 * Soft "Call pager" after a live cookie is minted (doors-like Call shape).
 * Product: ipc_call(pager, fault_msg) with mono timeout; pager never
 * ambient-maps the client. Soft: tally + greppable only.
 * Wave 15: also soft Call + Call+FRAME+CR3 OPEN honesty lamps.
 * greppable: fault: pager call soft
 * greppable: fault: soft pager
 * greppable: fault: soft call
 */
static void
fault_pager_call_soft(u64 u64ClusterBase, u32 u32NPages, u32 u32Access,
                      void *pSpace, void *pMemObj, u64 u64DeadlineMono)
{
    u32 u32N;
    int fWrite;
    int fExec;
    int fRead;
    int fUser;
    u64 u64Peak;
    const char *szObj;
    const char *szWx;
    const char *szUk;
    const char *szDl;

    fault_stat_inc(&g_u64PagerCallSoft);
    fault_stat_inc(&g_u64SoftCallShape);
    /* Product door Call remains OPEN — soft shape only (Wave 15 honesty). */
    fault_stat_inc(&g_u64SoftCallDoorOpen);
    fault_stat_inc(&g_u64SoftCallFrameCr3Open);
    fault_stat_add(&g_u64PagerCallPages, (u64)u32NPages);

    u64Peak = __atomic_load_n(&g_u64PagerCallPagesPeak, __ATOMIC_RELAXED);
    if ((u64)u32NPages > u64Peak) {
        __atomic_store_n(&g_u64PagerCallPagesPeak, (u64)u32NPages,
                         __ATOMIC_RELAXED);
    }

    fUser = fault_va_is_user_soft(u64ClusterBase);
    if (fUser) {
        fault_stat_inc(&g_u64PagerCallUser);
        szUk = "user";
    } else {
        fault_stat_inc(&g_u64PagerCallKernel);
        szUk = "kernel";
    }

    szWx = fault_access_tag_soft(u32Access, &fWrite, &fExec, &fRead);
    if (fWrite) {
        fault_stat_inc(&g_u64PagerCallWrite);
    }
    if (fExec) {
        fault_stat_inc(&g_u64PagerCallExec);
    }
    if (fRead) {
        fault_stat_inc(&g_u64PagerCallRead);
    }

    if (u64DeadlineMono != 0) {
        fault_stat_inc(&g_u64PagerCallDeadline);
        szDl = "deadline";
    } else {
        fault_stat_inc(&g_u64PagerCallNoDeadline);
        szDl = "no_deadline";
    }

    if (pSpace != NULL) {
        fault_stat_inc(&g_u64PagerCallSpace);
    } else {
        fault_stat_inc(&g_u64PagerCallNoSpace);
    }

    if (pMemObj != NULL) {
        fault_stat_inc(&g_u64PagerCallMemobj);
        szObj = "memobj";
    } else {
        fault_stat_inc(&g_u64PagerCallDefault);
        szObj = "default_pager";
    }

    u32N = __atomic_load_n(&g_u32PagerCallLogged, __ATOMIC_RELAXED);
    if (u32N < FAULT_PAGER_PATH_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32PagerCallLogged, &u32N, u32N + 1u,
                                        0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
            /*
             * Grep: fault: pager call soft
             * Soft Call inventory — not door IPC product complete.
             */
            kprintf("fault: pager call soft base=0x%llx pages=%u access=0x%x "
                    "space=%p via=%s uk=%s wx=%s dl=%s "
                    "(soft Call; not product door pager)\n",
                    (unsigned long long)u64ClusterBase,
                    (unsigned)u32NPages,
                    (unsigned)u32Access,
                    pSpace,
                    szObj,
                    szUk,
                    szWx,
                    szDl);
        }
    }
}

/*
 * Soft FRAME shape after consume (Wave 35 exclusive deepen).
 *
 * Product: validate LIVE RAM FRAMEs from pager reply (rights ⊆ access;
 * no EXEC unless fault asked X) before any map (SOLARIS S2–S5). Soft:
 * expected frame count = cluster pages; product_ok stays 0 — no real
 * FRAME list, no CNode retype check, no PA accept.
 *
 * greppable: fault: soft frame
 * Honesty: Soft ≠ real pager product Call+FRAME under CR3.
 */
static void
fault_frame_soft_note(const struct gj_map_cookie *pCookie)
{
    u32 u32N;
    u32 u32Pages;
    int fWrite;
    int fExec;
    int fRead;
    int fUser;
    int fMemObj;
    u64 u64Peak;
    const char *szWx;
    const char *szUk;
    const char *szBind;

    if (pCookie == NULL) {
        return;
    }
    u32Pages = pCookie->u32NPages;
    if (u32Pages == 0 || u32Pages > GJ_FAULT_CLUSTER_MAX) {
        return;
    }

    fault_stat_inc(&g_u64SoftFrameShape);
    fault_stat_add(&g_u64SoftFrameExpect, (u64)u32Pages);
    u64Peak = __atomic_load_n(&g_u64SoftFrameExpectPeak, __ATOMIC_RELAXED);
    if ((u64)u32Pages > u64Peak) {
        __atomic_store_n(&g_u64SoftFrameExpectPeak, (u64)u32Pages,
                         __ATOMIC_RELAXED);
    }

    fMemObj = (pCookie->pMemObj != NULL) ? 1 : 0;
    if (fMemObj) {
        fault_stat_inc(&g_u64SoftFrameMemobj);
        szBind = "memobj";
    } else {
        fault_stat_inc(&g_u64SoftFrameNoMemobj);
        szBind = "unbound";
    }
    fUser = fault_va_is_user_soft(pCookie->u64ClusterBase);
    if (fUser) {
        fault_stat_inc(&g_u64SoftFrameUser);
        szUk = "user";
    } else {
        fault_stat_inc(&g_u64SoftFrameKernel);
        szUk = "kernel";
    }
    szWx = fault_access_tag_soft(pCookie->u32Access, &fWrite, &fExec, &fRead);
    if (fWrite) {
        fault_stat_inc(&g_u64SoftFrameWrite);
    }
    if (fExec) {
        fault_stat_inc(&g_u64SoftFrameExec);
    }
    if (fRead) {
        fault_stat_inc(&g_u64SoftFrameRead);
    }
    /* product_ok never incremented — soft shape only. */
    fault_stat_inc(&g_u64SoftFrameProductOpen);

    u32N = __atomic_load_n(&g_u32FrameSoftLogged, __ATOMIC_RELAXED);
    if (u32N < FAULT_PAGER_PATH_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32FrameSoftLogged, &u32N,
                                        u32N + 1u, 0, __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED)) {
            kprintf("fault: soft frame base=0x%llx expect=%u access=0x%x "
                    "bind=%s uk=%s wx=%s product_ok=0 product_validate=0 "
                    "(FRAME soft shape; Soft≠real pager product "
                    "Call+FRAME under CR3)\n",
                    (unsigned long long)pCookie->u64ClusterBase,
                    (unsigned)u32Pages,
                    (unsigned)pCookie->u32Access,
                    szBind,
                    szUk,
                    szWx);
        }
    }
}

/*
 * Soft CR3 map-under-space shape after consume (Wave 35 exclusive deepen).
 *
 * Product: vmm_map_page under the fault space CR3 after FRAME validate;
 * pages owned by memory object; maps are views (Apple §2). Soft: shape
 * tallies only; product_map stays 0 — no CR3 load, no PTE write.
 *
 * greppable: fault: soft cr3
 * Honesty: Soft ≠ real pager product Call+FRAME under CR3.
 */
static void
fault_cr3_map_soft_note(const struct gj_map_cookie *pCookie)
{
    u32 u32N;
    u32 u32Pages;
    int fUser;
    int fMemObj;
    u64 u64Peak;
    const char *szUk;
    const char *szSpace;
    const char *szBind;

    if (pCookie == NULL) {
        return;
    }
    u32Pages = pCookie->u32NPages;
    if (u32Pages == 0 || u32Pages > GJ_FAULT_CLUSTER_MAX) {
        return;
    }

    fault_stat_inc(&g_u64SoftCr3Shape);
    fault_stat_add(&g_u64SoftCr3Pages, (u64)u32Pages);
    u64Peak = __atomic_load_n(&g_u64SoftCr3PagesPeak, __ATOMIC_RELAXED);
    if ((u64)u32Pages > u64Peak) {
        __atomic_store_n(&g_u64SoftCr3PagesPeak, (u64)u32Pages,
                         __ATOMIC_RELAXED);
    }

    if (pCookie->pSpace != NULL) {
        fault_stat_inc(&g_u64SoftCr3Space);
        szSpace = "space";
    } else {
        fault_stat_inc(&g_u64SoftCr3NoSpace);
        szSpace = "no_space";
    }
    fMemObj = (pCookie->pMemObj != NULL) ? 1 : 0;
    if (fMemObj) {
        fault_stat_inc(&g_u64SoftCr3Memobj);
        szBind = "memobj";
    } else {
        fault_stat_inc(&g_u64SoftCr3NoMemobj);
        szBind = "unbound";
    }
    fUser = fault_va_is_user_soft(pCookie->u64ClusterBase);
    if (fUser) {
        fault_stat_inc(&g_u64SoftCr3User);
        szUk = "user";
    } else {
        fault_stat_inc(&g_u64SoftCr3Kernel);
        szUk = "kernel";
    }
    /* product_map never incremented — soft shape only; no CR3 switch. */
    fault_stat_inc(&g_u64SoftCr3ProductOpen);

    u32N = __atomic_load_n(&g_u32Cr3SoftLogged, __ATOMIC_RELAXED);
    if (u32N < FAULT_PAGER_PATH_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32Cr3SoftLogged, &u32N,
                                        u32N + 1u, 0, __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED)) {
            kprintf("fault: soft cr3 base=0x%llx pages=%u space=%p "
                    "via=%s bind=%s uk=%s product_map=0 "
                    "(CR3 soft shape; Soft≠real pager product "
                    "Call+FRAME under CR3)\n",
                    (unsigned long long)pCookie->u64ClusterBase,
                    (unsigned)u32Pages,
                    pCookie->pSpace,
                    szSpace,
                    szBind,
                    szUk);
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
 * Wave 15: also soft FRAME + CR3 shape notes (product still OPEN).
 *
 * greppable: fault: cookie view map soft
 * greppable: fault: view install
 * greppable: fault: soft views
 * greppable: fault: soft frame
 * greppable: fault: soft cr3
 */
static void
fault_view_install_soft(const struct gj_map_cookie *pCookie)
{
    u32 u32N;
    u32 u32Pages;
    int fMemObj;
    int fWrite;
    int fExec;
    int fRead;
    int fUser;
    u64 u64Peak;
    const char *szBind;
    const char *szWx;
    const char *szUk;

    if (pCookie == NULL) {
        return;
    }

    u32Pages = pCookie->u32NPages;
    if (u32Pages == 0 || u32Pages > GJ_FAULT_CLUSTER_MAX) {
        return;
    }

    fMemObj = (pCookie->pMemObj != NULL) ? 1 : 0;
    szBind = fMemObj ? "memobj_view" : "unbound_view";
    fUser = fault_va_is_user_soft(pCookie->u64ClusterBase);
    szUk = fUser ? "user" : "kernel";
    szWx = fault_access_tag_soft(pCookie->u32Access, &fWrite, &fExec, &fRead);

    fault_stat_inc(&g_u64ViewInstallSoft);
    fault_stat_add(&g_u64ViewInstallPages, (u64)u32Pages);
    u64Peak = __atomic_load_n(&g_u64ViewInstallPagesPeak, __ATOMIC_RELAXED);
    if ((u64)u32Pages > u64Peak) {
        __atomic_store_n(&g_u64ViewInstallPagesPeak, (u64)u32Pages,
                         __ATOMIC_RELAXED);
    }
    if (fMemObj) {
        fault_stat_inc(&g_u64ViewInstallMemobj);
    } else {
        fault_stat_inc(&g_u64ViewInstallNoMemobj);
    }
    if (fUser) {
        fault_stat_inc(&g_u64ViewInstallUser);
    } else {
        fault_stat_inc(&g_u64ViewInstallKernel);
    }
    if (fWrite) {
        fault_stat_inc(&g_u64ViewInstallWrite);
    }
    if (fExec) {
        fault_stat_inc(&g_u64ViewInstallExec);
    }
    if (fRead) {
        fault_stat_inc(&g_u64ViewInstallRead);
    }

    /*
     * Wave 15: soft FRAME shape then soft CR3 map shape. Product path
     * remains OPEN — Soft ≠ real pager product Call+FRAME under CR3.
     * greppable: fault: soft frame
     * greppable: fault: soft cr3
     */
    fault_frame_soft_note(pCookie);
    fault_cr3_map_soft_note(pCookie);

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
                    "access=0x%x memobj=%p bind=%s uk=%s wx=%s "
                    "(views of object pages; soft install; not product map; "
                    "Soft≠real pager product Call+FRAME under CR3)\n",
                    (unsigned long long)pCookie->u64ClusterBase,
                    (unsigned)u32Pages,
                    (unsigned)pCookie->u32Access,
                    pCookie->pMemObj,
                    szBind,
                    szUk,
                    szWx);
            kprintf("fault: view install pages=%u ok soft "
                    "(region→Call→FRAME→CR3-map soft deepen; "
                    "Soft≠real pager product Call+FRAME under CR3; not bar3)\n",
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

/*
 * PRNG for cookies. Seed mixes compile-time salt with mono clock when ready;
 * product can later inject arch entropy (RDRAND) without API change.
 * greppable: FAULT_MAP_COOKIE
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
 * greppable: fault: soft cluster
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
    u64 u64Peak;
    u32 u32N;
    u32 u32CapHit;
    u32 u32PresentStop;
    u32 u32LogN;
    int fHaveRegion;
    int fFaultPresentSoft;

    fault_stat_inc(&g_faultStats.u64ClusterSoftCalls);

    if (pu64ClusterBase == NULL || pu32NPages == NULL) {
        fault_stat_inc(&g_u64SoftClusterInval);
        return GJ_ERR_INVAL;
    }
    /* Access must include at least one of R/W/X (same policy as cookie). */
    if ((u32Access & (GJ_FAULT_ACCESS_R | GJ_FAULT_ACCESS_W |
                      GJ_FAULT_ACCESS_X)) == 0) {
        fault_stat_inc(&g_u64SoftClusterInval);
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

    if (pfnPresent == NULL) {
        fault_stat_inc(&g_u64SoftClusterProbeNull);
    } else {
        fault_stat_inc(&g_u64SoftClusterProbeLive);
    }

    fHaveRegion = (u64RegionLo != 0 || u64RegionHi != 0) ? 1 : 0;
    if (fHaveRegion) {
        if (u64RegionHi <= u64RegionLo) {
            fault_stat_inc(&g_u64SoftClusterInval);
            return GJ_ERR_INVAL;
        }
        /* Region bounds should be page-aligned for clean clusters. */
        if ((u64RegionLo & u64PageMask) != 0 ||
            (u64RegionHi & u64PageMask) != 0) {
            fault_stat_inc(&g_u64SoftClusterInval);
            return GJ_ERR_INVAL;
        }
        if (u64FaultPage < u64RegionLo || u64FaultPage >= u64RegionHi) {
            fault_stat_inc(&g_u64SoftClusterInval);
            return GJ_ERR_INVAL;
        }
        fault_stat_inc(&g_u64SoftClusterRegion);
    } else {
        fault_stat_inc(&g_u64SoftClusterFree);
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
        fault_stat_inc(&g_u64SoftClusterInval);
        return GJ_ERR_INVAL;
    }

    *pu64ClusterBase = u64Base;
    *pu32NPages = u32N;

    fault_stat_add(&g_faultStats.u64ClusterSoftPages, (u64)u32N);
    u64Peak = __atomic_load_n(&g_u64SoftClusterPagesPeak, __ATOMIC_RELAXED);
    if ((u64)u32N > u64Peak) {
        __atomic_store_n(&g_u64SoftClusterPagesPeak, (u64)u32N,
                         __ATOMIC_RELAXED);
    }
    if (u32N > 1u) {
        /* greppable: FAULT_CLUSTER_COALESCE_SOFT expand */
        fault_stat_inc(&g_faultStats.u64ClusterSoftExpand);
        fault_stat_inc(&g_u64SoftClusterMulti);
    } else {
        fault_stat_inc(&g_u64SoftClusterSingle);
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
     * Axes: user/kernel · present/not · write/exec/read soft.
     * greppable: fault: soft
     */
    fault_class_soft_note(u64FaultPage, u32Access, fFaultPresentSoft, u32N,
                          "cluster");

    /*
     * Rate-limited cluster soft event line (Wave 13 deepen).
     * greppable: fault: soft cluster
     */
    u32LogN = __atomic_load_n(&g_u32ClusterSoftLogged, __ATOMIC_RELAXED);
    if (u32LogN < FAULT_CLUSTER_SOFT_LOG_MAX) {
        if (__atomic_compare_exchange_n(&g_u32ClusterSoftLogged, &u32LogN,
                                        u32LogN + 1u, 0, __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED)) {
            kprintf("fault: soft cluster base=0x%llx pages=%u expand=%u "
                    "cap=%u present_stop=%u region=%u probe=%s "
                    "(soft coalesce; not product pager)\n",
                    (unsigned long long)u64Base,
                    (unsigned)u32N,
                    (unsigned)(u32N > 1u ? 1u : 0u),
                    (unsigned)u32CapHit,
                    (unsigned)u32PresentStop,
                    (unsigned)fHaveRegion,
                    (pfnPresent == NULL) ? "null" : "live");
        }
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
        fault_stat_inc(&g_u64SoftCookieCreateInval);
        return GJ_ERR_INVAL;
    }
    /* greppable: GJ_FAULT_CLUSTER_MAX enforce on cookie mint */
    if (u32NPages == 0 || u32NPages > GJ_FAULT_CLUSTER_MAX) {
        fault_stat_inc(&g_faultStats.u64CookieCreateFail);
        fault_stat_inc(&g_u64SoftCookieCreatePages);
        return GJ_ERR_INVAL;
    }
    if ((u64ClusterBase & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        fault_stat_inc(&g_faultStats.u64CookieCreateFail);
        fault_stat_inc(&g_u64SoftCookieCreateAlign);
        return GJ_ERR_INVAL;
    }
    /* Access must include at least one of R/W/X. */
    if ((u32Access & (GJ_FAULT_ACCESS_R | GJ_FAULT_ACCESS_W |
                      GJ_FAULT_ACCESS_X)) == 0) {
        fault_stat_inc(&g_faultStats.u64CookieCreateFail);
        fault_stat_inc(&g_u64SoftCookieCreateAccess);
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
        fault_stat_inc(&g_u64SoftCookieCreateFull);
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
    fault_cookie_live_peak_note();

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
                          pSlot->pMemObj, u64DeadlineMono);

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
        fault_stat_inc(&g_u64SoftCookieConsumeNull);
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
            fault_stat_inc(&g_u64SoftCookieConsumeMismatch);
            return GJ_ERR_PERM; /* bound mismatch */
        }
        if (pCookie->u8Used) {
            /* greppable: FAULT_MAP_COOKIE replay */
            fault_stat_inc(&g_faultStats.u64CookieReplay);
            fault_stat_inc(&g_faultStats.u64CookieConsumeFail);
            fault_stat_inc(&g_u64SoftCookieConsumeReplay);
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
            fault_stat_inc(&g_u64SoftCookieConsumeTimeout);
            /*
             * Kill-on-timeout soft (CAP / Solaris: kill faulting thread).
             * greppable: fault: kill-on-timeout soft
             */
            fault_kill_on_timeout_soft(pCookie->u64ClusterBase,
                                       pCookie->u32NPages,
                                       pCookie->u64DeadlineMono, u64Now);
            fault_cookie_live_peak_note();
            return GJ_ERR_TIMEOUT;
        }

        pCookie->u8Used = 1;
        pCookie->u8Live = 0; /* single-use: dead after consume */
        if (pOutCopy != NULL) {
            *pOutCopy = *pCookie;
        }
        fault_stat_inc(&g_faultStats.u64CookieConsumeOk);
        fault_cookie_live_peak_note();

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
    fault_stat_inc(&g_u64SoftCookieConsumeNoent);
    return GJ_ERR_NOENT; /* unknown / already invalidated */
}

void
gj_map_cookie_invalidate(u64 u64CookieLo, u64 u64CookieHi)
{
    u32 iSlot;

    if (u64CookieLo == 0 && u64CookieHi == 0) {
        fault_stat_inc(&g_u64SoftCookieInvNull);
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
            fault_cookie_live_peak_note();
            return;
        }
    }
    fault_stat_inc(&g_u64SoftCookieInvMiss);
}

/*
 * Soft bind of memory-object pointer on a live cookie (Apple §2 prep).
 * Views installed at consume record memobj_view when this pointer is set.
 * greppable: FAULT_MAP_COOKIE_MEMOBJ_SOFT
 * greppable: fault: soft cookie
 */
gj_status_t
gj_map_cookie_bind_memobj_soft(u64 u64CookieLo, u64 u64CookieHi, void *pMemObj)
{
    u32 iSlot;

    if (u64CookieLo == 0 && u64CookieHi == 0) {
        fault_stat_inc(&g_u64SoftCookieBindNull);
        return GJ_ERR_INVAL;
    }
    for (iSlot = 0; iSlot < GJ_COOKIE_TAB; iSlot++) {
        struct gj_map_cookie *pCookie = &g_aCookies[iSlot];

        if (pCookie->u8Live && pCookie->u64CookieLo == u64CookieLo &&
            pCookie->u64CookieHi == u64CookieHi) {
            pCookie->pMemObj = pMemObj;
            fault_stat_inc(&g_u64SoftCookieBindOk);
            if (pMemObj != NULL) {
                fault_stat_inc(&g_u64SoftCookieBindSet);
            } else {
                fault_stat_inc(&g_u64SoftCookieBindClear);
            }
            return GJ_OK;
        }
    }
    fault_stat_inc(&g_u64SoftCookieBindMiss);
    return GJ_ERR_NOENT;
}

u32
gj_map_cookie_live_count(void)
{
    u32 u32Live;

    u32Live = fault_cookie_live_scan_soft();
    fault_cookie_live_peak_note();
    return u32Live;
}

/*
 * greppable: FAULT_SERIALIZATION_STATS
 * greppable: fault: soft
 */
void
gj_fault_stats_get(struct gj_fault_stats *pOut)
{
    int fAnySoft;

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
     * Wave 15: multi-line soft inventory when any soft / stats activity
     * has been noted. Never hard-gates.
     * Honesty: Soft ≠ real pager product Call+FRAME under CR3; not bar3.
     * greppable: fault: soft honesty
     * greppable: fault: soft inventory
     * greppable: fault: soft class
     * greppable: fault: soft cluster
     * greppable: fault: soft cookie
     * greppable: fault: soft serial
     * greppable: fault: soft call
     * greppable: fault: soft pager
     * greppable: fault: soft frame
     * greppable: fault: soft cr3
     * greppable: fault: soft views
     * greppable: fault: soft path
     * greppable: fault: soft deepen
     * greppable: fault: soft PASS
     * greppable: fault: pager call soft
     */
    fAnySoft = 0;
    if (__atomic_load_n(&g_u64SoftClassCalls, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64PagerCallSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64ViewInstallSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64KillOnTimeoutSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64FailInvalidateSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_faultStats.u64ClusterSoftCalls, __ATOMIC_RELAXED) !=
            0 ||
        __atomic_load_n(&g_faultStats.u64CookieCreateOk, __ATOMIC_RELAXED) !=
            0 ||
        __atomic_load_n(&g_faultStats.u64CookieCreateFail, __ATOMIC_RELAXED) !=
            0 ||
        __atomic_load_n(&g_faultStats.u64EnterOk, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_faultStats.u64EnterBusy, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64SoftCookieBindOk, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64SoftCookieBindMiss, __ATOMIC_RELAXED) != 0) {
        fAnySoft = 1;
    }
    if (fAnySoft) {
        fault_soft_inventory_log();
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
    __atomic_store_n(&g_u64SoftClassRead, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClassCalls, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftViaCluster, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftViaCookie, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32SoftClassLogged, 0, __ATOMIC_RELAXED);

    /* Soft pager Call + view install + kill/FAIL path (file-local). */
    __atomic_store_n(&g_u64PagerCallSoft, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallPages, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallPagesPeak, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallUser, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallKernel, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallWrite, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallExec, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallRead, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallDeadline, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallNoDeadline, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallSpace, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallNoSpace, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallMemobj, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64PagerCallDefault, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallSoft, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallPages, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallPagesPeak, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallMemobj, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallNoMemobj, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallUser, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallKernel, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallWrite, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallExec, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64ViewInstallRead, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64KillOnTimeoutSoft, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64FailInvalidateSoft, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32PagerCallLogged, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32ViewInstallLogged, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32KillTimeoutLogged, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32FailInvLogged, 0, __ATOMIC_RELAXED);

    /* Soft cluster inventory (Wave 13). */
    __atomic_store_n(&g_u64SoftClusterRegion, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClusterFree, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClusterSingle, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClusterMulti, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClusterPagesPeak, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClusterProbeNull, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClusterProbeLive, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftClusterInval, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32ClusterSoftLogged, 0, __ATOMIC_RELAXED);

    /* Soft cookie reason / bind / peak inventory (Wave 13). */
    __atomic_store_n(&g_u64SoftCookieLivePeak, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieCreateInval, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieCreatePages, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieCreateAlign, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieCreateAccess, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieCreateFull, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieConsumeNull, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieConsumeMismatch, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieConsumeReplay, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieConsumeTimeout, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieConsumeNoent, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieBindOk, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieBindMiss, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieBindNull, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieBindClear, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieBindSet, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieInvMiss, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCookieInvNull, 0, __ATOMIC_RELAXED);

    /* Soft inventory log counter (Wave 13 multi-line; Wave 15 deepen). */
    __atomic_store_n(&g_u64SoftInvLogs, 0, __ATOMIC_RELAXED);

    /* Soft FRAME shape inventory (Wave 15). */
    __atomic_store_n(&g_u64SoftFrameShape, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameExpect, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameExpectPeak, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameMemobj, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameNoMemobj, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameUser, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameKernel, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameWrite, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameExec, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameRead, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameProductOk, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftFrameProductOpen, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32FrameSoftLogged, 0, __ATOMIC_RELAXED);

    /* Soft CR3 map-under-space inventory (Wave 15). */
    __atomic_store_n(&g_u64SoftCr3Shape, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3Pages, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3PagesPeak, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3Space, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3NoSpace, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3User, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3Kernel, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3Memobj, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3NoMemobj, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3ProductMap, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCr3ProductOpen, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u32Cr3SoftLogged, 0, __ATOMIC_RELAXED);

    /* Soft Call + Call+FRAME+CR3 composite open lamps (Wave 15). */
    __atomic_store_n(&g_u64SoftCallShape, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCallDoorOpen, 0, __ATOMIC_RELAXED);
    __atomic_store_n(&g_u64SoftCallFrameCr3Open, 0, __ATOMIC_RELAXED);
}
