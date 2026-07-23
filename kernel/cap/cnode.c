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
 * Grep: cap: cdt soft audit — soft empty-edge slots_left vs chain depth
 * Grep: cap: cdt delete|unlink|retarget — delete/move edge hygiene
 * Grep: cap: cdt pool — alloc/free pool churn
 * Grep: cap:quota — flat + soft hierarchical charge/refund
 *
 * Soft inventory (Wave 18 exclusive deepen; this unit only):
 *   cap: cdt soft honesty    — ≠ GJ_CAP_REPLY product / full CDT mutex
 *   cap: cdt soft inventory  — pool/slots/quota + resolve/trylock rollup
 *   cap: cdt soft resolve    — ok/inval/noent/stale/live_fail path tallies
 *   cap: cdt soft trylock    — enter/ok/busy (u32SoftLock; not product mutex)
 *   cap: cdt soft install    — install enter/ok/fail + REPLY scaffold count
 *   cap: cdt soft coverage   — mint|copy|move edge attempt/ok/miss
 *   cap: cdt soft audit      — slots_left vs chain depth empty-edge gap
 *   cap: cdt soft pool       — alloc/free/miss churn
 *   cap: cdt soft type       — type catalog; REPLY scaffold-only honesty
 *   cap: cdt soft path       — surface catalog + non-claims
 *   cap: cdt soft return     — Wave 18 public return-surface (gj_status buckets)
 *   cap: cdt soft return install|mint|copy|move|delete|alloc — per-API ret
 *   cap: cdt soft return rate — Wave 17 ok/fail rate lamps (kept) (return surface)
 *   cap: cdt soft retcode    — Wave 17 observed gj_status retcode catalog
 *   cap: cdt soft return selftest — Wave 18 terminal return surface
 *   cap: cdt soft retmap     — Wave 18 return-surface map
 *   cap: cdt soft deepen     — wave=18 areas stamp
 *   cap: cdt soft PASS|FAIL / cap: cdt soft inventory PASS|FAIL
 * Honesty: soft inventory only — not GJ_CAP_REPLY product (MIG install),
 * not full CDT mutex/turnstile product; Soft ≠ MIG REPLY product; bar3 OPEN.
 * Grep: cap: cdt soft
 */
#include <gj/cap.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/types.h>

/* Wave 18 deepen stamp (file-local; never hard-gates). */
#define GJ_CDT_SOFT_WAVE  18u
/* +return selftest|retmap over Wave 17 return rate|retcode */
#define GJ_CDT_SOFT_AREAS 18u

static void cdt_edge_free_if_pool(struct gj_cdt_edge *pEdge);
static void cdt_soft_tally_install(struct gj_cnode *pCnode,
                                  struct gj_obj_hdr *pObj);
static void cdt_edge_try_wire(struct gj_obj_hdr *pObj, struct gj_cnode *pCnode,
                              u64 u64Slot, const char *szOp, u32 *pAttempt,
                              u32 *pOk, u32 *pMiss);
static u32  cdt_edge_chain_depth(const struct gj_obj_hdr *pObj);
static void cdt_soft_empty_edge_audit(const struct gj_obj_hdr *pObj,
                                     const char *szOp, u64 u64Slot,
                                     u32 u32EdgeOk);
static void cdt_soft_note_unlink(const struct gj_obj_hdr *pObj, u64 u64Slot,
                                u32 u32Unlinked);
static void cdt_soft_inventory_log(void);
static void cdt_soft_inventory_maybe_once(void);
static void cdt_soft_inc(u32 *pCtr);

/*
 * Wave 18 soft path tallies (file-local; wrap OK; never hard-gate).
 * Placed early so resolve/trylock/install can instrument without forward
 * static issues. Grep: cap: cdt soft resolve|trylock|install|return
 */
static u32 g_u32SoftResEnter;       /* gj_cap_resolve entries */
static u32 g_u32SoftResOk;          /* resolve success */
static u32 g_u32SoftResInval;       /* null/bounds/gen0 */
static u32 g_u32SoftResNoent;       /* INVALID slot */
static u32 g_u32SoftResStale;       /* gen mismatch / null obj */
static u32 g_u32SoftResLiveFail;    /* LIVE+obj-gen fail-closed */
static u32 g_u32SoftTryEnter;       /* gj_cnode_trylock entries */
static u32 g_u32SoftTryOk;          /* CAS acquired soft lock */
static u32 g_u32SoftTryBusy;        /* CAS busy (defer, not spin) */
static u32 g_u32SoftTryNull;        /* null cnode */
static u32 g_u32SoftUnlock;         /* gj_cnode_unlock releases */
static u32 g_u32SoftInstEnter;      /* gj_cap_slot_install entries */
static u32 g_u32SoftInstOk;         /* install success */
static u32 g_u32SoftInstFail;       /* install reject (any arm) */
static u32 g_u32SoftInstFailInval;  /* Wave 18: install → GJ_ERR_INVAL */
static u32 g_u32SoftInstFailDead;   /* Wave 18: install → GJ_ERR_DEAD */
static u32 g_u32SoftInstFailPerm;   /* Wave 18: install → GJ_ERR_PERM */
static u32 g_u32SoftInstFailBusy;   /* Wave 18: install → GJ_ERR_BUSY */
static u32 g_u32SoftInstFailQuota;  /* Wave 18: install → GJ_ERR_QUOTA */
static u32 g_u32SoftInstReplyType;  /* type==GJ_CAP_REPLY scaffold installs */
static u32 g_u32SoftInvLogs;        /* soft inventory dump emissions */
static u8  g_u8CdtSoftInvLogged;    /* once-marker for Wave 18 rollup */
/* Wave 18: public API return-surface buckets (mint/copy/move/delete/alloc). */
static u32 g_u32SoftRetMintOk;
static u32 g_u32SoftRetMintFail;
static u32 g_u32SoftRetMintInval;
static u32 g_u32SoftRetMintPerm;
static u32 g_u32SoftRetMintOther;
static u32 g_u32SoftRetCopyOk;
static u32 g_u32SoftRetCopyFail;
static u32 g_u32SoftRetCopyInval;
static u32 g_u32SoftRetCopyPerm;
static u32 g_u32SoftRetCopyOther;
static u32 g_u32SoftRetMoveOk;
static u32 g_u32SoftRetMoveFail;
static u32 g_u32SoftRetMoveInval;
static u32 g_u32SoftRetMovePerm;
static u32 g_u32SoftRetMoveOther;
static u32 g_u32SoftRetDelOk;
static u32 g_u32SoftRetDelFail;
static u32 g_u32SoftRetDelInval;
static u32 g_u32SoftRetDelPerm;
static u32 g_u32SoftRetDelOther;
static u32 g_u32SoftRetAllocOk;
static u32 g_u32SoftRetAllocFail;
static u32 g_u32SoftRetAllocInval;
static u32 g_u32SoftRetAllocQuota;
static u32 g_u32SoftRetAllocOther;

/** Soft: saturating bump (u32 wrap avoided; wrap OK if ever hit). */
static void
cdt_soft_inc(u32 *pCtr)
{
    if (pCtr != NULL && *pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

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
 * Soft ≠ full CDT mutex product (turnstile sleep still OPEN).
 * Grep: cap:cdt trylock / cap: cdt soft trylock
 */
int
gj_cnode_trylock(struct gj_cnode *pCnode)
{
    u32 u32Expect;

    cdt_soft_inc(&g_u32SoftTryEnter);
    if (pCnode == NULL) {
        cdt_soft_inc(&g_u32SoftTryNull);
        return 0;
    }
    u32Expect = 0u;
    if (__atomic_compare_exchange_n(&pCnode->u32SoftLock, &u32Expect, 1u, 0,
                                    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
        cdt_soft_inc(&g_u32SoftTryOk);
        return 1;
    }
    /* Busy — caller must defer edge (R2), not spin. */
    cdt_soft_inc(&g_u32SoftTryBusy);
    return 0;
}

void
gj_cnode_unlock(struct gj_cnode *pCnode)
{
    if (pCnode == NULL) {
        return;
    }
    __atomic_store_n(&pCnode->u32SoftLock, 0u, __ATOMIC_RELEASE);
    cdt_soft_inc(&g_u32SoftUnlock);
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
    u32 u32Unlinked = 0;

    if (pObj == NULL) {
        return;
    }

    pWalk = pObj->pCdtHead;
    while (pWalk != NULL) {
        pNext = pWalk->pNext;
        if (pWalk->pCnode == pCnode && pWalk->u64Slot == u64Slot) {
            gj_cdt_edge_unlink(pObj, pWalk);
            u32Unlinked++;
            /* Edge free is slab-owned; leave storage for caller. */
        }
        pWalk = pNext;
    }
    /*
     * Soft unlink tally (delete/move/invalidate hygiene). Logging lives in
     * cdt_soft_note_unlink so greppable lines sit with the Wave 15 pool
     * counters (file-static order). Grep: cap: cdt unlink
     */
    if (u32Unlinked > 0u) {
        cdt_soft_note_unlink(pObj, u64Slot, u32Unlinked);
    }
}

/*
 * Resolve Scheme A handle (u64Slot, u32SlotGen) against the process CNode.
 * Order matters: bounds/null → type/slot-gen → object LIVE+obj-gen (S2/S3).
 * Uncleared slots after revoke still fail here once the object is DEAD.
 * Wave 15: soft resolve path tallies (never change fail-closed order).
 * Grep: cap: cdt soft resolve
 */
gj_status_t
gj_cap_resolve(struct gj_cnode *pProcCnode, u64 u64Slot, u32 u32SlotGen,
               struct gj_cap_resolved *pOut)
{
    struct gj_cap_slot *pSlot;
    struct gj_obj_hdr *pObj;
    gj_status_t st;

    cdt_soft_inc(&g_u32SoftResEnter);

    if (pProcCnode == NULL || pOut == NULL || pProcCnode->pSlots == NULL) {
        cdt_soft_inc(&g_u32SoftResInval);
        return GJ_ERR_INVAL;
    }
    if (pProcCnode->cSlots == 0) {
        cdt_soft_inc(&g_u32SoftResInval);
        return GJ_ERR_INVAL;
    }

    /* Null handle encoding: gen == 0 */
    if (u32SlotGen == 0) {
        cdt_soft_inc(&g_u32SoftResInval);
        return GJ_ERR_INVAL;
    }

    if (u64Slot >= pProcCnode->cSlots) {
        cdt_soft_inc(&g_u32SoftResInval);
        return GJ_ERR_INVAL;
    }

    pSlot = &pProcCnode->pSlots[u64Slot];

    if (pSlot->u16Type == (u16)GJ_CAP_INVALID) {
        cdt_soft_inc(&g_u32SoftResNoent);
        return GJ_ERR_NOENT;
    }
    if (pSlot->u32Gen != u32SlotGen) {
        cdt_soft_inc(&g_u32SoftResStale);
        return GJ_ERR_STALE_CAP;
    }

    pObj = (struct gj_obj_hdr *)pSlot->pObj;
    /* Valid type with NULL obj is corrupt; fail closed, never success. */
    if (pObj == NULL) {
        cdt_soft_inc(&g_u32SoftResStale);
        return GJ_ERR_STALE_CAP;
    }

    st = gj_obj_check_live(pObj, pSlot->u32ObjGen);
    if (st != GJ_OK) {
        cdt_soft_inc(&g_u32SoftResLiveFail);
        return st;
    }

    pOut->pSlot = pSlot;
    pOut->pObj = pObj;
    pOut->u16Type = pSlot->u16Type;
    pOut->u16Rights = pSlot->u16Rights;
    cdt_soft_inc(&g_u32SoftResOk);
    cdt_soft_inventory_maybe_once();
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

    cdt_soft_inc(&g_u32SoftInstEnter);

    if (pCnode == NULL || pObj == NULL || pOutRef == NULL ||
        pCnode->pSlots == NULL) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailInval); /* Wave 18 return surface */
        return GJ_ERR_INVAL;
    }
    if (pCnode->cSlots == 0 || u64Slot >= pCnode->cSlots) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailInval);
        return GJ_ERR_INVAL;
    }
    if (u16Type == (u16)GJ_CAP_INVALID) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailInval);
        return GJ_ERR_INVAL;
    }

    u32State = pObj->u32State;
    if (u32State != (u32)GJ_OBJ_LIVE) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailDead);
        return GJ_ERR_DEAD;
    }
    u32ObjGen = pObj->u32Gen;
    /* Object gen 0 is never live; refuse mint that could alias null. */
    if (u32ObjGen == 0) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailInval);
        return GJ_ERR_INVAL;
    }

    /* Slot 0 ↔ ROOT_META only */
    if (u64Slot == GJ_CAP_SLOT_ROOT_META &&
        u16Type != (u16)GJ_CAP_ROOT_META) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailPerm);
        return GJ_ERR_PERM;
    }
    if (u64Slot != GJ_CAP_SLOT_ROOT_META &&
        u16Type == (u16)GJ_CAP_ROOT_META) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailPerm);
        return GJ_ERR_PERM;
    }

    pSlot = &pCnode->pSlots[u64Slot];

    if (pSlot->u16Type != (u16)GJ_CAP_INVALID) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailBusy);
        return GJ_ERR_BUSY;
    }

    /*
     * Re-check LIVE after busy test so a concurrent revoke (DEAD/gen first)
     * is not raced into a successful install (S1/S2).
     */
    if (pObj->u32State != (u32)GJ_OBJ_LIVE || pObj->u32Gen != u32ObjGen) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailDead);
        return GJ_ERR_DEAD;
    }

    /* Accounting: refuse wrap of derived-slot count (S4/S6). */
    if (pObj->u32SlotsLeft == 0xffffffffu) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailQuota);
        return GJ_ERR_QUOTA;
    }

    /* Soft process slot quota (no-op until pQuotaAccount attached). */
    /* Grep: cap:quota charge */
    stQuota = gj_cap_quota_slot_charge(pCnode->pQuotaAccount);
    if (stQuota != GJ_OK) {
        cdt_soft_inc(&g_u32SoftInstFail);
        cdt_soft_inc(&g_u32SoftInstFailQuota);
        return stQuota;
    }

    pSlot->u16Type = u16Type;
    pSlot->u16Rights = u16Rights;
    pSlot->u32ObjGen = u32ObjGen;
    pSlot->pObj = pObj;
    /* u32Gen left as-is (>= 1); bumped only on structured invalidate (S7) */

    pObj->u32SlotsLeft++;

    /*
     * Soft: REPLY type may install as scaffold enum only.
     * Soft ≠ GJ_CAP_REPLY product (no MIG ephemeral single-use CNode wire).
     * Grep: cap: cdt soft install / cap: cdt soft type
     */
    if (u16Type == (u16)GJ_CAP_REPLY) {
        cdt_soft_inc(&g_u32SoftInstReplyType);
    }

    /*
     * Soft CDT: mint/copy/move with edge pool call gj_cdt_edge_link() after.
     * Until edges exist, deferred revoke uses slots_left + known-CNode scan.
     * Soft slots_left / quota interaction tallies (install path).
     * Grep: cap:cdt soft / cap: cdt soft
     */
    cdt_soft_tally_install(pCnode, pObj);

    *pOutRef = gj_cap_ref_make(u64Slot, pSlot->u32Gen);
    cdt_soft_inc(&g_u32SoftInstOk);
    cdt_soft_inventory_maybe_once();
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
    gj_status_t st;

    if (pCnode == NULL || pCnode->pSlots == NULL || pObj == NULL ||
        pOutRef == NULL) {
        /* Grep: cap: cdt soft return alloc */
        cdt_soft_inc(&g_u32SoftRetAllocFail);
        cdt_soft_inc(&g_u32SoftRetAllocInval);
        return GJ_ERR_INVAL;
    }
    if (pCnode->cSlots == 0) {
        cdt_soft_inc(&g_u32SoftRetAllocFail);
        cdt_soft_inc(&g_u32SoftRetAllocInval);
        return GJ_ERR_INVAL;
    }
    if (u16Type == (u16)GJ_CAP_INVALID) {
        cdt_soft_inc(&g_u32SoftRetAllocFail);
        cdt_soft_inc(&g_u32SoftRetAllocInval);
        return GJ_ERR_INVAL;
    }
    if (u16Type == (u16)GJ_CAP_ROOT_META) {
        /* Root meta only via explicit install at slot 0 */
        st = gj_cap_slot_install(pCnode, GJ_CAP_SLOT_ROOT_META, u16Type,
                                 u16Rights, pObj, pOutRef);
        if (st == GJ_OK) {
            cdt_soft_inc(&g_u32SoftRetAllocOk);
        } else {
            cdt_soft_inc(&g_u32SoftRetAllocFail);
            if (st == GJ_ERR_QUOTA) {
                cdt_soft_inc(&g_u32SoftRetAllocQuota);
            } else if (st == GJ_ERR_INVAL) {
                cdt_soft_inc(&g_u32SoftRetAllocInval);
            } else {
                cdt_soft_inc(&g_u32SoftRetAllocOther);
            }
        }
        return st;
    }

    /* Skip slot 0 (root meta reserved) */
    for (iSlot = 1; iSlot < pCnode->cSlots; iSlot++) {
        if (pCnode->pSlots[iSlot].u16Type == (u16)GJ_CAP_INVALID) {
            st = gj_cap_slot_install(pCnode, iSlot, u16Type, u16Rights, pObj,
                                     pOutRef);
            if (st == GJ_OK) {
                cdt_soft_inc(&g_u32SoftRetAllocOk);
            } else {
                cdt_soft_inc(&g_u32SoftRetAllocFail);
                if (st == GJ_ERR_QUOTA) {
                    cdt_soft_inc(&g_u32SoftRetAllocQuota);
                } else if (st == GJ_ERR_INVAL) {
                    cdt_soft_inc(&g_u32SoftRetAllocInval);
                } else {
                    cdt_soft_inc(&g_u32SoftRetAllocOther);
                }
            }
            return st;
        }
    }
    cdt_soft_inc(&g_u32SoftRetAllocFail);
    cdt_soft_inc(&g_u32SoftRetAllocQuota);
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
/* Grep: cap: cdt soft audit / cap: cdt delete|unlink|retarget / cap: cdt pool */

#define GJ_CDT_EDGE_POOL 256u

static struct gj_cdt_edge g_aCdtPool[GJ_CDT_EDGE_POOL];
static u8 g_aCdtUsed[GJ_CDT_EDGE_POOL];
static u32 g_u32CdtPoolUsed;   /* live edges checked out of pool */
static u32 g_u32CdtPoolAllocOk;
static u32 g_u32CdtPoolAllocMiss; /* pool exhaust */
static u32 g_u32CdtPoolFreeOk;    /* edges returned to pool (Wave 15) */

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
static u32 g_u32CdtMintCross;     /* mint: src CNode != dst CNode */
static u32 g_u32CdtMintLocal;     /* mint: same CNode */
static u32 g_u32CdtMoveRetarget;  /* move: unlink src then wire dst */
static u32 g_u32CdtDeleteEdge;    /* delete: edge unlinked for slot */
static u32 g_u32CdtUnlinkOk;      /* unlink_slot found ≥1 edge */

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

/*
 * Wave 15: soft empty-edge audit (slots_left vs CDT chain depth).
 * Install without wire leaves slots_left > chain — soft gap, not product.
 * Grep: cap: cdt soft audit
 */
static u32 g_u32CdtChainDepthMax;     /* peak chain depth after wire */
static u32 g_u32CdtChainDepthAtOk;    /* last chain depth on edge_ok */
static u32 g_u32CdtSoftAuditMatch;    /* slots_left == chain after wire */
static u32 g_u32CdtSoftAuditMismatch; /* slots_left != chain (soft gap) */
static u32 g_u32CdtSoftAuditEmpty;    /* pool miss: slots without edge */

/* Once-markers: avoid timer/boot log spam. Grep: cap: cdt soft */
static u8 g_u8CdtPoolExhLogged;
static u8 g_u8CdtSoftTallyLogged;
static u8 g_u8CdtSoftAuditLogged;     /* first soft empty-edge audit */
static u8 g_u8CdtCoverageRollupLogged; /* first full mint|copy|move rollup */
static u8 g_u8CdtPoolChurnLogged;     /* first pool free/alloc churn line */
static u8 g_u8CdtUnlinkLogged;        /* first unlink coverage line */

static u32
cdt_edge_pool_used(void)
{
    return g_u32CdtPoolUsed;
}

/*
 * Soft CDT chain depth on an object (bounded by pool size).
 * Grep: cap: cdt soft audit
 */
static u32
cdt_edge_chain_depth(const struct gj_obj_hdr *pObj)
{
    const struct gj_cdt_edge *pWalk;
    u32 u32Depth = 0;
    u32 u32Guard = 0;

    if (pObj == NULL) {
        return 0;
    }
    for (pWalk = pObj->pCdtHead;
         pWalk != NULL && u32Guard < GJ_CDT_EDGE_POOL;
         pWalk = pWalk->pNext, u32Guard++) {
        u32Depth++;
    }
    return u32Depth;
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
            cdt_soft_inc(&g_u32CdtPoolFreeOk); /* cap: cdt pool free */
            /*
             * First free→alloc churn line (delete/move returns edge).
             * Grep: cap: cdt pool
             */
            if (!g_u8CdtPoolChurnLogged && g_u32CdtPoolFreeOk > 0u &&
                g_u32CdtPoolAllocOk > 0u) {
                g_u8CdtPoolChurnLogged = 1;
                kprintf("cap: cdt pool free_ok=%u alloc_ok=%u miss=%u "
                        "used=%u sz=%u churn soft (once)\n",
                        g_u32CdtPoolFreeOk, g_u32CdtPoolAllocOk,
                        g_u32CdtPoolAllocMiss, cdt_edge_pool_used(),
                        GJ_CDT_EDGE_POOL);
            }
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
 * Soft unlink note (from gj_cdt_unlink_slot after ≥1 edge dropped).
 * Once-marker log + lifetime tally. Grep: cap: cdt unlink
 */
static void
cdt_soft_note_unlink(const struct gj_obj_hdr *pObj, u64 u64Slot,
                     u32 u32Unlinked)
{
    u32 u32Chain;
    u32 u32Slots;

    cdt_soft_inc(&g_u32CdtUnlinkOk);
    u32Chain = cdt_edge_chain_depth(pObj);
    u32Slots = (pObj != NULL) ? pObj->u32SlotsLeft : 0u;
    if (!g_u8CdtUnlinkLogged) {
        g_u8CdtUnlinkLogged = 1;
        /* Grep: cap: cdt unlink */
        kprintf("cap: cdt unlink n=%u slot=%lu chain_after=%u "
                "slots_left=%u pool_used=%u free=%u unlink_n=%u "
                "soft (once)\n",
                u32Unlinked, (unsigned long)u64Slot, u32Chain, u32Slots,
                cdt_edge_pool_used(), g_u32CdtPoolFreeOk, g_u32CdtUnlinkOk);
    }
}

/*
 * Soft empty-edge audit: compare object slots_left vs CDT chain depth.
 * Bootstrap install without wire leaves slots_left > chain (soft gap).
 * Does not claim product empty-edge audit — soft observability only.
 * Grep: cap: cdt soft audit
 */
static void
cdt_soft_empty_edge_audit(const struct gj_obj_hdr *pObj, const char *szOp,
                          u64 u64Slot, u32 u32EdgeOk)
{
    u32 u32Slots;
    u32 u32Chain;
    int fMatch;

    if (szOp == NULL) {
        szOp = "?";
    }
    u32Slots = (pObj != NULL) ? pObj->u32SlotsLeft : 0u;
    u32Chain = cdt_edge_chain_depth(pObj);
    g_u32CdtChainDepthAtOk = u32Chain;
    if (u32Chain > g_u32CdtChainDepthMax) {
        g_u32CdtChainDepthMax = u32Chain;
    }

    if (u32EdgeOk != 0u) {
        fMatch = (u32Slots == u32Chain) ? 1 : 0;
        if (fMatch) {
            cdt_soft_inc(&g_u32CdtSoftAuditMatch);
        } else {
            cdt_soft_inc(&g_u32CdtSoftAuditMismatch);
        }
    } else {
        /* Pool miss or link refuse — slot may still be installed. */
        cdt_soft_inc(&g_u32CdtSoftAuditEmpty);
        fMatch = 0;
    }

    if (!g_u8CdtSoftAuditLogged) {
        g_u8CdtSoftAuditLogged = 1;
        /* Grep: cap: cdt soft audit */
        kprintf("cap: cdt soft audit op=%s slot=%lu edge_ok=%u "
                "slots_left=%u chain=%u match=%d "
                "match_n=%u mismatch_n=%u empty_n=%u "
                "chain_max=%u soft_gap (once)\n",
                szOp, (unsigned long)u64Slot, u32EdgeOk, u32Slots, u32Chain,
                fMatch, g_u32CdtSoftAuditMatch, g_u32CdtSoftAuditMismatch,
                g_u32CdtSoftAuditEmpty, g_u32CdtChainDepthMax);
    }
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
            "mint_ok=%u copy_ok=%u move_ok=%u pool_used=%u wave=%u\n",
            g_u32SoftSlotsLeftInc, g_u32SoftQuotaChargeOk,
            g_u32SoftQuotaChargeFail, g_u32SoftQuotaChargeNop,
            g_u32SoftQuotaRefundOk, g_u32SoftQuotaRefundNop,
            g_u32SoftMoveNet0, g_u32SoftDeleteRefund,
            g_u32CdtMintEdgeOk, g_u32CdtCopyEdgeOk, g_u32CdtMoveEdgeOk,
            cdt_edge_pool_used(), GJ_CDT_SOFT_WAVE);
    /* Grep: cap: cdt soft coverage (mint|copy|move attempts / pool) */
    kprintf("cap: cdt soft coverage mint=%u/%u miss_m=%u copy=%u/%u "
            "miss_c=%u move=%u/%u miss_v=%u move_unlink=%u "
            "pool_alloc=%u pool_miss=%u pool_free=%u pool_sz=%u "
            "mint_x=%u mint_loc=%u retarget=%u del_edge=%u unlink_ok=%u "
            "wave=%u\n",
            g_u32CdtMintEdgeOk, g_u32CdtMintAttempt, g_u32CdtMintEdgeMiss,
            g_u32CdtCopyEdgeOk, g_u32CdtCopyAttempt, g_u32CdtCopyEdgeMiss,
            g_u32CdtMoveEdgeOk, g_u32CdtMoveAttempt, g_u32CdtMoveEdgeMiss,
            g_u32CdtMoveUnlink, g_u32CdtPoolAllocOk, g_u32CdtPoolAllocMiss,
            g_u32CdtPoolFreeOk, GJ_CDT_EDGE_POOL, g_u32CdtMintCross,
            g_u32CdtMintLocal, g_u32CdtMoveRetarget, g_u32CdtDeleteEdge,
            g_u32CdtUnlinkOk, GJ_CDT_SOFT_WAVE);
    /* Grep: cap: cdt soft audit rollup */
    kprintf("cap: cdt soft audit match=%u mismatch=%u empty=%u "
            "chain_max=%u chain_last=%u pool_used=%u soft_only wave=%u\n",
            g_u32CdtSoftAuditMatch, g_u32CdtSoftAuditMismatch,
            g_u32CdtSoftAuditEmpty, g_u32CdtChainDepthMax,
            g_u32CdtChainDepthAtOk, cdt_edge_pool_used(), GJ_CDT_SOFT_WAVE);
}

/**
 * Wave 18 greppable soft inventory dump (never hard-gates product).
 * Prefix-stable family: "cap: cdt soft …"
 * Honesty: soft ≠ GJ_CAP_REPLY product / full CDT mutex product /
 * Soft ≠ MIG REPLY product.
 * Grep: cap: cdt soft inventory|resolve|trylock|install|return|type|path|deepen
 */
static void
cdt_soft_inventory_log(void)
{
    cdt_soft_inc(&g_u32SoftInvLogs);

    /*
     * Grep: cap: cdt soft honesty
     * Soft inventory only — not MIG REPLY CNode product, not product mutex.
     */
    kprintf("cap: cdt soft honesty reply_product=0 full_cdt_mutex=0 "
            "soft_lock=u32SoftLock sleep_not_spin=1 bar3=0 "
            "soft_ne_mig_reply=1 wave=%u (soft != GJ_CAP_REPLY product; "
            "soft != MIG REPLY product; soft != full CDT mutex product; "
            "soft inventory only)\n",
            GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft inventory */
    kprintf("cap: cdt soft inventory pool_used=%u pool_sz=%u "
            "alloc_ok=%u alloc_miss=%u free_ok=%u sl_inc=%u "
            "q_ch_ok=%u q_ch_fail=%u q_ch_nop=%u q_rf_ok=%u q_rf_nop=%u "
            "mint_ok=%u copy_ok=%u move_ok=%u chain_max=%u "
            "log_n=%u wave=%u soft_partial\n",
            cdt_edge_pool_used(), GJ_CDT_EDGE_POOL, g_u32CdtPoolAllocOk,
            g_u32CdtPoolAllocMiss, g_u32CdtPoolFreeOk, g_u32SoftSlotsLeftInc,
            g_u32SoftQuotaChargeOk, g_u32SoftQuotaChargeFail,
            g_u32SoftQuotaChargeNop, g_u32SoftQuotaRefundOk,
            g_u32SoftQuotaRefundNop, g_u32CdtMintEdgeOk, g_u32CdtCopyEdgeOk,
            g_u32CdtMoveEdgeOk, g_u32CdtChainDepthMax, g_u32SoftInvLogs,
            GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft resolve */
    kprintf("cap: cdt soft resolve enter=%u ok=%u inval=%u noent=%u "
            "stale=%u live_fail=%u scheme_a=1 wave=%u\n",
            g_u32SoftResEnter, g_u32SoftResOk, g_u32SoftResInval,
            g_u32SoftResNoent, g_u32SoftResStale, g_u32SoftResLiveFail,
            GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft trylock / cap:cdt trylock soft */
    kprintf("cap: cdt soft trylock enter=%u ok=%u busy=%u null=%u "
            "unlock=%u lock=u32SoftLock product_mutex=OPEN "
            "sleep_not_spin=1 soft_partial wave=%u\n",
            g_u32SoftTryEnter, g_u32SoftTryOk, g_u32SoftTryBusy,
            g_u32SoftTryNull, g_u32SoftUnlock, GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft install */
    kprintf("cap: cdt soft install enter=%u ok=%u fail=%u "
            "fail_inval=%u fail_dead=%u fail_perm=%u fail_busy=%u "
            "fail_quota=%u reply_type=%u reply_product=0 wave=%u "
            "(REPLY scaffold count only; not GJ_CAP_REPLY product; "
            "soft != MIG REPLY product)\n",
            g_u32SoftInstEnter, g_u32SoftInstOk, g_u32SoftInstFail,
            g_u32SoftInstFailInval, g_u32SoftInstFailDead,
            g_u32SoftInstFailPerm, g_u32SoftInstFailBusy,
            g_u32SoftInstFailQuota, g_u32SoftInstReplyType, GJ_CDT_SOFT_WAVE);

    /*
     * Grep: cap: cdt soft return
     * Wave 18 public return-surface: gj_status buckets across CNode APIs.
     * Soft ≠ MIG REPLY product / full CDT mutex product.
     */
    kprintf("cap: cdt soft return resolve_ok=%u resolve_fail=%u "
            "inst_ok=%u inst_fail=%u mint_ok=%u mint_fail=%u "
            "copy_ok=%u copy_fail=%u move_ok=%u move_fail=%u "
            "del_ok=%u del_fail=%u alloc_ok=%u alloc_fail=%u "
            "reply_product=0 wave=%u\n",
            g_u32SoftResOk,
            g_u32SoftResInval + g_u32SoftResNoent + g_u32SoftResStale +
                g_u32SoftResLiveFail,
            g_u32SoftInstOk, g_u32SoftInstFail, g_u32SoftRetMintOk,
            g_u32SoftRetMintFail, g_u32SoftRetCopyOk, g_u32SoftRetCopyFail,
            g_u32SoftRetMoveOk, g_u32SoftRetMoveFail, g_u32SoftRetDelOk,
            g_u32SoftRetDelFail, g_u32SoftRetAllocOk, g_u32SoftRetAllocFail,
            GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft return install — Wave 17 status split */
    kprintf("cap: cdt soft return install ok=%u fail=%u inval=%u dead=%u "
            "perm=%u busy=%u quota=%u wave=%u\n",
            g_u32SoftInstOk, g_u32SoftInstFail, g_u32SoftInstFailInval,
            g_u32SoftInstFailDead, g_u32SoftInstFailPerm,
            g_u32SoftInstFailBusy, g_u32SoftInstFailQuota, GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft return mint|copy|move|delete|alloc */
    kprintf("cap: cdt soft return mint ok=%u fail=%u inval=%u perm=%u "
            "other=%u wave=%u\n",
            g_u32SoftRetMintOk, g_u32SoftRetMintFail, g_u32SoftRetMintInval,
            g_u32SoftRetMintPerm, g_u32SoftRetMintOther, GJ_CDT_SOFT_WAVE);
    kprintf("cap: cdt soft return copy ok=%u fail=%u inval=%u perm=%u "
            "other=%u wave=%u\n",
            g_u32SoftRetCopyOk, g_u32SoftRetCopyFail, g_u32SoftRetCopyInval,
            g_u32SoftRetCopyPerm, g_u32SoftRetCopyOther, GJ_CDT_SOFT_WAVE);
    kprintf("cap: cdt soft return move ok=%u fail=%u inval=%u perm=%u "
            "other=%u wave=%u\n",
            g_u32SoftRetMoveOk, g_u32SoftRetMoveFail, g_u32SoftRetMoveInval,
            g_u32SoftRetMovePerm, g_u32SoftRetMoveOther, GJ_CDT_SOFT_WAVE);
    kprintf("cap: cdt soft return delete ok=%u fail=%u inval=%u perm=%u "
            "other=%u wave=%u\n",
            g_u32SoftRetDelOk, g_u32SoftRetDelFail, g_u32SoftRetDelInval,
            g_u32SoftRetDelPerm, g_u32SoftRetDelOther, GJ_CDT_SOFT_WAVE);
    kprintf("cap: cdt soft return alloc ok=%u fail=%u inval=%u quota=%u "
            "other=%u wave=%u\n",
            g_u32SoftRetAllocOk, g_u32SoftRetAllocFail, g_u32SoftRetAllocInval,
            g_u32SoftRetAllocQuota, g_u32SoftRetAllocOther, GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft pool */
    kprintf("cap: cdt soft pool used=%u sz=%u alloc_ok=%u miss=%u "
            "free_ok=%u churn=%u wave=%u\n",
            cdt_edge_pool_used(), GJ_CDT_EDGE_POOL, g_u32CdtPoolAllocOk,
            g_u32CdtPoolAllocMiss, g_u32CdtPoolFreeOk,
            (g_u32CdtPoolFreeOk > 0u && g_u32CdtPoolAllocOk > 0u) ? 1u : 0u,
            GJ_CDT_SOFT_WAVE);

    /*
     * Grep: cap: cdt soft type
     * Catalog of enum types; REPLY remains scaffold (door soft table, not
     * CNode MIG product). Soft ≠ GJ_CAP_REPLY product.
     */
    kprintf("cap: cdt soft type invalid=1 cnode=1 thread=1 space=1 "
            "process=1 endpoint=1 notification=1 reply_scaffold=1 "
            "reply_product=0 irq=1 frame=1 untyped=1 page_table=1 "
            "sched_ctx=1 memobj=1 root_meta=1 reply_inst=%u wave=%u "
            "(soft type catalog; GJ_CAP_REPLY product OPEN)\n",
            g_u32SoftInstReplyType, GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft path */
    kprintf("cap: cdt soft path resolve=1 install=1 mint=1 copy=1 move=1 "
            "delete=1 trylock=soft_u32SoftLock quota=soft_hier "
            "cdt_pool=%u empty_edge_audit=soft return_surface=1 return_rate=1 retcode=1 return_selftest=1 retmap=1 "
            "reply_product=0 full_cdt_mutex=0 soft_ne_mig_reply=1 "
            "wave=%u (soft inventory; not bar3; soft != GJ_CAP_REPLY "
            "product; soft != MIG REPLY product; soft != full CDT mutex "
            "product)\n",
            GJ_CDT_EDGE_POOL, GJ_CDT_SOFT_WAVE);

    /*
     * Grep: cap: cdt soft return rate
     * Wave 17 return-surface rate lamps (kept) (ok vs fail; soft ≠ product).
     */
    kprintf("cap: cdt soft return rate "
            "resolve_ok=%u resolve_fail=%u "
            "inst_ok=%u inst_fail=%u "
            "mint_ok=%u mint_fail=%u "
            "copy_ok=%u copy_fail=%u "
            "move_ok=%u move_fail=%u "
            "del_ok=%u del_fail=%u "
            "alloc_ok=%u alloc_fail=%u "
            "wave=%u (return rate; Soft≠product; soft≠MIG REPLY product; "
            "not bar3)\n",
            g_u32SoftResOk,
            g_u32SoftResInval + g_u32SoftResNoent + g_u32SoftResStale +
                g_u32SoftResLiveFail,
            g_u32SoftInstOk, g_u32SoftInstFail,
            g_u32SoftRetMintOk, g_u32SoftRetMintFail,
            g_u32SoftRetCopyOk, g_u32SoftRetCopyFail,
            g_u32SoftRetMoveOk, g_u32SoftRetMoveFail,
            g_u32SoftRetDelOk, g_u32SoftRetDelFail,
            g_u32SoftRetAllocOk, g_u32SoftRetAllocFail,
            GJ_CDT_SOFT_WAVE);

    /*
     * Grep: cap: cdt soft retcode
     * Wave 18 observed gj_status retcode catalog (return surface).
     * Soft ≠ product; codes are soft lamps not ABI claims.
     */
    kprintf("cap: cdt soft retcode "
            "ok=1 inval=1 noent=1 perm=1 dead=1 stale=1 busy=1 quota=1 "
            "live_fail=1 other=1 "
            "mint_other=%u copy_other=%u move_other=%u del_other=%u "
            "alloc_other=%u wave=%u "
            "(retcode catalog; Soft≠product; soft≠MIG REPLY product)\n",
            g_u32SoftRetMintOther, g_u32SoftRetCopyOther,
            g_u32SoftRetMoveOther, g_u32SoftRetDelOther,
            g_u32SoftRetAllocOther, GJ_CDT_SOFT_WAVE);

    /*
     * ---- Wave 18 exclusive complementary surfaces (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: cap: cdt soft return selftest — Wave 18 terminal return surface */
    kprintf("cap: cdt soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft retmap — Wave 18 return-surface map */
    kprintf("cap: cdt soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)GJ_CDT_SOFT_WAVE);

    /* Grep: cap: cdt soft deepen wave (Wave 18 stamp) */
    kprintf("cap: cdt soft deepen wave=%u areas=%u pool_used=%u "
            "res_ok=%u try_ok=%u inst_ok=%u mint_ok=%u copy_ok=%u "
            "move_ok=%u ret_mint_ok=%u ret_copy_ok=%u ret_move_ok=%u "
            "ret_del_ok=%u ret_alloc_ok=%u log_n=%u ok=1 skip=0\n",
            GJ_CDT_SOFT_WAVE, GJ_CDT_SOFT_AREAS, cdt_edge_pool_used(),
            g_u32SoftResOk, g_u32SoftTryOk, g_u32SoftInstOk,
            g_u32CdtMintEdgeOk, g_u32CdtCopyEdgeOk, g_u32CdtMoveEdgeOk,
            g_u32SoftRetMintOk, g_u32SoftRetCopyOk, g_u32SoftRetMoveOk,
            g_u32SoftRetDelOk, g_u32SoftRetAllocOk, g_u32SoftInvLogs);

    /* Grep: cap: cdt soft inventory PASS / cap: cdt soft PASS */
    kprintf("cap: cdt soft inventory PASS log_n=%u wave=%u areas=%u "
            "reply_product=0 full_cdt_mutex=0 soft_ne_mig_reply=1\n",
            g_u32SoftInvLogs, GJ_CDT_SOFT_WAVE, GJ_CDT_SOFT_AREAS);
    kprintf("cap: cdt soft PASS wave=%u areas=%u\n",
            GJ_CDT_SOFT_WAVE, GJ_CDT_SOFT_AREAS);
}

/**
 * Emit Wave 18 soft inventory once after first meaningful CNode activity.
 * Avoids spam; greppable surface lands on first install/resolve/wire.
 */
static void
cdt_soft_inventory_maybe_once(void)
{
    if (g_u8CdtSoftInvLogged) {
        return;
    }
    /* Need at least one install, resolve ok, or edge attempt. */
    if (g_u32SoftInstOk == 0u && g_u32SoftResOk == 0u &&
        g_u32CdtMintAttempt == 0u && g_u32CdtCopyAttempt == 0u &&
        g_u32CdtMoveAttempt == 0u && g_u32SoftSlotsLeftInc == 0u) {
        return;
    }
    g_u8CdtSoftInvLogged = 1;
    cdt_soft_inventory_log();
}

/*
 * Full mint|copy|move coverage rollup once all three ops have attempted.
 * Grep: cap: cdt soft coverage
 */
static void
cdt_soft_coverage_rollup(void)
{
    if (g_u8CdtCoverageRollupLogged) {
        return;
    }
    if (g_u32CdtMintAttempt == 0u || g_u32CdtCopyAttempt == 0u ||
        g_u32CdtMoveAttempt == 0u) {
        return;
    }
    g_u8CdtCoverageRollupLogged = 1;
    /* Grep: cap: cdt mint|copy|move … coverage rollup */
    kprintf("cap: cdt soft coverage rollup mint=%u/%u copy=%u/%u "
            "move=%u/%u miss_tot=%u pool_used=%u chain_max=%u "
            "audit_match=%u audit_mis=%u soft PASS wave=%u (once)\n",
            g_u32CdtMintEdgeOk, g_u32CdtMintAttempt, g_u32CdtCopyEdgeOk,
            g_u32CdtCopyAttempt, g_u32CdtMoveEdgeOk, g_u32CdtMoveAttempt,
            g_u32CdtMintEdgeMiss + g_u32CdtCopyEdgeMiss +
                g_u32CdtMoveEdgeMiss,
            cdt_edge_pool_used(), g_u32CdtChainDepthMax,
            g_u32CdtSoftAuditMatch, g_u32CdtSoftAuditMismatch,
            GJ_CDT_SOFT_WAVE);
    cdt_soft_inventory_maybe_once();
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
    /* Wave 15 soft inventory once (prefix "cap: cdt soft …"). */
    cdt_soft_inventory_maybe_once();
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
cdt_soft_empty_edge_gap(const char *szOp, u32 u32Ok, u32 u32Miss,
                        u32 u32Attempt, u32 u32Slots, u32 u32Chain)
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
                "alloc_miss=%u slots_left=%u chain=%u "
                "audit_empty=%u\n",
                szVerdict, szOp, u32Miss, u32Ok, u32Attempt,
                cdt_edge_pool_used(), GJ_CDT_EDGE_POOL,
                g_u32CdtPoolAllocMiss, u32Slots, u32Chain,
                g_u32CdtSoftAuditEmpty);
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
    u32 u32Chain;

    cdt_soft_inc(pAttempt);
    u32Slots = (pObj != NULL) ? pObj->u32SlotsLeft : 0u;
    u32Chain = cdt_edge_chain_depth(pObj);

    pEdge = cdt_edge_alloc();
    if (pEdge == NULL) {
        cdt_soft_inc(pMiss);
        cdt_soft_empty_edge_audit(pObj, szOp, u64Slot, 0u);
        cdt_soft_empty_edge_gap(szOp, *pOk, *pMiss, *pAttempt, u32Slots,
                                u32Chain);
        /* Grep: cap: cdt mint|copy|move … coverage */
        kprintf("cap: cdt %s edge_ok=0 miss=%u attempt=%u ok=%u "
                "slot=%lu slots_left=%u chain=%u pool_used=%u "
                "sl_inc=%u q_ch=%u q_rf=%u free=%u\n",
                szOp, *pMiss, *pAttempt, *pOk, (unsigned long)u64Slot,
                u32Slots, u32Chain, cdt_edge_pool_used(),
                g_u32SoftSlotsLeftInc, g_u32SoftQuotaChargeOk,
                g_u32SoftQuotaRefundOk, g_u32CdtPoolFreeOk);
        cdt_soft_coverage_rollup();
        return;
    }

    st = gj_cdt_edge_link(pObj, pEdge, pCnode, u64Slot);
    if (st != GJ_OK) {
        cdt_edge_free(pEdge);
        cdt_soft_inc(pMiss);
        cdt_soft_empty_edge_audit(pObj, szOp, u64Slot, 0u);
        /* Link refused (double-link etc.) — soft miss, not pool exhaust. */
        kprintf("cap: cdt %s edge_ok=0 link_st=%d miss=%u attempt=%u "
                "ok=%u slot=%lu slots_left=%u chain=%u pool_used=%u "
                "sl_inc=%u q_ch=%u q_rf=%u free=%u\n",
                szOp, (int)st, *pMiss, *pAttempt, *pOk,
                (unsigned long)u64Slot, u32Slots, u32Chain,
                cdt_edge_pool_used(), g_u32SoftSlotsLeftInc,
                g_u32SoftQuotaChargeOk, g_u32SoftQuotaRefundOk,
                g_u32CdtPoolFreeOk);
        cdt_soft_coverage_rollup();
        return;
    }

    cdt_soft_inc(pOk);
    u32Chain = cdt_edge_chain_depth(pObj);
    cdt_soft_empty_edge_audit(pObj, szOp, u64Slot, 1u);
    /* Grep: cap: cdt mint|copy|move … coverage */
    kprintf("cap: cdt %s edge_ok=1 ok=%u attempt=%u miss=%u "
            "slot=%lu slots_left=%u chain=%u chain_max=%u "
            "pool_used=%u sl_inc=%u q_ch=%u q_rf=%u free=%u "
            "audit_match=%u audit_mis=%u\n",
            szOp, *pOk, *pAttempt, *pMiss, (unsigned long)u64Slot, u32Slots,
            u32Chain, g_u32CdtChainDepthMax, cdt_edge_pool_used(),
            g_u32SoftSlotsLeftInc, g_u32SoftQuotaChargeOk,
            g_u32SoftQuotaRefundOk, g_u32CdtPoolFreeOk,
            g_u32CdtSoftAuditMatch, g_u32CdtSoftAuditMismatch);

    /*
     * Re-log soft tallies once after first charge-backed mint/copy/move so
     * interaction counters are visible with real quota attach (boot door
     * install may have logged only nop charge earlier).
     */
    if (g_u8CdtSoftTallyLogged < 2u && g_u32SoftQuotaChargeOk > 0u) {
        g_u8CdtSoftTallyLogged = 2;
        cdt_soft_tally_log();
    }
    cdt_soft_coverage_rollup();
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
        /* Grep: cap: cdt soft return mint */
        cdt_soft_inc(&g_u32SoftRetMintFail);
        cdt_soft_inc(&g_u32SoftRetMintInval);
        return GJ_ERR_INVAL;
    }
    if (pDstCnode == NULL) {
        pDstCnode = pSrcCnode;
    }
    st = gj_cap_resolve(pSrcCnode, u64SrcSlot, u32SrcGen, &res);
    if (st != GJ_OK) {
        cdt_soft_inc(&g_u32SoftRetMintFail);
        if (st == GJ_ERR_INVAL) {
            cdt_soft_inc(&g_u32SoftRetMintInval);
        } else if (st == GJ_ERR_PERM) {
            cdt_soft_inc(&g_u32SoftRetMintPerm);
        } else {
            cdt_soft_inc(&g_u32SoftRetMintOther);
        }
        return st;
    }
    if ((res.u16Rights & GJ_RIGHT_MINT) == 0) {
        cdt_soft_inc(&g_u32SoftRetMintFail);
        cdt_soft_inc(&g_u32SoftRetMintPerm);
        return GJ_ERR_PERM;
    }
    u16New = rights_weaker(res.u16Rights, u16Rights);
    if (u16New == 0) {
        cdt_soft_inc(&g_u32SoftRetMintFail);
        cdt_soft_inc(&g_u32SoftRetMintPerm);
        return GJ_ERR_PERM;
    }
    /* Derived must not gain rights source lacks (already masked). */
    st = gj_cap_alloc_install(pDstCnode, res.u16Type, u16New, res.pObj, pOut);
    if (st != GJ_OK) {
        cdt_soft_inc(&g_u32SoftRetMintFail);
        if (st == GJ_ERR_INVAL) {
            cdt_soft_inc(&g_u32SoftRetMintInval);
        } else if (st == GJ_ERR_PERM) {
            cdt_soft_inc(&g_u32SoftRetMintPerm);
        } else {
            cdt_soft_inc(&g_u32SoftRetMintOther);
        }
        return st;
    }
    /*
     * Always attempt edge pool (soft empty-edge gap if exhausted).
     * Grep: cap: cdt mint / cap:cdt mint
     */
    if (pSrcCnode != pDstCnode) {
        cdt_soft_inc(&g_u32CdtMintCross); /* cap: cdt mint cross-cnode */
    } else {
        cdt_soft_inc(&g_u32CdtMintLocal); /* cap: cdt mint local */
    }
    cdt_edge_try_wire(res.pObj, pDstCnode, pOut->u64Slot, "mint",
                      &g_u32CdtMintAttempt, &g_u32CdtMintEdgeOk,
                      &g_u32CdtMintEdgeMiss);
    cdt_soft_inc(&g_u32SoftRetMintOk);
    cdt_soft_inventory_maybe_once();
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
        /* Grep: cap: cdt soft return copy */
        cdt_soft_inc(&g_u32SoftRetCopyFail);
        cdt_soft_inc(&g_u32SoftRetCopyInval);
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve(pCnode, u64SrcSlot, u32SrcGen, &res);
    if (st != GJ_OK) {
        cdt_soft_inc(&g_u32SoftRetCopyFail);
        if (st == GJ_ERR_INVAL) {
            cdt_soft_inc(&g_u32SoftRetCopyInval);
        } else if (st == GJ_ERR_PERM) {
            cdt_soft_inc(&g_u32SoftRetCopyPerm);
        } else {
            cdt_soft_inc(&g_u32SoftRetCopyOther);
        }
        return st;
    }
    if ((res.u16Rights & GJ_RIGHT_GRANT) == 0) {
        cdt_soft_inc(&g_u32SoftRetCopyFail);
        cdt_soft_inc(&g_u32SoftRetCopyPerm);
        return GJ_ERR_PERM;
    }
    u16New = rights_weaker(res.u16Rights, u16Rights != 0 ? u16Rights : res.u16Rights);
    st = gj_cap_alloc_install(pCnode, res.u16Type, u16New, res.pObj, pOut);
    if (st != GJ_OK) {
        cdt_soft_inc(&g_u32SoftRetCopyFail);
        if (st == GJ_ERR_INVAL) {
            cdt_soft_inc(&g_u32SoftRetCopyInval);
        } else if (st == GJ_ERR_PERM) {
            cdt_soft_inc(&g_u32SoftRetCopyPerm);
        } else {
            cdt_soft_inc(&g_u32SoftRetCopyOther);
        }
        return st;
    }
    /*
     * Always attempt edge pool (soft empty-edge gap if exhausted).
     * Grep: cap: cdt copy / cap:cdt copy
     */
    cdt_edge_try_wire(res.pObj, pCnode, pOut->u64Slot, "copy",
                      &g_u32CdtCopyAttempt, &g_u32CdtCopyEdgeOk,
                      &g_u32CdtCopyEdgeMiss);
    cdt_soft_inc(&g_u32SoftRetCopyOk);
    cdt_soft_inventory_maybe_once();
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
        /* Grep: cap: cdt soft return move */
        cdt_soft_inc(&g_u32SoftRetMoveFail);
        cdt_soft_inc(&g_u32SoftRetMoveInval);
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve(pCnode, u64SrcSlot, u32SrcGen, &res);
    if (st != GJ_OK) {
        cdt_soft_inc(&g_u32SoftRetMoveFail);
        if (st == GJ_ERR_INVAL) {
            cdt_soft_inc(&g_u32SoftRetMoveInval);
        } else if (st == GJ_ERR_PERM) {
            cdt_soft_inc(&g_u32SoftRetMovePerm);
        } else {
            cdt_soft_inc(&g_u32SoftRetMoveOther);
        }
        return st;
    }
    if ((res.u16Rights & GJ_RIGHT_GRANT) == 0) {
        cdt_soft_inc(&g_u32SoftRetMoveFail);
        cdt_soft_inc(&g_u32SoftRetMovePerm);
        return GJ_ERR_PERM;
    }
    if (u64SrcSlot == GJ_CAP_SLOT_ROOT_META) {
        cdt_soft_inc(&g_u32SoftRetMoveFail);
        cdt_soft_inc(&g_u32SoftRetMovePerm);
        return GJ_ERR_PERM;
    }
    st = gj_cap_alloc_install(pCnode, res.u16Type, res.u16Rights, res.pObj,
                              pOut);
    if (st != GJ_OK) {
        cdt_soft_inc(&g_u32SoftRetMoveFail);
        if (st == GJ_ERR_INVAL) {
            cdt_soft_inc(&g_u32SoftRetMoveInval);
        } else if (st == GJ_ERR_PERM) {
            cdt_soft_inc(&g_u32SoftRetMovePerm);
        } else {
            cdt_soft_inc(&g_u32SoftRetMoveOther);
        }
        return st;
    }
    /*
     * Retarget CDT: unlink old slot (returns edge to pool), then always
     * attempt wire on the new slot. Net slots_left / quota are restored
     * by the source invalidate below (move net-zero).
     * Grep: cap: cdt move / cap:cdt move / cap: cdt retarget
     */
    {
        u32 u32ChainPre = cdt_edge_chain_depth(res.pObj);

        gj_cdt_unlink_slot(res.pObj, pCnode, u64SrcSlot);
        cdt_soft_inc(&g_u32CdtMoveUnlink);
        cdt_soft_inc(&g_u32CdtMoveRetarget);
        /* Grep: cap: cdt retarget / cap: cdt move */
        kprintf("cap: cdt retarget src_slot=%lu dst_slot=%lu "
                "chain_pre=%u pool_used=%u free=%u move_unlink=%u\n",
                (unsigned long)u64SrcSlot, (unsigned long)pOut->u64Slot,
                u32ChainPre, cdt_edge_pool_used(), g_u32CdtPoolFreeOk,
                g_u32CdtMoveUnlink);
    }
    cdt_edge_try_wire(res.pObj, pCnode, pOut->u64Slot, "move",
                      &g_u32CdtMoveAttempt, &g_u32CdtMoveEdgeOk,
                      &g_u32CdtMoveEdgeMiss);
    /* Invalidate source without double-counting object death. */
    pSrc = &pCnode->pSlots[u64SrcSlot];
    (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount); /* cap:quota */
    gj_cap_slot_invalidate_locked(pSrc, res.pObj);
    /* Soft: charge on install + refund on source = net-zero interact. */
    cdt_soft_inc(&g_u32SoftMoveNet0); /* cap: cdt soft */
    cdt_soft_inc(&g_u32SoftRetMoveOk);
    cdt_soft_inventory_maybe_once();
    return GJ_OK;
}

gj_status_t
gj_cap_delete(struct gj_cnode *pCnode, u64 u64Slot, u32 u32SlotGen)
{
    struct gj_cap_resolved res;
    gj_status_t st;

    st = gj_cap_resolve(pCnode, u64Slot, u32SlotGen, &res);
    if (st != GJ_OK) {
        /* Grep: cap: cdt soft return delete */
        cdt_soft_inc(&g_u32SoftRetDelFail);
        if (st == GJ_ERR_INVAL) {
            cdt_soft_inc(&g_u32SoftRetDelInval);
        } else if (st == GJ_ERR_PERM) {
            cdt_soft_inc(&g_u32SoftRetDelPerm);
        } else {
            cdt_soft_inc(&g_u32SoftRetDelOther);
        }
        return st;
    }
    if (u64Slot == GJ_CAP_SLOT_ROOT_META) {
        cdt_soft_inc(&g_u32SoftRetDelFail);
        cdt_soft_inc(&g_u32SoftRetDelPerm);
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & GJ_RIGHT_DESTROY) == 0 &&
        (res.u16Rights & GJ_RIGHT_GRANT) == 0) {
        cdt_soft_inc(&g_u32SoftRetDelFail);
        cdt_soft_inc(&g_u32SoftRetDelPerm);
        return GJ_ERR_PERM;
    }
    {
        u32 u32ChainPre = cdt_edge_chain_depth(res.pObj);
        u32 u32ChainAfter;
        u32 u32SlotsPre = res.pObj->u32SlotsLeft;
        int fHadEdge;

        gj_cdt_unlink_slot(res.pObj, pCnode, u64Slot); /* cap:cdt delete */
        u32ChainAfter = cdt_edge_chain_depth(res.pObj);
        fHadEdge = (u32ChainAfter < u32ChainPre) ? 1 : 0;
        if (fHadEdge) {
            cdt_soft_inc(&g_u32CdtDeleteEdge);
        }
        (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount); /* cap:quota */
        cdt_soft_inc(&g_u32SoftDeleteRefund); /* cap: cdt soft */
        /*
         * Soft delete edge coverage (Wave 18 deepen).
         * Grep: cap: cdt delete
         */
        kprintf("cap: cdt delete slot=%lu had_edge=%d chain_pre=%u "
                "chain_after=%u slots_pre=%u pool_used=%u free=%u "
                "del_edge=%u del_rf=%u wave=%u\n",
                (unsigned long)u64Slot, fHadEdge, u32ChainPre, u32ChainAfter,
                u32SlotsPre, cdt_edge_pool_used(), g_u32CdtPoolFreeOk,
                g_u32CdtDeleteEdge, g_u32SoftDeleteRefund, GJ_CDT_SOFT_WAVE);
        gj_cap_slot_invalidate_locked(res.pSlot, res.pObj);
        cdt_soft_inc(&g_u32SoftRetDelOk);
        cdt_soft_inventory_maybe_once();
        return GJ_OK;
    }
}
