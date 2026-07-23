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
 *
 * R2 try-lock slot walk (this file):
 *   Soft CNode lock is gj_cnode_trylock (u32SoftLock CAS). Product-true
 *   mutex/turnstile is still missing — R2 is intentionally PARTIAL.
 *   Policy is sleep-not-spin: never busy-wait a busy CNode; leave the edge
 *   linked, try siblings, soft-retry a bounded number of re-walks, then
 *   defer to timer/idle (R7). Counters: spins_avoided, retries, slots_cleared.
 *
 * Grep: cap:cdt deferred / cap:cdt walk / cap:quota soft
 * Grep: cap: revoke try-lock / cap:cdt R2 soft / cap:cdt trylock
 */
#include <gj/cap.h>
#include <gj/config.h>
#include <gj/klog.h>

/* Max objects waiting for slot hygiene / reclaim on this simple M0–M2 queue. */
#define GJ_REVOKE_Q_MAX 64u

/*
 * Bounded soft re-walks of busy CDT edges within one batch (R2).
 * Not a spin: each pass only try-locks once per edge; further progress
 * yields to the deferred driver (timer/idle) — sleep-not-spin.
 */
#define GJ_REVOKE_R2_SOFT_RETRY_MAX 3u

struct gj_revoke_qent {
    struct gj_obj_hdr *pObj;
    u8                 u8Active;
};

static struct gj_revoke_qent g_aRevokeQ[GJ_REVOKE_Q_MAX];
static u32                   g_u32RevokeQLen;
/* Round-robin cursor so multi-object queues make progress (R7). */
static u32                   g_u32RevokeQCursor;
/* Soft once-marker: empty CDT while slots_left > 0 (avoid timer log spam). */
static u8                    g_u8CdtSoftLogged;
/* Soft once-marker: try-lock busy deferred an edge (cap:cdt trylock). */
static u8                    g_u8CdtTrylockLogged;
/* Soft once-marker: R2 try-lock walk summary (cap: revoke try-lock). */
static u8                    g_u8RevokeTrylockLogged;
/* Soft once-marker: R2 soft defer while edges remain (cap:cdt R2 soft). */
static u8                    g_u8CdtR2SoftLogged;

/*
 * R2 observability counters (lifetime, process-wide soft stats).
 *   spins_avoided — try-lock busy → deferred (did not spin)
 *   retries       — soft re-walk passes after a busy edge
 *   slots_cleared — structured invalidates via CDT walk batch
 * Grep: cap: revoke try-lock / cap:cdt R2 soft
 */
static u32 g_u32R2SpinsAvoided;
static u32 g_u32R2Retries;
static u32 g_u32R2SlotsCleared;

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
        /* Grep: revoke: deferred / cap:cdt deferred */
        kprintf("cap:cdt deferred queue full\n");
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
 * Single path for slots_left; soft quota refund is done by callers that know
 * the owning CNode (CDT edge / invalidate_obj_slots). This path refunds NULL
 * so accounting stays single-pathed when the CNode is known upstream.
 * Grep: cap:quota soft
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
 *
 * R2 try-lock slot walk:
 *   - Soft try-lock each CNode (gj_cnode_trylock / u32SoftLock).
 *   - If busy: count spins_avoided, leave edge linked, try siblings.
 *   - After a pass with busy edges, soft-retry up to
 *     GJ_REVOKE_R2_SOFT_RETRY_MAX re-walks (not a spin: one try-lock per
 *     edge per pass). If still busy, return; deferred driver redrives (R7).
 *   - Sleep-not-spin: never busy-wait trylock in a tight loop. A product
 *     path with a real mutex would sleep on the CNode turnstile; that
 *     mutex is still missing — R2 product try-lock remains PARTIAL.
 *   - Stale/bad edges are unlinked. Cleared slots get quota refund against
 *     the owning CNode account.
 *
 * Grep: cap:cdt walk / cap: revoke try-lock / cap:cdt R2 soft
 */
u32
gj_revoke_cdt_walk_batch(struct gj_obj_hdr *pObj, u32 u32MaxSlots)
{
    u32 u32Cleared = 0;
    u32 u32Busy = 0;
    u32 u32Visited = 0;
    u32 u32Pass;
    u32 u32Stale = 0;
    /* Bound visits so a long list under lock contention cannot spin. */
    const u32 u32VisitCap = u32MaxSlots * 4u + 8u;

    if (pObj == NULL || u32MaxSlots == 0) {
        return 0;
    }

    /*
     * Soft multi-pass: walk → soft-retry busy edges → defer if still held.
     * Pass 0 is the primary walk; passes 1..SOFT_RETRY_MAX are retries
     * after at least one busy try-lock (sleep-not-spin policy).
     */
    for (u32Pass = 0;
         u32Pass <= GJ_REVOKE_R2_SOFT_RETRY_MAX &&
         u32Cleared < u32MaxSlots &&
         u32Visited < u32VisitCap;
         u32Pass++) {
        struct gj_cdt_edge *pEdge;
        struct gj_cdt_edge *pNext;
        u32 u32BusyThisPass = 0;

        if (pObj->pCdtHead == NULL) {
            break;
        }

        if (u32Pass > 0) {
            /*
             * Soft retry after busy (R2): do NOT spin. Re-scan remaining
             * edges once; holder may have dropped the soft lock. If still
             * busy after SOFT_RETRY_MAX, yield to deferred driver (R7) —
             * that is the "sleep" half of sleep-not-spin without a real
             * turnstile.
             * Grep: cap:cdt R2 soft
             */
            g_u32R2Retries++;
        }

        pEdge = pObj->pCdtHead;
        while (pEdge != NULL && u32Cleared < u32MaxSlots &&
               u32Visited < u32VisitCap) {
            struct gj_cnode *pCnode;
            u64 u64Slot;
            int fUnlink = 0;

            u32Visited++;
            pNext = pEdge->pNext;
            pCnode = pEdge->pCnode;
            u64Slot = pEdge->u64Slot;

            if (pCnode == NULL || pCnode->pSlots == NULL ||
                u64Slot >= pCnode->cSlots) {
                /* Stale edge — drop. Grep: cap:cdt stale */
                fUnlink = 1;
                u32Stale++;
            } else if (!gj_cnode_trylock(pCnode)) {
                /*
                 * R2: CNode busy — leave edge linked, try siblings, soft
                 * retry later. Count as spin avoided (never busy-wait).
                 * Grep: cap:cdt trylock / cap: revoke try-lock
                 */
                u32BusyThisPass++;
                g_u32R2SpinsAvoided++;
                if (!g_u8CdtTrylockLogged) {
                    g_u8CdtTrylockLogged = 1;
                    kprintf("cap:cdt trylock busy (once)\n");
                }
                pEdge = pNext;
                continue;
            } else {
                struct gj_cap_slot *pSlot = &pCnode->pSlots[u64Slot];

                if (pSlot->u16Type != (u16)GJ_CAP_INVALID &&
                    pSlot->pObj == (void *)pObj) {
                    /* Refund against owning CNode account when known. */
                    /* Grep: cap:quota refund */
                    (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount);
                    /*
                     * slots_left decremented inside invalidate_locked; avoid
                     * double soft-quota by passing account only above once.
                     * invalidate_locked still calls refund(NULL) — soft no-op.
                     */
                    gj_cap_slot_invalidate_locked(pSlot, pObj);
                    u32Cleared++;
                    g_u32R2SlotsCleared++;
                    fUnlink = 1;
                } else {
                    /* Slot already cleared or retargeted — drop edge. */
                    fUnlink = 1;
                    u32Stale++;
                }
                gj_cnode_unlock(pCnode);
            }

            if (fUnlink) {
                gj_cdt_edge_unlink(pObj, pEdge);
            }
            pEdge = pNext;
        }

        u32Busy = u32BusyThisPass;
        if (u32BusyThisPass == 0) {
            /* Clean pass — no soft-retry needed. */
            break;
        }
        /* Busy edges remain: soft-retry next pass, or exit at max. */
    }

    /*
     * Greppable R2 summary (once). Honesty: soft u32SoftLock only —
     * product try-lock still partial without a real CNode mutex.
     * Grep: cap: revoke try-lock
     */
    if (!g_u8RevokeTrylockLogged &&
        (u32Cleared > 0 || g_u32R2SpinsAvoided > 0 || u32Stale > 0)) {
        g_u8RevokeTrylockLogged = 1;
        kprintf("cap: revoke try-lock walk cleared=%u spins_avoided=%u "
                "retries=%u slots_cleared=%u busy_left=%u "
                "soft_partial (once)\n",
                u32Cleared, g_u32R2SpinsAvoided, g_u32R2Retries,
                g_u32R2SlotsCleared, u32Busy);
    }

    return u32Cleared;
}

/*
 * Phase A′: drive deferred slot work (bounded; R2 — no spin on CNode locks).
 * Prefer full CDT try-lock walk when edges exist (primary, more complete
 * than the empty-edge soft gap). Round-robin across queue so one lagging
 * object cannot starve siblings (R7). Soft-marker only if slots lag
 * without edges.
 *
 * Sleep-not-spin (R2): walk batch may soft-retry busy CNodes; if edges
 * still remain, keep the object queued and return — timer/idle redrives.
 * Never tight-loop trylock here.
 *
 * Honesty: R2 product try-lock is still PARTIAL (soft u32SoftLock, not a
 * real CNode mutex/turnstile).
 *
 * Grep: cap:cdt deferred / cap: revoke try-lock / cap:cdt R2 soft
 */
u32
gj_revoke_process_deferred(u32 u32MaxSlots)
{
    u32 u32Cleared = 0;
    u32 u32Limit;
    u32 u32Scanned;
    u32 iEnt;
    u32 u32Start;

    if (u32MaxSlots == 0) {
        return 0;
    }

    /* Cap scan to published high-water; never past GJ_REVOKE_Q_MAX. */
    u32Limit = g_u32RevokeQLen;
    if (u32Limit > GJ_REVOKE_Q_MAX) {
        u32Limit = GJ_REVOKE_Q_MAX;
    }
    if (u32Limit == 0) {
        return 0;
    }

    /* Round-robin: start past last cursor so every active ent gets turns. */
    u32Start = g_u32RevokeQCursor % u32Limit;

    for (u32Scanned = 0; u32Scanned < u32Limit && u32Cleared < u32MaxSlots;
         u32Scanned++) {
        struct gj_obj_hdr *pObj;
        u32 u32State;
        u32 u32Batch;
        u32 u32Budget;

        iEnt = (u32Start + u32Scanned) % u32Limit;

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

        /*
         * Edges present: full CDT try-lock batch is the primary hygiene path
         * (more complete than empty-edge soft gap). Walk batch itself does
         * bounded soft retries under sleep-not-spin.
         * Grep: cap:cdt walk / cap: revoke try-lock
         */
        if (pObj->pCdtHead != NULL) {
            u32Batch = gj_revoke_cdt_walk_batch(pObj, u32Budget);
            u32Cleared += u32Batch;

            /*
             * Second soft push within this deferred tick if edges remain and
             * budget remains: one more batch only (not a spin). Further work
             * stays on the queue for R7 redrive.
             * Grep: cap:cdt R2 soft
             */
            if (pObj->pCdtHead != NULL && u32Cleared < u32MaxSlots) {
                u32 u32Push;

                u32Budget = u32MaxSlots - u32Cleared;
                u32Push = gj_revoke_cdt_walk_batch(pObj, u32Budget);
                if (u32Push > 0) {
                    g_u32R2Retries++; /* outer deferred soft push */
                }
                u32Cleared += u32Push;
            }

            /* Advance cursor past this ent so next call rotates fairly. */
            g_u32RevokeQCursor = (iEnt + 1u) % u32Limit;

            if (pObj->u32SlotsLeft == 0 && pObj->pCdtHead == NULL) {
                continue;
            }

            /* Edges remain (budget or try-lock busy) — keep queued (R7). */
            if (pObj->pCdtHead != NULL) {
                /*
                 * Soft R2 defer: edges still linked after try-lock walks.
                 * Product mutex still missing — partial R2.
                 * Grep: cap:cdt R2 soft / cap: revoke try-lock
                 */
                if (!g_u8CdtR2SoftLogged) {
                    g_u8CdtR2SoftLogged = 1;
                    kprintf("cap:cdt R2 soft defer edges remain "
                            "spins_avoided=%u retries=%u slots_cleared=%u "
                            "soft_partial (once)\n",
                            g_u32R2SpinsAvoided, g_u32R2Retries,
                            g_u32R2SlotsCleared);
                }
                continue;
            }
        }

        /*
         * Soft gap: derived slots counted but no CDT edges to walk.
         * Security already done (DEAD/gen); accounting complete needs mint
         * to wire edges or a known-CNode scan (gj_cnode_invalidate_obj_slots).
         * Only when edges are absent — walk path above is preferred.
         * Grep: cap:cdt soft
         */
        if (pObj->u32SlotsLeft > 0 && pObj->pCdtHead == NULL) {
            if (!g_u8CdtSoftLogged) {
                g_u8CdtSoftLogged = 1;
                kprintf("cap:cdt soft empty edges slots_left>0 (once)\n");
            }
            g_u32RevokeQCursor = (iEnt + 1u) % u32Limit;
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
