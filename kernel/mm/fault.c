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
 * Soft deepen (Wave 13 exclusive; this unit only):
 *   Multi-line greppable "fault: soft …" inventory + pager soft axes:
 *     inventory | class | cluster | cookie | serial | pager | views | path
 *     deepen wave=13 stamp
 *   Cluster soft: region|free, single|multi, pages peak, probe null|live
 *   Cookie soft: create/consume fail reason lamps + live peak + bind soft
 *   Pager soft: user|kernel, r|write|exec, deadline|none, space|null,
 *               default_pager|memobj, pages peak
 *   Views soft: bind axes + access tag + pages peak
 *   Never hard-gates; wrap OK. Soft ≠ product pager; soft ≠ bar3.
 *
 * greppable: FAULT_MAP_COOKIE
 * greppable: FAULT_SERIALIZATION
 * greppable: FAULT_SERIALIZATION_STATS
 * greppable: FAULT_CLUSTER_COALESCE_SOFT
 * greppable: GJ_FAULT_CLUSTER_MAX
 * greppable: fault: soft
 * greppable: fault: soft inventory
 * greppable: fault: soft class
 * greppable: fault: soft cluster
 * greppable: fault: soft cookie
 * greppable: fault: soft serial
 * greppable: fault: soft pager
 * greppable: fault: soft views
 * greppable: fault: soft path
 * greppable: fault: soft deepen
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

/* Wave 13 soft inventory stamp (file-local; never product gate). */
#define FAULT_SOFT_WAVE 13u

/* Soft inventory area count (fixed greppable categories for deepen stamp). */
#define FAULT_SOFT_AREAS 8u

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

/* Soft inventory dump emissions (Wave 13 multi-line). */
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
 * Wave 13 multi-line greppable soft inventory.
 * Prefix-stable "fault: soft …" (agent / product greps):
 *   fault: soft inventory | class | cluster | cookie | serial |
 *          pager | views | path | deepen
 * greppable: fault: soft
 * Honesty: soft inventory only — not product pager; not bar3.
 */
static void
fault_soft_inventory_log(void)
{
    u32 u32Live;
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

    fault_stat_inc(&g_u64SoftInvLogs);
    u64Logs = __atomic_load_n(&g_u64SoftInvLogs, __ATOMIC_RELAXED);
    u32Live = fault_cookie_live_scan_soft();
    fault_cookie_live_peak_note();

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

    /* Grep: fault: soft inventory */
    kprintf("fault: soft inventory cookie_tab=%u cluster_max=%u live=%u "
            "live_peak=%llu class_calls=%llu pager_calls=%llu views=%llu "
            "logs=%llu wave=%u "
            "(soft; not product pager; not bar3)\n",
            (unsigned)GJ_COOKIE_TAB,
            (unsigned)GJ_FAULT_CLUSTER_MAX,
            (unsigned)u32Live,
            (unsigned long long)__atomic_load_n(&g_u64SoftCookieLivePeak,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64SoftClassCalls,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64PagerCallSoft,
                                                __ATOMIC_RELAXED),
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallSoft,
                                                __ATOMIC_RELAXED),
            (unsigned long long)u64Logs,
            (unsigned)FAULT_SOFT_WAVE);

    /* Grep: fault: soft class */
    fault_class_soft_log();

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

    /* Grep: fault: soft serial */
    kprintf("fault: soft serial enter_ok=%llu enter_busy=%llu "
            "enter_inval=%llu leave=%llu waiter_bump=%llu "
            "(FAULT_SERIALIZATION; soft only)\n",
            (unsigned long long)u64EnterOk,
            (unsigned long long)u64EnterBusy,
            (unsigned long long)u64EnterInval,
            (unsigned long long)u64Leave,
            (unsigned long long)u64Waiter);

    /* Grep: fault: soft pager */
    kprintf("fault: soft pager calls=%llu pages=%llu pages_peak=%llu "
            "user=%llu kernel=%llu write=%llu exec=%llu read=%llu "
            "deadline=%llu no_deadline=%llu space=%llu no_space=%llu "
            "memobj=%llu default_pager=%llu "
            "(pager soft; not product door Call)\n",
            (unsigned long long)__atomic_load_n(&g_u64PagerCallSoft,
                                                __ATOMIC_RELAXED),
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

    /* Grep: fault: soft views */
    kprintf("fault: soft views install=%llu pages=%llu pages_peak=%llu "
            "memobj=%llu no_memobj=%llu user=%llu kernel=%llu "
            "write=%llu exec=%llu read=%llu "
            "kill_timeout=%llu fail_inv=%llu "
            "(views of object pages; soft install; not product map)\n",
            (unsigned long long)__atomic_load_n(&g_u64ViewInstallSoft,
                                                __ATOMIC_RELAXED),
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

    /* Grep: fault: soft path */
    kprintf("fault: soft path create→Call→views|FAIL|kill-timeout "
            "cookie_secret=1 single_use=1 cluster_cap=%u "
            "door_call=OPEN frame_map=OPEN product_pager=OPEN "
            "bar3=OPEN (honesty; soft only)\n",
            (unsigned)GJ_FAULT_CLUSTER_MAX);

    /* Grep: fault: soft deepen wave */
    kprintf("fault: soft deepen wave=%u areas=%u logs=%llu "
            "(Wave 13 exclusive; not product pager; not bar3)\n",
            (unsigned)FAULT_SOFT_WAVE,
            (unsigned)FAULT_SOFT_AREAS,
            (unsigned long long)u64Logs);

    /* Keep legacy single-line pager inventory (prefix-stable greps). */
    if (__atomic_load_n(&g_u64PagerCallSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64ViewInstallSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64KillOnTimeoutSoft, __ATOMIC_RELAXED) != 0 ||
        __atomic_load_n(&g_u64FailInvalidateSoft, __ATOMIC_RELAXED) != 0) {
        fault_pager_path_inventory_log();
    }
}

/*
 * Soft "Call pager" after a live cookie is minted (doors-like Call shape).
 * Product: ipc_call(pager, fault_msg) with mono timeout; pager never
 * ambient-maps the client. Soft: tally + greppable only.
 * greppable: fault: pager call soft
 * greppable: fault: soft pager
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
 * Soft install views of object pages after single-use cookie consume.
 *
 * Product path (open): validate FRAMEs from pager reply, then vmm_map_page
 * under the fault space CR3; pages owned by memory object; maps are views
 * (Apple §2 / CAP §1.6). Soft path: record view-install counts + greppable
 * success — no ambient PTE write without validated frames (fail closed soft).
 *
 * greppable: fault: cookie view map soft
 * greppable: fault: view install
 * greppable: fault: soft views
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
                    "(views of object pages; soft install; not product map)\n",
                    (unsigned long long)pCookie->u64ClusterBase,
                    (unsigned)u32Pages,
                    (unsigned)pCookie->u32Access,
                    pCookie->pMemObj,
                    szBind,
                    szUk,
                    szWx);
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
     * Wave 13: multi-line soft inventory when any soft / stats activity
     * has been noted. Never hard-gates.
     * Honesty: soft region→pager→views deepen; not product pager; not bar3.
     * greppable: fault: soft inventory
     * greppable: fault: soft class
     * greppable: fault: soft cluster
     * greppable: fault: soft cookie
     * greppable: fault: soft serial
     * greppable: fault: soft pager
     * greppable: fault: soft views
     * greppable: fault: soft path
     * greppable: fault: soft deepen
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

    /* Soft inventory log counter (Wave 13 multi-line dumps). */
    __atomic_store_n(&g_u64SoftInvLogs, 0, __ATOMIC_RELAXED);
}
