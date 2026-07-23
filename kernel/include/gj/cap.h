/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Capability tables + x86_64 addressing Scheme A (docs/CAP_ADDRESSING.md).
 *
 * Handle = (u64 slot, u32 slot_gen) — two registers / two syscall args.
 * Slot 0 = root meta (reserved).
 * One CNode per process; all threads in the process share it.
 * Security: object DEAD + gen first; use checks slot gen + object gen.
 *
 * Revoke (§1.1): Phase A DEAD/gen, Phase A′ deferred slot hygiene via CDT
 * walk + structured invalidate (S7), Phase C reclaim when slots/refs/pins 0.
 * Soft CDT/quota hooks are present so mint and ledger can wire without
 * redesigning this surface (docs/SOLARIS_STYLE_REMAINING.md §2).
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

/* ---- Object lifecycle --------------------------------------------------- */

enum gj_obj_state {
    GJ_OBJ_LIVE     = 0,
    GJ_OBJ_REVOKING = 1,
    GJ_OBJ_DEAD     = 2,
};

enum gj_cap_type {
    GJ_CAP_INVALID = 0,
    GJ_CAP_CNODE,
    GJ_CAP_THREAD,
    GJ_CAP_SPACE,
    GJ_CAP_PROCESS, /* task port — parent mint on spawn; not root meta */
    GJ_CAP_ENDPOINT,
    GJ_CAP_NOTIFICATION,
    GJ_CAP_REPLY, /* kernel ephemeral single-use (Call) */
    GJ_CAP_IRQ,
    GJ_CAP_FRAME,
    GJ_CAP_UNTYPED,
    GJ_CAP_PAGE_TABLE,
    GJ_CAP_SCHED_CONTEXT,
    GJ_CAP_MEMORY_OBJECT, /* Apple channel: owns pages; maps are views */
    GJ_CAP_ROOT_META,     /* slot 0 only — process/root meta */
};

/* Rights bitmask (generic + process/task where noted) */
#define GJ_RIGHT_READ     (1u << 0)
#define GJ_RIGHT_WRITE    (1u << 1)
#define GJ_RIGHT_GRANT    (1u << 2)
#define GJ_RIGHT_MINT     (1u << 3)
#define GJ_RIGHT_DESTROY  (1u << 4)  /* includes KILL on PROCESS */
#define GJ_RIGHT_MAP      (1u << 5)
#define GJ_RIGHT_IDENTIFY (1u << 6)
#define GJ_RIGHT_WAIT     (1u << 7)  /* PROCESS: reap / wait */
#define GJ_RIGHT_VM       (1u << 8)  /* PROCESS: default pager / regions */
#define GJ_RIGHT_SPAWN    (1u << 9)  /* PROCESS: further spawn under ledger */
#define GJ_RIGHT_INSPECT  (1u << 10) /* PROCESS: debug — off by default */
#define GJ_RIGHT_JIT      (1u << 11) /* task/space: MAP_JIT / RWX exception */

#define GJ_CNODE_SLOTS_DEFAULT 1024u
#define GJ_CAP_SLOT_ROOT_META  0ull /* reserved root meta index */

/*
 * Scheme A user handle: not packed into one u64.
 * Syscall: arg0 = u64Slot, arg1 = u32SlotGen.
 * Null handle: u32SlotGen == 0 (live slots use gen >= 1).
 */
struct gj_cap_ref {
    u64 u64Slot;
    u32 u32SlotGen;
};

static inline int
gj_cap_ref_is_null(const struct gj_cap_ref *pRef)
{
    return pRef == NULL || pRef->u32SlotGen == 0;
}

static inline struct gj_cap_ref
gj_cap_ref_make(u64 u64Slot, u32 u32SlotGen)
{
    struct gj_cap_ref r;

    r.u64Slot = u64Slot;
    r.u32SlotGen = u32SlotGen;
    return r;
}

static inline struct gj_cap_ref
gj_cap_ref_null(void)
{
    return gj_cap_ref_make(0, 0);
}

/*
 * One slot in a CNode.
 * u32Gen     = slot generation (handle.u32SlotGen)
 * u32ObjGen  = object generation at install
 */
struct gj_cap_slot {
    u16  u16Type;
    u16  u16Rights;
    u32  u32Gen;
    u32  u32ObjGen;
    void *pObj;
};

/* Forward: CNode used by soft CDT edges before full struct is defined. */
struct gj_cnode;
struct gj_cdt_edge;

/*
 * Object header — first field of every kernel cap object.
 * u32SlotsLeft: outstanding derived CNode bindings (S4/S6).
 * pCdtHead:     soft CDT child-edge list (NULL until mint wires edges).
 */
struct gj_obj_hdr {
    u32                 u32State;
    u32                 u32Gen;
    u32                 u32Ref;
    u32                 u32Pin;
    u32                 u32SlotsLeft;
    struct gj_cdt_edge *pCdtHead; /* soft CDT; see gj_cdt_edge_link */
};

/*
 * Soft CDT child edge: one derived slot binding object ← CNode[slot].
 * Storage is mint/slab-owned; link/unlink only manipulate the list.
 * Full walk lands in gj_revoke_cdt_walk_batch (revoke.c).
 */
struct gj_cdt_edge {
    struct gj_cdt_edge *pNext;
    struct gj_cnode    *pCnode;
    u64                 u64Slot;
};

/* One CNode per process; all threads of that process share this table. */
struct gj_cnode {
    struct gj_obj_hdr    hdr;
    u64                  cSlots;   /* table length; index is u64 */
    struct gj_cap_slot  *pSlots;
    /*
     * Soft process slot-quota account (hierarchical ledger later).
     * NULL ⇒ charge/refund hooks no-op. Grep: cap:quota soft
     */
    void                *pQuotaAccount;
};

struct gj_cap_resolved {
    struct gj_cap_slot *pSlot;
    struct gj_obj_hdr  *pObj;
    u16                 u16Type;
    u16                 u16Rights;
};

/* ---- Live check (S2) ---------------------------------------------------- */

static inline gj_status_t
gj_obj_check_live(const struct gj_obj_hdr *pObj, u32 u32ObjGenExpected)
{
    if (pObj == NULL) {
        return GJ_ERR_INVAL;
    }
    if (pObj->u32State != (u32)GJ_OBJ_LIVE) {
        return GJ_ERR_DEAD;
    }
    if (pObj->u32Gen != u32ObjGenExpected) {
        return GJ_ERR_STALE_CAP;
    }
    return GJ_OK;
}

/*
 * S7 structured slot field clear (type/gen/ptr). Does not touch CDT or
 * slots_left — callers that know the object should use
 * gj_cap_slot_invalidate_locked so accounting stays single-pathed.
 */
static inline void
gj_cap_slot_invalidate(struct gj_cap_slot *pSlot)
{
    if (pSlot == NULL) {
        return;
    }
    pSlot->u16Type = (u16)GJ_CAP_INVALID;
    pSlot->u16Rights = 0;
    pSlot->u32Gen++;
    if (pSlot->u32Gen == 0) {
        pSlot->u32Gen = 1; /* skip 0 — reserved for null handles */
    }
    pSlot->u32ObjGen = 0;
    pSlot->pObj = NULL;
}

/* ---- CNode / resolve (cnode.c) ------------------------------------------ */

void gj_obj_hdr_init(struct gj_obj_hdr *pHdr);

void gj_cnode_init(struct gj_cnode *pCnode, struct gj_cap_slot *pSlots,
                   u64 cSlots);

/**
 * Resolve Scheme A handle against the process CNode (shared by its threads).
 */
gj_status_t gj_cap_resolve(struct gj_cnode *pProcCnode, u64 u64Slot,
                           u32 u32SlotGen, struct gj_cap_resolved *pOut);

static inline gj_status_t
gj_cap_resolve_ref(struct gj_cnode *pProcCnode, const struct gj_cap_ref *pRef,
                   struct gj_cap_resolved *pOut)
{
    if (pRef == NULL) {
        return GJ_ERR_INVAL;
    }
    return gj_cap_resolve(pProcCnode, pRef->u64Slot, pRef->u32SlotGen, pOut);
}

/**
 * Install into a free/invalid slot. Slot 0 (root meta) only allows
 * GJ_CAP_ROOT_META unless fAllowRootMeta path used by bootstrap.
 * Soft: charges pCnode->pQuotaAccount when non-NULL; bumps u32SlotsLeft.
 * CDT edge link is caller's responsibility when edges are allocated.
 */
gj_status_t gj_cap_slot_install(struct gj_cnode *pCnode, u64 u64Slot,
                                u16 u16Type, u16 u16Rights,
                                struct gj_obj_hdr *pObj,
                                struct gj_cap_ref *pOutRef);

/**
 * Find free slot starting at 1 (skips root meta), install, return ref.
 */
gj_status_t gj_cap_alloc_install(struct gj_cnode *pCnode, u16 u16Type,
                                 u16 u16Rights, struct gj_obj_hdr *pObj,
                                 struct gj_cap_ref *pOutRef);

/**
 * Soft CDT: push edge at head of pObj->pCdtHead. Edge storage is caller-owned.
 * No-op-safe: NULL args return INVAL. Duplicate link of same edge is refused.
 */
gj_status_t gj_cdt_edge_link(struct gj_obj_hdr *pObj, struct gj_cdt_edge *pEdge,
                             struct gj_cnode *pCnode, u64 u64Slot);

/**
 * Soft CDT: remove one edge from pObj's child list (does not free edge).
 */
void gj_cdt_edge_unlink(struct gj_obj_hdr *pObj, struct gj_cdt_edge *pEdge);

/**
 * Soft CDT: unlink any edge on pObj that names this CNode slot (S7 companion).
 * Safe when pCdtHead is NULL (no edges wired yet).
 */
void gj_cdt_unlink_slot(struct gj_obj_hdr *pObj, struct gj_cnode *pCnode,
                        u64 u64Slot);

/**
 * Soft quota hooks — hierarchical accounts land later (M2.2).
 * pAccount NULL or unwired ⇒ GJ_OK (no charge). Grep: cap:quota soft
 */
gj_status_t gj_cap_quota_slot_charge(void *pAccount);
gj_status_t gj_cap_quota_slot_refund(void *pAccount);

/**
 * Deferred invalidate helper: scan pCnode for slots pointing at pObj and
 * apply structured invalidate (bounded by u32MaxSlots). Use when the CNode
 * is known (process death / local hygiene) without a full CDT walk.
 * Returns number of slots cleared.
 */
u32 gj_cnode_invalidate_obj_slots(struct gj_cnode *pCnode,
                                  struct gj_obj_hdr *pObj, u32 u32MaxSlots);

/* ---- Revoke API (revoke.c) ---------------------------------------------- */

/**
 * Phase A: CAS LIVE→REVOKING, then DEAD + bump gen (S1). Enqueues deferred
 * slot hygiene. Concurrent revoke → DEAD; queue full → AGAIN (object still
 * secure).
 */
gj_status_t gj_obj_revoke_begin(struct gj_obj_hdr *pObj);

/**
 * Phase A′ driver (R2/R7): bounded deferred CDT/slot work. Call from timer,
 * idle, or syscall exit. Returns slots cleared this invocation.
 */
u32 gj_revoke_process_deferred(u32 u32MaxSlots);

/**
 * Soft CDT walk batch: for each edge on pObj, try structured invalidate of
 * the named slot (try-lock stub until CNode mutex exists). Unlinks cleared
 * edges. Returns slots cleared. Grep: cap:cdt walk
 */
u32 gj_revoke_cdt_walk_batch(struct gj_obj_hdr *pObj, u32 u32MaxSlots);

/** Active entries on the deferred revoke queue (R7 progress observability). */
u32 gj_revoke_deferred_pending(void);

int         gj_obj_reclaim_ready(const struct gj_obj_hdr *pObj);
gj_status_t gj_obj_reclaim(struct gj_obj_hdr *pObj);

/**
 * Structured slot invalidate with object match + slots_left/quota/CDT
 * accounting (S7). Caller holds CNode lock in full impl. If pObj non-NULL,
 * only touch a slot still pointing at that object.
 */
void gj_cap_slot_invalidate_locked(struct gj_cap_slot *pSlot,
                                   struct gj_obj_hdr *pObj);
