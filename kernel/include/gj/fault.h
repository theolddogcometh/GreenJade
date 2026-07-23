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
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

/* Max pages per fault cluster (security + DoS bound). */
#define GJ_FAULT_CLUSTER_MAX 16u

/* Access bits for fault / map */
#define GJ_FAULT_ACCESS_R (1u << 0)
#define GJ_FAULT_ACCESS_W (1u << 1)
#define GJ_FAULT_ACCESS_X (1u << 2)

/*
 * Fault message kernel → pager (doors-like Call payload).
 * u64Cookie* is a kernel-only secret; pager must echo it on reply.
 * Userspace must not treat cookie as a capability.
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

/* Space fault serialization: one lock per address space (L1–L4). */
struct gj_space_fault {
    /* v1: simple flag; full impl uses mutex + CV (Solaris-style) */
    u32 u32FaultInProgress; /* 0 free, 1 busy */
    u32 u32Waiters;         /* optional wait count */
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
 */
gj_status_t gj_space_fault_enter(struct gj_space_fault *pF);

/** Leave fault path; wake waiters when implemented. */
void gj_space_fault_leave(struct gj_space_fault *pF);

/**
 * Create a single-use kernel map cookie for this cluster.
 * Fills pMsg cookie fields; registers in kernel cookie table.
 * Rejects zero-access, misaligned base, or empty/oversized cluster.
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
 */
gj_status_t gj_map_cookie_consume(u64 u64CookieLo, u64 u64CookieHi,
                                  u64 u64ClusterBase, u32 u32NPages,
                                  struct gj_map_cookie *pOutCopy);

/** Invalidate cookie without map (FAIL, timeout, death). */
void gj_map_cookie_invalidate(u64 u64CookieLo, u64 u64CookieHi);
