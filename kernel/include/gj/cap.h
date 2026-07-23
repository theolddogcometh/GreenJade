/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Capability tables + x86_64 addressing Scheme A (docs/CAP_ADDRESSING.md).
 *
 * Authority model (SECURITY_CORE_DESIGN §1):
 *   seL4-style typed caps in CNodes; untyped retype; CDT + revoke §1.1.
 *   No ambient root after bootstrap seal; every act needs a cap.
 *   Mint never escalates rights; Grant required to copy/move authority.
 *
 * Handle = (u64 slot, u32 slot_gen) — two registers / two syscall args.
 *   Slot 0 = root meta (reserved; GJ_CAP_ROOT_META only; kernel ops).
 *   One CNode per process; all threads in the process share it.
 *   Null handle: u32SlotGen == 0 (live slots use gen >= 1).
 *   Never: packed (gen<<32)|slot; never hardware tagged pointers.
 *
 * Security — object DEAD + gen first (S1/S2):
 *   Every use checks slot gen + object LIVE + object gen.
 *   Uncleared slots after revoke must not grant success (fail closed).
 *   Structured slot invalidate (S7): type INVALID, bump slot gen (skip 0),
 *   clear rights/obj, unlink CDT edge, refund quota.
 *
 * Revoke (§1.1) — split invalidate from reclaim (no stop-the-world):
 *   Phase A   — CAS LIVE→REVOKING, then DEAD + bump gen; enqueue hygiene
 *   Phase A′  — deferred CDT walk + structured invalidate (S4/S7); try-lock
 *               CNode and defer if busy (R2); driven until all slots cleared
 *   Phase C   — reclaim when slots_left / refs / pins are 0
 * Soft CDT/quota hooks are present so mint and ledger wire without
 * redesigning this surface (docs/SOLARIS_STYLE_REMAINING.md §2).
 *
 * Lock order (full impl; soft stubs prepare the same topology):
 *   CNode → Object → Endpoint   (never reverse; R1)
 * Soft CNode try-lock: u32SoftLock (R2 slot hygiene). Grep: cap:cdt trylock
 *
 * Grep markers (cap surface — keep stable for smokes / tooling):
 *   cap:cdt           — soft CDT edges, walk, deferred hygiene
 *   cap:cdt trylock   — CNode soft try-lock / unlock
 *   cap:cdt walk      — gj_revoke_cdt_walk_batch
 *   cap:cdt deferred  — gj_revoke_process_deferred
 *   cap:quota         — flat/hierarchical slot quota ledger
 *   cap:quota parent  — hierarchical roll-up via pParent
 *   cap:quota soft    — charge/refund no-op when account NULL
 *
 * Implementation:
 *   kernel/cap/cnode.c  — resolve, install, mint/copy/move/delete, CDT, quota
 *   kernel/cap/revoke.c — Phase A begin, deferred hygiene, reclaim
 * This header is the normative API surface; race/ordering detail lives in .c.
 *
 * Companion docs:
 *   docs/CAP_ADDRESSING.md
 *   docs/SECURITY_CORE_DESIGN.md §1 / §1.1 / §2
 *   docs/SOLARIS_STYLE_REMAINING.md §2 (CDT/quota soft path)
 *   docs/DESIGN_SPEC_COMPLETE.md (doors as ENDPOINT install)
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

/* ---- Object lifecycle --------------------------------------------------- */

/**
 * Object header state machine (security-visible).
 *
 * LIVE     — usable; resolve + gen match may succeed
 * REVOKING — one revoker won CAS; transitional (R6); use still fails closed
 *            once DEAD is published (S1 does DEAD+gen before long work)
 * DEAD     — all use paths return GJ_ERR_DEAD / STALE; reclaim later (Phase C)
 */
enum gj_obj_state {
    GJ_OBJ_LIVE     = 0,
    GJ_OBJ_REVOKING = 1,
    GJ_OBJ_DEAD     = 2,
};

/**
 * Capability type tags stored in slot.u16Type.
 *
 * INVALID      — empty / free after structured invalidate
 * CNODE        — nested table (v1 product path is one process CNode; type kept)
 * THREAD       — thread control object
 * SPACE        — address space
 * PROCESS      — task port: parent mint on spawn; not root meta (CAP_ADDRESSING)
 * ENDPOINT     — doors-shaped sync IPC (see door.h / G-DOOR-1)
 * NOTIFICATION — async badge bitmask pulse/wait (see notify.h)
 * REPLY        — kernel ephemeral single-use reply (Call path; scaffold)
 * IRQ          — interrupt binding authority
 * FRAME        — physical frame
 * UNTYPED      — retype source (RAM / device regions)
 * PAGE_TABLE   — paging structure
 * SCHED_CONTEXT— CPU budget / period authority
 * MEMORY_OBJECT— Apple channel: owns pages; maps are views
 * ROOT_META    — slot 0 only; process/root identity; kernel ops only (K1–K6)
 */
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

/*
 * Rights bitmask (generic + process/task where noted).
 * Mint/copy may only pass a subset of the source rights (no escalation).
 */
#define GJ_RIGHT_READ     (1u << 0)
#define GJ_RIGHT_WRITE    (1u << 1)
#define GJ_RIGHT_GRANT    (1u << 2)  /* copy/move authority to another slot */
#define GJ_RIGHT_MINT     (1u << 3)  /* derive weaker/equal rights + CDT edge */
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
 *
 * SMEP/SMAP: handles are never kernel pointers. Resolve always goes
 * process CNode → slot → object under LIVE+gen checks.
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
 *
 * u16Type    — gj_cap_type (INVALID ⇒ free for install)
 * u16Rights  — rights bitmask for this binding
 * u32Gen     — slot generation (handle.u32SlotGen); bump on invalidate; skip 0
 * u32ObjGen  — object generation captured at install (S2 match)
 * pObj       — object pointer; NULL when INVALID
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
struct gj_cap_quota;

/*
 * Object header — first field of every kernel cap object.
 *
 * u32State     — gj_obj_state (LIVE / REVOKING / DEAD)
 * u32Gen       — object generation; starts at 1; bump on DEAD (S1); 0 reserved
 * u32Ref       — kernel refs held across use; Phase C waits for 0
 * u32Pin       — short-term pins (e.g. in-flight op); Phase C waits via sleep
 * u32SlotsLeft — outstanding derived CNode bindings (S4/S6 accounting)
 * pCdtHead     — soft CDT child-edge list (NULL until mint wires edges)
 *
 * Grep: cap:cdt
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
 *
 * Storage is mint/slab-owned; link/unlink only manipulate the list.
 * Full walk lands in gj_revoke_cdt_walk_batch (revoke.c). Edges whose CNode
 * try-lock is busy are left for a later deferred pass (R2) — security is
 * already done once the object is DEAD.
 *
 * Grep: cap:cdt
 */
struct gj_cdt_edge {
    struct gj_cdt_edge *pNext;
    struct gj_cnode    *pCnode;
    u64                 u64Slot;
};

/*
 * One CNode per process; all threads of that process share this table.
 *
 * hdr          — CNode is itself a cap object (state/gen/ref/pin/CDT)
 * cSlots       — table length; index space is u64 (Scheme A headroom)
 * pSlots       — slot array (caller-owned storage for static/bootstrap tables)
 * pQuotaAccount— soft process slot-quota account (hierarchical via pParent);
 *                NULL ⇒ charge/refund hooks no-op. Grep: cap:quota soft
 * u32SoftLock  — soft try-lock stub for R2 slot hygiene (0 free, 1 held).
 *                Full impl replaces with a real mutex; order CNode → Object →
 *                Endpoint. Grep: cap:cdt trylock
 * u32PadLock   — align / future waiter bits
 */
struct gj_cnode {
    struct gj_obj_hdr    hdr;
    u64                  cSlots;   /* table length; index is u64 */
    struct gj_cap_slot  *pSlots;
    /*
     * Soft process slot-quota account (hierarchical ledger via pParent).
     * NULL ⇒ charge/refund hooks no-op. Grep: cap:quota soft
     */
    void                *pQuotaAccount;
    u32                  u32SoftLock; /* 0 free, 1 held — soft stub */
    u32                  u32PadLock;  /* align / future waiter bits */
};

/**
 * Result of a successful resolve: live slot binding + object header view.
 * Caller must still enforce type-specific rights for the intended op.
 */
struct gj_cap_resolved {
    struct gj_cap_slot *pSlot;
    struct gj_obj_hdr  *pObj;
    u16                 u16Type;
    u16                 u16Rights;
};

/* ---- Live check (S2) ---------------------------------------------------- */

/**
 * Fail-closed live check used on every use path after resolve (S2).
 *
 *   GJ_ERR_INVAL     — null object
 *   GJ_ERR_DEAD      — state != LIVE (includes REVOKING once not LIVE)
 *   GJ_ERR_STALE_CAP — object gen != expected (slot.u32ObjGen or caller's)
 *   GJ_OK            — LIVE and gen match
 *
 * Does not inspect slot gen — resolve does that first.
 */
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
 *
 * Slot gen bumps and skips 0 so null handles stay distinguishable.
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

/**
 * Initialize object header to LIVE, gen=1, zero ref/pin/slots_left, no CDT.
 * gen 0 is reserved for null / never-live.
 */
void gj_obj_hdr_init(struct gj_obj_hdr *pHdr);

/**
 * Initialize CNode over caller-provided slot storage.
 * All slots start INVALID with gen=1; slot 0 remains reserved for root meta
 * (still INVALID until bootstrap installs GJ_CAP_ROOT_META). Soft lock free;
 * pQuotaAccount NULL until ledger attach.
 */
void gj_cnode_init(struct gj_cnode *pCnode, struct gj_cap_slot *pSlots,
                   u64 cSlots);

/**
 * Soft CNode try-lock (R2). Returns 1 if acquired, 0 if busy / NULL.
 * Soft stub uses u32SoftLock atomic; full impl → real mutex.
 * Never spin unbounded for slot hygiene — defer instead (R2/R7).
 * Grep: cap:cdt trylock
 */
int  gj_cnode_trylock(struct gj_cnode *pCnode);
void gj_cnode_unlock(struct gj_cnode *pCnode);

/**
 * Resolve Scheme A handle against the process CNode (shared by its threads).
 *
 * Checks (fail closed):
 *   range, slot gen match, type not INVALID, object non-NULL,
 *   LIVE + object gen == slot.u32ObjGen (via S2).
 *
 * Common statuses: GJ_OK, GJ_ERR_INVAL, GJ_ERR_NOENT, GJ_ERR_STALE_CAP,
 * GJ_ERR_DEAD.
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
 * GJ_CAP_ROOT_META unless bootstrap uses a privileged path.
 *
 * Soft: charges pCnode->pQuotaAccount when non-NULL (GJ_ERR_QUOTA if
 * exhausted); bumps pObj->u32SlotsLeft. CDT edge link is caller's
 * responsibility when edges are allocated (mint wires via edge pool).
 *
 * On success fills *pOutRef with (slot, new slot gen).
 */
gj_status_t gj_cap_slot_install(struct gj_cnode *pCnode, u64 u64Slot,
                                u16 u16Type, u16 u16Rights,
                                struct gj_obj_hdr *pObj,
                                struct gj_cap_ref *pOutRef);

/**
 * Find free slot starting at 1 (skips root meta), install, return ref.
 * Same soft quota / slots_left behavior as gj_cap_slot_install.
 */
gj_status_t gj_cap_alloc_install(struct gj_cnode *pCnode, u16 u16Type,
                                 u16 u16Rights, struct gj_obj_hdr *pObj,
                                 struct gj_cap_ref *pOutRef);

/**
 * Soft CDT: push edge at head of pObj->pCdtHead. Edge storage is caller-owned.
 * No-op-safe: NULL args return INVAL. Duplicate link of same edge is refused.
 * Grep: cap:cdt
 */
gj_status_t gj_cdt_edge_link(struct gj_obj_hdr *pObj, struct gj_cdt_edge *pEdge,
                             struct gj_cnode *pCnode, u64 u64Slot);

/**
 * Soft CDT: remove one edge from pObj's child list (does not free edge).
 * Grep: cap:cdt
 */
void gj_cdt_edge_unlink(struct gj_obj_hdr *pObj, struct gj_cdt_edge *pEdge);

/**
 * Soft CDT: unlink any edge on pObj that names this CNode slot (S7 companion).
 * Safe when pCdtHead is NULL (no edges wired yet).
 * Grep: cap:cdt
 */
void gj_cdt_unlink_slot(struct gj_obj_hdr *pObj, struct gj_cnode *pCnode,
                        u64 u64Slot);

/**
 * Soft process slot quota ledger with optional hierarchical parent.
 * Attach via gj_cap_quota_attach. Charge rolls up pParent (zone-like).
 * Grep: cap:quota
 *
 * u32Limit     — max occupied slots charged at this node
 * u32Used      — currently charged at this node
 * u32Exhaust   — times charge returned QUOTA (this node)
 * u32HighWater — peak u32Used since init (observability)
 * u32ChargeOk  — successful slot charges (observability)
 * u32RefundOk  — successful slot refunds (observability)
 * pParent      — soft hierarchical parent; NULL = flat leaf/root
 *
 * SECURITY_CORE §2: no create without quota when an account is attached.
 * NULL account remains a soft no-op so early bring-up can install before
 * ledger attach without redesigning call sites.
 */
struct gj_cap_quota {
    u32                 u32Limit;
    u32                 u32Used;
    u32                 u32Exhaust;
    u32                 u32HighWater;
    u32                 u32ChargeOk;
    u32                 u32RefundOk;
    struct gj_cap_quota *pParent; /* soft hierarchy; grep: cap:quota parent */
};

/** Max parent-chain depth for soft roll-up (cycle guard). Grep: cap:quota */
#define GJ_CAP_QUOTA_DEPTH_MAX 8u

void gj_cap_quota_init(struct gj_cap_quota *pQ, u32 u32Limit);
void gj_cap_quota_attach(struct gj_cnode *pCnode, struct gj_cap_quota *pQ);

/**
 * Soft hierarchical link: pQ charges roll up into pParent.
 * NULL parent → flat. Refuses simple self/cycle. Grep: cap:quota parent
 */
void gj_cap_quota_set_parent(struct gj_cap_quota *pQ,
                             struct gj_cap_quota *pParent);

u32  gj_cap_quota_used(const struct gj_cap_quota *pQ);
u32  gj_cap_quota_limit(const struct gj_cap_quota *pQ);
u32  gj_cap_quota_highwater(const struct gj_cap_quota *pQ);
u32  gj_cap_quota_exhaust_count(const struct gj_cap_quota *pQ);

/**
 * Soft quota hooks. pAccount NULL ⇒ GJ_OK (no charge).
 * Real account: charge/refund one slot at leaf and each pParent (soft hierarchy).
 * GJ_ERR_QUOTA if any node on the chain is exhausted (depth capped by
 * GJ_CAP_QUOTA_DEPTH_MAX). Grep: cap:quota
 */
gj_status_t gj_cap_quota_slot_charge(void *pAccount);
gj_status_t gj_cap_quota_slot_refund(void *pAccount);

/**
 * Mint: derive a new slot in pDstCnode from src handle with rights ⊆ src.
 * Requires MINT on source. Wires soft CDT edge from edge pool when available.
 * Cross-CNode mint is the derivation path for parent→child task ports etc.
 * Grep: cap:cdt
 */
gj_status_t gj_cap_mint(struct gj_cnode *pSrcCnode, u64 u64SrcSlot,
                        u32 u32SrcGen, u16 u16Rights,
                        struct gj_cnode *pDstCnode, struct gj_cap_ref *pOut);

/**
 * Copy: same object, new slot in the same CNode; requires GRANT on source.
 * Rights must be ⊆ source. Soft CDT edge recorded when pool allows.
 */
gj_status_t gj_cap_copy(struct gj_cnode *pCnode, u64 u64SrcSlot, u32 u32SrcGen,
                        u16 u16Rights, struct gj_cap_ref *pOut);

/**
 * Move: install into new slot then invalidate source (same object).
 * Requires GRANT on source. CDT edge retargeted to new slot (no net
 * slots_left change on success). Source handle becomes stale (gen bump).
 */
gj_status_t gj_cap_move(struct gj_cnode *pCnode, u64 u64SrcSlot, u32 u32SrcGen,
                        struct gj_cap_ref *pOut);

/**
 * Delete/invalidate one slot (structured); refunds quota + CDT unlink.
 * Does not by itself revoke the object if other slots remain — only this
 * binding. Use gj_obj_revoke_begin for object-wide DEAD.
 */
gj_status_t gj_cap_delete(struct gj_cnode *pCnode, u64 u64Slot, u32 u32SlotGen);

/**
 * Deferred invalidate helper: scan pCnode for slots pointing at pObj and
 * apply structured invalidate (bounded by u32MaxSlots). Use when the CNode
 * is known (process death / local hygiene) without a full CDT walk.
 * Returns number of slots cleared.
 * Grep: cap:cdt
 */
u32 gj_cnode_invalidate_obj_slots(struct gj_cnode *pCnode,
                                  struct gj_obj_hdr *pObj, u32 u32MaxSlots);

/* ---- Revoke API (revoke.c) ---------------------------------------------- */

/**
 * Phase A: CAS LIVE→REVOKING, then DEAD + bump gen (S1). Enqueues deferred
 * slot hygiene. Concurrent revoke → object already DEAD/secure; queue full →
 * AGAIN (object still fail-closed). Does not free memory.
 *
 * Wake of object waiters (IPC PEER_DEAD etc.) is object-type specific
 * (e.g. door_mark_dead pairs revoke begin with door_abort_waiters).
 */
gj_status_t gj_obj_revoke_begin(struct gj_obj_hdr *pObj);

/**
 * Phase A′ driver (R2/R7): bounded deferred CDT/slot work. Call from timer,
 * idle, or syscall exit. Returns slots cleared this invocation.
 * Must be driven until gj_revoke_deferred_pending() drains and S4 holds —
 * no silent drop of work items (R7).
 * Grep: cap:cdt deferred
 */
u32 gj_revoke_process_deferred(u32 u32MaxSlots);

/**
 * Soft CDT walk batch: for each edge on pObj, try-lock CNode (soft stub),
 * structured invalidate of the named slot. Unlinks cleared/stale edges;
 * leaves edges whose CNode try-lock is busy (R2). Returns slots cleared.
 * Grep: cap:cdt walk
 */
u32 gj_revoke_cdt_walk_batch(struct gj_obj_hdr *pObj, u32 u32MaxSlots);

/** Active entries on the deferred revoke queue (R7 progress observability). */
u32 gj_revoke_deferred_pending(void);

/**
 * Non-zero when Phase C may free: DEAD, slots_left==0, ref==0, pin==0.
 * Does not free; caller invokes gj_obj_reclaim under its memory policy.
 */
int         gj_obj_reclaim_ready(const struct gj_obj_hdr *pObj);
gj_status_t gj_obj_reclaim(struct gj_obj_hdr *pObj);

/**
 * Structured slot invalidate with object match + slots_left/quota/CDT
 * accounting (S7). Caller holds CNode lock in full impl. If pObj non-NULL,
 * only touch a slot still pointing at that object (stale race safe).
 *
 * Prefer this over bare gj_cap_slot_invalidate whenever the object is known
 * so u32SlotsLeft / quota / CDT stay single-pathed.
 */
void gj_cap_slot_invalidate_locked(struct gj_cap_slot *pSlot,
                                   struct gj_obj_hdr *pObj);
