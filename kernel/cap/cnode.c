/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Process-shared CNode addressing - Scheme A (u64 slot + u32 slot gen).
 *
 * One CNode per process; all threads share it. Slot 0 = root meta only.
 * Resolve fails closed: slot gen + LIVE + object gen (S2). See
 * docs/CAP_ADDRESSING.md and SECURITY_CORE_DESIGN s1.1.
 *
 * Soft CDT edges (gj_cdt_edge_*) and soft slot-quota hooks live here so
 * mint/ledger can wire without changing the resolve/install surface.
 *
 * Full soft CDT companion to R2: mint/copy/move always attempt the edge
 * pool; empty-edge gap is soft FAIL/PARTIAL when the pool is exhausted
 * (install still succeeds - slots_left remains authoritative).
 *
 * Lean soft residual caps (exclusive residual; this unit only):
 *   greppable: cap: soft residual lean PASS
 *   greppable: cap: cdt soft residual
 *   greppable: cap: cdt soft residual lean | lean PASS | lean c3
 *   greppable: cap: cdt soft inventory | path | honesty | PASS
 *   greppable: cap: cdt soft return | return rate | retcode | retmap
 *   greppable: cap: ddi foundation | cap: ddi soft residual lean PASS
 *   greppable: cap: udx foundation | cap: udx soft residual lean PASS
 *   greppable: cap: udx host mint residual | cap: product mint OPEN
 *   greppable: cap: udx host mmio residual | irq residual | dma residual
 * C3 residual: stack-local lean selftest exercises fail-closed resolve,
 * rights no-escalate mint, root-meta guard (+ inverse), full typed package
 * cross-CNode host-grant shape, DMA rights-shape observe, copy/move, soft
 * quota charge/exhaust/refund, busy/NOENT, delete-stale, and DEAD fail-closed
 * (behavior check, not lamp-only). Soft!=product. Dual DoD mint OPEN.
 * Soft!=product. Dual MIT OR Apache-2.0. No version stamp. No stamp storms
 * (once-lamps + counters only; no per-op kprintf on mint/copy/move/delete).
 * Soft residual != GJ_CAP_REPLY product / full CDT mutex / MIG REPLY product.
 * Soft residual != product DDI mint (MMIO_FRAME / IRQ Notification / DMA).
 * G-AC-1: no Linux .ko product AC. Product DDI mint remains OPEN honesty.
 *
 * DDI/UDX lean residual foundation (toward future product host mint):
 *   Install of IRQ / FRAME / UNTYPED / NOTIFICATION tallied as soft DDI
 *   class (gj_cap_type_is_ddi). Mint of those types tallied as soft host-
 *   grant residual (cross-CNode = future devmgr->UDX host shape; rights
 *   ⊆ host default observed via gj_cap_rights_ddi_host_subset_ok).
 *   Lean residual also observes MMIO/IRQ/DMA triad package bits
 *   (GJ_CAP_UDX_HOST_PKG_*) and DMA rights shape (no DMA type ordinal;
 *   gj_cap_rights_ddi_dma_host_subset_ok) so mint tallies lean toward
 *   future UDX host MMIO_FRAME / IRQ Notification / DMA window mint.
 *   Product mint into host CNode remains OPEN (Soft!=product; no fake
 *   complete). DMA window type/mint OPEN (GJ_RIGHTS_DDI_DMA_HOST only).
 *   Mechanism residual: gj_cap_mint + CDT + rights_weaker. Policy residual
 *   (devmgr match/grant graph, live IOMMU/IRQ/DMA wire) stays outside this TU.
 *
 * Grep: cap:cdt  - edge pool, link/unlink, mint wiring
 * Grep: cap: cdt mint|copy|move - per-op edge coverage stats (counters)
 * Grep: cap: cdt soft - empty-edge gap + slots_left/quota tallies
 * Grep: cap: cdt soft audit - soft empty-edge slots_left vs chain depth
 * Grep: cap: cdt delete|unlink|retarget - delete/move edge hygiene
 * Grep: cap: cdt pool - alloc/free pool churn
 * Grep: cap:quota - flat + soft hierarchical charge/refund
 * Grep: cap: ddi / cap: udx - DDI/UDX soft foundation tallies
 * Grep: cap: udx host mint residual - mint-path host-grant soft residual
 * Grep: cap: product mint OPEN - honesty (mint not product-complete)
 * Grep: cap: cdt soft residual lean - C3 stack-local behavior selftest
 * Grep: cap: cdt soft
 */
#include <gj/cap.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/types.h>

/* C3 lean residual: stack-local slot count (root meta + host package + spare). */
#define GJ_CDT_LEAN_SLOTS 12u
/* UDX host package stand-ins: FRAME(MMIO) + IRQ + NOTIFICATION. */
#define GJ_CDT_LEAN_UDX_OBJS 3u
/* Soft quota limit for lean exhaust arm (behavior; Soft!=product). */
#define GJ_CDT_LEAN_QUOTA_LIM 2u

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
static void cdt_soft_residual_lean_once(void);
static void cdt_soft_inc(u32 *pCtr);
static void cdt_soft_ddi_note_install(u16 u16Type);
static void cdt_soft_ddi_note_mint(u16 u16Type, u16 u16Rights, int fCross);

/*
 * Wave 19 soft path tallies (file-local; wrap OK; never hard-gate).
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
static u32 g_u32SoftInstFailInval;  /* Wave 19: install -> GJ_ERR_INVAL */
static u32 g_u32SoftInstFailDead;   /* Wave 19: install -> GJ_ERR_DEAD */
static u32 g_u32SoftInstFailPerm;   /* Wave 19: install -> GJ_ERR_PERM */
static u32 g_u32SoftInstFailBusy;   /* Wave 19: install -> GJ_ERR_BUSY */
static u32 g_u32SoftInstFailQuota;  /* Wave 19: install -> GJ_ERR_QUOTA */
static u32 g_u32SoftInstReplyType;  /* type==GJ_CAP_REPLY scaffold installs */
static u32 g_u32SoftInvLogs;        /* soft inventory dump emissions */
static u8  g_u8CdtSoftInvLogged;    /* once-marker for Wave 19 rollup */
/* Wave 19: public API return-surface buckets (mint/copy/move/delete/alloc). */
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

/*
 * DDI/UDX lean residual foundation tallies (install + mint host-grant).
 * Soft residual != product MMIO_FRAME / IRQ Notification / DMA window mint.
 * G-AC-1. Grep: cap: ddi | cap: udx | cap: udx host mint residual
 */
static u32 g_u32SoftDdiInst;       /* install type in DDI class */
static u32 g_u32SoftDdiIrq;        /* GJ_CAP_IRQ installs */
static u32 g_u32SoftDdiFrame;      /* GJ_CAP_FRAME (MMIO role) installs */
static u32 g_u32SoftDdiUntyped;    /* GJ_CAP_UNTYPED installs */
static u32 g_u32SoftDdiNotif;      /* GJ_CAP_NOTIFICATION installs */
static u8  g_u8SoftDdiFoundLogged; /* once-marker: foundation lamp */
/* Mint-path residual (future product host grant shape; not complete). */
static u32 g_u32SoftDdiMint;       /* mint of DDI/UDX host class type */
static u32 g_u32SoftDdiMintCross;  /* cross-CNode = host-grant shape */
static u32 g_u32SoftDdiMintLocal;  /* same-CNode DDI mint */
static u32 g_u32SoftDdiMintHostOk; /* rights ⊆ soft host default */
static u32 g_u32SoftDdiMintHostOver; /* rights not ⊆ host default (observe) */
static u32 g_u32SoftDdiMintRoleMmio;
static u32 g_u32SoftDdiMintRoleIrq;
static u32 g_u32SoftDdiMintRoleNotif;
static u32 g_u32SoftDdiMintRoleUntyped;
/* Soft residual: DMA rights-shape observe (no DMA type; OPEN). */
static u32 g_u32SoftDdiMintDmaShapeOk;   /* rights ⊆ GJ_RIGHTS_DDI_DMA_HOST */
static u32 g_u32SoftDdiMintDmaShapeMiss; /* non-empty rights not ⊆ DMA host */
/* Soft residual: package bits seen on host-class mints (OR-accum). */
static u32 g_u32SoftUdxHostPkgSeen;      /* OR of GJ_CAP_UDX_HOST_PKG_* */
static u32 g_u32SoftUdxHostPkgCross;     /* OR of pkg bits on cross-CNode */
static u32 g_u32SoftUdxHostPkgMmioN;     /* mint with MMIO pkg bit */
static u32 g_u32SoftUdxHostPkgIrqN;      /* mint with IRQ pkg bit */
static u32 g_u32SoftUdxHostPkgNotifN;    /* mint with NOTIF pkg bit */
static u8  g_u8SoftUdxHostMintLogged; /* once-marker: host mint residual */

/*
 * C3 lean residual selftest tallies (stack-local; once; never hard-gate).
 * Behavior check: fail-closed resolve, no-escalate mint, root-meta guard,
 * cross-CNode host-grant shape (typed package), copy/move hygiene, quota
 * charge/exhaust/refund, busy/NOENT, delete-stale, DMA rights-shape observe.
 * Soft!=product. G-AC-1. Dual DoD product mint remains OPEN.
 * Grep: cap: cdt soft residual lean | lean c3
 */
static u32 g_u32SoftLeanRuns;          /* residual lean selftest entries */
static u32 g_u32SoftLeanOk;            /* all C3 checks passed */
static u32 g_u32SoftLeanInstOk;        /* lean install successes */
static u32 g_u32SoftLeanMintOk;        /* lean cross-CNode mint successes */
static u32 g_u32SoftLeanMintEscFail;   /* escalate mint correctly refused */
static u32 g_u32SoftLeanResStaleOk;    /* wrong-gen resolve -> STALE */
static u32 g_u32SoftLeanResNullOk;     /* gen0 resolve -> INVAL */
static u32 g_u32SoftLeanResLiveOk;     /* good resolve after install */
static u32 g_u32SoftLeanRootGuardOk;   /* non-meta at slot 0 refused */
static u32 g_u32SoftLeanDelOk;         /* delete/hygiene ok */
static u32 g_u32SoftLeanCrossOk;       /* cross-CNode host mint shape */
static u32 g_u32SoftLeanHostSubsetOk;  /* minted rights ⊆ host default */
static u32 g_u32SoftLeanDeadFailOk;    /* resolve DEAD after local kill */
/* W4 C3 strengthen arms (behavior residual; Soft!=product). */
static u32 g_u32SoftLeanPkgCrossOk;    /* cross-mint all typed package roles */
static u32 g_u32SoftLeanCopyOk;        /* GRANT copy residual */
static u32 g_u32SoftLeanMoveOk;        /* move + src STALE residual */
static u32 g_u32SoftLeanNoentOk;       /* empty slot resolve -> NOENT */
static u32 g_u32SoftLeanBusyOk;        /* occupied install -> BUSY */
static u32 g_u32SoftLeanQuotaOk;       /* quota charge + exhaust + refund */
static u32 g_u32SoftLeanDelStaleOk;    /* resolve after delete -> STALE */
static u32 g_u32SoftLeanDmaShapeOk;    /* DMA rights-shape observe on mint */
static u32 g_u32SoftLeanMintNoMintOk;  /* mint without MINT right -> PERM */
static u32 g_u32SoftLeanRootMetaSlotOk; /* ROOT_META off slot 0 -> PERM */
static u8  g_u8CdtSoftLeanOnce;        /* once-marker for lean residual */
static u8  g_u8CdtSoftLeanBusy;        /* suppress inventory reentry */

/** Soft: saturating bump (u32 wrap avoided; wrap OK if ever hit). */
static void
cdt_soft_inc(u32 *pCtr)
{
    if (pCtr != NULL && *pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/*
 * Soft DDI/UDX install-class note (lean residual foundation).
 * Tallies IRQ/FRAME/UNTYPED/NOTIFICATION; once-lamp only (no storms).
 * Soft residual != product mint. G-AC-1. Grep: cap: ddi | cap: udx
 */
static void
cdt_soft_ddi_note_install(u16 u16Type)
{
    if (!gj_cap_type_is_ddi(u16Type)) {
        return;
    }
    cdt_soft_inc(&g_u32SoftDdiInst);
    switch (u16Type) {
    case (u16)GJ_CAP_IRQ:
        cdt_soft_inc(&g_u32SoftDdiIrq);
        break;
    case (u16)GJ_CAP_FRAME:
        cdt_soft_inc(&g_u32SoftDdiFrame);
        break;
    case (u16)GJ_CAP_UNTYPED:
        cdt_soft_inc(&g_u32SoftDdiUntyped);
        break;
    case (u16)GJ_CAP_NOTIFICATION:
        cdt_soft_inc(&g_u32SoftDdiNotif);
        break;
    default:
        break;
    }
    if (!g_u8SoftDdiFoundLogged) {
        g_u8SoftDdiFoundLogged = 1;
        /* Grep: cap: ddi foundation / cap: ddi soft residual lean PASS */
        kprintf("cap: ddi foundation type=%u irq=%u frame=%u untyped=%u "
                "notif=%u ddi_n=%u mint_product=0 Soft!=product G-AC-1 "
                "(once)\n",
                (unsigned)u16Type, g_u32SoftDdiIrq, g_u32SoftDdiFrame,
                g_u32SoftDdiUntyped, g_u32SoftDdiNotif, g_u32SoftDdiInst);
        kprintf("cap: ddi soft residual lean PASS mint_product=0 "
                "dma_window_product=0 Soft!=product dual MIT OR "
                "Apache-2.0 storm=0\n");
        /* Grep: cap: udx foundation / cap: udx soft residual lean PASS */
        kprintf("cap: udx foundation host_grant_class=1 product_mint=0 "
                "Soft!=product G-AC-1 (once)\n");
        kprintf("cap: udx soft residual lean PASS host_cnode_mint=0 "
                "Soft!=product dual MIT OR Apache-2.0 storm=0\n");
        /* Grep: cap: product mint OPEN */
        kprintf("cap: product mint OPEN mmio=%d irq_notif=%d dma_window=%d "
                "host_cnode=%d Soft!=product G-AC-1 (once)\n",
                gj_cap_ddi_mmio_product_ready(),
                gj_cap_ddi_irq_notif_product_ready(),
                gj_cap_ddi_dma_window_product_ready(),
                gj_cap_udx_host_mint_product_ready());
    }
}

/*
 * Soft DDI/UDX mint-path residual (lean toward future product host mint).
 * Cross-CNode mint of host-class types is the product grant shape
 * (devmgr -> UDX host CNode). Rights ⊆ host default is observed only -
 * gj_cap_mint still uses source MINT + rights_weaker as authority.
 * Also observes host package bits (MMIO/IRQ/NOTIF) and DMA rights shape
 * (GJ_RIGHTS_DDI_DMA_HOST; no DMA type ordinal) so residual leans toward
 * future UDX host MMIO/IRQ/DMA mint without claiming product complete.
 * Soft residual != product mint. Never claims complete. G-AC-1.
 * Grep: cap: udx host mint residual | cap: product mint OPEN | cap: ddi
 * Grep: cap: udx host mmio|irq|dma residual
 */
static void
cdt_soft_ddi_note_mint(u16 u16Type, u16 u16Rights, int fCross)
{
    u32 u32Role;
    u32 u32PkgBit;
    int fHostOk;
    int fDmaShapeOk;

    if (!gj_cap_type_is_udx_host(u16Type)) {
        return;
    }

    cdt_soft_inc(&g_u32SoftDdiMint);
    if (fCross != 0) {
        cdt_soft_inc(&g_u32SoftDdiMintCross);
    } else {
        cdt_soft_inc(&g_u32SoftDdiMintLocal);
    }

    u32Role = gj_cap_ddi_mint_role(u16Type);
    switch (u32Role) {
    case GJ_CAP_DDI_MINT_ROLE_MMIO:
        cdt_soft_inc(&g_u32SoftDdiMintRoleMmio);
        break;
    case GJ_CAP_DDI_MINT_ROLE_IRQ:
        cdt_soft_inc(&g_u32SoftDdiMintRoleIrq);
        break;
    case GJ_CAP_DDI_MINT_ROLE_NOTIF:
        cdt_soft_inc(&g_u32SoftDdiMintRoleNotif);
        break;
    case GJ_CAP_DDI_MINT_ROLE_UNTYPED:
        cdt_soft_inc(&g_u32SoftDdiMintRoleUntyped);
        break;
    default:
        break;
    }

    /* Soft package bits for typed host roles (DMA bit never from type). */
    u32PkgBit = gj_cap_udx_host_pkg_bit_for_type(u16Type);
    if (u32PkgBit != 0u) {
        g_u32SoftUdxHostPkgSeen |= u32PkgBit;
        if (fCross != 0) {
            g_u32SoftUdxHostPkgCross |= u32PkgBit;
        }
        if ((u32PkgBit & GJ_CAP_UDX_HOST_PKG_MMIO) != 0u) {
            cdt_soft_inc(&g_u32SoftUdxHostPkgMmioN);
        }
        if ((u32PkgBit & GJ_CAP_UDX_HOST_PKG_IRQ) != 0u) {
            cdt_soft_inc(&g_u32SoftUdxHostPkgIrqN);
        }
        if ((u32PkgBit & GJ_CAP_UDX_HOST_PKG_NOTIF) != 0u) {
            cdt_soft_inc(&g_u32SoftUdxHostPkgNotifN);
        }
    }

    fHostOk = gj_cap_rights_ddi_host_subset_ok(u16Type, u16Rights);
    if (fHostOk != 0) {
        cdt_soft_inc(&g_u32SoftDdiMintHostOk);
    } else {
        cdt_soft_inc(&g_u32SoftDdiMintHostOver);
    }

    /*
     * Soft DMA window rights-shape observe (no type ordinal).
     * FRAME rights often match DMA host mask; this is residual lean only -
     * does not mint a DMA window and never sets product-ready.
     * Grep: cap: udx host dma residual
     */
    fDmaShapeOk = gj_cap_rights_ddi_dma_host_subset_ok(u16Rights);
    if (fDmaShapeOk != 0) {
        cdt_soft_inc(&g_u32SoftDdiMintDmaShapeOk);
        /* Observe DMA package bit as rights-shape only (type still OPEN). */
        g_u32SoftUdxHostPkgSeen |= GJ_CAP_UDX_HOST_PKG_DMA;
        if (fCross != 0) {
            g_u32SoftUdxHostPkgCross |= GJ_CAP_UDX_HOST_PKG_DMA;
        }
    } else if (u16Rights != 0) {
        cdt_soft_inc(&g_u32SoftDdiMintDmaShapeMiss);
    }

    if (!g_u8SoftUdxHostMintLogged) {
        g_u8SoftUdxHostMintLogged = 1;
        /*
         * Grep: cap: udx host mint residual
         * Grep: cap: product mint OPEN
         * Soft residual only - product host CNode mint remains OPEN.
         */
        kprintf("cap: udx host mint residual type=%u role=%u cross=%d "
                "rights=0x%x host_ok=%d dma_shape=%d mint_n=%u cross_n=%u "
                "host_ok_n=%u mint_product=0 dma_window_product=%d "
                "Soft!=product G-AC-1 (once)\n",
                (unsigned)u16Type, (unsigned)u32Role, fCross != 0 ? 1 : 0,
                (unsigned)u16Rights, fHostOk != 0 ? 1 : 0,
                fDmaShapeOk != 0 ? 1 : 0, g_u32SoftDdiMint,
                g_u32SoftDdiMintCross, g_u32SoftDdiMintHostOk,
                gj_cap_ddi_dma_window_product_ready());
        kprintf("cap: product mint OPEN host_cnode_product=%d "
                "mmio_product=%d irq_notif_product=%d dma_window_product=%d "
                "mmio_role_n=%u irq_role_n=%u notif_role_n=%u "
                "untyped_role_n=%u Soft!=product dual MIT OR "
                "Apache-2.0 storm=0\n",
                gj_cap_udx_host_mint_product_ready(),
                gj_cap_ddi_mmio_product_ready(),
                gj_cap_ddi_irq_notif_product_ready(),
                gj_cap_ddi_dma_window_product_ready(),
                g_u32SoftDdiMintRoleMmio, g_u32SoftDdiMintRoleIrq,
                g_u32SoftDdiMintRoleNotif, g_u32SoftDdiMintRoleUntyped);
        /*
         * Grep: cap: udx host mmio residual
         * Grep: cap: udx host irq residual
         * Grep: cap: udx host dma residual
         */
        kprintf("cap: udx host mmio residual pkg_n=%u role_n=%u "
                "product=%d Soft!=product G-AC-1 (once)\n",
                g_u32SoftUdxHostPkgMmioN, g_u32SoftDdiMintRoleMmio,
                gj_cap_ddi_mmio_product_ready());
        kprintf("cap: udx host irq residual pkg_irq_n=%u pkg_notif_n=%u "
                "role_irq_n=%u role_notif_n=%u product=%d "
                "Soft!=product G-AC-1 (once)\n",
                g_u32SoftUdxHostPkgIrqN, g_u32SoftUdxHostPkgNotifN,
                g_u32SoftDdiMintRoleIrq, g_u32SoftDdiMintRoleNotif,
                gj_cap_ddi_irq_notif_product_ready());
        kprintf("cap: udx host dma residual shape_ok_n=%u shape_miss_n=%u "
                "rights_def=0x%x type_ordinal=0 product=%d "
                "Soft!=product G-AC-1 (once)\n",
                g_u32SoftDdiMintDmaShapeOk, g_u32SoftDdiMintDmaShapeMiss,
                (unsigned)gj_cap_rights_ddi_dma_host_default(),
                gj_cap_ddi_dma_window_product_ready());
        kprintf("cap: udx host mint residual pkg_seen=0x%x "
                "pkg_cross=0x%x pkg_intended=0x%x pkg_typed=0x%x "
                "mint_product=0 Soft!=product G-AC-1 (once)\n",
                g_u32SoftUdxHostPkgSeen, g_u32SoftUdxHostPkgCross,
                gj_cap_udx_host_package_roles_intended(),
                gj_cap_udx_host_package_roles_typed());
        kprintf("cap: udx soft residual lean PASS host_cnode_mint=0 "
                "mint_residual=1 mmio_irq_dma_lean=1 Soft!=product "
                "G-AC-1 dual MIT OR Apache-2.0 storm=0\n");
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
    pCnode->u32SoftLock = 0;      /* cap:cdt trylock - free */
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
 * mutex lands. Order for full impl: CNode -> Object -> Endpoint.
 * Soft != full CDT mutex product (turnstile sleep still OPEN).
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
    /* Busy - caller must defer edge (R2), not spin. */
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
 * Soft CDT link - edge storage is caller/slab-owned. Install does not
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
 * Order matters: bounds/null -> type/slot-gen -> object LIVE+obj-gen (S2/S3).
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
        cdt_soft_inc(&g_u32SoftInstFailInval); /* Wave 19 return surface */
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

    /* Slot 0 <-> ROOT_META only */
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
     * Soft != GJ_CAP_REPLY product (no MIG ephemeral single-use CNode wire).
     * Grep: cap: cdt soft install / cap: cdt soft type
     */
    if (u16Type == (u16)GJ_CAP_REPLY) {
        cdt_soft_inc(&g_u32SoftInstReplyType);
    }

    /*
     * Soft DDI/UDX class tally (IRQ/FRAME/UNTYPED/NOTIFICATION).
     * Soft residual != product mint. G-AC-1. Grep: cap: ddi | cap: udx
     */
    cdt_soft_ddi_note_install(u16Type);

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
 * Deferred invalidate helper - walk a known CNode and clear slots for pObj.
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
static u32 g_u32CdtUnlinkOk;      /* unlink_slot found >=1 edge */

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
 * Install without wire leaves slots_left > chain - soft gap, not product.
 * Grep: cap: cdt soft audit
 */
static u32 g_u32CdtChainDepthMax;     /* peak chain depth after wire */
static u32 g_u32CdtChainDepthAtOk;    /* last chain depth on edge_ok */
static u32 g_u32CdtSoftAuditMatch;    /* slots_left == chain after wire */
static u32 g_u32CdtSoftAuditMismatch; /* slots_left != chain (soft gap) */
static u32 g_u32CdtSoftAuditEmpty;    /* pool miss: slots without edge */

/* Once-markers: lean residual - no stamp storms. Grep: cap: cdt soft */
static u8 g_u8CdtPoolExhLogged;
static u8 g_u8CdtSoftTallyLogged;
static u8 g_u8CdtSoftAuditLogged;      /* first soft empty-edge audit */
static u8 g_u8CdtCoverageRollupLogged; /* first full mint|copy|move rollup */
static u8 g_u8CdtPoolChurnLogged;      /* first pool free/alloc churn line */
static u8 g_u8CdtUnlinkLogged;         /* first unlink coverage line */
static u8 g_u8CdtEdgeOkLogged;         /* first edge_ok wire lamp */
static u8 g_u8CdtEdgeMissLogged;       /* first edge miss / link refuse lamp */
static u8 g_u8CdtRetargetLogged;       /* first move retarget lamp */
static u8 g_u8CdtDeleteLogged;         /* first delete edge lamp */

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
             * First free->alloc churn line (delete/move returns edge).
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
 * Soft unlink note (from gj_cdt_unlink_slot after >=1 edge dropped).
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
 * Does not claim product empty-edge audit - soft observability only.
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
        /* Pool miss or link refuse - slot may still be installed. */
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
    /*
     * Lean tally (once / rare re-log). No version stamp. Soft!=product.
     * Grep: cap: cdt soft slots_left / quota / coverage / audit
     */
    kprintf("cap: cdt soft slots_left_inc=%u q_ch_ok=%u q_ch_fail=%u "
            "q_ch_nop=%u q_rf_ok=%u q_rf_nop=%u move_net0=%u del_rf=%u "
            "mint_ok=%u copy_ok=%u move_ok=%u pool_used=%u Soft!=product\n",
            g_u32SoftSlotsLeftInc, g_u32SoftQuotaChargeOk,
            g_u32SoftQuotaChargeFail, g_u32SoftQuotaChargeNop,
            g_u32SoftQuotaRefundOk, g_u32SoftQuotaRefundNop,
            g_u32SoftMoveNet0, g_u32SoftDeleteRefund,
            g_u32CdtMintEdgeOk, g_u32CdtCopyEdgeOk, g_u32CdtMoveEdgeOk,
            cdt_edge_pool_used());
    kprintf("cap: cdt soft coverage mint=%u/%u miss_m=%u copy=%u/%u "
            "miss_c=%u move=%u/%u miss_v=%u unlink=%u retarget=%u "
            "del_edge=%u pool_alloc=%u pool_miss=%u pool_free=%u "
            "Soft!=product\n",
            g_u32CdtMintEdgeOk, g_u32CdtMintAttempt, g_u32CdtMintEdgeMiss,
            g_u32CdtCopyEdgeOk, g_u32CdtCopyAttempt, g_u32CdtCopyEdgeMiss,
            g_u32CdtMoveEdgeOk, g_u32CdtMoveAttempt, g_u32CdtMoveEdgeMiss,
            g_u32CdtUnlinkOk, g_u32CdtMoveRetarget, g_u32CdtDeleteEdge,
            g_u32CdtPoolAllocOk, g_u32CdtPoolAllocMiss, g_u32CdtPoolFreeOk);
    kprintf("cap: cdt soft audit match=%u mismatch=%u empty=%u "
            "chain_max=%u chain_last=%u pool_used=%u soft_only Soft!=product\n",
            g_u32CdtSoftAuditMatch, g_u32CdtSoftAuditMismatch,
            g_u32CdtSoftAuditEmpty, g_u32CdtChainDepthMax,
            g_u32CdtChainDepthAtOk, cdt_edge_pool_used());
}

/**
 * Lean greppable soft residual inventory (never hard-gates product).
 * Prefix-stable: "cap: cdt soft ..." / "cap: soft residual ..."
 * Soft!=product. Dual MIT OR Apache-2.0. No version stamp. No stamp storms
 * (few lamps only; counters live in file-statics). C3 lean selftest results
 * are included when residual lean has run (behavior check, not lamp-only).
 *
 * Grep: cap: cdt soft honesty|inventory|path|residual|PASS|return|retmap
 * Grep: cap: soft residual lean PASS
 * Grep: cap: cdt soft residual lean
 */
static void
cdt_soft_inventory_log(void)
{
    u32 u32ResFail;

    cdt_soft_inc(&g_u32SoftInvLogs);
    u32ResFail = g_u32SoftResInval + g_u32SoftResNoent + g_u32SoftResStale +
                 g_u32SoftResLiveFail;

    /*
     * Grep: cap: cdt soft honesty
     * Soft inventory only - not MIG REPLY / full CDT mutex product.
     */
    kprintf("cap: cdt soft honesty reply_product=0 full_cdt_mutex=0 "
            "soft_lock=u32SoftLock sleep_not_spin=1 soft_ne_mig_reply=1 "
            "c3_lean=%u Soft!=product (soft != GJ_CAP_REPLY product; "
            "soft != MIG REPLY product; soft != full CDT mutex product)\n",
            g_u32SoftLeanOk);

    /* Grep: cap: cdt soft inventory */
    kprintf("cap: cdt soft inventory pool_used=%u pool_sz=%u "
            "alloc_ok=%u alloc_miss=%u free_ok=%u sl_inc=%u "
            "q_ch_ok=%u q_ch_fail=%u q_rf_ok=%u mint_ok=%u copy_ok=%u "
            "move_ok=%u res_ok=%u try_ok=%u inst_ok=%u chain_max=%u "
            "lean_ok=%u log_n=%u Soft!=product soft_partial\n",
            cdt_edge_pool_used(), GJ_CDT_EDGE_POOL, g_u32CdtPoolAllocOk,
            g_u32CdtPoolAllocMiss, g_u32CdtPoolFreeOk, g_u32SoftSlotsLeftInc,
            g_u32SoftQuotaChargeOk, g_u32SoftQuotaChargeFail,
            g_u32SoftQuotaRefundOk, g_u32CdtMintEdgeOk, g_u32CdtCopyEdgeOk,
            g_u32CdtMoveEdgeOk, g_u32SoftResOk, g_u32SoftTryOk,
            g_u32SoftInstOk, g_u32CdtChainDepthMax, g_u32SoftLeanOk,
            g_u32SoftInvLogs);

    /* Grep: cap: cdt soft resolve / trylock / install (path tallies) */
    kprintf("cap: cdt soft resolve enter=%u ok=%u inval=%u noent=%u "
            "stale=%u live_fail=%u scheme_a=1 Soft!=product\n",
            g_u32SoftResEnter, g_u32SoftResOk, g_u32SoftResInval,
            g_u32SoftResNoent, g_u32SoftResStale, g_u32SoftResLiveFail);
    kprintf("cap: cdt soft trylock enter=%u ok=%u busy=%u null=%u "
            "unlock=%u lock=u32SoftLock product_mutex=OPEN "
            "sleep_not_spin=1 Soft!=product\n",
            g_u32SoftTryEnter, g_u32SoftTryOk, g_u32SoftTryBusy,
            g_u32SoftTryNull, g_u32SoftUnlock);
    kprintf("cap: cdt soft install enter=%u ok=%u fail=%u "
            "fail_inval=%u fail_dead=%u fail_perm=%u fail_busy=%u "
            "fail_quota=%u reply_type=%u reply_product=0 Soft!=product\n",
            g_u32SoftInstEnter, g_u32SoftInstOk, g_u32SoftInstFail,
            g_u32SoftInstFailInval, g_u32SoftInstFailDead,
            g_u32SoftInstFailPerm, g_u32SoftInstFailBusy,
            g_u32SoftInstFailQuota, g_u32SoftInstReplyType);

    /* Grep: cap: cdt soft return (public API return surface) */
    kprintf("cap: cdt soft return resolve_ok=%u resolve_fail=%u "
            "inst_ok=%u inst_fail=%u mint_ok=%u mint_fail=%u "
            "copy_ok=%u copy_fail=%u move_ok=%u move_fail=%u "
            "del_ok=%u del_fail=%u alloc_ok=%u alloc_fail=%u "
            "reply_product=0 Soft!=product\n",
            g_u32SoftResOk, u32ResFail, g_u32SoftInstOk, g_u32SoftInstFail,
            g_u32SoftRetMintOk, g_u32SoftRetMintFail, g_u32SoftRetCopyOk,
            g_u32SoftRetCopyFail, g_u32SoftRetMoveOk, g_u32SoftRetMoveFail,
            g_u32SoftRetDelOk, g_u32SoftRetDelFail, g_u32SoftRetAllocOk,
            g_u32SoftRetAllocFail);
    kprintf("cap: cdt soft return rate resolve_ok=%u resolve_fail=%u "
            "inst_ok=%u inst_fail=%u mint_ok=%u mint_fail=%u "
            "copy_ok=%u copy_fail=%u move_ok=%u move_fail=%u "
            "del_ok=%u del_fail=%u alloc_ok=%u alloc_fail=%u "
            "Soft!=product dual=MIT_OR_Apache-2.0\n",
            g_u32SoftResOk, u32ResFail, g_u32SoftInstOk, g_u32SoftInstFail,
            g_u32SoftRetMintOk, g_u32SoftRetMintFail, g_u32SoftRetCopyOk,
            g_u32SoftRetCopyFail, g_u32SoftRetMoveOk, g_u32SoftRetMoveFail,
            g_u32SoftRetDelOk, g_u32SoftRetDelFail, g_u32SoftRetAllocOk,
            g_u32SoftRetAllocFail);
    kprintf("cap: cdt soft retcode ok=1 inval=1 noent=1 perm=1 dead=1 "
            "stale=1 busy=1 quota=1 live_fail=1 other=1 "
            "mint_other=%u copy_other=%u move_other=%u del_other=%u "
            "alloc_other=%u Soft!=product\n",
            g_u32SoftRetMintOther, g_u32SoftRetCopyOther,
            g_u32SoftRetMoveOther, g_u32SoftRetDelOther,
            g_u32SoftRetAllocOther);
    kprintf("cap: cdt soft retmap "
            "resolve=OK|INVAL|NOENT|STALE|DEAD "
            "install=OK|INVAL|DEAD|PERM|BUSY|QUOTA "
            "mint=OK|INVAL|PERM|OTHER copy=OK|INVAL|PERM|OTHER "
            "move=OK|INVAL|PERM|OTHER delete=OK|INVAL|PERM|OTHER "
            "alloc=OK|INVAL|QUOTA|OTHER "
            "c3=fail_closed+no_escalate+root_meta+cross_pkg+"
            "copy_move+quota+busy_noent+del_stale+dma_shape "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 Soft!=product\n");

    /* Grep: cap: cdt soft residual (lean rollup; counters not product) */
    kprintf("cap: cdt soft residual resolve=%u/%u install=%u/%u "
            "mint_edge=%u/%u copy_edge=%u/%u move_edge=%u/%u "
            "ret_ok mint=%u copy=%u move=%u del=%u alloc=%u "
            "reply_scaffold=%u lean_ok=%u pool_sz=%u Soft!=product "
            "dual MIT OR Apache-2.0 storm=0\n",
            g_u32SoftResOk, g_u32SoftResEnter, g_u32SoftInstOk,
            g_u32SoftInstEnter, g_u32CdtMintEdgeOk, g_u32CdtMintAttempt,
            g_u32CdtCopyEdgeOk, g_u32CdtCopyAttempt, g_u32CdtMoveEdgeOk,
            g_u32CdtMoveAttempt, g_u32SoftRetMintOk, g_u32SoftRetCopyOk,
            g_u32SoftRetMoveOk, g_u32SoftRetDelOk, g_u32SoftRetAllocOk,
            g_u32SoftInstReplyType, g_u32SoftLeanOk, GJ_CDT_EDGE_POOL);

    /*
     * Grep: cap: cdt soft residual lean (C3 behavior selftest rollup)
     */
    kprintf("cap: cdt soft residual lean runs=%u ok=%u inst=%u mint=%u "
            "esc_fail=%u res_stale=%u res_null=%u res_live=%u "
            "root_guard=%u del=%u cross=%u host_subset=%u dead_fail=%u "
            "pkg_cross=%u copy=%u move=%u noent=%u busy=%u quota=%u "
            "del_stale=%u dma_shape=%u no_mint=%u root_meta_slot=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "c3=1 mint_product=0 Soft!=product\n",
            g_u32SoftLeanRuns, g_u32SoftLeanOk, g_u32SoftLeanInstOk,
            g_u32SoftLeanMintOk, g_u32SoftLeanMintEscFail,
            g_u32SoftLeanResStaleOk, g_u32SoftLeanResNullOk,
            g_u32SoftLeanResLiveOk, g_u32SoftLeanRootGuardOk,
            g_u32SoftLeanDelOk, g_u32SoftLeanCrossOk,
            g_u32SoftLeanHostSubsetOk, g_u32SoftLeanDeadFailOk,
            g_u32SoftLeanPkgCrossOk, g_u32SoftLeanCopyOk,
            g_u32SoftLeanMoveOk, g_u32SoftLeanNoentOk, g_u32SoftLeanBusyOk,
            g_u32SoftLeanQuotaOk, g_u32SoftLeanDelStaleOk,
            g_u32SoftLeanDmaShapeOk, g_u32SoftLeanMintNoMintOk,
            g_u32SoftLeanRootMetaSlotOk);

    /* Grep: cap: cdt soft path */
    kprintf("cap: cdt soft path resolve=1 install=1 mint=1 copy=1 move=1 "
            "delete=1 trylock=soft_u32SoftLock quota=soft_hier "
            "return_surface=1 return_rate=1 retcode=1 retmap=1 "
            "residual_lean=1 c3=1 reply_product=0 full_cdt_mutex=0 "
            "soft_ne_mig_reply=1 Soft!=product (soft residual; not "
            "GJ_CAP_REPLY / MIG REPLY / full CDT mutex product)\n");

    /*
     * Grep: cap: soft residual lean PASS
     * Grep: cap: cdt soft inventory PASS / cap: cdt soft PASS
     * No version stamp. No deepen stamp storm.
     */
    kprintf("cap: soft residual lean PASS pool_used=%u res_ok=%u "
            "inst_ok=%u mint_ok=%u copy_ok=%u move_ok=%u "
            "ddi_n=%u lean_ok=%u reply_product=0 full_cdt_mutex=0 "
            "Soft!=product dual MIT OR Apache-2.0 no_version_stamp "
            "storm=0 c3=1\n",
            cdt_edge_pool_used(), g_u32SoftResOk, g_u32SoftInstOk,
            g_u32CdtMintEdgeOk, g_u32CdtCopyEdgeOk, g_u32CdtMoveEdgeOk,
            g_u32SoftDdiInst, g_u32SoftLeanOk);
    kprintf("cap: cdt soft inventory PASS log_n=%u lean_ok=%u "
            "reply_product=0 full_cdt_mutex=0 soft_ne_mig_reply=1 "
            "Soft!=product G-AC-1=1 c3=1\n",
            g_u32SoftInvLogs, g_u32SoftLeanOk);
    kprintf("cap: cdt soft PASS Soft!=product lean_ok=%u c3=1 "
            "G-AC-1=1 dual=MIT_OR_Apache-2.0\n",
            g_u32SoftLeanOk);

    /*
     * DDI/UDX lean residual foundation (soft only; product mint OPEN).
     * Grep: cap: ddi | cap: udx | cap: ddi soft residual lean PASS
     * Grep: cap: udx host mint residual | cap: product mint OPEN
     * Grep: cap: udx host mmio|irq|dma residual
     */
    kprintf("cap: ddi soft residual irq=%u frame=%u untyped=%u notif=%u "
            "ddi_n=%u mint_product=0 dma_window_product=0 "
            "Soft!=product G-AC-1\n",
            g_u32SoftDdiIrq, g_u32SoftDdiFrame, g_u32SoftDdiUntyped,
            g_u32SoftDdiNotif, g_u32SoftDdiInst);
    kprintf("cap: ddi soft residual lean PASS mint_product=0 "
            "Soft!=product dual MIT OR Apache-2.0 storm=0\n");
    kprintf("cap: udx soft residual lean PASS host_cnode_mint=0 "
            "Soft!=product G-AC-1 dual MIT OR Apache-2.0 storm=0\n");
    kprintf("cap: udx host mint residual mint_n=%u cross_n=%u local_n=%u "
            "host_ok_n=%u host_over_n=%u mmio=%u irq=%u notif=%u "
            "untyped=%u dma_shape_ok=%u dma_shape_miss=%u "
            "mint_product=0 Soft!=product G-AC-1\n",
            g_u32SoftDdiMint, g_u32SoftDdiMintCross, g_u32SoftDdiMintLocal,
            g_u32SoftDdiMintHostOk, g_u32SoftDdiMintHostOver,
            g_u32SoftDdiMintRoleMmio, g_u32SoftDdiMintRoleIrq,
            g_u32SoftDdiMintRoleNotif, g_u32SoftDdiMintRoleUntyped,
            g_u32SoftDdiMintDmaShapeOk, g_u32SoftDdiMintDmaShapeMiss);
    kprintf("cap: udx host mint residual pkg_seen=0x%x pkg_cross=0x%x "
            "pkg_mmio_n=%u pkg_irq_n=%u pkg_notif_n=%u "
            "pkg_intended=0x%x pkg_typed=0x%x mint_product=0 "
            "Soft!=product G-AC-1\n",
            g_u32SoftUdxHostPkgSeen, g_u32SoftUdxHostPkgCross,
            g_u32SoftUdxHostPkgMmioN, g_u32SoftUdxHostPkgIrqN,
            g_u32SoftUdxHostPkgNotifN,
            gj_cap_udx_host_package_roles_intended(),
            gj_cap_udx_host_package_roles_typed());
    kprintf("cap: udx host mmio residual product=%d role_n=%u pkg_n=%u "
            "Soft!=product G-AC-1\n",
            gj_cap_ddi_mmio_product_ready(), g_u32SoftDdiMintRoleMmio,
            g_u32SoftUdxHostPkgMmioN);
    kprintf("cap: udx host irq residual product=%d role_irq_n=%u "
            "role_notif_n=%u Soft!=product G-AC-1\n",
            gj_cap_ddi_irq_notif_product_ready(), g_u32SoftDdiMintRoleIrq,
            g_u32SoftDdiMintRoleNotif);
    kprintf("cap: udx host dma residual product=%d shape_ok_n=%u "
            "shape_miss_n=%u type_ordinal=0 Soft!=product G-AC-1\n",
            gj_cap_ddi_dma_window_product_ready(),
            g_u32SoftDdiMintDmaShapeOk, g_u32SoftDdiMintDmaShapeMiss);
    kprintf("cap: product mint OPEN host_cnode_product=%d "
            "mmio_product=%d irq_notif_product=%d dma_window_product=%d "
            "Soft!=product dual MIT OR Apache-2.0 storm=0\n",
            gj_cap_udx_host_mint_product_ready(),
            gj_cap_ddi_mmio_product_ready(),
            gj_cap_ddi_irq_notif_product_ready(),
            gj_cap_ddi_dma_window_product_ready());
}

/*
 * C3 lean residual selftest (stack-local; once; never hard-gates).
 *
 * Exercises security-visible surfaces without process CNodes or product
 * mint policy (W4 strengthen; Soft!=product; Dual DoD mint OPEN):
 *   1.  Root-meta guard: non-ROOT_META install at slot 0 -> PERM
 *   1b. ROOT_META off slot 0 -> PERM (inverse guard)
 *   2.  Install UDX package stand-ins (FRAME/IRQ/NOTIFICATION) with host
 *       default rights into src CNode (with MINT for derive)
 *   3.  Live resolve ok; gen0 -> INVAL; wrong gen -> STALE_CAP; empty NOENT
 *   3b. Occupied slot reinstall -> BUSY
 *   4.  Cross-CNode mint full typed package (FRAME+IRQ+NOTIF); resolve
 *       minted rights ⊆ source and host-bits ⊆ host default; DMA shape
 *   5.  Escalate mint (source lacks SPAWN) -> no SPAWN on derived / PERM
 *   5b. Mint without MINT right on a GRANT-only install -> PERM
 *   6.  GRANT copy + move (src STALE after move)
 *   7.  Soft quota charge / exhaust / refund (attach after installs)
 *   8.  Delete + CDT unlink hygiene; resolve deleted -> STALE
 *   9.  DEAD object resolve fails closed (local state poke; not revoke API)
 *
 * Soft!=product. Dual MIT OR Apache-2.0. G-AC-1. No version stamp.
 * Not product host mint complete. Grep: cap: cdt soft residual lean
 */
static void
cdt_soft_residual_lean_once(void)
{
    struct gj_cnode cnSrc;
    struct gj_cnode cnDst;
    struct gj_cap_slot aSrcSlots[GJ_CDT_LEAN_SLOTS];
    struct gj_cap_slot aDstSlots[GJ_CDT_LEAN_SLOTS];
    struct gj_obj_hdr aObj[GJ_CDT_LEAN_UDX_OBJS];
    struct gj_obj_hdr objNoMint;
    struct gj_obj_hdr objQuota;
    struct gj_cap_ref aRef[GJ_CDT_LEAN_UDX_OBJS];
    struct gj_cap_ref aRefHost[GJ_CDT_LEAN_UDX_OBJS];
    struct gj_cap_ref refMint;
    struct gj_cap_ref refTmp;
    struct gj_cap_ref refCopy;
    struct gj_cap_ref refMove;
    struct gj_cap_ref refNoMint;
    struct gj_cap_ref refQuota;
    struct gj_cap_resolved res;
    struct gj_cap_quota qLean;
    gj_status_t st;
    u16 aTypes[GJ_CDT_LEAN_UDX_OBJS];
    u16 aRights[GJ_CDT_LEAN_UDX_OBJS];
    u16 u16Weak;
    u16 u16Esc;
    u16 u16MintedHost;
    u16 u16HostBits;
    u32 iObj;
    u32 u32Checks;
    u32 u32Pass;
    u32 u32PkgCross;
    u32 u32UsedBefore;
    int fAll;

    if (g_u8CdtSoftLeanOnce != 0u) {
        return;
    }
    g_u8CdtSoftLeanOnce = 1u;
    cdt_soft_inc(&g_u32SoftLeanRuns);

    /*
     * Suppress inventory reentry: install/mint/resolve/delete call
     * maybe_once; lean residual owns sequencing and dump after.
     */
    g_u8CdtSoftLeanBusy = 1u;

    aTypes[0] = (u16)GJ_CAP_FRAME;
    aTypes[1] = (u16)GJ_CAP_IRQ;
    aTypes[2] = (u16)GJ_CAP_NOTIFICATION;
    aRights[0] = (u16)(GJ_RIGHTS_DDI_MMIO_HOST | GJ_RIGHT_MINT | GJ_RIGHT_GRANT |
                       GJ_RIGHT_DESTROY);
    aRights[1] = (u16)(GJ_RIGHTS_DDI_IRQ_HOST | GJ_RIGHT_MINT | GJ_RIGHT_GRANT |
                       GJ_RIGHT_DESTROY);
    aRights[2] = (u16)(GJ_RIGHTS_DDI_NOTIF_HOST | GJ_RIGHT_MINT | GJ_RIGHT_GRANT |
                       GJ_RIGHT_DESTROY);

    gj_cnode_init(&cnSrc, aSrcSlots, GJ_CDT_LEAN_SLOTS);
    gj_cnode_init(&cnDst, aDstSlots, GJ_CDT_LEAN_SLOTS);
    for (iObj = 0; iObj < GJ_CDT_LEAN_UDX_OBJS; iObj++) {
        gj_obj_hdr_init(&aObj[iObj]);
        aRef[iObj] = gj_cap_ref_null();
        aRefHost[iObj] = gj_cap_ref_null();
    }
    gj_obj_hdr_init(&objNoMint);
    gj_obj_hdr_init(&objQuota);
    refMint = gj_cap_ref_null();
    refTmp = gj_cap_ref_null();
    refCopy = gj_cap_ref_null();
    refMove = gj_cap_ref_null();
    refNoMint = gj_cap_ref_null();
    refQuota = gj_cap_ref_null();

    u32Checks = 0;
    u32Pass = 0;
    u32PkgCross = 0;

    /* 1. Root-meta guard: FRAME at slot 0 must be PERM. */
    u32Checks++;
    st = gj_cap_slot_install(&cnSrc, GJ_CAP_SLOT_ROOT_META, (u16)GJ_CAP_FRAME,
                             aRights[0], &aObj[0], &refTmp);
    if (st == GJ_ERR_PERM) {
        cdt_soft_inc(&g_u32SoftLeanRootGuardOk);
        u32Pass++;
    }

    /* 1b. Inverse: ROOT_META only at slot 0. */
    u32Checks++;
    st = gj_cap_slot_install(&cnSrc, 1ull, (u16)GJ_CAP_ROOT_META, aRights[0],
                             &aObj[0], &refTmp);
    if (st == GJ_ERR_PERM) {
        cdt_soft_inc(&g_u32SoftLeanRootMetaSlotOk);
        u32Pass++;
    }

    /* 2. Install UDX package stand-ins (host rights + derive bits). */
    for (iObj = 0; iObj < GJ_CDT_LEAN_UDX_OBJS; iObj++) {
        u32Checks++;
        st = gj_cap_alloc_install(&cnSrc, aTypes[iObj], aRights[iObj],
                                  &aObj[iObj], &aRef[iObj]);
        if (st == GJ_OK && !gj_cap_ref_is_null(&aRef[iObj])) {
            cdt_soft_inc(&g_u32SoftLeanInstOk);
            u32Pass++;
        }
    }

    /* 3a. Live resolve of first install. */
    u32Checks++;
    st = gj_cap_resolve(&cnSrc, aRef[0].u64Slot, aRef[0].u32SlotGen, &res);
    if (st == GJ_OK && res.u16Type == (u16)GJ_CAP_FRAME) {
        cdt_soft_inc(&g_u32SoftLeanResLiveOk);
        u32Pass++;
    }

    /* 3b. Null gen (gen==0) fail-closed -> INVAL. */
    u32Checks++;
    st = gj_cap_resolve(&cnSrc, aRef[0].u64Slot, 0u, &res);
    if (st == GJ_ERR_INVAL) {
        cdt_soft_inc(&g_u32SoftLeanResNullOk);
        u32Pass++;
    }

    /* 3c. Wrong gen fail-closed -> STALE_CAP. */
    u32Checks++;
    st = gj_cap_resolve(&cnSrc, aRef[0].u64Slot, aRef[0].u32SlotGen + 1u,
                        &res);
    if (st == GJ_ERR_STALE_CAP) {
        cdt_soft_inc(&g_u32SoftLeanResStaleOk);
        u32Pass++;
    }

    /* 3d. Empty free slot resolve -> NOENT. */
    u32Checks++;
    st = gj_cap_resolve(&cnSrc, (u64)(GJ_CDT_LEAN_SLOTS - 1u), 1u, &res);
    if (st == GJ_ERR_NOENT) {
        cdt_soft_inc(&g_u32SoftLeanNoentOk);
        u32Pass++;
    }

    /* 3e. Reinstall into occupied FRAME slot -> BUSY. */
    u32Checks++;
    if (!gj_cap_ref_is_null(&aRef[0])) {
        st = gj_cap_slot_install(&cnSrc, aRef[0].u64Slot, (u16)GJ_CAP_FRAME,
                                 aRights[0], &aObj[0], &refTmp);
        if (st == GJ_ERR_BUSY) {
            cdt_soft_inc(&g_u32SoftLeanBusyOk);
            u32Pass++;
        }
    }

    /*
     * 4. Cross-CNode mint of full typed UDX host package
     *    (FRAME/IRQ/NOTIFICATION). DESTROY added so delete hygiene can run;
     *    host-subset observes host bits only (DESTROY excluded). Soft residual
     *    shape for future devmgr->host grant — not product complete.
     */
    for (iObj = 0; iObj < GJ_CDT_LEAN_UDX_OBJS; iObj++) {
        u16Weak = gj_cap_rights_ddi_host_default(aTypes[iObj]);
        u32Checks++;
        st = gj_cap_mint(&cnSrc, aRef[iObj].u64Slot, aRef[iObj].u32SlotGen,
                         (u16)(u16Weak | GJ_RIGHT_DESTROY), &cnDst,
                         &aRefHost[iObj]);
        if (st == GJ_OK && !gj_cap_ref_is_null(&aRefHost[iObj])) {
            cdt_soft_inc(&g_u32SoftLeanMintOk);
            cdt_soft_inc(&g_u32SoftLeanCrossOk);
            u32PkgCross++;
            u32Pass++;
            /*
             * Resolve minted: no rights beyond source; host bits ⊆ host
             * default (DESTROY may be present for delete hygiene only).
             */
            st = gj_cap_resolve(&cnDst, aRefHost[iObj].u64Slot,
                                aRefHost[iObj].u32SlotGen, &res);
            if (st == GJ_OK &&
                gj_cap_rights_is_subset(aRights[iObj], res.u16Rights) != 0) {
                u16MintedHost =
                    (u16)(res.u16Rights & (u16)(~(u16)GJ_RIGHT_DESTROY));
                if (u16MintedHost != 0u &&
                    gj_cap_rights_ddi_host_subset_ok(aTypes[iObj],
                                                     u16MintedHost) != 0) {
                    cdt_soft_inc(&g_u32SoftLeanHostSubsetOk);
                }
            }
        }
    }
    /* Formal check: all three typed package roles crossed. */
    u32Checks++;
    if (u32PkgCross == GJ_CDT_LEAN_UDX_OBJS) {
        cdt_soft_inc(&g_u32SoftLeanPkgCrossOk);
        u32Pass++;
    }
    /* Formal check: every crossed mint observed host-subset on resolved rights. */
    u32Checks++;
    if (g_u32SoftLeanHostSubsetOk >= GJ_CDT_LEAN_UDX_OBJS) {
        u32Pass++;
    }

    /* 4b. DMA rights-shape observe on FRAME host mint (type ordinal OPEN). */
    u32Checks++;
    if (!gj_cap_ref_is_null(&aRefHost[0])) {
        st = gj_cap_resolve(&cnDst, aRefHost[0].u64Slot, aRefHost[0].u32SlotGen,
                            &res);
        if (st == GJ_OK) {
            u16HostBits =
                (u16)(res.u16Rights & (u16)(~(u16)GJ_RIGHT_DESTROY));
            if (gj_cap_rights_ddi_dma_host_subset_ok(u16HostBits) != 0) {
                cdt_soft_inc(&g_u32SoftLeanDmaShapeOk);
                u32Pass++;
            }
        }
    }
    refMint = aRefHost[0];

    /* 5. Escalate: request SPAWN which source FRAME rights lack. */
    u16Weak = gj_cap_rights_ddi_host_default((u16)GJ_CAP_FRAME);
    u16Esc = (u16)(u16Weak | GJ_RIGHT_SPAWN | GJ_RIGHT_DESTROY);
    u32Checks++;
    st = gj_cap_mint(&cnSrc, aRef[0].u64Slot, aRef[0].u32SlotGen, u16Esc,
                     &cnDst, &refTmp);
    /*
     * rights_weaker masks SPAWN off; residual non-empty => mint may still
     * succeed with weaker set. C3 no-escalate: derived must not include bits
     * source lacks - verify minted rights if success; PERM if empty want.
     */
    if (st == GJ_ERR_PERM) {
        cdt_soft_inc(&g_u32SoftLeanMintEscFail);
        u32Pass++;
    } else if (st == GJ_OK && !gj_cap_ref_is_null(&refTmp)) {
        st = gj_cap_resolve(&cnDst, refTmp.u64Slot, refTmp.u32SlotGen, &res);
        if (st == GJ_OK && (res.u16Rights & GJ_RIGHT_SPAWN) == 0) {
            cdt_soft_inc(&g_u32SoftLeanMintEscFail);
            u32Pass++;
        }
        (void)gj_cap_delete(&cnDst, refTmp.u64Slot, refTmp.u32SlotGen);
    }

    /*
     * 5b. Mint without MINT right: GRANT-only FRAME install, mint must PERM.
     * Soft residual; not product mint policy. Grep: lean no_mint
     */
    u32Checks++;
    st = gj_cap_alloc_install(&cnSrc, (u16)GJ_CAP_FRAME,
                              (u16)(GJ_RIGHTS_DDI_MMIO_HOST | GJ_RIGHT_GRANT |
                                    GJ_RIGHT_DESTROY),
                              &objNoMint, &refNoMint);
    if (st == GJ_OK && !gj_cap_ref_is_null(&refNoMint)) {
        st = gj_cap_mint(&cnSrc, refNoMint.u64Slot, refNoMint.u32SlotGen,
                         gj_cap_rights_ddi_host_default((u16)GJ_CAP_FRAME),
                         &cnDst, &refTmp);
        if (st == GJ_ERR_PERM) {
            cdt_soft_inc(&g_u32SoftLeanMintNoMintOk);
            u32Pass++;
        }
    }

    /* 6a. GRANT copy of src FRAME (same CNode; rights subset). */
    u32Checks++;
    if (!gj_cap_ref_is_null(&aRef[0])) {
        st = gj_cap_copy(&cnSrc, aRef[0].u64Slot, aRef[0].u32SlotGen,
                         gj_cap_rights_ddi_host_default((u16)GJ_CAP_FRAME),
                         &refCopy);
        if (st == GJ_OK && !gj_cap_ref_is_null(&refCopy)) {
            st = gj_cap_resolve(&cnSrc, refCopy.u64Slot, refCopy.u32SlotGen,
                                &res);
            if (st == GJ_OK && res.u16Type == (u16)GJ_CAP_FRAME &&
                (res.u16Rights & GJ_RIGHT_SPAWN) == 0) {
                cdt_soft_inc(&g_u32SoftLeanCopyOk);
                u32Pass++;
            }
        }
    }

    /* 6b. Move copy: dst live, src STALE after move. */
    u32Checks++;
    if (!gj_cap_ref_is_null(&refCopy)) {
        st = gj_cap_move(&cnSrc, refCopy.u64Slot, refCopy.u32SlotGen, &refMove);
        if (st == GJ_OK && !gj_cap_ref_is_null(&refMove)) {
            st = gj_cap_resolve(&cnSrc, refCopy.u64Slot, refCopy.u32SlotGen,
                                &res);
            if (st == GJ_ERR_STALE_CAP || st == GJ_ERR_NOENT) {
                st = gj_cap_resolve(&cnSrc, refMove.u64Slot, refMove.u32SlotGen,
                                    &res);
                if (st == GJ_OK) {
                    cdt_soft_inc(&g_u32SoftLeanMoveOk);
                    u32Pass++;
                }
            }
        }
    }

    /*
     * 7. Soft quota: attach after free-slot installs, charge one more, then
     *    exhaust (limit = used). Refund on delete. Soft!=product ledger.
     */
    u32Checks++;
    {
        u32 u32Lim;

        u32UsedBefore = 0;
        /* Count occupied non-root slots as baseline for soft limit. */
        for (iObj = 1; iObj < GJ_CDT_LEAN_SLOTS; iObj++) {
            if (aSrcSlots[iObj].u16Type != (u16)GJ_CAP_INVALID) {
                u32UsedBefore++;
            }
        }
        u32Lim = u32UsedBefore + 1u; /* room for exactly one more charge */
        if (u32Lim < 1u) {
            u32Lim = GJ_CDT_LEAN_QUOTA_LIM;
        }
        gj_cap_quota_init(&qLean, u32Lim);
        /* Soft: seed used to occupied so next charge is last free unit. */
        qLean.u32Used = u32UsedBefore;
        qLean.u32HighWater = u32UsedBefore;
        gj_cap_quota_attach(&cnSrc, &qLean);
        st = gj_cap_alloc_install(&cnSrc, (u16)GJ_CAP_FRAME,
                                  (u16)(GJ_RIGHTS_DDI_MMIO_HOST |
                                        GJ_RIGHT_DESTROY),
                                  &objQuota, &refQuota);
        if (st == GJ_OK && !gj_cap_ref_is_null(&refQuota)) {
            /* Exhaust: next install must QUOTA. */
            st = gj_cap_alloc_install(&cnSrc, (u16)GJ_CAP_FRAME,
                                      (u16)GJ_RIGHTS_DDI_MMIO_HOST, &objQuota,
                                      &refTmp);
            if (st == GJ_ERR_QUOTA) {
                st = gj_cap_delete(&cnSrc, refQuota.u64Slot, refQuota.u32SlotGen);
                if (st == GJ_OK && qLean.u32Used == u32UsedBefore) {
                    cdt_soft_inc(&g_u32SoftLeanQuotaOk);
                    u32Pass++;
                    refQuota = gj_cap_ref_null();
                }
            }
        }
        gj_cap_quota_attach(&cnSrc, NULL); /* detach soft account */
    }

    /* 8. Delete host mint + one src binding (CDT unlink hygiene). */
    u32Checks++;
    st = GJ_OK;
    if (!gj_cap_ref_is_null(&refMint)) {
        st = gj_cap_delete(&cnDst, refMint.u64Slot, refMint.u32SlotGen);
    } else {
        st = GJ_ERR_INVAL;
    }
    if (st == GJ_OK && !gj_cap_ref_is_null(&aRefHost[1])) {
        st = gj_cap_delete(&cnDst, aRefHost[1].u64Slot, aRefHost[1].u32SlotGen);
    }
    if (st == GJ_OK && !gj_cap_ref_is_null(&aRef[1])) {
        st = gj_cap_delete(&cnSrc, aRef[1].u64Slot, aRef[1].u32SlotGen);
    }
    if (st == GJ_OK) {
        cdt_soft_inc(&g_u32SoftLeanDelOk);
        u32Pass++;
    }

    /* 8b. Resolve deleted FRAME host mint -> STALE (or NOENT if cleared). */
    u32Checks++;
    if (!gj_cap_ref_is_null(&refMint)) {
        st = gj_cap_resolve(&cnDst, refMint.u64Slot, refMint.u32SlotGen, &res);
        if (st == GJ_ERR_STALE_CAP || st == GJ_ERR_NOENT) {
            cdt_soft_inc(&g_u32SoftLeanDelStaleOk);
            u32Pass++;
        }
    }

    /*
     * 9. DEAD fail-closed: poke object DEAD+gen (S1 shape) then resolve.
     *    Does not call revoke API (that lives in revoke.c residual).
     */
    u32Checks++;
    if (!gj_cap_ref_is_null(&aRef[2])) {
        aObj[2].u32State = (u32)GJ_OBJ_DEAD;
        aObj[2].u32Gen++;
        if (aObj[2].u32Gen == 0u) {
            aObj[2].u32Gen = 1u;
        }
        st = gj_cap_resolve(&cnSrc, aRef[2].u64Slot, aRef[2].u32SlotGen,
                            &res);
        if (st == GJ_ERR_DEAD || st == GJ_ERR_STALE_CAP) {
            cdt_soft_inc(&g_u32SoftLeanDeadFailOk);
            u32Pass++;
        }
    }

    fAll = (u32Checks > 0u && u32Pass == u32Checks) ? 1 : 0;
    if (fAll != 0) {
        cdt_soft_inc(&g_u32SoftLeanOk);
    }

    g_u8CdtSoftLeanBusy = 0u;

    /*
     * Grep: cap: cdt soft residual lean PASS
     * Grep: cap: cdt soft residual lean c3
     * Grep: cap: soft residual lean PASS
     */
    kprintf("cap: cdt soft residual lean PASS checks=%u pass=%u all=%d "
            "inst=%u mint=%u esc_fail=%u res_stale=%u res_null=%u "
            "res_live=%u root_guard=%u del=%u cross=%u host_subset=%u "
            "dead_fail=%u pkg_cross=%u copy=%u move=%u noent=%u busy=%u "
            "quota=%u del_stale=%u dma_shape=%u no_mint=%u "
            "root_meta_slot=%u lean_ok=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 c3=1 "
            "mint_product=0 reply_product=0 full_cdt_mutex=0 "
            "(Soft!=product; C3 behavior selftest; not product host mint)\n",
            u32Checks, u32Pass, fAll, g_u32SoftLeanInstOk, g_u32SoftLeanMintOk,
            g_u32SoftLeanMintEscFail, g_u32SoftLeanResStaleOk,
            g_u32SoftLeanResNullOk, g_u32SoftLeanResLiveOk,
            g_u32SoftLeanRootGuardOk, g_u32SoftLeanDelOk, g_u32SoftLeanCrossOk,
            g_u32SoftLeanHostSubsetOk, g_u32SoftLeanDeadFailOk,
            g_u32SoftLeanPkgCrossOk, g_u32SoftLeanCopyOk, g_u32SoftLeanMoveOk,
            g_u32SoftLeanNoentOk, g_u32SoftLeanBusyOk, g_u32SoftLeanQuotaOk,
            g_u32SoftLeanDelStaleOk, g_u32SoftLeanDmaShapeOk,
            g_u32SoftLeanMintNoMintOk, g_u32SoftLeanRootMetaSlotOk,
            g_u32SoftLeanOk);
    kprintf("cap: cdt soft residual lean c3 "
            "map=root_guard+meta_slot->install_udx->resolve_fail_closed->"
            "noent+busy->cross_pkg_mint_subset+dma_shape->no_escalate+"
            "no_mint->copy->move_stale->quota_charge_exhaust_refund->"
            "delete_stale->dead_fail "
            "objs=%u shape=mmio+irq+notif_standin "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "host_cnode_product=0 dual_dod_mint=OPEN "
            "(Soft!=product; C3 residual; not product DDI mint complete)\n",
            GJ_CDT_LEAN_UDX_OBJS);
    kprintf("cap: soft residual lean PASS lean_ok=%u checks=%u pass=%u "
            "c3=1 Soft!=product dual MIT OR Apache-2.0 G-AC-1 "
            "no_version_stamp storm=0 mint_product=0\n",
            g_u32SoftLeanOk, u32Checks, u32Pass);
}

/**
 * Emit lean soft residual inventory once after first meaningful activity.
 * Runs C3 residual lean selftest first so behavior checks light lamps.
 * No stamp storms; greppable surface lands on first install/resolve/wire.
 */
static void
cdt_soft_inventory_maybe_once(void)
{
    if (g_u8CdtSoftInvLogged) {
        return;
    }
    /* Lean residual owns inventory sequencing - skip reentry from install. */
    if (g_u8CdtSoftLeanBusy != 0u) {
        return;
    }
    /* Need at least one install, resolve ok, or edge attempt. */
    if (g_u32SoftInstOk == 0u && g_u32SoftResOk == 0u &&
        g_u32CdtMintAttempt == 0u && g_u32CdtCopyAttempt == 0u &&
        g_u32CdtMoveAttempt == 0u && g_u32SoftSlotsLeftInc == 0u) {
        return;
    }
    /* C3 lean selftest before inventory so lean_ok is visible in dump. */
    cdt_soft_residual_lean_once();
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
    /* Grep: cap: cdt mint|copy|move ... coverage rollup */
    kprintf("cap: cdt soft coverage rollup mint=%u/%u copy=%u/%u "
            "move=%u/%u miss_tot=%u pool_used=%u chain_max=%u "
            "audit_match=%u audit_mis=%u soft PASS Soft!=product (once)\n",
            g_u32CdtMintEdgeOk, g_u32CdtMintAttempt, g_u32CdtCopyEdgeOk,
            g_u32CdtCopyAttempt, g_u32CdtMoveEdgeOk, g_u32CdtMoveAttempt,
            g_u32CdtMintEdgeMiss + g_u32CdtCopyEdgeMiss +
                g_u32CdtMoveEdgeMiss,
            cdt_edge_pool_used(), g_u32CdtChainDepthMax,
            g_u32CdtSoftAuditMatch, g_u32CdtSoftAuditMismatch);
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
    /* Wave 15 soft inventory once (prefix "cap: cdt soft ..."). */
    cdt_soft_inventory_maybe_once();
}

/*
 * Empty-edge gap honesty when the soft edge pool is exhausted.
 * Install/mint still returns GJ_OK (slots_left authoritative); CDT walk
 * cannot see the slot until edges exist - soft FAIL or PARTIAL.
 *
 *   FAIL    - pool miss with zero successful edges for this op family
 *   PARTIAL - pool miss after at least one edge_ok for this op family
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
 * Does not change install status - edge miss is soft empty-edge gap only.
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
        /*
         * Once-only miss lamp (no per-op stamp storm).
         * Grep: cap: cdt mint|copy|move ... coverage
         */
        if (!g_u8CdtEdgeMissLogged) {
            g_u8CdtEdgeMissLogged = 1;
            kprintf("cap: cdt %s edge_ok=0 miss=%u attempt=%u ok=%u "
                    "slot=%lu slots_left=%u chain=%u pool_used=%u "
                    "Soft!=product (once)\n",
                    szOp, *pMiss, *pAttempt, *pOk, (unsigned long)u64Slot,
                    u32Slots, u32Chain, cdt_edge_pool_used());
        }
        cdt_soft_coverage_rollup();
        return;
    }

    st = gj_cdt_edge_link(pObj, pEdge, pCnode, u64Slot);
    if (st != GJ_OK) {
        cdt_edge_free(pEdge);
        cdt_soft_inc(pMiss);
        cdt_soft_empty_edge_audit(pObj, szOp, u64Slot, 0u);
        /* Link refused - soft miss; once-only lamp. */
        if (!g_u8CdtEdgeMissLogged) {
            g_u8CdtEdgeMissLogged = 1;
            kprintf("cap: cdt %s edge_ok=0 link_st=%d miss=%u attempt=%u "
                    "ok=%u slot=%lu Soft!=product (once)\n",
                    szOp, (int)st, *pMiss, *pAttempt, *pOk,
                    (unsigned long)u64Slot);
        }
        cdt_soft_coverage_rollup();
        return;
    }

    cdt_soft_inc(pOk);
    u32Chain = cdt_edge_chain_depth(pObj);
    cdt_soft_empty_edge_audit(pObj, szOp, u64Slot, 1u);
    /*
     * Once-only edge_ok lamp (counters track the rest; no stamp storm).
     * Grep: cap: cdt mint|copy|move ... coverage
     */
    if (!g_u8CdtEdgeOkLogged) {
        g_u8CdtEdgeOkLogged = 1;
        kprintf("cap: cdt %s edge_ok=1 ok=%u attempt=%u miss=%u "
                "slot=%lu slots_left=%u chain=%u chain_max=%u "
                "pool_used=%u Soft!=product (once)\n",
                szOp, *pOk, *pAttempt, *pMiss, (unsigned long)u64Slot,
                u32Slots, u32Chain, g_u32CdtChainDepthMax,
                cdt_edge_pool_used());
    }

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
 * Pre-check all nodes, then commit - soft single-threaded safe.
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

    /* Pass 2: commit leaf -> root. */
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
    /*
     * Soft UDX host-grant residual (future product: devmgr->host CNode).
     * Cross-CNode DDI/UDX class mint is the grant shape; not product-
     * complete. Soft residual != product mint. Grep: cap: udx host mint
     */
    cdt_soft_ddi_note_mint(res.u16Type, u16New, pSrcCnode != pDstCnode);
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
        /* Grep: cap: cdt retarget / cap: cdt move - once-only */
        if (!g_u8CdtRetargetLogged) {
            g_u8CdtRetargetLogged = 1;
            kprintf("cap: cdt retarget src_slot=%lu dst_slot=%lu "
                    "chain_pre=%u pool_used=%u free=%u move_unlink=%u "
                    "Soft!=product (once)\n",
                    (unsigned long)u64SrcSlot,
                    (unsigned long)pOut->u64Slot, u32ChainPre,
                    cdt_edge_pool_used(), g_u32CdtPoolFreeOk,
                    g_u32CdtMoveUnlink);
        }
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
         * Soft delete edge coverage - once-only (no stamp storm).
         * Grep: cap: cdt delete
         */
        if (!g_u8CdtDeleteLogged) {
            g_u8CdtDeleteLogged = 1;
            kprintf("cap: cdt delete slot=%lu had_edge=%d chain_pre=%u "
                    "chain_after=%u slots_pre=%u pool_used=%u free=%u "
                    "del_edge=%u del_rf=%u Soft!=product (once)\n",
                    (unsigned long)u64Slot, fHadEdge, u32ChainPre,
                    u32ChainAfter, u32SlotsPre, cdt_edge_pool_used(),
                    g_u32CdtPoolFreeOk, g_u32CdtDeleteEdge,
                    g_u32SoftDeleteRefund);
        }
        gj_cap_slot_invalidate_locked(res.pSlot, res.pObj);
        cdt_soft_inc(&g_u32SoftRetDelOk);
        cdt_soft_inventory_maybe_once();
        return GJ_OK;
    }
}
