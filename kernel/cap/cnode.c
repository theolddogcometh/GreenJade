/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Process-shared CNode addressing — Scheme A (u64 slot + u32 slot gen).
 *
 * One CNode per process; all threads share it. Slot 0 = root meta only.
 * Resolve fails closed: slot gen + LIVE + object gen (S2). See
 * docs/CAP_ADDRESSING.md and SECURITY_CORE_DESIGN §1.1.
 *
 * Soft CDT edges (gj_cdt_edge_*) and soft slot-quota hooks live here so
 * mint/ledger can wire without changing the resolve/install surface.
 *
 * Full soft CDT companion to R2: mint/copy/move always attempt the edge
 * pool; empty-edge gap is soft FAIL/PARTIAL when the pool is exhausted
 * (install still succeeds — slots_left remains authoritative).
 *
 * Grep: cap:cdt  — edge pool, link/unlink, mint wiring
 * Grep: cap: cdt mint|copy|move — per-op edge coverage stats
 * Grep: cap: cdt soft — empty-edge gap + slots_left/quota tallies
 * Grep: cap:quota — flat + soft hierarchical charge/refund
 */
#include <gj/cap.h>
#include <gj/klog.h>
#include <gj/types.h>

static void cdt_edge_free_if_pool(struct gj_cdt_edge *pEdge);
static void cdt_soft_tally_install(struct gj_cnode *pCnode,
                                  struct gj_obj_hdr *pObj);
static void cdt_edge_try_wire(struct gj_obj_hdr *pObj, struct gj_cnode *pCnode,
                              u64 u64Slot, const char *szOp, u32 *pAttempt,
                              u32 *pOk, u32 *pMiss);

void
gj_obj_hdr_init(struct gj_obj_hdr *pHdr)
{
    if (pHdr == NULL) {
        return;
    }
    pHdr->u32State = (u32)GJ_OBJ_LIVE;
    pHdr->u32Gen = 1; /* 0 reserved for null / never-live */
    pHdr->u32Ref = 0;
    pHdr->u32Pin = 0;
    pHdr->u32SlotsLeft = 0;
    pHdr->pCdtHead = NULL; /* soft CDT; mint links edges when allocated */
}

void
gj_cnode_init(struct gj_cnode *pCnode, struct gj_cap_slot *pSlots, u64 cSlots)
{
    u64 iSlot;

    if (pCnode == NULL || pSlots == NULL || cSlots == 0) {
        return;
    }

    gj_obj_hdr_init(&pCnode->hdr);
    pCnode->cSlots = cSlots;
    pCnode->pSlots = pSlots;
    pCnode->pQuotaAccount = NULL; /* soft: ledger attaches later */
    pCnode->u32SoftLock = 0;      /* cap:cdt trylock — free */
    pCnode->u32PadLock = 0;

    for (iSlot = 0; iSlot < cSlots; iSlot++) {
        pSlots[iSlot].u16Type = (u16)GJ_CAP_INVALID;
        pSlots[iSlot].u16Rights = 0;
        pSlots[iSlot].u32Gen = 1; /* live slot gens never 0 (null handle) */
        pSlots[iSlot].u32ObjGen = 0;
        pSlots[iSlot].pObj = NULL;
    }

    /*
     * Slot 0 is reserved for root meta. Leave INVALID until bootstrap
     * installs GJ_CAP_ROOT_META; alloc_install always starts at slot 1.
     */
}

/*
 * Soft CNode try-lock stub (R2). Atomic CAS on u32SoftLock until a real
 * mutex lands. Order for full impl: CNode → Object → Endpoint.
 * Grep: cap:cdt trylock
 */
int
gj_cnode_trylock(struct gj_cnode *pCnode)
{
    u32 u32Expect;

    if (pCnode == NULL) {
        return 0;
    }
    u32Expect = 0u;
    if (__atomic_compare_exchange_n(&pCnode->u32SoftLock, &u32Expect, 1u, 0,
                                    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
        return 1;
    }
    /* Busy — caller must defer edge (R2), not spin. */
    return 0;
}

void
gj_cnode_unlock(struct gj_cnode *pCnode)
{
    if (pCnode == NULL) {
        return;
    }
    __atomic_store_n(&pCnode->u32SoftLock, 0u, __ATOMIC_RELEASE);
}

/*
 * Soft CDT link — edge storage is caller/slab-owned. Install does not
 * allocate edges yet; mint with CDT will call this after successful install.
 * Grep: cap:cdt
 */
gj_status_t
gj_cdt_edge_link(struct gj_obj_hdr *pObj, struct gj_cdt_edge *pEdge,
                 struct gj_cnode *pCnode, u64 u64Slot)
{
    struct gj_cdt_edge *pWalk;

    if (pObj == NULL || pEdge == NULL || pCnode == NULL) {
        return GJ_ERR_INVAL;
    }
    if (pCnode->pSlots == NULL || u64Slot >= pCnode->cSlots) {
        return GJ_ERR_INVAL;
    }

    /* Refuse double-link of the same edge node. */
    for (pWalk = pObj->pCdtHead; pWalk != NULL; pWalk = pWalk->pNext) {
        if (pWalk == pEdge) {
            return GJ_ERR_BUSY;
        }
    }

    pEdge->pCnode = pCnode;
    pEdge->u64Slot = u64Slot;
    pEdge->pNext = pObj->pCdtHead;
    pObj->pCdtHead = pEdge;
    return GJ_OK;
}

void
gj_cdt_edge_unlink(struct gj_obj_hdr *pObj, struct gj_cdt_edge *pEdge)
{
    struct gj_cdt_edge *pWalk;
    struct gj_cdt_edge *pPrev;

    if (pObj == NULL || pEdge == NULL) {
        return;
    }

    pPrev = NULL;
    for (pWalk = pObj->pCdtHead; pWalk != NULL; pWalk = pWalk->pNext) {
        if (pWalk == pEdge) {
            if (pPrev == NULL) {
                pObj->pCdtHead = pEdge->pNext;
            } else {
                pPrev->pNext = pEdge->pNext;
            }
            pEdge->pNext = NULL;
            pEdge->pCnode = NULL;
            pEdge->u64Slot = 0;
            cdt_edge_free_if_pool(pEdge);
            return;
        }
        pPrev = pWalk;
    }
}

void
gj_cdt_unlink_slot(struct gj_obj_hdr *pObj, struct gj_cnode *pCnode,
                   u64 u64Slot)
{
    struct gj_cdt_edge *pWalk;
    struct gj_cdt_edge *pNext;

    if (pObj == NULL) {
        return;
    }

    pWalk = pObj->pCdtHead;
    while (pWalk != NULL) {
        pNext = pWalk->pNext;
        if (pWalk->pCnode == pCnode && pWalk->u64Slot == u64Slot) {
            gj_cdt_edge_unlink(pObj, pWalk);
            /* Edge free is slab-owned; leave storage for caller. */
        }
        pWalk = pNext;
    }
}

/*
 * Resolve Scheme A handle (u64Slot, u32SlotGen) against the process CNode.
 * Order matters: bounds/null → type/slot-gen → object LIVE+obj-gen (S2/S3).
 * Uncleared slots after revoke still fail here once the object is DEAD.
 */
gj_status_t
gj_cap_resolve(struct gj_cnode *pProcCnode, u64 u64Slot, u32 u32SlotGen,
               struct gj_cap_resolved *pOut)
{
    struct gj_cap_slot *pSlot;
    struct gj_obj_hdr *pObj;
    gj_status_t st;

    if (pProcCnode == NULL || pOut == NULL || pProcCnode->pSlots == NULL) {
        return GJ_ERR_INVAL;
    }
    if (pProcCnode->cSlots == 0) {
        return GJ_ERR_INVAL;
    }

    /* Null handle encoding: gen == 0 */
    if (u32SlotGen == 0) {
        return GJ_ERR_INVAL;
    }

    if (u64Slot >= pProcCnode->cSlots) {
        return GJ_ERR_INVAL;
    }

    pSlot = &pProcCnode->pSlots[u64Slot];

    if (pSlot->u16Type == (u16)GJ_CAP_INVALID) {
        return GJ_ERR_NOENT;
    }
    if (pSlot->u32Gen != u32SlotGen) {
        return GJ_ERR_STALE_CAP;
    }

    pObj = (struct gj_obj_hdr *)pSlot->pObj;
    /* Valid type with NULL obj is corrupt; fail closed, never success. */
    if (pObj == NULL) {
        return GJ_ERR_STALE_CAP;
    }

    st = gj_obj_check_live(pObj, pSlot->u32ObjGen);
    if (st != GJ_OK) {
        return st;
    }

    pOut->pSlot = pSlot;
    pOut->pObj = pObj;
    pOut->u16Type = pSlot->u16Type;
    pOut->u16Rights = pSlot->u16Rights;
    return GJ_OK;
}

/*
 * Install into an INVALID slot. Does not overwrite occupied slots (BUSY).
 * Root-meta type and slot 0 are mutually exclusive with general caps.
 * Soft quota charge then slots_left++; CDT edge is optional (mint wires).
 * Grep: cap:quota / cap:cdt
 */
gj_status_t
gj_cap_slot_install(struct gj_cnode *pCnode, u64 u64Slot, u16 u16Type,
                    u16 u16Rights, struct gj_obj_hdr *pObj,
                    struct gj_cap_ref *pOutRef)
{
    struct gj_cap_slot *pSlot;
    u32 u32ObjGen;
    u32 u32State;
    gj_status_t stQuota;

    if (pCnode == NULL || pObj == NULL || pOutRef == NULL ||
        pCnode->pSlots == NULL) {
        return GJ_ERR_INVAL;
    }
    if (pCnode->cSlots == 0 || u64Slot >= pCnode->cSlots) {
        return GJ_ERR_INVAL;
    }
    if (u16Type == (u16)GJ_CAP_INVALID) {
        return GJ_ERR_INVAL;
    }

    u32State = pObj->u32State;
    if (u32State != (u32)GJ_OBJ_LIVE) {
        return GJ_ERR_DEAD;
    }
    u32ObjGen = pObj->u32Gen;
    /* Object gen 0 is never live; refuse mint that could alias null. */
    if (u32ObjGen == 0) {
        return GJ_ERR_INVAL;
    }

    /* Slot 0 ↔ ROOT_META only */
    if (u64Slot == GJ_CAP_SLOT_ROOT_META &&
        u16Type != (u16)GJ_CAP_ROOT_META) {
        return GJ_ERR_PERM;
    }
    if (u64Slot != GJ_CAP_SLOT_ROOT_META &&
        u16Type == (u16)GJ_CAP_ROOT_META) {
        return GJ_ERR_PERM;
    }

    pSlot = &pCnode->pSlots[u64Slot];

    if (pSlot->u16Type != (u16)GJ_CAP_INVALID) {
        return GJ_ERR_BUSY;
    }

    /*
     * Re-check LIVE after busy test so a concurrent revoke (DEAD/gen first)
     * is not raced into a successful install (S1/S2).
     */
    if (pObj->u32State != (u32)GJ_OBJ_LIVE || pObj->u32Gen != u32ObjGen) {
        return GJ_ERR_DEAD;
    }

    /* Accounting: refuse wrap of derived-slot count (S4/S6). */
    if (pObj->u32SlotsLeft == 0xffffffffu) {
        return GJ_ERR_QUOTA;
    }

    /* Soft process slot quota (no-op until pQuotaAccount attached). */
    /* Grep: cap:quota charge */
    stQuota = gj_cap_quota_slot_charge(pCnode->pQuotaAccount);
    if (stQuota != GJ_OK) {
        return stQuota;
    }

    pSlot->u16Type = u16Type;
    pSlot->u16Rights = u16Rights;
    pSlot->u32ObjGen = u32ObjGen;
    pSlot->pObj = pObj;
    /* u32Gen left as-is (>= 1); bumped only on structured invalidate (S7) */

    pObj->u32SlotsLeft++;

    /*
     * Soft CDT: mint/copy/move with edge pool call gj_cdt_edge_link() after.
     * Until edges exist, deferred revoke uses slots_left + known-CNode scan.
     * Soft slots_left / quota interaction tallies (install path).
     * Grep: cap:cdt soft / cap: cdt soft
     */
    cdt_soft_tally_install(pCnode, pObj);

    *pOutRef = gj_cap_ref_make(u64Slot, pSlot->u32Gen);
    return GJ_OK;
}

/*
 * First free slot from 1..cSlots-1. Root meta only via explicit install
 * at slot 0 (or type ROOT_META routed there).
 */
gj_status_t
gj_cap_alloc_install(struct gj_cnode *pCnode, u16 u16Type, u16 u16Rights,
                     struct gj_obj_hdr *pObj, struct gj_cap_ref *pOutRef)
{
    u64 iSlot;

    if (pCnode == NULL || pCnode->pSlots == NULL || pObj == NULL ||
        pOutRef == NULL) {
        return GJ_ERR_INVAL;
    }
    if (pCnode->cSlots == 0) {
        return GJ_ERR_INVAL;
    }
    if (u16Type == (u16)GJ_CAP_INVALID) {
        return GJ_ERR_INVAL;
    }
    if (u16Type == (u16)GJ_CAP_ROOT_META) {
        /* Root meta only via explicit install at slot 0 */
        return gj_cap_slot_install(pCnode, GJ_CAP_SLOT_ROOT_META, u16Type,
                                   u16Rights, pObj, pOutRef);
    }

    /* Skip slot 0 (root meta reserved) */
    for (iSlot = 1; iSlot < pCnode->cSlots; iSlot++) {
        if (pCnode->pSlots[iSlot].u16Type == (u16)GJ_CAP_INVALID) {
            return gj_cap_slot_install(pCnode, iSlot, u16Type, u16Rights, pObj,
                                       pOutRef);
        }
    }
    return GJ_ERR_QUOTA;
}

/*
 * Deferred invalidate helper — walk a known CNode and clear slots for pObj.
 * Prefer CDT walk when edges exist; this covers process-local hygiene and
 * tests without a global object index.
 * Grep: cap:cdt
 */
u32
gj_cnode_invalidate_obj_slots(struct gj_cnode *pCnode, struct gj_obj_hdr *pObj,
                              u32 u32MaxSlots)
{
    u64 iSlot;
    u32 u32Cleared = 0;

    if (pCnode == NULL || pObj == NULL || pCnode->pSlots == NULL ||
        u32MaxSlots == 0) {
        return 0;
    }

    /* Soft try-lock: if busy, return 0 so caller can defer (R2). */
    if (!gj_cnode_trylock(pCnode)) {
        return 0; /* cap:cdt trylock busy */
    }

    for (iSlot = 0; iSlot < pCnode->cSlots && u32Cleared < u32MaxSlots;
         iSlot++) {
        struct gj_cap_slot *pSlot = &pCnode->pSlots[iSlot];

        if (pSlot->u16Type == (u16)GJ_CAP_INVALID) {
            continue;
        }
        if (pSlot->pObj != (void *)pObj) {
            continue;
        }
        /*
         * Soft quota refund against this CNode's account.
         * Grep: cap:quota soft
         */
        (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount);
        gj_cap_slot_invalidate_locked(pSlot, pObj);
        /* Also drop any soft CDT edge for this (cnode, slot). */
        gj_cdt_unlink_slot(pObj, pCnode, iSlot);
        u32Cleared++;
    }

    gj_cnode_unlock(pCnode);
    return u32Cleared;
}

/* ---- Soft CDT edge pool + mint/copy/move/delete ------------------------ */
/* Grep: cap:cdt pool / cap: cdt mint|copy|move / cap: cdt soft */

#define GJ_CDT_EDGE_POOL 256u

static struct gj_cdt_edge g_aCdtPool[GJ_CDT_EDGE_POOL];
static u8 g_aCdtUsed[GJ_CDT_EDGE_POOL];
static u32 g_u32CdtPoolUsed;   /* live edges checked out of pool */
static u32 g_u32CdtPoolAllocOk;
static u32 g_u32CdtPoolAllocMiss; /* pool exhaust */

/*
 * Per-op soft CDT coverage (always attempt edge pool on mint/copy/move).
 * Grep: cap: cdt mint|copy|move
 */
static u32 g_u32CdtMintAttempt;
static u32 g_u32CdtMintEdgeOk;
static u32 g_u32CdtMintEdgeMiss;
static u32 g_u32CdtCopyAttempt;
static u32 g_u32CdtCopyEdgeOk;
static u32 g_u32CdtCopyEdgeMiss;
static u32 g_u32CdtMoveAttempt;
static u32 g_u32CdtMoveEdgeOk;
static u32 g_u32CdtMoveEdgeMiss;
static u32 g_u32CdtMoveUnlink;

/*
 * Soft slots_left / quota interaction tallies (install + charge/refund).
 * Grep: cap: cdt soft
 */
static u32 g_u32SoftSlotsLeftInc;     /* install ++slots_left */
static u32 g_u32SoftQuotaChargeOk;    /* charge returned GJ_OK with account */
static u32 g_u32SoftQuotaChargeFail;  /* charge GJ_ERR_QUOTA */
static u32 g_u32SoftQuotaChargeNop;   /* charge with NULL account */
static u32 g_u32SoftQuotaRefundOk;    /* refund decremented used */
static u32 g_u32SoftQuotaRefundNop;   /* refund with NULL account */
static u32 g_u32SoftMoveNet0;         /* move: charge+install then refund+inv */
static u32 g_u32SoftDeleteRefund;     /* delete path explicit refund */

/* Once-markers: avoid timer/boot log spam. Grep: cap: cdt soft */
static u8 g_u8CdtPoolExhLogged;
static u8 g_u8CdtSoftTallyLogged;

static void
cdt_soft_inc(u32 *pCtr)
{
    if (pCtr != NULL && *pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

static u32
cdt_edge_pool_used(void)
{
    return g_u32CdtPoolUsed;
}

static struct gj_cdt_edge *
cdt_edge_alloc(void)
{
    u32 i;

    for (i = 0; i < GJ_CDT_EDGE_POOL; i++) {
        if (!g_aCdtUsed[i]) {
            g_aCdtUsed[i] = 1;
            g_aCdtPool[i].pNext = NULL;
            g_aCdtPool[i].pCnode = NULL;
            g_aCdtPool[i].u64Slot = 0;
            cdt_soft_inc(&g_u32CdtPoolUsed);
            cdt_soft_inc(&g_u32CdtPoolAllocOk);
            return &g_aCdtPool[i];
        }
    }
    cdt_soft_inc(&g_u32CdtPoolAllocMiss);
    return NULL;
}

static void
cdt_edge_free(struct gj_cdt_edge *pEdge)
{
    u32 i;

    if (pEdge == NULL) {
        return;
    }
    for (i = 0; i < GJ_CDT_EDGE_POOL; i++) {
        if (&g_aCdtPool[i] == pEdge) {
            if (g_aCdtUsed[i] != 0u && g_u32CdtPoolUsed > 0u) {
                g_u32CdtPoolUsed--;
            }
            g_aCdtUsed[i] = 0;
            pEdge->pNext = NULL;
            pEdge->pCnode = NULL;
            pEdge->u64Slot = 0;
            return;
        }
    }
}

/* Return edge to pool after unlink (used by delete/move paths). */
static void
cdt_edge_free_if_pool(struct gj_cdt_edge *pEdge)
{
    cdt_edge_free(pEdge);
}

/*
 * Soft slots_left / quota interaction tally after a successful install.
 * Grep: cap: cdt soft
 */
static void
cdt_soft_tally_log(void)
{
    /* Grep: cap: cdt soft slots_left / quota */
    kprintf("cap: cdt soft slots_left_inc=%u quota_ch_ok=%u "
            "quota_ch_fail=%u quota_ch_nop=%u quota_rf_ok=%u "
            "quota_rf_nop=%u move_net0=%u del_rf=%u "
            "mint_ok=%u copy_ok=%u move_ok=%u pool_used=%u\n",
            g_u32SoftSlotsLeftInc, g_u32SoftQuotaChargeOk,
            g_u32SoftQuotaChargeFail, g_u32SoftQuotaChargeNop,
            g_u32SoftQuotaRefundOk, g_u32SoftQuotaRefundNop,
            g_u32SoftMoveNet0, g_u32SoftDeleteRefund,
            g_u32CdtMintEdgeOk, g_u32CdtCopyEdgeOk, g_u32CdtMoveEdgeOk,
            cdt_edge_pool_used());
    /* Grep: cap: cdt soft coverage (mint|copy|move attempts / pool) */
    kprintf("cap: cdt soft coverage mint=%u/%u miss_m=%u copy=%u/%u "
            "miss_c=%u move=%u/%u miss_v=%u move_unlink=%u "
            "pool_alloc=%u pool_miss=%u pool_sz=%u\n",
            g_u32CdtMintEdgeOk, g_u32CdtMintAttempt, g_u32CdtMintEdgeMiss,
            g_u32CdtCopyEdgeOk, g_u32CdtCopyAttempt, g_u32CdtCopyEdgeMiss,
            g_u32CdtMoveEdgeOk, g_u32CdtMoveAttempt, g_u32CdtMoveEdgeMiss,
            g_u32CdtMoveUnlink, g_u32CdtPoolAllocOk, g_u32CdtPoolAllocMiss,
            GJ_CDT_EDGE_POOL);
}

static void
cdt_soft_tally_install(struct gj_cnode *pCnode, struct gj_obj_hdr *pObj)
{
    cdt_soft_inc(&g_u32SoftSlotsLeftInc);
    (void)pCnode;
    (void)pObj;
    /*
     * Emit greppable tallies once on first install so early smokes see
     * slots_left/quota coupling; a richer re-log may follow first mint.
     */
    if (!g_u8CdtSoftTallyLogged) {
        g_u8CdtSoftTallyLogged = 1;
        cdt_soft_tally_log();
    }
}

/*
 * Empty-edge gap honesty when the soft edge pool is exhausted.
 * Install/mint still returns GJ_OK (slots_left authoritative); CDT walk
 * cannot see the slot until edges exist — soft FAIL or PARTIAL.
 *
 *   FAIL    — pool miss with zero successful edges for this op family
 *   PARTIAL — pool miss after at least one edge_ok for this op family
 *
 * Grep: cap: cdt soft FAIL|PARTIAL
 */
static void
cdt_soft_empty_edge_gap(const char *szOp, u32 u32Ok, u32 u32Miss, u32 u32Attempt)
{
    const char *szVerdict;

    if (szOp == NULL) {
        szOp = "?";
    }
    szVerdict = (u32Ok == 0u) ? "FAIL" : "PARTIAL";

    if (!g_u8CdtPoolExhLogged) {
        g_u8CdtPoolExhLogged = 1;
        kprintf("cap: cdt soft %s empty-edge pool exhaust op=%s "
                "miss=%u ok=%u attempt=%u pool_used=%u pool_sz=%u "
                "alloc_miss=%u\n",
                szVerdict, szOp, u32Miss, u32Ok, u32Attempt,
                cdt_edge_pool_used(), GJ_CDT_EDGE_POOL,
                g_u32CdtPoolAllocMiss);
    }
}

/*
 * Always attempt edge pool after mint/copy/move install success.
 * Does not change install status — edge miss is soft empty-edge gap only.
 * Grep: cap: cdt mint|copy|move
 */
static void
cdt_edge_try_wire(struct gj_obj_hdr *pObj, struct gj_cnode *pCnode,
                  u64 u64Slot, const char *szOp, u32 *pAttempt, u32 *pOk,
                  u32 *pMiss)
{
    struct gj_cdt_edge *pEdge;
    gj_status_t st;
    u32 u32Slots;

    cdt_soft_inc(pAttempt);
    u32Slots = (pObj != NULL) ? pObj->u32SlotsLeft : 0u;

    pEdge = cdt_edge_alloc();
    if (pEdge == NULL) {
        cdt_soft_inc(pMiss);
        cdt_soft_empty_edge_gap(szOp, *pOk, *pMiss, *pAttempt);
        /* Grep: cap: cdt mint|copy|move … coverage */
        kprintf("cap: cdt %s edge_ok=0 miss=%u attempt=%u ok=%u "
                "slots_left=%u pool_used=%u sl_inc=%u q_ch=%u q_rf=%u\n",
                szOp, *pMiss, *pAttempt, *pOk, u32Slots,
                cdt_edge_pool_used(), g_u32SoftSlotsLeftInc,
                g_u32SoftQuotaChargeOk, g_u32SoftQuotaRefundOk);
        return;
    }

    st = gj_cdt_edge_link(pObj, pEdge, pCnode, u64Slot);
    if (st != GJ_OK) {
        cdt_edge_free(pEdge);
        cdt_soft_inc(pMiss);
        /* Link refused (double-link etc.) — soft miss, not pool exhaust. */
        kprintf("cap: cdt %s edge_ok=0 link_st=%d miss=%u attempt=%u "
                "ok=%u slots_left=%u pool_used=%u sl_inc=%u q_ch=%u "
                "q_rf=%u\n",
                szOp, (int)st, *pMiss, *pAttempt, *pOk, u32Slots,
                cdt_edge_pool_used(), g_u32SoftSlotsLeftInc,
                g_u32SoftQuotaChargeOk, g_u32SoftQuotaRefundOk);
        return;
    }

    cdt_soft_inc(pOk);
    /* Grep: cap: cdt mint|copy|move … coverage */
    kprintf("cap: cdt %s edge_ok=1 ok=%u attempt=%u miss=%u "
            "slots_left=%u pool_used=%u sl_inc=%u q_ch=%u q_rf=%u\n",
            szOp, *pOk, *pAttempt, *pMiss, u32Slots, cdt_edge_pool_used(),
            g_u32SoftSlotsLeftInc, g_u32SoftQuotaChargeOk,
            g_u32SoftQuotaRefundOk);

    /*
     * Re-log soft tallies once after first charge-backed mint/copy/move so
     * interaction counters are visible with real quota attach (boot door
     * install may have logged only nop charge earlier).
     */
    if (g_u8CdtSoftTallyLogged < 2u && g_u32SoftQuotaChargeOk > 0u) {
        g_u8CdtSoftTallyLogged = 2;
        cdt_soft_tally_log();
    }
}

/* ---- Soft quota (flat + hierarchical parent roll-up) ------------------- */
/* Grep: cap:quota */

void
gj_cap_quota_init(struct gj_cap_quota *pQ, u32 u32Limit)
{
    if (pQ == NULL) {
        return;
    }
    pQ->u32Limit = u32Limit;
    pQ->u32Used = 0;
    pQ->u32Exhaust = 0;
    pQ->u32HighWater = 0;
    pQ->u32ChargeOk = 0;
    pQ->u32RefundOk = 0;
    pQ->pParent = NULL; /* flat until gj_cap_quota_set_parent */
}

void
gj_cap_quota_attach(struct gj_cnode *pCnode, struct gj_cap_quota *pQ)
{
    if (pCnode == NULL) {
        return;
    }
    pCnode->pQuotaAccount = pQ; /* cap:quota attach */
}

/*
 * Soft hierarchical parent. Zone-like: child charges roll up to parent.
 * Refuses self and shallow cycles. Grep: cap:quota parent
 */
void
gj_cap_quota_set_parent(struct gj_cap_quota *pQ, struct gj_cap_quota *pParent)
{
    struct gj_cap_quota *pWalk;
    u32 u32Depth;

    if (pQ == NULL) {
        return;
    }
    if (pParent == NULL) {
        pQ->pParent = NULL;
        return;
    }
    if (pParent == pQ) {
        return; /* refuse self */
    }
    /* Refuse if pQ already appears on pParent's ancestor chain. */
    pWalk = pParent;
    for (u32Depth = 0; pWalk != NULL && u32Depth < GJ_CAP_QUOTA_DEPTH_MAX;
         u32Depth++) {
        if (pWalk == pQ) {
            return; /* would cycle */
        }
        pWalk = pWalk->pParent;
    }
    pQ->pParent = pParent;
}

u32
gj_cap_quota_used(const struct gj_cap_quota *pQ)
{
    return pQ != NULL ? pQ->u32Used : 0u;
}

u32
gj_cap_quota_limit(const struct gj_cap_quota *pQ)
{
    return pQ != NULL ? pQ->u32Limit : 0u;
}

u32
gj_cap_quota_highwater(const struct gj_cap_quota *pQ)
{
    return pQ != NULL ? pQ->u32HighWater : 0u;
}

u32
gj_cap_quota_exhaust_count(const struct gj_cap_quota *pQ)
{
    return pQ != NULL ? pQ->u32Exhaust : 0u;
}

/*
 * Charge one slot at leaf and each soft parent (hierarchical roll-up).
 * Pre-check all nodes, then commit — soft single-threaded safe.
 * Grep: cap:quota charge
 */
gj_status_t
gj_cap_quota_slot_charge(void *pAccount)
{
    struct gj_cap_quota *pQ = (struct gj_cap_quota *)pAccount;
    struct gj_cap_quota *pWalk;
    u32 u32Depth;

    if (pQ == NULL) {
        /* Soft: NULL account no-op. Grep: cap:quota soft / cap: cdt soft */
        cdt_soft_inc(&g_u32SoftQuotaChargeNop);
        return GJ_OK;
    }

    /* Pass 1: every node on the chain must have room. */
    pWalk = pQ;
    for (u32Depth = 0; pWalk != NULL && u32Depth < GJ_CAP_QUOTA_DEPTH_MAX;
         u32Depth++) {
        if (pWalk->u32Used >= pWalk->u32Limit) {
            pWalk->u32Exhaust++;
            cdt_soft_inc(&g_u32SoftQuotaChargeFail);
            return GJ_ERR_QUOTA; /* cap:quota exhaust */
        }
        pWalk = pWalk->pParent;
    }

    /* Pass 2: commit leaf → root. */
    pWalk = pQ;
    for (u32Depth = 0; pWalk != NULL && u32Depth < GJ_CAP_QUOTA_DEPTH_MAX;
         u32Depth++) {
        pWalk->u32Used++;
        if (pWalk->u32Used > pWalk->u32HighWater) {
            pWalk->u32HighWater = pWalk->u32Used;
        }
        pWalk->u32ChargeOk++;
        pWalk = pWalk->pParent;
    }
    cdt_soft_inc(&g_u32SoftQuotaChargeOk); /* cap: cdt soft quota interact */
    return GJ_OK;
}

/*
 * Refund one slot at leaf and each soft parent (mirrors charge).
 * Grep: cap:quota refund
 */
gj_status_t
gj_cap_quota_slot_refund(void *pAccount)
{
    struct gj_cap_quota *pQ = (struct gj_cap_quota *)pAccount;
    struct gj_cap_quota *pWalk;
    u32 u32Depth;

    if (pQ == NULL) {
        /* Soft: NULL account no-op. Grep: cap:quota soft / cap: cdt soft */
        cdt_soft_inc(&g_u32SoftQuotaRefundNop);
        return GJ_OK;
    }

    pWalk = pQ;
    for (u32Depth = 0; pWalk != NULL && u32Depth < GJ_CAP_QUOTA_DEPTH_MAX;
         u32Depth++) {
        if (pWalk->u32Used > 0u) {
            pWalk->u32Used--;
            pWalk->u32RefundOk++;
        }
        pWalk = pWalk->pParent;
    }
    cdt_soft_inc(&g_u32SoftQuotaRefundOk); /* cap: cdt soft quota interact */
    return GJ_OK;
}

static u16
rights_weaker(u16 u16Src, u16 u16Want)
{
    return (u16)(u16Src & u16Want);
}

gj_status_t
gj_cap_mint(struct gj_cnode *pSrcCnode, u64 u64SrcSlot, u32 u32SrcGen,
            u16 u16Rights, struct gj_cnode *pDstCnode, struct gj_cap_ref *pOut)
{
    struct gj_cap_resolved res;
    gj_status_t st;
    u16 u16New;

    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    if (pDstCnode == NULL) {
        pDstCnode = pSrcCnode;
    }
    st = gj_cap_resolve(pSrcCnode, u64SrcSlot, u32SrcGen, &res);
    if (st != GJ_OK) {
        return st;
    }
    if ((res.u16Rights & GJ_RIGHT_MINT) == 0) {
        return GJ_ERR_PERM;
    }
    u16New = rights_weaker(res.u16Rights, u16Rights);
    if (u16New == 0) {
        return GJ_ERR_PERM;
    }
    /* Derived must not gain rights source lacks (already masked). */
    st = gj_cap_alloc_install(pDstCnode, res.u16Type, u16New, res.pObj, pOut);
    if (st != GJ_OK) {
        return st;
    }
    /*
     * Always attempt edge pool (soft empty-edge gap if exhausted).
     * Grep: cap: cdt mint / cap:cdt mint
     */
    cdt_edge_try_wire(res.pObj, pDstCnode, pOut->u64Slot, "mint",
                      &g_u32CdtMintAttempt, &g_u32CdtMintEdgeOk,
                      &g_u32CdtMintEdgeMiss);
    return GJ_OK;
}

gj_status_t
gj_cap_copy(struct gj_cnode *pCnode, u64 u64SrcSlot, u32 u32SrcGen,
            u16 u16Rights, struct gj_cap_ref *pOut)
{
    struct gj_cap_resolved res;
    gj_status_t st;
    u16 u16New;

    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve(pCnode, u64SrcSlot, u32SrcGen, &res);
    if (st != GJ_OK) {
        return st;
    }
    if ((res.u16Rights & GJ_RIGHT_GRANT) == 0) {
        return GJ_ERR_PERM;
    }
    u16New = rights_weaker(res.u16Rights, u16Rights != 0 ? u16Rights : res.u16Rights);
    st = gj_cap_alloc_install(pCnode, res.u16Type, u16New, res.pObj, pOut);
    if (st != GJ_OK) {
        return st;
    }
    /*
     * Always attempt edge pool (soft empty-edge gap if exhausted).
     * Grep: cap: cdt copy / cap:cdt copy
     */
    cdt_edge_try_wire(res.pObj, pCnode, pOut->u64Slot, "copy",
                      &g_u32CdtCopyAttempt, &g_u32CdtCopyEdgeOk,
                      &g_u32CdtCopyEdgeMiss);
    return GJ_OK;
}

gj_status_t
gj_cap_move(struct gj_cnode *pCnode, u64 u64SrcSlot, u32 u32SrcGen,
            struct gj_cap_ref *pOut)
{
    struct gj_cap_resolved res;
    struct gj_cap_slot *pSrc;
    gj_status_t st;

    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve(pCnode, u64SrcSlot, u32SrcGen, &res);
    if (st != GJ_OK) {
        return st;
    }
    if ((res.u16Rights & GJ_RIGHT_GRANT) == 0) {
        return GJ_ERR_PERM;
    }
    if (u64SrcSlot == GJ_CAP_SLOT_ROOT_META) {
        return GJ_ERR_PERM;
    }
    st = gj_cap_alloc_install(pCnode, res.u16Type, res.u16Rights, res.pObj,
                              pOut);
    if (st != GJ_OK) {
        return st;
    }
    /*
     * Retarget CDT: unlink old slot (returns edge to pool), then always
     * attempt wire on the new slot. Net slots_left / quota are restored
     * by the source invalidate below (move net-zero).
     * Grep: cap: cdt move / cap:cdt move
     */
    gj_cdt_unlink_slot(res.pObj, pCnode, u64SrcSlot);
    cdt_soft_inc(&g_u32CdtMoveUnlink);
    cdt_edge_try_wire(res.pObj, pCnode, pOut->u64Slot, "move",
                      &g_u32CdtMoveAttempt, &g_u32CdtMoveEdgeOk,
                      &g_u32CdtMoveEdgeMiss);
    /* Invalidate source without double-counting object death. */
    pSrc = &pCnode->pSlots[u64SrcSlot];
    (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount); /* cap:quota */
    gj_cap_slot_invalidate_locked(pSrc, res.pObj);
    /* Soft: charge on install + refund on source = net-zero interact. */
    cdt_soft_inc(&g_u32SoftMoveNet0); /* cap: cdt soft */
    return GJ_OK;
}

gj_status_t
gj_cap_delete(struct gj_cnode *pCnode, u64 u64Slot, u32 u32SlotGen)
{
    struct gj_cap_resolved res;
    gj_status_t st;

    st = gj_cap_resolve(pCnode, u64Slot, u32SlotGen, &res);
    if (st != GJ_OK) {
        return st;
    }
    if (u64Slot == GJ_CAP_SLOT_ROOT_META) {
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & GJ_RIGHT_DESTROY) == 0 &&
        (res.u16Rights & GJ_RIGHT_GRANT) == 0) {
        return GJ_ERR_PERM;
    }
    gj_cdt_unlink_slot(res.pObj, pCnode, u64Slot); /* cap:cdt delete */
    (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount); /* cap:quota */
    cdt_soft_inc(&g_u32SoftDeleteRefund); /* cap: cdt soft */
    gj_cap_slot_invalidate_locked(res.pSlot, res.pObj);
    return GJ_OK;
}
