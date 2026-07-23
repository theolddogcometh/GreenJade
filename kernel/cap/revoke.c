/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Capability revoke: DEAD/gen first (fail closed), then mandatory deferred
 * CNode slot invalidation (S1–S7, R1–R9). See SECURITY_CORE_DESIGN §1.1.
 *
 * Phase A  — logical invalidate (DEAD + gen); security complete.
 * Phase A′ — deferred CDT walk + structured slot invalidate (this file).
 * Phase C  — reclaim when slots_left/refs/pins allow.
 *
 * Soft CDT: edges on pObj->pCdtHead when mint wires them; empty list with
 * slots_left > 0 is a known soft gap (grep: cap:cdt soft).
 */
#include <gj/cap.h>
#include <gj/config.h>
#include <gj/klog.h>

/* Max objects waiting for slot hygiene / reclaim on this simple M0–M2 queue. */
#define GJ_REVOKE_Q_MAX 64u

struct gj_revoke_qent {
    struct gj_obj_hdr *pObj;
    u8                 u8Active;
};

static struct gj_revoke_qent g_aRevokeQ[GJ_REVOKE_Q_MAX];
static u32                   g_u32RevokeQLen;
/* Soft once-marker: empty CDT while slots_left > 0 (avoid timer log spam). */
static u8                    g_u8CdtSoftLogged;

/*
 * Enqueue once per object. Scan for duplicates before taking a free slot so
 * a sparse active set cannot double-queue the same object.
 */
static int
revoke_q_push(struct gj_obj_hdr *pObj)
{
    u32 iEnt;
    u32 u32Free = GJ_REVOKE_Q_MAX;

    if (pObj == NULL) {
        return -1;
    }

    for (iEnt = 0; iEnt < GJ_REVOKE_Q_MAX; iEnt++) {
        if (g_aRevokeQ[iEnt].u8Active) {
            if (g_aRevokeQ[iEnt].pObj == pObj) {
                return 0; /* already queued */
            }
        } else if (u32Free == GJ_REVOKE_Q_MAX) {
            u32Free = iEnt;
        }
    }

    if (u32Free >= GJ_REVOKE_Q_MAX) {
        return -1;
    }

    g_aRevokeQ[u32Free].pObj = pObj;
    g_aRevokeQ[u32Free].u8Active = 1;
    if (u32Free >= g_u32RevokeQLen) {
        g_u32RevokeQLen = u32Free + 1;
    }
    return 0;
}

/* Drop one queue entry; shrink published high-water when trailing empties. */
static void
revoke_q_drop(u32 iEnt)
{
    u32 iScan;

    if (iEnt >= GJ_REVOKE_Q_MAX) {
        return;
    }
    g_aRevokeQ[iEnt].u8Active = 0;
    g_aRevokeQ[iEnt].pObj = NULL;

    if (g_u32RevokeQLen == 0) {
        return;
    }
    iScan = g_u32RevokeQLen;
    while (iScan > 0) {
        if (g_aRevokeQ[iScan - 1].u8Active) {
            break;
        }
        iScan--;
    }
    g_u32RevokeQLen = iScan;
}

u32
gj_revoke_deferred_pending(void)
{
    u32 iEnt;
    u32 u32Pending = 0;
    u32 u32Limit;

    u32Limit = g_u32RevokeQLen;
    if (u32Limit > GJ_REVOKE_Q_MAX) {
        u32Limit = GJ_REVOKE_Q_MAX;
    }
    for (iEnt = 0; iEnt < u32Limit; iEnt++) {
        if (g_aRevokeQ[iEnt].u8Active) {
            u32Pending++;
        }
    }
    return u32Pending;
}

/*
 * Phase A: DEAD + gen first (S1). Slot work is deferred (S4, R2).
 * Security is complete after DEAD/gen; queue full does not undo that.
 */
gj_status_t
gj_obj_revoke_begin(struct gj_obj_hdr *pObj)
{
    u32 u32Expected;
    u32 u32New;
    u32 u32Cur;

    if (pObj == NULL) {
        return GJ_ERR_INVAL;
    }

    u32Expected = (u32)GJ_OBJ_LIVE;
    u32New = (u32)GJ_OBJ_REVOKING;

    /* One revoker (R6). Concurrent revoke → DEAD/BUSY, never a second owner. */
    if (!__atomic_compare_exchange_n(&pObj->u32State, &u32Expected, u32New,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        u32Cur = __atomic_load_n(&pObj->u32State, __ATOMIC_ACQUIRE);
        if (u32Cur == (u32)GJ_OBJ_DEAD ||
            u32Cur == (u32)GJ_OBJ_REVOKING) {
            return GJ_ERR_DEAD;
        }
        return GJ_ERR_BUSY;
    }

    /*
     * S1 — security boundary: DEAD + bump gen BEFORE any deferrable work.
     * From here, gj_obj_check_live() fails closed (S2/S3) even if slots lag.
     */
    __atomic_store_n(&pObj->u32State, (u32)GJ_OBJ_DEAD, __ATOMIC_RELEASE);
    __atomic_fetch_add(&pObj->u32Gen, 1u, __ATOMIC_ACQ_REL);

    /*
     * Waiters would be woken here (IPC queues, fault waiters) when those
     * subsystems exist — PEER_DEAD / REVOKED. Never block on userspace (R4).
     */

    /* Queue mandatory slot hygiene (S4) + later reclaim (S6, R9). */
    if (revoke_q_push(pObj) != 0) {
        /* Grep: revoke: deferred */
        kprintf("revoke: deferred queue full\n");
        /* Object is still DEAD — secure; hygiene must be retried (R7). */
        return GJ_ERR_AGAIN;
    }

    return GJ_OK;
}

/*
 * Structured slot invalidate (S7). Caller holds the CNode lock in full impl.
 * If pObj is non-NULL, only touch a slot that still points at that object —
 * never clear an unrelated cap during a CDT-driven walk.
 *
 * Single path for slots_left + soft quota refund; CDT unlink when the
 * owning CNode is known from an edge (walk path) or left to unlink_slot.
 */
void
gj_cap_slot_invalidate_locked(struct gj_cap_slot *pSlot, struct gj_obj_hdr *pObj)
{
    void *pAccount = NULL;

    if (pSlot == NULL) {
        return;
    }
    if (pSlot->u16Type == (u16)GJ_CAP_INVALID) {
        return;
    }

    if (pObj != NULL) {
        if (pSlot->pObj != (void *)pObj) {
            /* Wrong object (or already cleared); leave slot alone. */
            return;
        }
        /* One less outstanding derived slot (S4/S6). Saturate at zero. */
        if (pObj->u32SlotsLeft > 0) {
            pObj->u32SlotsLeft--;
        }
    } else if (pSlot->pObj != NULL) {
        struct gj_obj_hdr *pSlotObj = (struct gj_obj_hdr *)pSlot->pObj;

        if (pSlotObj->u32SlotsLeft > 0) {
            pSlotObj->u32SlotsLeft--;
        }
    }

    /*
     * Soft quota refund: CNode account is not on the slot; refund via NULL
     * until invalidate path carries the owning CNode (CDT edge has it).
     * Walk batch refunds explicitly when edge->pCnode is known.
     * Grep: cap:quota soft
     */
    (void)gj_cap_quota_slot_refund(pAccount);

    /* S7: type INVALID, slot gen++, clear ptr + obj gen */
    gj_cap_slot_invalidate(pSlot);
}

/*
 * Soft CDT walk (Phase A′ batch). Iterative; work-limited; does not delay S1.
 * Try-lock stub: no CNode mutex yet — treat as acquired (R2 lands with lock).
 * Grep: cap:cdt walk
 */
u32
gj_revoke_cdt_walk_batch(struct gj_obj_hdr *pObj, u32 u32MaxSlots)
{
    struct gj_cdt_edge *pEdge;
    struct gj_cdt_edge *pNext;
    u32 u32Cleared = 0;

    if (pObj == NULL || u32MaxSlots == 0) {
        return 0;
    }

    pEdge = pObj->pCdtHead;
    while (pEdge != NULL && u32Cleared < u32MaxSlots) {
        struct gj_cnode *pCnode;
        u64 u64Slot;

        pNext = pEdge->pNext;
        pCnode = pEdge->pCnode;
        u64Slot = pEdge->u64Slot;

        /*
         * R2 full impl: try-lock pCnode; if busy, leave edge and defer.
         * Soft: no mutex — proceed (single-threaded / cooperative bring-up).
         */
        if (pCnode != NULL && pCnode->pSlots != NULL &&
            u64Slot < pCnode->cSlots) {
            struct gj_cap_slot *pSlot = &pCnode->pSlots[u64Slot];

            if (pSlot->u16Type != (u16)GJ_CAP_INVALID &&
                pSlot->pObj == (void *)pObj) {
                /* Refund against owning CNode account when known. */
                (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount);
                /*
                 * slots_left decremented inside invalidate_locked; avoid
                 * double soft-quota by passing account only above once.
                 * invalidate_locked still calls refund(NULL) — soft no-op.
                 */
                gj_cap_slot_invalidate_locked(pSlot, pObj);
                u32Cleared++;
            }
        }

        /* Unlink edge regardless: slot gone or stale; mint owns free. */
        gj_cdt_edge_unlink(pObj, pEdge);
        pEdge = pNext;
    }

    return u32Cleared;
}

/*
 * Phase A′: drive deferred slot work (bounded; R2 — no spin on CNode locks).
 * Prefer CDT walk when edges exist; otherwise soft-marker if slots lag.
 */
u32
gj_revoke_process_deferred(u32 u32MaxSlots)
{
    u32 u32Cleared = 0;
    u32 iEnt;
    u32 u32Limit;

    if (u32MaxSlots == 0) {
        return 0;
    }

    /* Cap scan to published high-water; never past GJ_REVOKE_Q_MAX. */
    u32Limit = g_u32RevokeQLen;
    if (u32Limit > GJ_REVOKE_Q_MAX) {
        u32Limit = GJ_REVOKE_Q_MAX;
    }

    for (iEnt = 0; iEnt < u32Limit && u32Cleared < u32MaxSlots; iEnt++) {
        struct gj_obj_hdr *pObj;
        u32 u32State;
        u32 u32Batch;
        u32 u32Budget;

        if (!g_aRevokeQ[iEnt].u8Active) {
            continue;
        }
        pObj = g_aRevokeQ[iEnt].pObj;
        if (pObj == NULL) {
            revoke_q_drop(iEnt);
            continue;
        }

        u32State = __atomic_load_n(&pObj->u32State, __ATOMIC_ACQUIRE);
        if (u32State != (u32)GJ_OBJ_DEAD) {
            /* Stale queue entry (reclaimed or never completed Phase A). */
            revoke_q_drop(iEnt);
            continue;
        }

        /* Hygiene done for this object — leave queued for reclaim drain. */
        if (pObj->u32SlotsLeft == 0 && pObj->pCdtHead == NULL) {
            continue;
        }

        u32Budget = u32MaxSlots - u32Cleared;
        u32Batch = gj_revoke_cdt_walk_batch(pObj, u32Budget);
        u32Cleared += u32Batch;

        if (pObj->u32SlotsLeft == 0 && pObj->pCdtHead == NULL) {
            continue;
        }

        /*
         * Soft gap: derived slots counted but no CDT edges to walk.
         * Security already done (DEAD/gen); accounting complete needs mint
         * to wire edges or a known-CNode scan (gj_cnode_invalidate_obj_slots).
         * Grep: cap:cdt soft
         */
        if (u32Batch == 0 && pObj->u32SlotsLeft > 0 && pObj->pCdtHead == NULL) {
            if (!g_u8CdtSoftLogged) {
                g_u8CdtSoftLogged = 1;
                kprintf("cap: soft CDT empty slots_left>0 (once)\n");
            }
        }
    }

    return u32Cleared;
}

int
gj_obj_reclaim_ready(const struct gj_obj_hdr *pObj)
{
    if (pObj == NULL) {
        return 0;
    }
    if (__atomic_load_n(&pObj->u32State, __ATOMIC_ACQUIRE) !=
        (u32)GJ_OBJ_DEAD) {
        return 0;
    }
    /* S6/R9: all derived slots invalidated, no kernel refs or pins */
    if (pObj->u32SlotsLeft != 0) {
        return 0;
    }
    if (pObj->pCdtHead != NULL) {
        return 0; /* soft CDT still has edges — walk must finish */
    }
    if (pObj->u32Ref != 0) {
        return 0;
    }
    if (pObj->u32Pin != 0) {
        return 0;
    }
    return 1;
}

gj_status_t
gj_obj_reclaim(struct gj_obj_hdr *pObj)
{
    u32 iEnt;

    if (pObj == NULL) {
        return GJ_ERR_INVAL;
    }
    if (!gj_obj_reclaim_ready(pObj)) {
        return GJ_ERR_BUSY;
    }

    /* Free object storage when slab/untyped free exists. Drop queue entry. */
    for (iEnt = 0; iEnt < GJ_REVOKE_Q_MAX; iEnt++) {
        if (g_aRevokeQ[iEnt].u8Active && g_aRevokeQ[iEnt].pObj == pObj) {
            revoke_q_drop(iEnt);
        }
    }
    return GJ_OK;
}
