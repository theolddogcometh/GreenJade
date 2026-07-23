/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Page-fault / pager protocol:
 *   CAP_ADDRESSING v1.6 + SOLARIS_STYLE_REMAINING §7 + APPLE_CHANNEL §1–2
 *
 * - Region → memory object → pager (PCB default pager is fallback)
 * - Kernel-only map cookie (not a CNode cap)
 * - Multi-page fault cluster (region-bounded)
 * - Object owns pages; maps are views
 * - One fault lock per address space
 *
 * greppable: FAULT_MAP_COOKIE
 * greppable: FAULT_SERIALIZATION
 * greppable: FAULT_CLUSTER_COALESCE_SOFT
 * greppable: GJ_FAULT_CLUSTER_MAX
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

/* Max pages per fault cluster (security + DoS bound). greppable: GJ_FAULT_CLUSTER_MAX */
#define GJ_FAULT_CLUSTER_MAX 16u

/* Access bits for fault / map */
#define GJ_FAULT_ACCESS_R (1u << 0)
#define GJ_FAULT_ACCESS_W (1u << 1)
#define GJ_FAULT_ACCESS_X (1u << 2)

/*
 * Fault message kernel → pager (doors-like Call payload).
 * u64Cookie* is a kernel-only secret; pager must echo it on reply.
 * Userspace must not treat cookie as a capability.
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

/* Pager reply status */
enum gj_fault_reply_status {
    GJ_FAULT_REPLY_OK   = 0,
    GJ_FAULT_REPLY_FAIL = 1,
};

/*
 * Kernel-side cookie table entry (not exposed as cap type).
 * Bound to one space + cluster; single-use; optional mono deadline.
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
 * greppable: FAULT_SERIALIZATION
 */
struct gj_space_fault {
    /* v1: simple flag; full impl uses mutex + CV (Solaris-style) */
    u32 u32FaultInProgress; /* 0 free, 1 busy */
    u32 u32Waiters;         /* soft waiter count (BUSY path bumps) */
};

/*
 * Global fault-path counters (serialization + cookie + soft cluster).
 * Atomic updates; snapshot via gj_fault_stats_get.
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
 * Returns GJ_OK if acquired; GJ_ERR_BUSY if another fault is in progress.
 * Full impl: sleep on CV with mono timeout instead of BUSY.
 * greppable: FAULT_SERIALIZATION
 */
gj_status_t gj_space_fault_enter(struct gj_space_fault *pF);

/** Leave fault path; wake waiters when implemented. greppable: FAULT_SERIALIZATION */
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
 * Fills pMsg cookie fields; registers in kernel cookie table.
 * Rejects zero-access, misaligned base, or empty/oversized cluster.
 * greppable: FAULT_MAP_COOKIE
 */
gj_status_t gj_map_cookie_create(struct gj_map_cookie *pOut,
                                 void *pSpace, void *pProc, void *pThread,
                                 u64 u64ClusterBase, u32 u32NPages,
                                 u32 u32Access, u64 u64DeadlineMono,
                                 struct gj_fault_msg *pMsg);

/**
 * Validate cookie from pager reply; must match live entry.
 * On success, marks cookie used and not live (single-use).
 * Returns GJ_ERR_TIMEOUT if past u64DeadlineMono (when set).
 * greppable: FAULT_MAP_COOKIE
 */
gj_status_t gj_map_cookie_consume(u64 u64CookieLo, u64 u64CookieHi,
                                  u64 u64ClusterBase, u32 u32NPages,
                                  struct gj_map_cookie *pOutCopy);

/** Invalidate cookie without map (FAIL, timeout, death). greppable: FAULT_MAP_COOKIE */
void gj_map_cookie_invalidate(u64 u64CookieLo, u64 u64CookieHi);

/**
 * Soft bind memory object pointer on a live cookie (view ownership prep).
 * No-op / NOENT if cookie not live. Does not mint caps.
 * greppable: FAULT_MAP_COOKIE_MEMOBJ_SOFT
 */
gj_status_t gj_map_cookie_bind_memobj_soft(u64 u64CookieLo, u64 u64CookieHi,
                                           void *pMemObj);

/** Snapshot global fault stats (relaxed atomics). greppable: FAULT_SERIALIZATION_STATS */
void gj_fault_stats_get(struct gj_fault_stats *pOut);

/** Zero global fault stats. greppable: FAULT_SERIALIZATION_STATS */
void gj_fault_stats_reset(void);

/** Count currently live map cookies (table scan). greppable: FAULT_MAP_COOKIE */
u32 gj_map_cookie_live_count(void);
