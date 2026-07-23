/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Page-fault / pager protocol (kernel-side cookie + space serialization).
 *
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0.
 * Implementation: kernel/mm/fault.c.
 *
 * Scope
 * -----
 * Mechanism for demand-fill and multi-page fault clusters without treating
 * the map cookie as a CNode capability. Objects own pages; maps are views
 * (Apple channel + CAP_ADDRESSING). One fault lock per address space so
 * concurrent faults in the same space serialize (Solaris-style L1–L4).
 *
 * Design anchors
 * --------------
 *   docs/CAP_ADDRESSING.md           v1.6 pager / map cookie (not a cap)
 *   docs/SOLARIS_STYLE_REMAINING.md  §7 fault serialization; C1–C2 cluster
 *   docs/APPLE_CHANNEL_REMAINING.md  §1–2 objects vs maps; default pager
 *   docs/SECURITY_CORE_DESIGN.md     single-use secrets; fail closed; DoS
 *   docs/DESIGN_SPEC_COMPLETE.md     G-MO / G-AS fault path notes
 *
 * Protocol sketch
 * ---------------
 *   1. #PF / soft fault → region lookup → memory object (or PCB default pager)
 *   2. gj_space_fault_enter (one lock per space)
 *   3. gj_fault_cluster_coalesce_soft (region-bounded, max GJ_FAULT_CLUSTER_MAX)
 *   4. gj_map_cookie_create → fill gj_fault_msg; doors-like Call to pager
 *   5. Pager fills frames / prep → reply echoes cookie
 *   6. gj_map_cookie_consume (single-use) → install maps / bind memobj soft
 *   7. On FAIL/death/timeout → gj_map_cookie_invalidate
 *   8. gj_space_fault_leave
 *
 * Security properties (normative)
 * -------------------------------
 *   - Cookie is a kernel-only secret pair (lo/hi); not a CNode slot/gen.
 *     Userspace must not treat it as a capability (confused-deputy safe).
 *   - Consume is single-use: replay → fail (u64CookieReplay).
 *   - Optional mono deadline → GJ_ERR_TIMEOUT past ceiling.
 *   - Cluster size capped (DoS): GJ_FAULT_CLUSTER_MAX.
 *   - Zero-access / misaligned base / empty cluster rejected at create.
 *
 * Soft product surface
 * --------------------
 *   FAULT_MAP_COOKIE            — create / consume / invalidate / live count
 *   FAULT_MAP_COOKIE_MEMOBJ_SOFT— bind object pointer on live cookie
 *   FAULT_SERIALIZATION         — enter / leave / waiter soft count
 *   FAULT_CLUSTER_COALESCE_SOFT — multi-page expand with present probe
 *   FAULT_SERIALIZATION_STATS   — global counters snapshot/reset
 *
 * Layering
 * --------
 *   process.fault (gj_space_fault) until true gj_space exists
 *   memobj regions supply object + VA bounds for cluster
 *   vmm_map_page / COW break complete install after consume
 *   door Call carries gj_fault_msg (kernel → pager)
 *
 * greppable: FAULT_MAP_COOKIE FAULT_SERIALIZATION FAULT_CLUSTER_COALESCE_SOFT
 * greppable: GJ_FAULT_CLUSTER_MAX FAULT_MSG_CLUSTER FAULT_MAP_COOKIE_MEMOBJ_SOFT
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

/* Max pages per fault cluster (security + DoS bound). greppable: GJ_FAULT_CLUSTER_MAX */
#define GJ_FAULT_CLUSTER_MAX 16u

/* Access bits for fault / map (pager policy + W^X at map time). */
#define GJ_FAULT_ACCESS_R (1u << 0)
#define GJ_FAULT_ACCESS_W (1u << 1)
#define GJ_FAULT_ACCESS_X (1u << 2)

/*
 * Fault message kernel → pager (doors-like Call payload).
 *
 * u64Cookie* is a kernel-only secret; pager must echo it on reply.
 * Userspace must not treat cookie as a capability.
 *
 * u64ClusterBase  page-aligned first VA of the cluster
 * u32NPages       1 .. GJ_FAULT_CLUSTER_MAX
 * u32Access       GJ_FAULT_ACCESS_* bitmask from the fault
 * u32Flags        reserved / soft policy bits (0 in v1 soft path)
 *
 * greppable: FAULT_MSG_CLUSTER
 */
struct gj_fault_msg {
    u64 u64ClusterBase; /* page-aligned */
    u32 u32NPages;      /* 1 .. GJ_FAULT_CLUSTER_MAX */
    u32 u32Access;      /* GJ_FAULT_ACCESS_* */
    u64 u64CookieLo;
    u64 u64CookieHi;
    u32 u32Flags;
    u32 u32Pad;
};

/** Pager reply status (payload side; not gj_status_t). */
enum gj_fault_reply_status {
    GJ_FAULT_REPLY_OK   = 0,
    GJ_FAULT_REPLY_FAIL = 1,
};

/*
 * Kernel-side cookie table entry (not exposed as cap type).
 * Bound to one space + cluster; single-use; optional mono deadline.
 *
 * pSpace / pMemObj / pProc / pThread are kernel pointers (opaque void* until
 * full types are always visible). pMemObj may be set via bind_memobj_soft
 * after create, before consume.
 *
 * greppable: FAULT_MAP_COOKIE
 */
struct gj_map_cookie {
    u64  u64CookieLo;
    u64  u64CookieHi;
    u64  u64ClusterBase;
    u32  u32NPages;
    u32  u32Access;
    void *pSpace;       /* gj_space* when space exists */
    void *pMemObj;      /* memory object; pages owned here after map */
    void *pProc;        /* gj_process* */
    void *pThread;      /* faulting thread; optional until threads exist */
    u64  u64DeadlineMono; /* 0 = no deadline; else timer_mono_nsec ceiling */
    u8   u8Live;        /* 1 = valid; 0 = consumed/expired */
    u8   u8Used;        /* 1 after successful map */
};

/*
 * Space fault serialization: one lock per address space (L1–L4).
 * v1: simple flag + waiter count; full impl uses mutex + CV (Solaris-style).
 * greppable: FAULT_SERIALIZATION
 */
struct gj_space_fault {
    u32 u32FaultInProgress; /* 0 free, 1 busy */
    u32 u32Waiters;         /* soft waiter count (BUSY path bumps) */
};

/*
 * Global fault-path counters (serialization + cookie + soft cluster).
 * Atomic updates; snapshot via gj_fault_stats_get. Wrap OK; diagnostics only.
 *
 * greppable: FAULT_SERIALIZATION_STATS
 * greppable: FAULT_CLUSTER_COALESCE_SOFT
 */
struct gj_fault_stats {
    u64 u64EnterOk;             /* space fault lock acquired */
    u64 u64EnterBusy;           /* concurrent fault in same space */
    u64 u64EnterInval;          /* NULL / bad space_fault */
    u64 u64Leave;               /* lock released */
    u64 u64WaiterBump;          /* waiter count increments on BUSY */
    u64 u64CookieCreateOk;      /* live cookie minted */
    u64 u64CookieCreateFail;    /* INVAL / table full */
    u64 u64CookieConsumeOk;     /* single-use consume success */
    u64 u64CookieConsumeFail;   /* miss / mismatch / replay / timeout */
    u64 u64CookieInvalidate;    /* FAIL / death / early drop */
    u64 u64CookieTimeout;       /* consume past mono deadline */
    u64 u64CookieReplay;        /* used cookie presented again */
    u64 u64ClusterSoftCalls;    /* soft coalesce invocations */
    u64 u64ClusterSoftExpand;   /* soft coalesce grew past 1 page */
    u64 u64ClusterSoftPages;    /* sum of soft cluster page counts */
    u64 u64ClusterSoftCapHit;   /* stopped at GJ_FAULT_CLUSTER_MAX */
    u64 u64ClusterSoftPresent;  /* soft walk hit a "present" probe stop */
};

/**
 * Zero space fault serialization state (free, no waiters).
 * Safe on NULL (no-op). Call when creating/embedding gj_space_fault.
 */
static inline void
gj_space_fault_init(struct gj_space_fault *pF)
{
    if (pF == NULL) {
        return;
    }
    pF->u32FaultInProgress = 0;
    pF->u32Waiters = 0;
}

/**
 * Try to enter fault path for a space (one lock per space).
 *
 * Returns GJ_OK if acquired; GJ_ERR_BUSY if another fault is in progress
 * (bumps waiter soft count). GJ_ERR_INVAL if pF NULL.
 * Full impl: sleep on CV with mono timeout instead of BUSY.
 * greppable: FAULT_SERIALIZATION
 */
gj_status_t gj_space_fault_enter(struct gj_space_fault *pF);

/**
 * Leave fault path; wake waiters when CV path is implemented.
 * greppable: FAULT_SERIALIZATION
 */
void gj_space_fault_leave(struct gj_space_fault *pF);

/**
 * Soft multi-page cluster coalesce (SOLARIS C1–C2 / CAP multi-page cluster).
 *
 * Page-aligns the fault VA, then expands the contiguous cluster within the
 * optional region [u64RegionLo, u64RegionHi) (both 0 ⇒ no region bound soft),
 * capped at GJ_FAULT_CLUSTER_MAX.
 *
 * pfnPresent:
 *   - NULL  ⇒ soft assume all pages not-present (max soft coalesce in region)
 *   - non-NULL ⇒ stop expansion when pfnPresent(pPresentCtx, page_va) != 0
 *     (true = present / not coalesceable). Soft path only; product wires
 *     VMM present-bits later without changing this signature.
 *
 * Always covers the fault page when arguments are valid. u32Access is
 * recorded for policy callers (W^X checked at map time); soft coalesce
 * itself only needs contiguous VA intent.
 *
 * On success writes *pu64ClusterBase and *pu32NPages (required non-NULL).
 * greppable: FAULT_CLUSTER_COALESCE_SOFT
 * greppable: GJ_FAULT_CLUSTER_MAX
 */
typedef int (*gj_fault_page_present_fn)(void *pCtx, u64 u64PageVa);

gj_status_t gj_fault_cluster_coalesce_soft(u64 u64FaultVa,
                                           u64 u64RegionLo,
                                           u64 u64RegionHi,
                                           u32 u32Access,
                                           gj_fault_page_present_fn pfnPresent,
                                           void *pPresentCtx,
                                           u64 *pu64ClusterBase,
                                           u32 *pu32NPages);

/**
 * Create a single-use kernel map cookie for this cluster.
 *
 * Fills pMsg cookie fields when pMsg non-NULL; registers in kernel cookie
 * table. Rejects zero-access, misaligned base, or empty/oversized cluster.
 * u64DeadlineMono 0 = no deadline; else timer_mono_nsec ceiling at consume.
 * greppable: FAULT_MAP_COOKIE
 */
gj_status_t gj_map_cookie_create(struct gj_map_cookie *pOut,
                                 void *pSpace, void *pProc, void *pThread,
                                 u64 u64ClusterBase, u32 u32NPages,
                                 u32 u32Access, u64 u64DeadlineMono,
                                 struct gj_fault_msg *pMsg);

/**
 * Validate cookie from pager reply; must match live entry.
 *
 * On success, marks cookie used and not live (single-use); optional copy
 * to pOutCopy. Returns GJ_ERR_TIMEOUT if past u64DeadlineMono (when set);
 * fail closed on miss / mismatch / replay.
 * greppable: FAULT_MAP_COOKIE
 */
gj_status_t gj_map_cookie_consume(u64 u64CookieLo, u64 u64CookieHi,
                                  u64 u64ClusterBase, u32 u32NPages,
                                  struct gj_map_cookie *pOutCopy);

/**
 * Invalidate cookie without map (FAIL, timeout, death, early drop).
 * Idempotent soft if already dead. greppable: FAULT_MAP_COOKIE
 */
void gj_map_cookie_invalidate(u64 u64CookieLo, u64 u64CookieHi);

/**
 * Soft bind memory object pointer on a live cookie (view ownership prep).
 * No-op / NOENT if cookie not live. Does not mint caps.
 * greppable: FAULT_MAP_COOKIE_MEMOBJ_SOFT
 */
gj_status_t gj_map_cookie_bind_memobj_soft(u64 u64CookieLo, u64 u64CookieHi,
                                           void *pMemObj);

/**
 * Snapshot global fault stats (relaxed atomics) into *pOut.
 * No-op if pOut NULL. greppable: FAULT_SERIALIZATION_STATS
 */
void gj_fault_stats_get(struct gj_fault_stats *pOut);

/**
 * Zero global fault stats. greppable: FAULT_SERIALIZATION_STATS
 */
void gj_fault_stats_reset(void);

/**
 * Count currently live map cookies (table scan).
 * greppable: FAULT_MAP_COOKIE
 */
u32 gj_map_cookie_live_count(void);
