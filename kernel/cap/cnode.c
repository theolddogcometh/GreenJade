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
 */
#include <gj/cap.h>
#include <gj/types.h>

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
 * Soft quota: hierarchical accounts not wired (M2.2). Always succeed so
 * install/revoke paths stay fail-closed only on real exhaustion later.
 * Grep: cap:quota soft
 */
gj_status_t
gj_cap_quota_slot_charge(void *pAccount)
{
    if (pAccount == NULL) {
        return GJ_OK; /* soft: no account ⇒ no charge */
    }
    /*
     * Full impl: charge one occupied CNode slot against pAccount; return
     * GJ_ERR_QUOTA if exhausted. Grep: cap:quota soft
     */
    (void)pAccount;
    return GJ_OK;
}

gj_status_t
gj_cap_quota_slot_refund(void *pAccount)
{
    if (pAccount == NULL) {
        return GJ_OK;
    }
    /* Full impl: refund one slot to pAccount. Grep: cap:quota soft */
    (void)pAccount;
    return GJ_OK;
}

/*
 * Soft CDT link — edge storage is caller/slab-owned. Install does not
 * allocate edges yet; mint with CDT will call this after successful install.
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
     * Soft CDT: mint with edge storage should gj_cdt_edge_link() here.
     * Until then deferred revoke uses slots_left + optional CNode scan.
     * Grep: cap:cdt soft
     */

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
         * Full impl holds CNode lock; soft path assumes single-threaded or
         * caller already serialised. R2 try-lock lands with CNode mutex.
         * Soft quota refund against this CNode's account (grep: cap:quota soft).
         */
        (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount);
        gj_cap_slot_invalidate_locked(pSlot, pObj);
        /* Also drop any soft CDT edge for this (cnode, slot). */
        gj_cdt_unlink_slot(pObj, pCnode, iSlot);
        u32Cleared++;
    }

    return u32Cleared;
}
