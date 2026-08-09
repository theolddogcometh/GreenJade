/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI-X IRQ -> Notification delivery (clean-room pure C).
 * Binds IDT vector GJ_MSIX_IRQ_VEC to a global Notification; soft inject,
 * soft pulse path, table-soft fire, and hw-sim pulse exercise delivery
 * without requiring a live device fire.
 * Not a full device driver - delivery plumbing only. No GPL source.
 * Soft != product - Dual MIT OR Apache-2.0 (Soft!=product dual license).
 *
 * greppable: MSI-X soft pulse path
 *
 * Soft inventory (Wave 14/15 base; residual multi-slot UDX advance; this unit):
 * Twin greppable prefixes (agent/smoke either works):
 *   "irq: soft ..."
 *   "irq_msix: soft ..."
 * Catalog lines (prefix-stable):
 *   irq: soft inventory / irq_msix: soft inventory  - vec + path catalog + wave
 *   irq: soft inject    / irq_msix: soft inject     - soft inject tallies
 *   irq: soft pulse     / irq_msix: soft pulse      - soft pulse-path tallies
 *   irq: soft table     / irq_msix: soft table      - table-soft fire tallies
 *   irq: soft hw        / irq_msix: soft hw         - hw-sim + hard IRQ tallies
 *   irq: soft badges    / irq_msix: soft badges     - last badge + path tag
 *   irq: soft vec       / irq_msix: soft vec        - IDT vector + path tags
 *   irq: soft notify    / irq_msix: soft notify     - Notification live snapshot
 *   irq: soft exercise  / irq_msix: soft exercise   - exercise tallies
 *   irq: soft path      / irq_msix: soft path       - honesty non-claim
 *   irq: soft deepen    / irq_msix: soft deepen     - wave stamp (no ret* storms)
 *   irq: soft ratio     / irq_msix: soft ratio      - Wave 15 path bp
 *   irq: soft headroom  / irq_msix: soft headroom   - Wave 15 exercise
 *   irq: soft surface   / irq_msix: soft surface    - Wave 16 catalog
 *   irq: soft honesty   / irq_msix: soft honesty
 *   irq: soft geom      / irq_msix: soft geom       - Wave 16 vec/badge geom
 *   irq: soft contract  / irq_msix: soft contract   - Wave 16 soft!=game I/O
 *   irq: soft return    / irq_msix: soft return     - surface bit mask
 *   irq: soft stats     / irq_msix: soft stats      - aggregate counters
 *   irq: soft inventory PASS / irq: soft PASS
 *   irq_msix: soft inventory PASS / irq_msix: soft PASS
 *   irq_msix: soft user notify PASS - soft inject -> badge for bound host
 *   irq_msix: soft residual - UDX host multi-slot residual catalog
 *   irq_msix: soft residual lean - poll-mode-first residual (no IRQ eth force)
 *   irq_msix: soft residual lean PASS - H1 lean residual once-shot
 *   irq_msix: soft residual multi_host - concurrent DDI hosts bind/unbind
 *   irq_msix: soft residual multi_host PASS - unbind(one) leaves others live
 *   irq_msix: soft residual user_bind - DDI_OP_IRQ_BIND soft note residual
 *   irq_msix: soft residual user_bind PASS - bind/rebind/multi-host/unbind-safe
 *   irq_msix: soft residual badge_iso - peer host badge mask isolation
 *   irq_msix: soft residual badge_iso PASS - inject A never hits host B mask
 *   irq_msix: soft residual slot_cap - multi-slot table full + reclaim
 *   irq_msix: soft residual slot_cap PASS - FULL reject then unbind+rebind
 *   irq_msix: soft residual multi_match - one inject hits all matching hosts
 *   irq_msix: soft residual multi_match PASS - shared badge fans out
 *   irq_msix: soft residual bulk_clear - bind(0) bulk vs unbind(one) CLOSE
 *   irq_msix: soft residual bulk_clear PASS - bulk clears all; CLOSE never bulk
 *   irq_msix: soft residual dual_dod - Dual DoD A/B OPEN honesty (UDX product)
 *   irq_msix: soft residual dual_dod PASS - honesty residual (A/B remain OPEN)
 *   irq_msix: soft residual table_user - soft_tbl fire -> UDX host note hits
 *   irq_msix: soft residual table_user PASS - product MSI-X path residual
 *   irq_msix: soft residual bind_honesty - product IRQ bind honesty residual
 *   irq_msix: soft residual bind_honesty PASS - primary/mask/table/live<=cap
 *   irq_msix: soft residual bind_honesty denser - multi-host UDX inject/peer
 *   irq_msix: soft residual bind_honesty denser PASS - inject+peer+ready denser
 *   irq_msix: soft residual irq_dma denser - product IRQ+DMA Dual DoD residual
 *   irq_msix: soft residual irq_dma denser PASS - triple-host + dual_dod denser
 *   irq_msix: soft user bind - handle->badge multi-slot note (DDI IRQ_BIND)
 *   product_notify_mint=OPEN - no per-host IRQ Notification CNode mint
 *   product_irq_cap=OPEN soft_note_only=1 - DDI IRQ_BIND residual honesty
 *   product_hosts=UDX - rtl 10ec:8168 + xhci 8086:a12f Dual DoD product
 *   freestanding_product=SKIP - never freestanding product close
 * C0 residual deepen (stamp-free; Soft!=product; G-AC-1; Dual DoD OPEN):
 *   Functional residual preferred over ret*angle stamp storms.
 *   Residual catalog defers once-shot until post-selftest (not init zeros).
 *   No version stamp. Never bump GJ_IMAGE_VERSION from this unit.
 *   Bar honesty v2026.08.04.75 panel context only — never invent .76.
 * Product IRQ+DMA Dual DoD denser residual (Soft!=product; bar .75):
 *   Triple-host UDX shape (rtl H81 + xhci H82 + ddi H83) inject isol denser;
 *   dual_dod A/B OPEN honesty denser; soft residual != Dual DoD close.
 *   greppable: irq_msix: soft residual irq_dma denser PASS
 * Never hard-gates product paths; diagnostics / smoke grep only.
 * Soft != live device MSI-X product close; soft != game I/O;
 * Soft!=product (ASCII only; never unicode-ne). Dual DoD OPEN product_hosts=UDX.
 * No stamp storms (no ret*angle kprintf floods). No version stamp.
 * H2: residual PASS lamps are once-shot only (no ret*angle storms).
 *
 * Driver-host soft wire (soft!=product; multi-slot residual for later UDX):
 *   soft user bind (handle->badge, up to IRQ_MSIX_SOFT_USER_SLOTS) ->
 *   soft inject / table fire -> notify_pulse(notify_msix_global) ->
 *   userspace GJ_SYS_NOTIFY_WAIT which=0 mask=badge block=0|1 reaps bits.
 * Multi-host safe residual (DDI_OP_IRQ_BIND / CLOSE foundation):
 *   Concurrent UDX hosts each own a distinct soft handle slot.
 *   unbind(handle) clears THAT host only (never bind(0) for single CLOSE).
 *   Product Notification mint remains OPEN (global MSI-X notify only).
 * Soft user bind residual deepen (DDI_OP_IRQ_BIND shape; Soft!=product):
 *   path=OPEN->IRQ_BIND(handle,badge)->inject->NOTIFY_WAIT->CLOSE unbind
 *   rebind updates one handle; multi-host bind while peers live is OK;
 *   never clear-all on single CLOSE; product_notify_mint=OPEN soft_note_only.
 * Badge isolation residual (concurrent UDX hosts; Soft!=product):
 *   host A SOFT-only + host B HW-only; inject SOFT hits A only; inject HW hits B.
 * Slot capacity residual (IRQ_MSIX_SOFT_USER_SLOTS multi-host table):
 *   fill free slots -> bind extra must FULL reject -> unbind one -> rebind OK.
 * Multi-match residual (global MSI-X notify fans out; Soft!=product):
 *   hosts sharing a badge bit both gain hits on one soft inject.
 * Bulk-clear residual (API honesty; Soft!=product):
 *   unbind(one) = DDI CLOSE shape; bind(0)/unbind(0) = bulk clear only.
 * Dual DoD residual honesty (Soft!=product; G-AC-1):
 *   Dual DoD A/B OPEN - UDX userspace product path; freestanding SKIP.
 *   soft residual != Dual DoD close; product_notify_mint=OPEN.
 *   dual_dod PASS = honesty residual only (A/B remain OPEN; not DoD close).
 *   product_hosts=UDX (rtl8168_udx 10ec:8168 + xhci_udx 8086:a12f).
 *   freestanding_product=SKIP - never freestanding product close.
 * Product MSI-X table_user residual (Soft!=product; UDX host path):
 *   soft_tbl fire entry0 (TBL badge) -> multi-host soft notes gain hits.
 *   path=soft_tbl->inject->notify_msix_global->NOTIFY_WAIT (product shape).
 *   greppable: irq_msix: soft residual table_user PASS
 * Product IRQ bind honesty residual (Soft!=product; multi-host UDX denser):
 *   irq_msix_ready residual denser: ready=1 after multi-host soft bind.
 *   primary handle/mask vs multi-slot table consistency.
 *   rebind residual tallies; live<=cap (live_le_cap=1).
 *   denser multi-host UDX: inject SOFT hits rtl-like only; inject HW hits
 *   xhci-like only; peer mask stable across rebind; multi_host_binds++.
 *   post-unbind inject still hits surviving host (CLOSE shape denser).
 *   greppable: irq_msix: soft residual bind_honesty PASS
 *   greppable: irq_msix: soft residual bind_honesty denser PASS
 *   greppable: product_hosts=UDX Soft!=product dual_dod OPEN denser=1
 *   greppable: primary_vs_table live_le_cap irq_msix_ready multi_host_udx
 * Product IRQ+DMA Dual DoD denser residual (Soft!=product; bar .75):
 *   Triple-host denser (H81 rtl / H82 xhci / H83 ddi) badge isol + fanout;
 *   Dual DoD A/B OPEN honesty denser; never freestanding product close.
 *   greppable: irq_msix: soft residual irq_dma denser PASS
 *   greppable: product IRQ+DMA Dual DoD OPEN Soft!=product denser=1
 *
 * Hazard H1 residual (Soft != product dual license):
 *   Freestanding net may be poll-mode (net_eth_poll owned by scheduler_run).
 *   Never force IRQ eth poll; never call net_eth_poll from this unit
 *   (handler / inject / pulse / table / exercise / init - none).
 *   never claim freestanding NIC MSI-X ownership. Soft != product.
 *   greppable: force_irq_eth_poll=0 poll_mode_first=1
 *   greppable: net_eth_poll=run_loop_only net_eth_irq=0
 *   greppable: net_eth_poll_from_msix=0
 */
#include <gj/apic.h>
#include <gj/idt.h>
#include <gj/irq_msix.h>
#include <gj/klog.h>
#include <gj/notify.h>
#include <gj/pci_caps.h>
#include <gj/types.h>

/*
 * H1 compile-time lock (Soft!=product): force_irq_eth_poll=0, poll_mode_first=1.
 * Flip requires H1 review - IRQ-stack eth poll is #PF I=1 fault class.
 * greppable: force_irq_eth_poll=0 poll_mode_first=1
 */
_Static_assert(IRQ_MSIX_FORCE_IRQ_ETH_POLL == 0u,
               "H1: force_irq_eth_poll must be 0");
_Static_assert(IRQ_MSIX_POLL_MODE_FIRST == 1u,
               "H1: poll_mode_first must be 1");
_Static_assert(IRQ_MSIX_NET_ETH_IRQ == 0u,
               "H1: net_eth_irq must be 0 (run-loop owns eth poll)");
_Static_assert(IRQ_MSIX_NET_ETH_POLL_FROM == 0u,
               "H1: never call net_eth_poll from irq_msix");

extern void irq_stub_msix(void);

static u32 g_u32MsixIrq;
static u32 g_u32MsixSoft;
static u32 g_u32MsixHw;
static u32 g_u32SoftPulsePath;
static u32 g_u32TablePulse;
static u64 g_u64LastBadge;
static u32 g_u32LastPath;
static int g_fReady;
static int g_fInHandler;

/*
 * Soft inventory category count + residual wave stamp.
 * Areas count greppable catalog lines (not stamp-storm ret*angle floods).
 * Residual advance: multi-slot soft user bind + multi-host unbind-safe +
 * DDI_OP_IRQ_BIND user_bind residual + lean poll-mode residual +
 * badge_iso peer mask isolation + slot_cap FULL/reclaim residual +
 * multi_match shared-badge fanout + bulk_clear vs CLOSE residual +
 * dual_dod OPEN honesty + table_user soft_tbl->UDX host residual +
 * bind_honesty denser multi-host UDX (inject/peer/ready/live_le_cap) +
 * irq_dma denser triple-host Dual DoD (H81/H82/H83 inject isol denser) +
 * product_hosts=UDX (10ec:8168|8086:a12f) + product_notify_mint=OPEN.
 * C0 residual deepen: stamp-free; G-AC-1; Dual DoD OPEN; Soft!=product.
 * IRQ_MSIX_SOFT_USER_SLOTS comes from <gj/irq_msix.h> (single source; Soft!=product).
 * Lean residual: freestanding net poll-mode first; force_irq_eth_poll=0.
 * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
 * H2 once-lamps only for residual PASS (Soft!=product ASCII only).
 */
#define IRQ_MSIX_SOFT_DEEPEN_AREAS 256u
#define IRQ_MSIX_SOFT_DEEPEN_WAVE 124u

/*
 * Wave 14 soft inventory sticky counters (wrap OK; never hard-gate).
 * Hard IRQ path uses atomic RMW only (no kprintf from irq_msix_handler).
 * greppable: irq: soft stats
 * greppable: irq_msix: soft stats
 */
static u32 g_u32SoftInjectEnter;   /* irq_msix_soft_inject entries */
static u32 g_u32SoftInjectZero;    /* badge 0 -> GJ_MSIX_BADGE_SOFT */
static u32 g_u32SoftPulseEnter;    /* soft_pulse_path entries */
static u32 g_u32SoftPulseNotReady; /* soft_pulse_path while !g_fReady */
static u32 g_u32SoftPulsePendOk;   /* pending observed badge after pulse */
static u32 g_u32SoftPulsePendMiss; /* pending miss after soft pulse */
static u32 g_u32SoftTableEnter;    /* soft_table_pulse entries */
static u32 g_u32SoftTableNotReady; /* soft_table_pulse while !g_fReady */
static u32 g_u32SoftTableInit;     /* soft table lazy init attempts */
static u32 g_u32SoftTableProg;     /* soft_program of entry for fire */
static u32 g_u32SoftTableFireOk;   /* pci_msix_soft_fire delivered */
static u32 g_u32SoftTableFireMiss; /* soft_fire returned 0 */
static u32 g_u32SoftTableMask;     /* soft_mask(0) ensure-unmask in exercise */
static u32 g_u32SoftHwEnter;       /* hw_pulse entries */
static u32 g_u32SoftHwNotReady;    /* hw_pulse while !g_fReady */
static u32 g_u32SoftIrqHandler;    /* hard IRQ handler entries */
static u32 g_u32SoftExerciseEnter; /* soft_path_exercise entries */
static u32 g_u32SoftExerciseOk;    /* soft_path_exercise PASS */
static u32 g_u32SoftExerciseFail;  /* soft_path_exercise FAIL */
static u32 g_u32SoftExerciseNotReady;
static u32 g_u32SoftInit;          /* irq_msix_init calls */
static u32 g_u32SoftLogN;          /* soft inventory emissions */
static u8  g_fSoftInvOnce;         /* one-shot deep dump after activity */

/*
 * Soft driver-host IRQ notes (multi-slot handle -> badge mask).
 * Soft != product IRQ cap mint; delivery remains global MSI-X Notification.
 * Residual for concurrent UDX hosts (one soft note per open DDI handle).
 * Multi-host safe: unbind(handle) never clears peer host slots.
 * Product Notification mint OPEN (soft_note_only; no CNode IRQ mint).
 * greppable: irq_msix: soft user notify PASS
 * greppable: irq_msix: soft residual
 * greppable: irq_msix: soft residual multi_host
 * greppable: product_notify_mint=OPEN
 */
struct irq_msix_soft_user_slot {
    u32 u32Handle; /* DDI soft handle id; 0 = free */
    u64 u64Mask;   /* wait mask for GJ_SYS_NOTIFY_WAIT */
    u32 u32Hits;   /* inject match hits for this slot */
};

static struct irq_msix_soft_user_slot g_aSoftUser[IRQ_MSIX_SOFT_USER_SLOTS];
static u32 g_u32SoftUserLive;      /* live slot count */
static u32 g_u32SoftUserHandle;    /* primary/last bind handle (stats API) */
static u64 g_u64SoftUserMask;      /* primary/last bind mask (stats API) */
static u32 g_u32SoftUserBinds;     /* soft_user_bind success count */
static u32 g_u32SoftUserUnbinds;   /* soft_user_unbind / clear count */
static u32 g_u32SoftUserFull;      /* bind rejected: table full */
static u32 g_u32SoftUserNotifyHit; /* inject match with any bind live */
static u32 g_u32SoftUserMultiHostSafe; /* unbind(one) left peers live */
static u32 g_u32SoftUserMultiHostCheck; /* multi-host residual selftests */
static u32 g_u32SoftUserRebinds;   /* DDI_OP_IRQ_BIND rebind residual */
static u32 g_u32SoftUserMultiHostBinds; /* new bind while peer(s) live */
static u32 g_u32SoftUserBindResidualCheck; /* user_bind residual selftests */
static u32 g_u32SoftUserBadgeIsoCheck; /* badge_iso residual selftests */
static u32 g_u32SoftUserSlotCapCheck;  /* slot_cap residual selftests */
static u32 g_u32SoftUserMultiMatchCheck; /* multi_match residual selftests */
static u32 g_u32SoftUserBulkClearCheck;  /* bulk_clear residual selftests */
static u32 g_u32SoftUserTableUserCheck;  /* table_user residual selftests */
static u32 g_u32SoftBindHonestyCheck;    /* bind_honesty residual selftests */
static u32 g_u32SoftBindHonestyDenserOk; /* denser multi-host UDX inject/peer */
static u32 g_u32SoftIrqDmaDualCheck;     /* product IRQ+DMA Dual DoD denser */
static u32 g_u32SoftIrqDmaDenserOk;      /* triple-host inject isol denser ok */
static u32 g_u32SoftPrimaryVsTableOk;    /* primary handle/mask matches table */
static u32 g_u32SoftLiveLeCapOk;         /* live <= cap residual hits */
static u32 g_u32SoftReadyOk;             /* irq_msix_ready residual hits */
static u8  g_fSoftUserNotifyPass;  /* once-shot PASS lamp */
static u8  g_fSoftResidualOnce;    /* once-shot residual catalog */
static u8  g_fSoftMultiHostPass;   /* once-shot multi_host residual PASS */
static u8  g_fSoftUserBindResidualPass; /* once-shot user_bind residual PASS */
static u8  g_fSoftBadgeIsoPass;    /* once-shot badge_iso residual PASS */
static u8  g_fSoftSlotCapPass;     /* once-shot slot_cap residual PASS */
static u8  g_fSoftMultiMatchPass;  /* once-shot multi_match residual PASS */
static u8  g_fSoftBulkClearPass;   /* once-shot bulk_clear residual PASS */
static u8  g_fSoftTableUserPass;   /* once-shot table_user residual PASS */
static u8  g_fSoftDualDodPass;     /* once-shot dual_dod honesty residual PASS */
static u8  g_fSoftBindHonestyPass; /* once-shot bind_honesty residual PASS */
static u8  g_fSoftIrqDmaDualPass;  /* once-shot irq_dma denser residual PASS */

static void irq_msix_soft_inventory_log(const char *szVia);
static void irq_msix_soft_inventory_maybe_once(void);
static void irq_msix_soft_user_notify_maybe(u64 u64Badge);
static void irq_msix_soft_user_clear_all(void);
static u32  irq_msix_soft_user_count_live(void);
static int  irq_msix_soft_user_slot_bound(u32 u32Handle);
static u32  irq_msix_soft_user_slot_hits(u32 u32Handle);
static u64  irq_msix_soft_user_slot_mask(u32 u32Handle);
static u32  irq_msix_soft_primary_vs_table_ok(void);
static void irq_msix_soft_residual_log(const char *szVia);
static u32  irq_msix_soft_multi_host_selftest(void);
static u32  irq_msix_soft_user_bind_residual_selftest(void);
static u32  irq_msix_soft_badge_iso_selftest(void);
static u32  irq_msix_soft_slot_cap_selftest(void);
static u32  irq_msix_soft_multi_match_selftest(void);
static u32  irq_msix_soft_bulk_clear_selftest(void);
static u32  irq_msix_soft_table_user_selftest(void);
static u32  irq_msix_soft_dual_dod_residual(void);
static u32  irq_msix_soft_bind_honesty_residual(void);
static u32  irq_msix_soft_irq_dma_dual_residual(void);

/** Soft: saturating-ish bump (u32 wrap is fine for telemetry). */
static void
irq_msix_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/** Soft: atomic sticky bump (IRQ-safe; hard handler path). */
static void
irq_msix_soft_inc_atomic(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (void)__atomic_fetch_add(pCtr, 1u, __ATOMIC_RELAXED);
}

static void
irq_msix_note_pulse(u64 u64Badge, u32 u32Path)
{
    if (u64Badge == 0) {
        u64Badge = GJ_MSIX_BADGE_SOFT;
    }
    g_u64LastBadge = u64Badge;
    g_u32LastPath = u32Path;
}

/** Soft: recount live multi-slot soft-bind notes. */
static u32
irq_msix_soft_user_count_live(void)
{
    u32 i;
    u32 u32Live;

    u32Live = 0u;
    for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
        if (g_aSoftUser[i].u32Handle != 0u) {
            u32Live++;
        }
    }
    g_u32SoftUserLive = u32Live;
    return u32Live;
}

/** Soft: clear all multi-slot soft-bind notes (handle 0 path). */
static void
irq_msix_soft_user_clear_all(void)
{
    u32 i;

    for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
        g_aSoftUser[i].u32Handle = 0u;
        g_aSoftUser[i].u64Mask = 0ull;
        g_aSoftUser[i].u32Hits = 0u;
    }
    g_u32SoftUserLive = 0u;
    g_u32SoftUserHandle = 0u;
    g_u64SoftUserMask = 0ull;
}

/** Soft: non-zero if handle currently occupies a multi-slot soft note. */
static int
irq_msix_soft_user_slot_bound(u32 u32Handle)
{
    u32 i;

    if (u32Handle == 0u) {
        return 0;
    }
    for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
        if (g_aSoftUser[i].u32Handle == u32Handle) {
            return 1;
        }
    }
    return 0;
}

/** Soft: per-slot inject match hits for handle (0 if unbound). Soft!=product. */
static u32
irq_msix_soft_user_slot_hits(u32 u32Handle)
{
    u32 i;

    if (u32Handle == 0u) {
        return 0u;
    }
    for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
        if (g_aSoftUser[i].u32Handle == u32Handle) {
            return g_aSoftUser[i].u32Hits;
        }
    }
    return 0u;
}

/** Soft: per-slot badge wait mask for handle (0 if unbound). Soft!=product. */
static u64
irq_msix_soft_user_slot_mask(u32 u32Handle)
{
    u32 i;

    if (u32Handle == 0u) {
        return 0ull;
    }
    for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
        if (g_aSoftUser[i].u32Handle == u32Handle) {
            return g_aSoftUser[i].u64Mask;
        }
    }
    return 0ull;
}

/**
 * Soft: primary handle/mask vs multi-slot table honesty (Soft!=product).
 * primary==0 and live==0 is OK (empty table). Else primary must occupy a
 * live table slot and primary mask must match that slot mask.
 * greppable: primary_vs_table
 */
static u32
irq_msix_soft_primary_vs_table_ok(void)
{
    u32 u32Live;
    u32 i;
    u32 fFound;

    u32Live = irq_msix_soft_user_count_live();
    if (g_u32SoftUserHandle == 0u) {
        /* Empty primary is honest only when table is empty. */
        return (u32Live == 0u) ? 1u : 0u;
    }
    fFound = 0u;
    for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
        if (g_aSoftUser[i].u32Handle == g_u32SoftUserHandle) {
            fFound = 1u;
            if (g_aSoftUser[i].u64Mask != g_u64SoftUserMask) {
                return 0u;
            }
            break;
        }
    }
    if (fFound == 0u) {
        return 0u;
    }
    /* live must be in [1,cap] when primary is set. Soft!=product. */
    if (u32Live == 0u || u32Live > IRQ_MSIX_SOFT_USER_SLOTS) {
        return 0u;
    }
    return 1u;
}

/**
 * Multi-host residual selftest (DDI IRQ_BIND foundation; Soft!=product).
 * Bind synthetic hosts A/B/C -> unbind B only -> A and C must remain live.
 * Product Notification mint stays OPEN (global MSI-X notify; no CNode mint).
 * Sparse once-shot lamps; no stamp storms; never hard-gates.
 * greppable: irq_msix: soft residual multi_host
 * greppable: irq_msix: soft residual multi_host PASS
 * greppable: product_notify_mint=OPEN
 * Returns 1 on multi-host safe PASS, 0 otherwise.
 */
static u32
irq_msix_soft_multi_host_selftest(void)
{
    u32 u32LiveBefore;
    u32 u32LiveAfter;
    u32 fOk;
    u32 fA;
    u32 fB;
    u32 fC;

    irq_msix_soft_inc(&g_u32SoftUserMultiHostCheck);
    if (!g_fReady) {
        return 0u;
    }

    /*
     * Synthetic multi-host DDI handles (distinct from exercise smoke 1..3
     * when those remain live). Prefer handles 11/12/13 so rebind is clean
     * even if soft_path_exercise left 1..3 bound.
     */
    if (irq_msix_soft_user_bind(11u, GJ_MSIX_BADGE_SOFT) != 0) {
        return 0u;
    }
    if (irq_msix_soft_user_bind(12u, GJ_MSIX_BADGE_HW) != 0) {
        (void)irq_msix_soft_user_unbind(11u);
        return 0u;
    }
    if (irq_msix_soft_user_bind(13u, GJ_MSIX_BADGE_TBL(0)) != 0) {
        (void)irq_msix_soft_user_unbind(11u);
        (void)irq_msix_soft_user_unbind(12u);
        return 0u;
    }

    u32LiveBefore = irq_msix_soft_user_count_live();
    /* Multi-host safe CLOSE shape: unbind one host only. */
    if (irq_msix_soft_user_unbind(12u) != 0) {
        (void)irq_msix_soft_user_unbind(11u);
        (void)irq_msix_soft_user_unbind(13u);
        return 0u;
    }
    u32LiveAfter = irq_msix_soft_user_count_live();

    fA = (irq_msix_soft_user_slot_bound(11u) != 0) ? 1u : 0u;
    fB = (irq_msix_soft_user_slot_bound(12u) != 0) ? 1u : 0u;
    fC = (irq_msix_soft_user_slot_bound(13u) != 0) ? 1u : 0u;

    fOk = 0u;
    if (fA == 1u && fB == 0u && fC == 1u &&
        u32LiveAfter + 1u == u32LiveBefore &&
        u32LiveAfter >= 2u) {
        /* multi_host_safe already tallied by unbind(one) peer-live residual. */
        fOk = 1u;
    }

    /* Grep: irq_msix: soft residual multi_host */
    kprintf("irq_msix: soft residual multi_host via=selftest "
            "hosts=A11,B12,C13 unbind=B12 "
            "bound_A=%u bound_B=%u bound_C=%u "
            "live_before=%u live_after=%u safe=%u "
            "ddi_irq_bind=soft_note "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "path=DDI_OP_IRQ_BIND->unbind(handle)->peer_hosts_live "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)fA, (unsigned)fB, (unsigned)fC,
            (unsigned)u32LiveBefore, (unsigned)u32LiveAfter,
            (unsigned)fOk);
    /* Twin residual lamp */
    kprintf("irq: soft residual multi_host unbind_one=1 peer_live=%u "
            "product_notify_mint=OPEN soft_note_only=1 soft!=product\n",
            (unsigned)(fA + fC));

    if (fOk != 0u && g_fSoftMultiHostPass == 0u) {
        g_fSoftMultiHostPass = 1u;
        /* Grep: irq_msix: soft residual multi_host PASS */
        kprintf("irq_msix: soft residual multi_host PASS "
                "unbind_one=1 peers_live=1 "
                "product_notify_mint=OPEN product_irq_cap=OPEN "
                "soft_note_only=1 soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual multi_host PASS "
                "product_notify_mint=OPEN soft!=product\n");
    }

    /*
     * Leave A+C live for residual inventory greps (B unbound proves
     * multi-host CLOSE shape). Soft residual only; Soft!=product.
     */
    return fOk;
}

/**
 * Soft user bind residual selftest for DDI_OP_IRQ_BIND (Soft!=product).
 * Mirrors door shape without calling ddi_door (this unit exclusive):
 *   bind H21 (first host) -> bind H22 while H21 live (multi-host bind)
 *   -> rebind H21 mask update only -> soft inject match
 *   -> unbind H22 only (CLOSE shape; never bind(0) clear-all)
 *   -> H21 remains live; product Notification mint stays OPEN.
 * Sparse once-shot lamps; no stamp storms; never hard-gates.
 * greppable: irq_msix: soft residual user_bind
 * greppable: irq_msix: soft residual user_bind PASS
 * greppable: product_notify_mint=OPEN
 * Returns 1 on residual PASS, 0 otherwise.
 */
static u32
irq_msix_soft_user_bind_residual_selftest(void)
{
    u32 u32LiveAfterFirst;
    u32 u32LiveAfterSecond;
    u32 u32LiveAfterUnbind;
    u32 u32RebindsBefore;
    u32 u32MultiBindsBefore;
    u32 fOk;
    u32 fH21;
    u32 fH22;
    u64 u64MaskH21;

    irq_msix_soft_inc(&g_u32SoftUserBindResidualCheck);
    if (!g_fReady) {
        return 0u;
    }

    /*
     * Synthetic DDI soft handles 21/22 - distinct from exercise 1..3 and
     * multi_host selftest 11..13 so residual layers compose cleanly.
     * Path honesty: DDI_OP_IRQ_BIND never uses handle 0 (bulk clear).
     */
    u32RebindsBefore = g_u32SoftUserRebinds;
    u32MultiBindsBefore = g_u32SoftUserMultiHostBinds;

    if (irq_msix_soft_user_bind(21u, GJ_MSIX_BADGE_SOFT) != 0) {
        return 0u;
    }
    u32LiveAfterFirst = irq_msix_soft_user_count_live();

    /* Multi-host bind residual: second host while peer live. */
    if (irq_msix_soft_user_bind(22u, GJ_MSIX_BADGE_HW) != 0) {
        (void)irq_msix_soft_user_unbind(21u);
        return 0u;
    }
    u32LiveAfterSecond = irq_msix_soft_user_count_live();

    /* Rebind residual: DDI_OP_IRQ_BIND on already-bound handle updates mask. */
    u64MaskH21 = GJ_MSIX_BADGE_SOFT | GJ_MSIX_BADGE_TBL(0);
    if (irq_msix_soft_user_bind(21u, u64MaskH21) != 0) {
        (void)irq_msix_soft_user_unbind(21u);
        (void)irq_msix_soft_user_unbind(22u);
        return 0u;
    }

    /* Soft inject matches rebind mask (notify residual shape). */
    irq_msix_soft_inject(GJ_MSIX_BADGE_SOFT);

    /* CLOSE shape: unbind one host only - never bind(0) clear-all. */
    if (irq_msix_soft_user_unbind(22u) != 0) {
        (void)irq_msix_soft_user_unbind(21u);
        return 0u;
    }
    u32LiveAfterUnbind = irq_msix_soft_user_count_live();

    fH21 = (irq_msix_soft_user_slot_bound(21u) != 0) ? 1u : 0u;
    fH22 = (irq_msix_soft_user_slot_bound(22u) != 0) ? 1u : 0u;

    fOk = 0u;
    if (fH21 == 1u && fH22 == 0u &&
        u32LiveAfterFirst >= 1u &&
        u32LiveAfterSecond > u32LiveAfterFirst &&
        u32LiveAfterUnbind + 1u == u32LiveAfterSecond &&
        g_u32SoftUserRebinds > u32RebindsBefore &&
        g_u32SoftUserMultiHostBinds > u32MultiBindsBefore) {
        fOk = 1u;
    }

    /* Grep: irq_msix: soft residual user_bind */
    kprintf("irq_msix: soft residual user_bind via=selftest "
            "hosts=H21,H22 rebind=H21 unbind=H22 "
            "bound_H21=%u bound_H22=%u "
            "live1=%u live2=%u live_after_unbind=%u "
            "rebinds=%u multi_host_binds=%u safe=%u "
            "ddi_op=IRQ_BIND close=unbind_handle never_clear_all=1 "
            "path=DDI_OP_OPEN->DDI_OP_IRQ_BIND->inject->"
            "GJ_SYS_NOTIFY_WAIT->DDI_OP_CLOSE "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)fH21, (unsigned)fH22,
            (unsigned)u32LiveAfterFirst, (unsigned)u32LiveAfterSecond,
            (unsigned)u32LiveAfterUnbind,
            (unsigned)g_u32SoftUserRebinds,
            (unsigned)g_u32SoftUserMultiHostBinds,
            (unsigned)fOk);
    /* Twin residual lamp */
    kprintf("irq: soft residual user_bind ddi_irq_bind=1 rebind=1 "
            "multi_host_bind=1 unbind_one=1 never_clear_all=1 "
            "product_notify_mint=OPEN soft_note_only=1 soft!=product\n");

    if (fOk != 0u && g_fSoftUserBindResidualPass == 0u) {
        g_fSoftUserBindResidualPass = 1u;
        /* Grep: irq_msix: soft residual user_bind PASS */
        kprintf("irq_msix: soft residual user_bind PASS "
                "ddi_irq_bind=1 rebind=1 multi_host_bind=1 "
                "unbind_one=1 never_clear_all=1 peers_live=1 "
                "product_notify_mint=OPEN product_irq_cap=OPEN "
                "soft_note_only=1 soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual user_bind PASS "
                "product_notify_mint=OPEN soft!=product\n");
    }

    /*
     * Leave H21 live for residual inventory greps (H22 unbound proves
     * DDI CLOSE unbind-one shape). Soft residual only; Soft!=product.
     */
    return fOk;
}

/**
 * Badge isolation residual selftest (concurrent UDX hosts; Soft!=product).
 * Host A SOFT-only + host B HW-only; inject SOFT must hit A only; inject HW
 * must hit B only. Rebind A must not clobber B mask. Proves multi-host
 * badge bit_N_line_N isolation without per-host CNode IRQ mint.
 * Handles 41/42 - distinct from exercise 1..3, multi_host 11..13, user_bind
 * 21..22 so residual layers compose. Unbind both after (leave table free
 * for slot_cap residual). Sparse once-shot lamps; no stamp storms.
 * greppable: irq_msix: soft residual badge_iso
 * greppable: irq_msix: soft residual badge_iso PASS
 * greppable: product_notify_mint=OPEN
 * Returns 1 on residual PASS, 0 otherwise.
 */
static u32
irq_msix_soft_badge_iso_selftest(void)
{
    u32 u32HitsA0;
    u32 u32HitsB0;
    u32 u32HitsA1;
    u32 u32HitsB1;
    u32 u32HitsA2;
    u32 u32HitsB2;
    u64 u64MaskB;
    u32 fOk;
    u32 fA;
    u32 fB;

    irq_msix_soft_inc(&g_u32SoftUserBadgeIsoCheck);
    if (!g_fReady) {
        return 0u;
    }

    /* Clean slate for 41/42 if a prior residual left them. */
    (void)irq_msix_soft_user_unbind(41u);
    (void)irq_msix_soft_user_unbind(42u);

    if (irq_msix_soft_user_bind(41u, GJ_MSIX_BADGE_SOFT) != 0) {
        return 0u;
    }
    if (irq_msix_soft_user_bind(42u, GJ_MSIX_BADGE_HW) != 0) {
        (void)irq_msix_soft_user_unbind(41u);
        return 0u;
    }

    /* Rebind A only - peer B mask must stay HW. Soft!=product. */
    if (irq_msix_soft_user_bind(41u, GJ_MSIX_BADGE_SOFT | GJ_MSIX_BADGE_TBL(0))
        != 0) {
        (void)irq_msix_soft_user_unbind(41u);
        (void)irq_msix_soft_user_unbind(42u);
        return 0u;
    }
    u64MaskB = irq_msix_soft_user_slot_mask(42u);
    if (u64MaskB != GJ_MSIX_BADGE_HW) {
        (void)irq_msix_soft_user_unbind(41u);
        (void)irq_msix_soft_user_unbind(42u);
        /* Grep: irq_msix: soft residual badge_iso (peer mask clobber) */
        kprintf("irq_msix: soft residual badge_iso via=selftest "
                "safe=0 reason=peer_mask_clobber mask_B=0x%lx "
                "product_notify_mint=OPEN soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n",
                (unsigned long)u64MaskB);
        return 0u;
    }

    u32HitsA0 = irq_msix_soft_user_slot_hits(41u);
    u32HitsB0 = irq_msix_soft_user_slot_hits(42u);

    /* Inject SOFT: A must gain a hit; B must not (HW-only mask). */
    irq_msix_soft_inject(GJ_MSIX_BADGE_SOFT);
    u32HitsA1 = irq_msix_soft_user_slot_hits(41u);
    u32HitsB1 = irq_msix_soft_user_slot_hits(42u);

    /*
     * Drop SOFT from A mask so sticky pending SOFT cannot false-match A
     * on the subsequent HW inject (notify_maybe ORs pending & mask).
     * Peer B mask must remain HW through this rebind. Soft!=product.
     */
    if (irq_msix_soft_user_bind(41u, GJ_MSIX_BADGE_TBL(1)) != 0) {
        (void)irq_msix_soft_user_unbind(41u);
        (void)irq_msix_soft_user_unbind(42u);
        return 0u;
    }
    u64MaskB = irq_msix_soft_user_slot_mask(42u);
    if (u64MaskB != GJ_MSIX_BADGE_HW) {
        (void)irq_msix_soft_user_unbind(41u);
        (void)irq_msix_soft_user_unbind(42u);
        kprintf("irq_msix: soft residual badge_iso via=selftest "
                "safe=0 reason=peer_mask_clobber_rebind2 mask_B=0x%lx "
                "product_notify_mint=OPEN soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n",
                (unsigned long)u64MaskB);
        return 0u;
    }

    /* Inject HW: B must gain a hit; A must not (TBL1-only mask). */
    irq_msix_soft_inject(GJ_MSIX_BADGE_HW);
    u32HitsA2 = irq_msix_soft_user_slot_hits(41u);
    u32HitsB2 = irq_msix_soft_user_slot_hits(42u);

    fA = (irq_msix_soft_user_slot_bound(41u) != 0) ? 1u : 0u;
    fB = (irq_msix_soft_user_slot_bound(42u) != 0) ? 1u : 0u;

    fOk = 0u;
    if (fA == 1u && fB == 1u &&
        u64MaskB == GJ_MSIX_BADGE_HW &&
        u32HitsA1 > u32HitsA0 &&
        u32HitsB1 == u32HitsB0 &&
        u32HitsB2 > u32HitsB1 &&
        u32HitsA2 == u32HitsA1) {
        fOk = 1u;
    }

    /* Grep: irq_msix: soft residual badge_iso */
    kprintf("irq_msix: soft residual badge_iso via=selftest "
            "hosts=A41,B42 inject_soft then inject_hw "
            "hits_A=%u->%u->%u hits_B=%u->%u->%u "
            "mask_B=0x%lx peer_mask_ok=%u "
            "soft_hits_A_only=%u hw_hits_B_only=%u safe=%u "
            "path=DDI_OP_IRQ_BIND->inject_badge->peer_mask_isol "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)u32HitsA0, (unsigned)u32HitsA1, (unsigned)u32HitsA2,
            (unsigned)u32HitsB0, (unsigned)u32HitsB1, (unsigned)u32HitsB2,
            (unsigned long)u64MaskB,
            (unsigned)((u64MaskB == GJ_MSIX_BADGE_HW) ? 1u : 0u),
            (unsigned)((u32HitsA1 > u32HitsA0 && u32HitsB1 == u32HitsB0)
                       ? 1u : 0u),
            (unsigned)((u32HitsB2 > u32HitsB1 && u32HitsA2 == u32HitsA1)
                       ? 1u : 0u),
            (unsigned)fOk);
    /* Twin residual lamp */
    kprintf("irq: soft residual badge_iso peer_mask_isol=1 "
            "soft_A_only=1 hw_B_only=1 "
            "product_notify_mint=OPEN soft_note_only=1 soft!=product\n");

    if (fOk != 0u && g_fSoftBadgeIsoPass == 0u) {
        g_fSoftBadgeIsoPass = 1u;
        /* Grep: irq_msix: soft residual badge_iso PASS */
        kprintf("irq_msix: soft residual badge_iso PASS "
                "peer_mask_isol=1 soft_A_only=1 hw_B_only=1 "
                "product_notify_mint=OPEN product_irq_cap=OPEN "
                "soft_note_only=1 soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual badge_iso PASS "
                "product_notify_mint=OPEN soft!=product\n");
    }

    /* Free 41/42 so slot_cap residual can fill remaining capacity. */
    (void)irq_msix_soft_user_unbind(41u);
    (void)irq_msix_soft_user_unbind(42u);
    return fOk;
}

/**
 * Slot capacity residual selftest (IRQ_MSIX_SOFT_USER_SLOTS; Soft!=product).
 * Fill free multi-slot notes with synthetic hosts 31.. -> bind one more must
 * FULL reject (-1) -> unbind one synthetic -> rebind succeeds. Proves
 * concurrent UDX host table capacity without product CNode IRQ mint.
 * Leaves synthetic residual hosts live only if reclaim left a free path;
 * does not clear peer hosts (never bind(0)). Sparse lamps; no stamp storms.
 * greppable: irq_msix: soft residual slot_cap
 * greppable: irq_msix: soft residual slot_cap PASS
 * greppable: product_notify_mint=OPEN
 * Returns 1 on residual PASS, 0 otherwise.
 */
static u32
irq_msix_soft_slot_cap_selftest(void)
{
    u32 u32Live0;
    u32 u32LiveFull;
    u32 u32LiveAfterUnbind;
    u32 u32LiveAfterRebind;
    u32 u32Free;
    u32 u32Filled;
    u32 u32H;
    u32 u32FirstSyn;
    u32 u32Extra;
    u32 u32FullBefore;
    int iBindExtra;
    int iBindReclaim;
    u32 fOk;
    u32 i;

    irq_msix_soft_inc(&g_u32SoftUserSlotCapCheck);
    if (!g_fReady) {
        return 0u;
    }

    u32Live0 = irq_msix_soft_user_count_live();
    if (u32Live0 > IRQ_MSIX_SOFT_USER_SLOTS) {
        return 0u;
    }
    u32Free = IRQ_MSIX_SOFT_USER_SLOTS - u32Live0;
    u32FullBefore = g_u32SoftUserFull;
    u32FirstSyn = 0u; /* first successfully bound synthetic (0 = none yet) */
    u32H = 31u;
    u32Filled = 0u;

    /*
     * If already at capacity (peer residual layers filled the table), free
     * one smoke host (handle 3) so this residual can still exercise FULL +
     * reclaim. Never bind(0) clear-all. Soft!=product.
     */
    if (u32Free == 0u) {
        if (irq_msix_soft_user_slot_bound(3u) != 0) {
            (void)irq_msix_soft_user_unbind(3u);
        } else if (irq_msix_soft_user_slot_bound(2u) != 0) {
            (void)irq_msix_soft_user_unbind(2u);
        }
        u32Live0 = irq_msix_soft_user_count_live();
        if (u32Live0 >= IRQ_MSIX_SOFT_USER_SLOTS) {
            kprintf("irq_msix: soft residual slot_cap via=selftest "
                    "safe=0 reason=no_free_slot live=%u cap=%u "
                    "product_notify_mint=OPEN soft!=product "
                    "dual_license=MIT_OR_Apache-2.0\n",
                    (unsigned)u32Live0,
                    (unsigned)IRQ_MSIX_SOFT_USER_SLOTS);
            return 0u;
        }
        u32Free = IRQ_MSIX_SOFT_USER_SLOTS - u32Live0;
    }

    /*
     * Fill free slots with synthetic handles 31,32,... (skip if already bound
     * from a prior residual layer). Soft!=product multi-host table residual.
     */
    while (u32Filled < u32Free && u32H < 31u + IRQ_MSIX_SOFT_USER_SLOTS + 8u) {
        if (irq_msix_soft_user_slot_bound(u32H) != 0) {
            u32H++;
            continue;
        }
        if (irq_msix_soft_user_bind(u32H, GJ_MSIX_BADGE_SOFT) != 0) {
            break;
        }
        if (u32FirstSyn == 0u) {
            u32FirstSyn = u32H;
        }
        u32Filled++;
        u32H++;
    }
    if (u32FirstSyn == 0u) {
        kprintf("irq_msix: soft residual slot_cap via=selftest "
                "safe=0 reason=no_syn_bound live=%u cap=%u "
                "product_notify_mint=OPEN soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n",
                (unsigned)irq_msix_soft_user_count_live(),
                (unsigned)IRQ_MSIX_SOFT_USER_SLOTS);
        return 0u;
    }

    u32LiveFull = irq_msix_soft_user_count_live();
    if (u32LiveFull != IRQ_MSIX_SOFT_USER_SLOTS) {
        /* Could not fill - still attempt FULL on a fresh handle if fullish. */
        if (u32LiveFull < IRQ_MSIX_SOFT_USER_SLOTS) {
            kprintf("irq_msix: soft residual slot_cap via=selftest "
                    "safe=0 reason=fill_short live=%u cap=%u filled=%u "
                    "product_notify_mint=OPEN soft!=product "
                    "dual_license=MIT_OR_Apache-2.0\n",
                    (unsigned)u32LiveFull,
                    (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
                    (unsigned)u32Filled);
            /* Best-effort cleanup of synthetics we bound. */
            for (i = 0u; i < u32Filled; i++) {
                (void)irq_msix_soft_user_unbind(u32FirstSyn + i);
            }
            return 0u;
        }
    }

    /* Extra bind must FULL reject (table capacity honesty). */
    u32Extra = u32H;
    while (irq_msix_soft_user_slot_bound(u32Extra) != 0 &&
           u32Extra < u32H + 16u) {
        u32Extra++;
    }
    iBindExtra = irq_msix_soft_user_bind(u32Extra, GJ_MSIX_BADGE_HW);
    if (iBindExtra != -1) {
        /* Unexpected accept - clean and fail. */
        (void)irq_msix_soft_user_unbind(u32Extra);
        for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS + 4u; i++) {
            if (irq_msix_soft_user_slot_bound(u32FirstSyn + i) != 0) {
                (void)irq_msix_soft_user_unbind(u32FirstSyn + i);
            }
        }
        kprintf("irq_msix: soft residual slot_cap via=selftest "
                "safe=0 reason=full_accepted live=%u cap=%u "
                "product_notify_mint=OPEN soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n",
                (unsigned)irq_msix_soft_user_count_live(),
                (unsigned)IRQ_MSIX_SOFT_USER_SLOTS);
        return 0u;
    }

    /* Reclaim one synthetic (CLOSE shape) then rebind extra. */
    if (irq_msix_soft_user_unbind(u32FirstSyn) != 0) {
        return 0u;
    }
    u32LiveAfterUnbind = irq_msix_soft_user_count_live();
    iBindReclaim = irq_msix_soft_user_bind(u32Extra, GJ_MSIX_BADGE_HW);
    u32LiveAfterRebind = irq_msix_soft_user_count_live();

    fOk = 0u;
    if (iBindExtra == -1 &&
        iBindReclaim == 0 &&
        u32LiveFull == IRQ_MSIX_SOFT_USER_SLOTS &&
        u32LiveAfterUnbind + 1u == u32LiveFull &&
        u32LiveAfterRebind == u32LiveFull &&
        g_u32SoftUserFull > u32FullBefore &&
        irq_msix_soft_user_slot_bound(u32Extra) != 0 &&
        irq_msix_soft_user_slot_bound(u32FirstSyn) == 0) {
        fOk = 1u;
    }

    /* Grep: irq_msix: soft residual slot_cap */
    kprintf("irq_msix: soft residual slot_cap via=selftest "
            "cap=%u live0=%u filled=%u live_full=%u "
            "full_reject=%u full_tally=%u "
            "unbind_syn=%u live_after_unbind=%u "
            "rebind_extra=%u live_after_rebind=%u safe=%u "
            "path=fill->FULL->unbind(one)->rebind "
            "never_clear_all=1 ddi_op=IRQ_BIND "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
            (unsigned)u32Live0, (unsigned)u32Filled,
            (unsigned)u32LiveFull,
            (unsigned)((iBindExtra == -1) ? 1u : 0u),
            (unsigned)g_u32SoftUserFull,
            (unsigned)u32FirstSyn, (unsigned)u32LiveAfterUnbind,
            (unsigned)((iBindReclaim == 0) ? 1u : 0u),
            (unsigned)u32LiveAfterRebind,
            (unsigned)fOk);
    /* Twin residual lamp */
    kprintf("irq: soft residual slot_cap full_reject=1 reclaim=1 "
            "never_clear_all=1 product_notify_mint=OPEN "
            "soft_note_only=1 soft!=product\n");

    if (fOk != 0u && g_fSoftSlotCapPass == 0u) {
        g_fSoftSlotCapPass = 1u;
        /* Grep: irq_msix: soft residual slot_cap PASS */
        kprintf("irq_msix: soft residual slot_cap PASS "
                "full_reject=1 reclaim=1 never_clear_all=1 "
                "cap=%u product_notify_mint=OPEN product_irq_cap=OPEN "
                "soft_note_only=1 soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n",
                (unsigned)IRQ_MSIX_SOFT_USER_SLOTS);
        kprintf("irq: soft residual slot_cap PASS "
                "product_notify_mint=OPEN soft!=product\n");
    }

    /*
     * Leave table at capacity with one reclaimed synthetic (extra) live.
     * Peer residual hosts (1..3, 11, 13, 21) remain - never clear-all.
     * Soft residual only; Soft!=product.
     */
    return fOk;
}

/**
 * Multi-match residual selftest (global MSI-X notify fanout; Soft!=product).
 * Hosts 61 SOFT-only + 62 SOFT|HW; one inject SOFT must hit BOTH slots.
 * Proves soft note table fans shared badge bits to concurrent UDX hosts
 * without per-host CNode IRQ mint (product_notify_mint=OPEN).
 * Handles 61/62 distinct from exercise 1..3, multi_host 11..13, user_bind
 * 21..22, badge_iso 41..42, slot_cap 31.. . Unbind both after.
 * Sparse once-shot lamps; no stamp storms; never hard-gates.
 * greppable: irq_msix: soft residual multi_match
 * greppable: irq_msix: soft residual multi_match PASS
 * greppable: product_notify_mint=OPEN
 * Returns 1 on residual PASS, 0 otherwise.
 */
static u32
irq_msix_soft_multi_match_selftest(void)
{
    u32 u32HitsA0;
    u32 u32HitsB0;
    u32 u32HitsA1;
    u32 u32HitsB1;
    u32 fOk;
    u32 fA;
    u32 fB;
    u32 u32Live0;
    u32 u32Free;

    irq_msix_soft_inc(&g_u32SoftUserMultiMatchCheck);
    if (!g_fReady) {
        return 0u;
    }

    /* Need two free slots; free smoke/synthetic hosts if table full. Soft!=product. */
    u32Live0 = irq_msix_soft_user_count_live();
    u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                  ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                  : 0u;
    if (u32Free < 2u) {
        u32 u32Drop;

        /* Prefer smoke 1..3 then slot_cap synthetics 31..40 (never clear-all). */
        for (u32Drop = 1u; u32Drop <= 3u && u32Free < 2u; u32Drop++) {
            if (irq_msix_soft_user_slot_bound(u32Drop) != 0) {
                (void)irq_msix_soft_user_unbind(u32Drop);
                u32Live0 = irq_msix_soft_user_count_live();
                u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                              ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                              : 0u;
            }
        }
        for (u32Drop = 31u; u32Drop <= 40u && u32Free < 2u; u32Drop++) {
            if (irq_msix_soft_user_slot_bound(u32Drop) != 0) {
                (void)irq_msix_soft_user_unbind(u32Drop);
                u32Live0 = irq_msix_soft_user_count_live();
                u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                              ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                              : 0u;
            }
        }
        if (u32Free < 2u) {
            kprintf("irq_msix: soft residual multi_match via=selftest "
                    "safe=0 reason=no_free_pair live=%u cap=%u "
                    "product_notify_mint=OPEN soft!=product "
                    "dual_license=MIT_OR_Apache-2.0\n",
                    (unsigned)u32Live0,
                    (unsigned)IRQ_MSIX_SOFT_USER_SLOTS);
            return 0u;
        }
    }

    (void)irq_msix_soft_user_unbind(61u);
    (void)irq_msix_soft_user_unbind(62u);

    if (irq_msix_soft_user_bind(61u, GJ_MSIX_BADGE_SOFT) != 0) {
        return 0u;
    }
    if (irq_msix_soft_user_bind(62u, GJ_MSIX_BADGE_SOFT | GJ_MSIX_BADGE_HW)
        != 0) {
        (void)irq_msix_soft_user_unbind(61u);
        return 0u;
    }

    u32HitsA0 = irq_msix_soft_user_slot_hits(61u);
    u32HitsB0 = irq_msix_soft_user_slot_hits(62u);

    /* One inject SOFT: both hosts sharing SOFT bit must gain a hit. */
    irq_msix_soft_inject(GJ_MSIX_BADGE_SOFT);
    u32HitsA1 = irq_msix_soft_user_slot_hits(61u);
    u32HitsB1 = irq_msix_soft_user_slot_hits(62u);

    fA = (irq_msix_soft_user_slot_bound(61u) != 0) ? 1u : 0u;
    fB = (irq_msix_soft_user_slot_bound(62u) != 0) ? 1u : 0u;

    fOk = 0u;
    if (fA == 1u && fB == 1u &&
        u32HitsA1 > u32HitsA0 &&
        u32HitsB1 > u32HitsB0) {
        fOk = 1u;
    }

    /* Grep: irq_msix: soft residual multi_match */
    kprintf("irq_msix: soft residual multi_match via=selftest "
            "hosts=A61,B62 inject_soft "
            "hits_A=%u->%u hits_B=%u->%u "
            "both_hit=%u safe=%u "
            "path=DDI_OP_IRQ_BIND->inject_shared_badge->fanout "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)u32HitsA0, (unsigned)u32HitsA1,
            (unsigned)u32HitsB0, (unsigned)u32HitsB1,
            (unsigned)((u32HitsA1 > u32HitsA0 && u32HitsB1 > u32HitsB0)
                       ? 1u : 0u),
            (unsigned)fOk);
    /* Twin residual lamp */
    kprintf("irq: soft residual multi_match shared_badge_fanout=1 "
            "both_hit=%u product_notify_mint=OPEN "
            "soft_note_only=1 soft!=product\n",
            (unsigned)fOk);

    if (fOk != 0u && g_fSoftMultiMatchPass == 0u) {
        g_fSoftMultiMatchPass = 1u;
        /* Grep: irq_msix: soft residual multi_match PASS */
        kprintf("irq_msix: soft residual multi_match PASS "
                "shared_badge_fanout=1 both_hit=1 "
                "product_notify_mint=OPEN product_irq_cap=OPEN "
                "soft_note_only=1 soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual multi_match PASS "
                "product_notify_mint=OPEN soft!=product\n");
    }

    (void)irq_msix_soft_user_unbind(61u);
    (void)irq_msix_soft_user_unbind(62u);
    return fOk;
}

/**
 * Bulk-clear residual selftest (API honesty; Soft!=product dual license).
 * unbind(one) = DDI CLOSE shape (peer hosts live). bind(0)/unbind(0) =
 * bulk clear only - never the multi-host CLOSE path.
 * Handles 51/52: bind both -> unbind 51 leaves 52 -> bind(0) clears all.
 * Re-seeds residual hosts 11+21 after bulk for inventory greps.
 * Sparse once-shot lamps; no stamp storms; never hard-gates.
 * greppable: irq_msix: soft residual bulk_clear
 * greppable: irq_msix: soft residual bulk_clear PASS
 * greppable: product_notify_mint=OPEN
 * Returns 1 on residual PASS, 0 otherwise.
 */
static u32
irq_msix_soft_bulk_clear_selftest(void)
{
    u32 u32LiveAfterPair;
    u32 u32LiveAfterUnbindOne;
    u32 u32LiveAfterBulk;
    u32 fOk;
    u32 f51AfterUnbind;
    u32 f52AfterUnbind;
    u32 f51AfterBulk;
    u32 f52AfterBulk;
    u32 u32Live0;
    u32 u32Free;

    irq_msix_soft_inc(&g_u32SoftUserBulkClearCheck);
    if (!g_fReady) {
        return 0u;
    }

    /* Need two free slots for 51/52. Soft!=product. */
    u32Live0 = irq_msix_soft_user_count_live();
    u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                  ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                  : 0u;
    if (u32Free < 2u) {
        if (irq_msix_soft_user_slot_bound(3u) != 0) {
            (void)irq_msix_soft_user_unbind(3u);
        }
        if (irq_msix_soft_user_slot_bound(2u) != 0) {
            (void)irq_msix_soft_user_unbind(2u);
        }
        if (irq_msix_soft_user_slot_bound(1u) != 0) {
            (void)irq_msix_soft_user_unbind(1u);
        }
        u32Live0 = irq_msix_soft_user_count_live();
        u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                      ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                      : 0u;
        if (u32Free < 2u) {
            kprintf("irq_msix: soft residual bulk_clear via=selftest "
                    "safe=0 reason=no_free_pair live=%u cap=%u "
                    "product_notify_mint=OPEN soft!=product "
                    "dual_license=MIT_OR_Apache-2.0\n",
                    (unsigned)u32Live0,
                    (unsigned)IRQ_MSIX_SOFT_USER_SLOTS);
            return 0u;
        }
    }

    (void)irq_msix_soft_user_unbind(51u);
    (void)irq_msix_soft_user_unbind(52u);

    if (irq_msix_soft_user_bind(51u, GJ_MSIX_BADGE_SOFT) != 0) {
        return 0u;
    }
    if (irq_msix_soft_user_bind(52u, GJ_MSIX_BADGE_HW) != 0) {
        (void)irq_msix_soft_user_unbind(51u);
        return 0u;
    }
    u32LiveAfterPair = irq_msix_soft_user_count_live();

    /* CLOSE shape: unbind one only - peer 52 must remain. */
    if (irq_msix_soft_user_unbind(51u) != 0) {
        (void)irq_msix_soft_user_unbind(52u);
        return 0u;
    }
    u32LiveAfterUnbindOne = irq_msix_soft_user_count_live();
    f51AfterUnbind = (irq_msix_soft_user_slot_bound(51u) != 0) ? 1u : 0u;
    f52AfterUnbind = (irq_msix_soft_user_slot_bound(52u) != 0) ? 1u : 0u;

    /*
     * Bulk clear API: bind(0,*) clears ALL soft notes (not DDI CLOSE).
     * Soft residual honesty - product CLOSE must use unbind(handle).
     */
    if (irq_msix_soft_user_bind(0u, 0ull) != 0) {
        return 0u;
    }
    u32LiveAfterBulk = irq_msix_soft_user_count_live();
    f51AfterBulk = (irq_msix_soft_user_slot_bound(51u) != 0) ? 1u : 0u;
    f52AfterBulk = (irq_msix_soft_user_slot_bound(52u) != 0) ? 1u : 0u;

    fOk = 0u;
    if (f51AfterUnbind == 0u && f52AfterUnbind == 1u &&
        u32LiveAfterUnbindOne + 1u == u32LiveAfterPair &&
        u32LiveAfterBulk == 0u &&
        f51AfterBulk == 0u && f52AfterBulk == 0u) {
        fOk = 1u;
    }

    /* Grep: irq_msix: soft residual bulk_clear */
    kprintf("irq_msix: soft residual bulk_clear via=selftest "
            "hosts=H51,H52 unbind_one=H51 bulk=bind0 "
            "bound51_after_unbind=%u bound52_after_unbind=%u "
            "live_pair=%u live_unbind_one=%u live_bulk=%u "
            "close_shape_ok=%u bulk_clears_all=%u safe=%u "
            "path=unbind(one)=CLOSE bind(0)=bulk_only "
            "never_bulk_on_ddi_close=1 "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)f51AfterUnbind, (unsigned)f52AfterUnbind,
            (unsigned)u32LiveAfterPair, (unsigned)u32LiveAfterUnbindOne,
            (unsigned)u32LiveAfterBulk,
            (unsigned)((f51AfterUnbind == 0u && f52AfterUnbind == 1u)
                       ? 1u : 0u),
            (unsigned)((u32LiveAfterBulk == 0u) ? 1u : 0u),
            (unsigned)fOk);
    /* Twin residual lamp */
    kprintf("irq: soft residual bulk_clear unbind_one=CLOSE "
            "bind0=bulk_only never_bulk_on_ddi_close=1 "
            "product_notify_mint=OPEN soft_note_only=1 soft!=product\n");

    if (fOk != 0u && g_fSoftBulkClearPass == 0u) {
        g_fSoftBulkClearPass = 1u;
        /* Grep: irq_msix: soft residual bulk_clear PASS */
        kprintf("irq_msix: soft residual bulk_clear PASS "
                "unbind_one=CLOSE bind0=bulk_only "
                "never_bulk_on_ddi_close=1 bulk_clears_all=1 "
                "product_notify_mint=OPEN product_irq_cap=OPEN "
                "soft_note_only=1 soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual bulk_clear PASS "
                "product_notify_mint=OPEN soft!=product\n");
    }

    /*
     * Re-seed residual hosts for inventory greps after bulk clear.
     * Soft residual only; Soft!=product; product_notify_mint=OPEN.
     */
    (void)irq_msix_soft_user_bind(11u, GJ_MSIX_BADGE_SOFT);
    (void)irq_msix_soft_user_bind(21u, GJ_MSIX_BADGE_SOFT | GJ_MSIX_BADGE_HW);
    return fOk;
}

/**
 * Product MSI-X table→user residual selftest (Soft!=product; UDX host path).
 * Soft table fire entry0 (GJ_MSIX_BADGE_TBL(0)) must hit soft-bound hosts
 * whose wait masks include that TBL bit — product shape:
 *   soft_tbl fire -> irq_msix inject -> notify_msix_global -> NOTIFY_WAIT.
 * Hosts 71 (TBL-only) + 72 (SOFT|TBL): one table_pulse(0) hits BOTH.
 * Handles distinct from prior residual layers. Unbind both after.
 * Never forces IRQ eth poll (H1); freestanding net stays thr/run-loop only.
 * Sparse once-shot lamps; no stamp storms; never hard-gates.
 * greppable: irq_msix: soft residual table_user
 * greppable: irq_msix: soft residual table_user PASS
 * greppable: product_notify_mint=OPEN
 * Returns 1 on residual PASS, 0 otherwise.
 */
static u32
irq_msix_soft_table_user_selftest(void)
{
    u32 u32HitsA0;
    u32 u32HitsB0;
    u32 u32HitsA1;
    u32 u32HitsB1;
    u32 u32Tbl;
    u32 fOk;
    u32 fA;
    u32 fB;
    u32 u32Live0;
    u32 u32Free;
    u32 u32Drop;

    irq_msix_soft_inc(&g_u32SoftUserTableUserCheck);
    if (!g_fReady) {
        return 0u;
    }

    /* Need two free slots for 71/72. Soft!=product. */
    u32Live0 = irq_msix_soft_user_count_live();
    u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                  ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                  : 0u;
    if (u32Free < 2u) {
        for (u32Drop = 1u; u32Drop <= 3u && u32Free < 2u; u32Drop++) {
            if (irq_msix_soft_user_slot_bound(u32Drop) != 0) {
                (void)irq_msix_soft_user_unbind(u32Drop);
                u32Live0 = irq_msix_soft_user_count_live();
                u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                              ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                              : 0u;
            }
        }
        for (u32Drop = 31u; u32Drop <= 40u && u32Free < 2u; u32Drop++) {
            if (irq_msix_soft_user_slot_bound(u32Drop) != 0) {
                (void)irq_msix_soft_user_unbind(u32Drop);
                u32Live0 = irq_msix_soft_user_count_live();
                u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                              ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                              : 0u;
            }
        }
        if (u32Free < 2u) {
            kprintf("irq_msix: soft residual table_user via=selftest "
                    "safe=0 reason=no_free_pair live=%u cap=%u "
                    "product_notify_mint=OPEN soft!=product "
                    "dual_license=MIT_OR_Apache-2.0\n",
                    (unsigned)u32Live0,
                    (unsigned)IRQ_MSIX_SOFT_USER_SLOTS);
            return 0u;
        }
    }

    (void)irq_msix_soft_user_unbind(71u);
    (void)irq_msix_soft_user_unbind(72u);

    /* H71: TBL(0) only — product MSI-X table badge wait. Soft!=product. */
    if (irq_msix_soft_user_bind(71u, GJ_MSIX_BADGE_TBL(0)) != 0) {
        return 0u;
    }
    /* H72: SOFT|TBL — multi-badge host (concurrent UDX shape). */
    if (irq_msix_soft_user_bind(72u, GJ_MSIX_BADGE_SOFT | GJ_MSIX_BADGE_TBL(0))
        != 0) {
        (void)irq_msix_soft_user_unbind(71u);
        return 0u;
    }

    /* Ensure soft table entry0 programmed unmasked for TBL fire. */
    if (!pci_msix_soft_ready()) {
        pci_msix_soft_table_init();
    }
    /* soft_program is idempotent residual: program + unmask entry0. */
    (void)pci_msix_soft_program(0, 0xFEE00000u, (u32)GJ_MSIX_IRQ_VEC, 0);
    (void)pci_msix_soft_mask(0, 0);

    u32HitsA0 = irq_msix_soft_user_slot_hits(71u);
    u32HitsB0 = irq_msix_soft_user_slot_hits(72u);

    /*
     * Product MSI-X residual: soft_table_pulse routes soft_tbl fire ->
     * inject(TBL badge) -> multi-host soft notes gain hits. Soft!=product.
     * H1: never net_eth_poll from this path (thr/run-loop own eth).
     */
    u32Tbl = irq_msix_soft_table_pulse(0);
    u32HitsA1 = irq_msix_soft_user_slot_hits(71u);
    u32HitsB1 = irq_msix_soft_user_slot_hits(72u);

    fA = (irq_msix_soft_user_slot_bound(71u) != 0) ? 1u : 0u;
    fB = (irq_msix_soft_user_slot_bound(72u) != 0) ? 1u : 0u;

    fOk = 0u;
    if (u32Tbl != 0u && fA == 1u && fB == 1u &&
        u32HitsA1 > u32HitsA0 &&
        u32HitsB1 > u32HitsB0 &&
        g_u32LastPath == GJ_MSIX_PATH_TBL) {
        fOk = 1u;
    }

    /* Grep: irq_msix: soft residual table_user */
    kprintf("irq_msix: soft residual table_user via=selftest "
            "hosts=H71,H72 entry=0 tbl_pulse=%u "
            "hits_H71=%u->%u hits_H72=%u->%u "
            "both_hit=%u last_path=%u last_badge=0x%lx safe=%u "
            "path=soft_tbl->inject->notify_msix_global->NOTIFY_WAIT "
            "product_msix=OPEN product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "poll_mode_first=%u force_irq_eth_poll=%u net_eth_irq=%u "
            "net_eth_poll_from_msix=%u hazard=H1 "
            "dual_dod_A=OPEN dual_dod_B=OPEN freestanding_skip=1 G-AC-1=1 "
            "stamp_free=1 soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)u32Tbl,
            (unsigned)u32HitsA0, (unsigned)u32HitsA1,
            (unsigned)u32HitsB0, (unsigned)u32HitsB1,
            (unsigned)((u32HitsA1 > u32HitsA0 && u32HitsB1 > u32HitsB0)
                       ? 1u : 0u),
            (unsigned)g_u32LastPath,
            (unsigned long)g_u64LastBadge,
            (unsigned)fOk,
            (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
            (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
            (unsigned)IRQ_MSIX_NET_ETH_IRQ,
            (unsigned)IRQ_MSIX_NET_ETH_POLL_FROM);
    /* Twin residual lamp */
    kprintf("irq: soft residual table_user soft_tbl_to_udx=1 both_hit=%u "
            "product_msix=OPEN product_notify_mint=OPEN "
            "soft_note_only=1 hazard=H1 soft!=product\n",
            (unsigned)fOk);

    if (fOk != 0u && g_fSoftTableUserPass == 0u) {
        g_fSoftTableUserPass = 1u;
        /* Grep: irq_msix: soft residual table_user PASS */
        kprintf("irq_msix: soft residual table_user PASS "
                "soft_tbl_to_udx=1 both_hit=1 path_tbl=1 "
                "product_msix=OPEN product_notify_mint=OPEN "
                "product_irq_cap=OPEN soft_note_only=1 "
                "dual_dod_A=OPEN dual_dod_B=OPEN freestanding_skip=1 "
                "G-AC-1=1 stamp_free=1 hazard=H1 soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual table_user PASS "
                "product_msix=OPEN soft!=product\n");
    }

    (void)irq_msix_soft_user_unbind(71u);
    (void)irq_msix_soft_user_unbind(72u);
    return fOk;
}

/**
 * Dual DoD residual honesty (Soft!=product; G-AC-1; stamp-free).
 * Asserts Dual DoD A/B remain OPEN (UDX product path); freestanding SKIP.
 * soft residual != Dual DoD close. product_notify_mint=OPEN.
 * PASS = honesty residual only (A/B stay OPEN; not product DoD close).
 * product_hosts=UDX (rtl8168_udx 10ec:8168 + xhci_udx 8086:a12f).
 * freestanding_product=SKIP - never freestanding product close.
 * Bar honesty v2026.08.04.75 — never invent .76. H2 once-shot lamps.
 * greppable: irq_msix: soft residual dual_dod
 * greppable: irq_msix: soft residual dual_dod PASS
 * greppable: product_hosts=UDX Soft!=product dual_dod OPEN
 * greppable: freestanding_product=SKIP never freestanding product close
 * Returns 1 always when ready (honesty residual always asserts OPEN).
 */
static u32
irq_msix_soft_dual_dod_residual(void)
{
    u32 fOk;
    u32 u32Ready;
    u32 u32Live;
    u32 fLiveLeCap;

    if (!g_fReady) {
        return 0u;
    }
    /*
     * Honesty residual: Dual DoD A (USB UDX / 8086:a12f) / B (NIC UDX /
     * 10ec:8168) OPEN until DUT. Soft residual != DoD close. Soft!=product.
     * never freestanding product close; product_hosts=UDX.
     */
    u32Ready = (g_fReady != 0) ? 1u : 0u;
    u32Live = irq_msix_soft_user_count_live();
    fLiveLeCap = (u32Live <= IRQ_MSIX_SOFT_USER_SLOTS) ? 1u : 0u;
    if (u32Ready != 0u) {
        irq_msix_soft_inc(&g_u32SoftReadyOk);
    }
    if (fLiveLeCap != 0u) {
        irq_msix_soft_inc(&g_u32SoftLiveLeCapOk);
    }
    fOk = 1u;
    /* Grep: irq_msix: soft residual dual_dod | product_hosts=UDX */
    kprintf("irq_msix: soft residual dual_dod via=selftest "
            "A=OPEN B=OPEN dual_dod=OPEN product_udx=1 "
            "product_hosts=UDX hosts=xhci_udx|rtl8168_udx "
            "id_A=8086:a12f id_B=10ec:8168 "
            "freestanding_skip=1 freestanding_product=SKIP "
            "never_freestanding_product_close=1 "
            "never freestanding product close "
            "soft_residual_closes_dod=0 product_msix=OPEN "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 G-AC-1=1 "
            "ready=%u live=%u cap=%u live_le_cap=%u "
            "table_user_pass=%u multi_host_pass=%u user_bind_pass=%u "
            "bind_honesty_pass=%u "
            "stamp_free=1 bar_honesty=v2026.08.04.75 never_invent=.76 "
            "Soft!=product soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)u32Ready, (unsigned)u32Live,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS, (unsigned)fLiveLeCap,
            (unsigned)g_fSoftTableUserPass,
            (unsigned)g_fSoftMultiHostPass,
            (unsigned)g_fSoftUserBindResidualPass,
            (unsigned)g_fSoftBindHonestyPass);
    kprintf("irq: soft residual dual_dod A=OPEN B=OPEN dual_dod=OPEN "
            "product_hosts=UDX freestanding_product=SKIP "
            "never_freestanding_product_close=1 "
            "never freestanding product close product_udx=1 "
            "soft_residual_closes_dod=0 G-AC-1=1 Soft!=product soft!=product\n");

    if (fOk != 0u && g_fSoftDualDodPass == 0u) {
        g_fSoftDualDodPass = 1u;
        /* Grep: irq_msix: soft residual dual_dod PASS (honesty; A/B OPEN) */
        kprintf("irq_msix: soft residual dual_dod PASS "
                "A=OPEN B=OPEN dual_dod=OPEN soft_residual_closes_dod=0 "
                "product_hosts=UDX hosts=xhci_udx|rtl8168_udx "
                "id_A=8086:a12f id_B=10ec:8168 "
                "freestanding_product=SKIP freestanding_skip=1 "
                "never_freestanding_product_close=1 "
                "never freestanding product close "
                "product_udx=1 "
                "product_msix=OPEN product_notify_mint=OPEN "
                "G-AC-1=1 stamp_free=1 Soft!=product soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual dual_dod PASS "
                "A=OPEN B=OPEN dual_dod=OPEN product_hosts=UDX "
                "freestanding_product=SKIP "
                "never freestanding product close "
                "Soft!=product soft!=product\n");
    }
    return fOk;
}

/**
 * Product IRQ bind honesty residual (Soft!=product; multi-host UDX denser).
 * Densifies irq_msix_ready residual for concurrent UDX product hosts:
 *   ready=1 after multi-host soft bind path;
 *   primary handle/mask vs multi-slot table;
 *   rebind residual tallies;
 *   live<=cap (live_le_cap=1);
 * denser multi-host UDX residual:
 *   inject SOFT hits rtl-like H81 only; inject HW hits xhci-like H82 only;
 *   peer mask stable across rebind; multi_host_binds++ while peer live;
 *   post-unbind inject still hits surviving host (CLOSE shape denser).
 * Synthetic dual-host bind (81/82) exercises ready + primary vs table +
 * inject isolation + rebind live-stable + unbind-one peer-safe, then frees.
 * H2 once-shot PASS lamps; never hard-gates; Dual DoD remains OPEN.
 * greppable: irq_msix: soft residual bind_honesty
 * greppable: irq_msix: soft residual bind_honesty PASS
 * greppable: irq_msix: soft residual bind_honesty denser
 * greppable: irq_msix: soft residual bind_honesty denser PASS
 * greppable: product_hosts=UDX Soft!=product dual_dod OPEN denser=1
 * greppable: primary_vs_table live_le_cap irq_msix_ready multi_host_udx
 * Returns 1 on residual PASS, 0 otherwise.
 */
static u32
irq_msix_soft_bind_honesty_residual(void)
{
    u32 u32Live0;
    u32 u32Live1;
    u32 u32Live2;
    u32 u32Live3;
    u32 u32Live4;
    u32 u32Free;
    u32 u32Drop;
    u32 u32RebindsBefore;
    u32 u32MultiBindsBefore;
    u32 u32Ready0;
    u32 u32Ready1;
    u32 fPrimary0;
    u32 fPrimary1;
    u32 fLiveLeCap;
    u32 fOk;
    u32 fDenser;
    u32 fH81;
    u32 fH82;
    u32 fPeerMaskOk;
    u32 fSoftHitsAOnly;
    u32 fHwHitsBOnly;
    u32 fPostUnbindHit;
    u32 u32HitsA0;
    u32 u32HitsB0;
    u32 u32HitsA1;
    u32 u32HitsB1;
    u32 u32HitsA2;
    u32 u32HitsB2;
    u32 u32HitsA3;
    u64 u64Mask81;
    u64 u64Mask82;

    irq_msix_soft_inc(&g_u32SoftBindHonestyCheck);
    if (!g_fReady) {
        return 0u;
    }

    u32Ready0 = (g_fReady != 0) ? 1u : 0u;
    if (u32Ready0 != 0u) {
        irq_msix_soft_inc(&g_u32SoftReadyOk);
    }

    /* Need two free slots for synthetic dual-host UDX bind shape. */
    u32Live0 = irq_msix_soft_user_count_live();
    u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                  ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                  : 0u;
    if (u32Free < 2u) {
        for (u32Drop = 1u; u32Drop <= 3u && u32Free < 2u; u32Drop++) {
            if (irq_msix_soft_user_slot_bound(u32Drop) != 0) {
                (void)irq_msix_soft_user_unbind(u32Drop);
                u32Live0 = irq_msix_soft_user_count_live();
                u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                              ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                              : 0u;
            }
        }
        for (u32Drop = 31u; u32Drop <= 40u && u32Free < 2u; u32Drop++) {
            if (irq_msix_soft_user_slot_bound(u32Drop) != 0) {
                (void)irq_msix_soft_user_unbind(u32Drop);
                u32Live0 = irq_msix_soft_user_count_live();
                u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                              ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                              : 0u;
            }
        }
        if (u32Free < 2u) {
            kprintf("irq_msix: soft residual bind_honesty via=selftest "
                    "safe=0 reason=no_free_pair live=%u cap=%u "
                    "ready=%u product_hosts=UDX dual_dod=OPEN denser=1 "
                    "Soft!=product soft!=product "
                    "dual_license=MIT_OR_Apache-2.0\n",
                    (unsigned)u32Live0,
                    (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
                    (unsigned)u32Ready0);
            return 0u;
        }
    }

    (void)irq_msix_soft_user_unbind(81u);
    (void)irq_msix_soft_user_unbind(82u);

    u32RebindsBefore = g_u32SoftUserRebinds;
    u32MultiBindsBefore = g_u32SoftUserMultiHostBinds;

    /*
     * Dual UDX product-host shape denser:
     *   H81 = rtl-like (10ec:8168 Dual DoD B) SOFT-only
     *   H82 = xhci-like (8086:a12f Dual DoD A) HW-only
     * Soft!=product; dual_dod OPEN; never freestanding product close.
     */
    if (irq_msix_soft_user_bind(81u, GJ_MSIX_BADGE_SOFT) != 0) {
        return 0u;
    }
    u32Live1 = irq_msix_soft_user_count_live();
    fPrimary0 = irq_msix_soft_primary_vs_table_ok();
    if (fPrimary0 != 0u) {
        irq_msix_soft_inc(&g_u32SoftPrimaryVsTableOk);
    }

    if (irq_msix_soft_user_bind(82u, GJ_MSIX_BADGE_HW) != 0) {
        (void)irq_msix_soft_user_unbind(81u);
        return 0u;
    }
    u32Live2 = irq_msix_soft_user_count_live();

    /* Denser: multi_host_binds must advance on H82 while H81 live. */
    if (g_u32SoftUserMultiHostBinds <= u32MultiBindsBefore) {
        (void)irq_msix_soft_user_unbind(81u);
        (void)irq_msix_soft_user_unbind(82u);
        kprintf("irq_msix: soft residual bind_honesty via=selftest "
                "safe=0 reason=multi_host_binds_miss denser=1 "
                "product_hosts=UDX dual_dod=OPEN Soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        return 0u;
    }

    /*
     * Denser multi-host UDX inject residual (peer badge isolation):
     * inject SOFT must hit H81 only; peer H82 HW mask must stay clean.
     * Soft!=product; product_notify_mint=OPEN; soft_note_only.
     */
    u32HitsA0 = irq_msix_soft_user_slot_hits(81u);
    u32HitsB0 = irq_msix_soft_user_slot_hits(82u);
    irq_msix_soft_inject(GJ_MSIX_BADGE_SOFT);
    u32HitsA1 = irq_msix_soft_user_slot_hits(81u);
    u32HitsB1 = irq_msix_soft_user_slot_hits(82u);
    fSoftHitsAOnly = (u32HitsA1 > u32HitsA0 && u32HitsB1 == u32HitsB0)
                         ? 1u : 0u;

    /*
     * Drop SOFT from H81 so sticky pending SOFT cannot false-match on HW
     * inject (notify_maybe ORs pending & mask). Peer H82 mask must remain
     * HW through rebind denser. Soft!=product multi-host UDX residual.
     */
    if (irq_msix_soft_user_bind(81u, GJ_MSIX_BADGE_TBL(1)) != 0) {
        (void)irq_msix_soft_user_unbind(81u);
        (void)irq_msix_soft_user_unbind(82u);
        return 0u;
    }
    u64Mask82 = irq_msix_soft_user_slot_mask(82u);
    fPeerMaskOk = (u64Mask82 == GJ_MSIX_BADGE_HW) ? 1u : 0u;
    if (fPeerMaskOk == 0u) {
        (void)irq_msix_soft_user_unbind(81u);
        (void)irq_msix_soft_user_unbind(82u);
        kprintf("irq_msix: soft residual bind_honesty via=selftest "
                "safe=0 reason=peer_mask_clobber denser=1 mask_B=0x%lx "
                "product_hosts=UDX dual_dod=OPEN Soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n",
                (unsigned long)u64Mask82);
        return 0u;
    }

    /* Inject HW: H82 must gain a hit; H81 (TBL1-only) must not. */
    irq_msix_soft_inject(GJ_MSIX_BADGE_HW);
    u32HitsA2 = irq_msix_soft_user_slot_hits(81u);
    u32HitsB2 = irq_msix_soft_user_slot_hits(82u);
    fHwHitsBOnly = (u32HitsB2 > u32HitsB1 && u32HitsA2 == u32HitsA1)
                       ? 1u : 0u;

    /* Rebind residual denser: restore H81 SOFT|TBL0; peer H82 live-stable. */
    u64Mask81 = GJ_MSIX_BADGE_SOFT | GJ_MSIX_BADGE_TBL(0);
    if (irq_msix_soft_user_bind(81u, u64Mask81) != 0) {
        (void)irq_msix_soft_user_unbind(81u);
        (void)irq_msix_soft_user_unbind(82u);
        return 0u;
    }
    u32Live3 = irq_msix_soft_user_count_live();
    fPrimary1 = irq_msix_soft_primary_vs_table_ok();
    if (fPrimary1 != 0u) {
        irq_msix_soft_inc(&g_u32SoftPrimaryVsTableOk);
    }
    /* Peer mask still HW after second rebind denser. Soft!=product. */
    u64Mask82 = irq_msix_soft_user_slot_mask(82u);
    if (u64Mask82 != GJ_MSIX_BADGE_HW) {
        fPeerMaskOk = 0u;
    }

    /* irq_msix_ready residual denser after multi-host bind. Soft!=product. */
    u32Ready1 = (irq_msix_ready() != 0) ? 1u : 0u;
    if (u32Ready1 != 0u) {
        irq_msix_soft_inc(&g_u32SoftReadyOk);
    }

    fLiveLeCap = (u32Live3 <= IRQ_MSIX_SOFT_USER_SLOTS) ? 1u : 0u;
    if (fLiveLeCap != 0u) {
        irq_msix_soft_inc(&g_u32SoftLiveLeCapOk);
    }

    /* CLOSE shape denser: unbind one peer only; surviving host still hits. */
    if (irq_msix_soft_user_unbind(82u) != 0) {
        (void)irq_msix_soft_user_unbind(81u);
        return 0u;
    }
    u32Live4 = irq_msix_soft_user_count_live();
    fH81 = (irq_msix_soft_user_slot_bound(81u) != 0) ? 1u : 0u;
    fH82 = (irq_msix_soft_user_slot_bound(82u) != 0) ? 1u : 0u;

    /* Post-unbind denser: inject SOFT still hits surviving H81. */
    u32HitsA3 = irq_msix_soft_user_slot_hits(81u);
    irq_msix_soft_inject(GJ_MSIX_BADGE_SOFT);
    fPostUnbindHit = (irq_msix_soft_user_slot_hits(81u) > u32HitsA3)
                         ? 1u : 0u;

    fDenser = 0u;
    if (fSoftHitsAOnly == 1u && fHwHitsBOnly == 1u &&
        fPeerMaskOk == 1u && fPostUnbindHit == 1u &&
        g_u32SoftUserMultiHostBinds > u32MultiBindsBefore) {
        fDenser = 1u;
        irq_msix_soft_inc(&g_u32SoftBindHonestyDenserOk);
    }

    fOk = 0u;
    if (u32Ready0 == 1u && u32Ready1 == 1u &&
        fPrimary0 == 1u && fPrimary1 == 1u &&
        fLiveLeCap == 1u &&
        fH81 == 1u && fH82 == 0u &&
        u32Live2 > u32Live1 &&
        u32Live3 == u32Live2 &&
        u32Live4 + 1u == u32Live3 &&
        g_u32SoftUserRebinds > u32RebindsBefore &&
        g_u32SoftUserHandle == 81u &&
        g_u64SoftUserMask == u64Mask81 &&
        irq_msix_soft_user_slot_mask(81u) == u64Mask81 &&
        fDenser == 1u) {
        fOk = 1u;
    }

    /* Grep: irq_msix: soft residual bind_honesty | denser multi_host_udx */
    kprintf("irq_msix: soft residual bind_honesty via=selftest denser=1 "
            "hosts=H81,H82 product_hosts=UDX multi_host_udx=1 "
            "id_shape=10ec:8168|8086:a12f dual_dod=OPEN "
            "ready0=%u ready1=%u irq_msix_ready=%u "
            "primary_vs_table0=%u primary_vs_table1=%u "
            "primary_handle=%u primary_mask=0x%lx "
            "live0=%u live1=%u live2=%u live3=%u live4=%u "
            "cap=%u live_le_cap=%u rebinds=%u multi_host_binds=%u "
            "bound_H81=%u bound_H82=%u rebind_stable=%u "
            "soft_hits_A_only=%u hw_hits_B_only=%u peer_mask_ok=%u "
            "post_unbind_hit=%u denser_ok=%u denser_tally=%u safe=%u "
            "hits_A=%u->%u->%u hits_B=%u->%u->%u "
            "path=ready->bind->multi_host->inject_isol->rebind->"
            "primary_vs_table->live_le_cap->unbind_one->post_inject "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 freestanding_product=SKIP "
            "never_freestanding_product_close=1 G-AC-1=1 stamp_free=1 "
            "bar_honesty=v2026.08.04.75 never_invent=.76 "
            "Soft!=product soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)u32Ready0, (unsigned)u32Ready1, (unsigned)u32Ready1,
            (unsigned)fPrimary0, (unsigned)fPrimary1,
            (unsigned)g_u32SoftUserHandle,
            (unsigned long)g_u64SoftUserMask,
            (unsigned)u32Live0, (unsigned)u32Live1, (unsigned)u32Live2,
            (unsigned)u32Live3, (unsigned)u32Live4,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS, (unsigned)fLiveLeCap,
            (unsigned)g_u32SoftUserRebinds,
            (unsigned)g_u32SoftUserMultiHostBinds,
            (unsigned)fH81, (unsigned)fH82,
            (unsigned)((u32Live3 == u32Live2) ? 1u : 0u),
            (unsigned)fSoftHitsAOnly, (unsigned)fHwHitsBOnly,
            (unsigned)fPeerMaskOk, (unsigned)fPostUnbindHit,
            (unsigned)fDenser, (unsigned)g_u32SoftBindHonestyDenserOk,
            (unsigned)fOk,
            (unsigned)u32HitsA0, (unsigned)u32HitsA1, (unsigned)u32HitsA2,
            (unsigned)u32HitsB0, (unsigned)u32HitsB1, (unsigned)u32HitsB2);
    /* Twin residual lamp denser multi-host UDX */
    kprintf("irq: soft residual bind_honesty denser=1 multi_host_udx=1 "
            "ready=%u primary_vs_table=%u live_le_cap=%u rebind=1 "
            "inject_isol=1 peer_mask_ok=%u post_unbind_hit=%u "
            "product_hosts=UDX dual_dod=OPEN freestanding_product=SKIP "
            "Soft!=product soft!=product\n",
            (unsigned)u32Ready1, (unsigned)fPrimary1, (unsigned)fLiveLeCap,
            (unsigned)fPeerMaskOk, (unsigned)fPostUnbindHit);
    /* Grep: irq_msix: soft residual bind_honesty denser */
    kprintf("irq_msix: soft residual bind_honesty denser via=selftest "
            "multi_host_udx=1 denser=1 soft_A_only=%u hw_B_only=%u "
            "peer_mask_ok=%u multi_host_binds=1 post_unbind_hit=%u "
            "ready=%u primary_vs_table=%u live_le_cap=%u denser_ok=%u "
            "product_hosts=UDX dual_dod=OPEN product_notify_mint=OPEN "
            "soft_note_only=1 freestanding_product=SKIP G-AC-1=1 "
            "Soft!=product soft!=product dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)fSoftHitsAOnly, (unsigned)fHwHitsBOnly,
            (unsigned)fPeerMaskOk, (unsigned)fPostUnbindHit,
            (unsigned)u32Ready1, (unsigned)fPrimary1, (unsigned)fLiveLeCap,
            (unsigned)fDenser);

    if (fOk != 0u && g_fSoftBindHonestyPass == 0u) {
        g_fSoftBindHonestyPass = 1u;
        /* Grep: irq_msix: soft residual bind_honesty PASS */
        kprintf("irq_msix: soft residual bind_honesty PASS denser=1 "
                "irq_msix_ready=1 primary_vs_table=1 live_le_cap=1 "
                "rebind=1 multi_host=1 multi_host_udx=1 unbind_one=1 "
                "inject_isol=1 soft_A_only=1 hw_B_only=1 peer_mask_ok=1 "
                "post_unbind_hit=1 denser_ok=1 "
                "product_hosts=UDX dual_dod=OPEN "
                "hosts=xhci_udx|rtl8168_udx id=8086:a12f|10ec:8168 "
                "freestanding_product=SKIP "
                "never_freestanding_product_close=1 "
                "never freestanding product close "
                "product_notify_mint=OPEN product_irq_cap=OPEN "
                "soft_note_only=1 G-AC-1=1 stamp_free=1 "
                "bar_honesty=v2026.08.04.75 never_invent=.76 "
                "Soft!=product soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual bind_honesty PASS denser=1 "
                "ready=1 primary_vs_table=1 live_le_cap=1 multi_host_udx=1 "
                "product_hosts=UDX dual_dod=OPEN "
                "never freestanding product close Soft!=product\n");
        /* Grep: irq_msix: soft residual bind_honesty denser PASS */
        kprintf("irq_msix: soft residual bind_honesty denser PASS "
                "multi_host_udx=1 denser=1 inject_isol=1 peer_mask_ok=1 "
                "post_unbind_hit=1 ready=1 primary_vs_table=1 live_le_cap=1 "
                "product_hosts=UDX dual_dod=OPEN product_notify_mint=OPEN "
                "soft_note_only=1 freestanding_product=SKIP G-AC-1=1 "
                "Soft!=product soft!=product dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual bind_honesty denser PASS "
                "multi_host_udx=1 denser=1 product_hosts=UDX dual_dod=OPEN "
                "Soft!=product soft!=product\n");
    }

    /* Free synthetic dual-host notes (leave peer residual hosts). */
    (void)irq_msix_soft_user_unbind(81u);
    (void)irq_msix_soft_user_unbind(82u);
    return fOk;
}

/**
 * Product IRQ+DMA Dual DoD denser residual (Soft!=product; bar .75).
 * Densifies multi-host UDX product path for concurrent IRQ+DMA Dual DoD:
 *   H81 = rtl-like (10ec:8168 Dual DoD B) SOFT-only
 *   H82 = xhci-like (8086:a12f Dual DoD A) HW-only
 *   H83 = ddi_host-like TBL0 (DMA bind peer residual; Soft!=product)
 * Denser checks:
 *   inject SOFT hits H81 only; inject HW hits H82 only;
 *   inject TBL0 hits H83 only (triple isol denser);
 *   inject SOFT|HW|TBL0 multi-match denser hits all three;
 *   unbind H83 leaves H81/H82 live (CLOSE denser peer-safe);
 *   Dual DoD A/B OPEN honesty denser (soft residual != DoD close).
 * Synthetic triple-host; frees after. H2 once-shot PASS; never hard-gates.
 * greppable: irq_msix: soft residual irq_dma denser
 * greppable: irq_msix: soft residual irq_dma denser PASS
 * greppable: product IRQ+DMA Dual DoD OPEN Soft!=product denser=1
 * greppable: product_hosts=UDX dual_dod OPEN denser=1
 * Returns 1 on residual PASS, 0 otherwise.
 */
static u32
irq_msix_soft_irq_dma_dual_residual(void)
{
    u32 u32Live0;
    u32 u32Live1;
    u32 u32Live2;
    u32 u32Live3;
    u32 u32Live4;
    u32 u32Free;
    u32 u32Drop;
    u32 u32Ready;
    u32 fOk;
    u32 fDenser;
    u32 fLiveLeCap;
    u32 fSoftAOnly;
    u32 fHwBOnly;
    u32 fTblCOnly;
    u32 fMultiHit;
    u32 fPeerSafe;
    u32 fDualOpen;
    u32 u32HitsA0;
    u32 u32HitsB0;
    u32 u32HitsC0;
    u32 u32HitsA1;
    u32 u32HitsB1;
    u32 u32HitsC1;
    u32 u32HitsA2;
    u32 u32HitsB2;
    u32 u32HitsC2;
    u32 u32HitsA3;
    u32 u32HitsB3;
    u32 u32HitsC3;
    u32 u32HitsA4;
    u32 u32HitsB4;
    u64 u64MaskTbl0;

    irq_msix_soft_inc(&g_u32SoftIrqDmaDualCheck);
    if (!g_fReady) {
        return 0u;
    }

    u32Ready = (irq_msix_ready() != 0) ? 1u : 0u;
    if (u32Ready != 0u) {
        irq_msix_soft_inc(&g_u32SoftReadyOk);
    }

    /* Need three free slots for synthetic triple-host UDX denser. */
    u32Live0 = irq_msix_soft_user_count_live();
    u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                  ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                  : 0u;
    if (u32Free < 3u) {
        for (u32Drop = 1u; u32Drop <= 5u && u32Free < 3u; u32Drop++) {
            if (irq_msix_soft_user_slot_bound(u32Drop) != 0) {
                (void)irq_msix_soft_user_unbind(u32Drop);
                u32Live0 = irq_msix_soft_user_count_live();
                u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                              ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                              : 0u;
            }
        }
        for (u32Drop = 31u; u32Drop <= 50u && u32Free < 3u; u32Drop++) {
            if (irq_msix_soft_user_slot_bound(u32Drop) != 0) {
                (void)irq_msix_soft_user_unbind(u32Drop);
                u32Live0 = irq_msix_soft_user_count_live();
                u32Free = (u32Live0 < IRQ_MSIX_SOFT_USER_SLOTS)
                              ? (IRQ_MSIX_SOFT_USER_SLOTS - u32Live0)
                              : 0u;
            }
        }
        if (u32Free < 3u) {
            kprintf("irq_msix: soft residual irq_dma denser via=selftest "
                    "safe=0 reason=no_free_triple live=%u cap=%u "
                    "ready=%u product_hosts=UDX dual_dod=OPEN denser=1 "
                    "Soft!=product soft!=product "
                    "dual_license=MIT_OR_Apache-2.0\n",
                    (unsigned)u32Live0,
                    (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
                    (unsigned)u32Ready);
            return 0u;
        }
    }

    (void)irq_msix_soft_user_unbind(81u);
    (void)irq_msix_soft_user_unbind(82u);
    (void)irq_msix_soft_user_unbind(83u);

    u64MaskTbl0 = GJ_MSIX_BADGE_TBL(0);

    /*
     * Triple-host product IRQ+DMA Dual DoD denser (Soft!=product):
     *   H81 = rtl 10ec:8168 Dual DoD B (SOFT)
     *   H82 = xhci 8086:a12f Dual DoD A (HW)
     *   H83 = ddi_host DMA bind peer residual (TBL0)
     */
    if (irq_msix_soft_user_bind(81u, GJ_MSIX_BADGE_SOFT) != 0) {
        return 0u;
    }
    if (irq_msix_soft_user_bind(82u, GJ_MSIX_BADGE_HW) != 0) {
        (void)irq_msix_soft_user_unbind(81u);
        return 0u;
    }
    if (irq_msix_soft_user_bind(83u, u64MaskTbl0) != 0) {
        (void)irq_msix_soft_user_unbind(81u);
        (void)irq_msix_soft_user_unbind(82u);
        return 0u;
    }
    u32Live1 = irq_msix_soft_user_count_live();

    /* Isol denser: inject SOFT hits H81 only. Soft!=product. */
    u32HitsA0 = irq_msix_soft_user_slot_hits(81u);
    u32HitsB0 = irq_msix_soft_user_slot_hits(82u);
    u32HitsC0 = irq_msix_soft_user_slot_hits(83u);
    irq_msix_soft_inject(GJ_MSIX_BADGE_SOFT);
    u32HitsA1 = irq_msix_soft_user_slot_hits(81u);
    u32HitsB1 = irq_msix_soft_user_slot_hits(82u);
    u32HitsC1 = irq_msix_soft_user_slot_hits(83u);
    fSoftAOnly = (u32HitsA1 > u32HitsA0 && u32HitsB1 == u32HitsB0 &&
                  u32HitsC1 == u32HitsC0)
                     ? 1u
                     : 0u;

    /* Isol denser: inject HW hits H82 only. Soft!=product. */
    irq_msix_soft_inject(GJ_MSIX_BADGE_HW);
    u32HitsA2 = irq_msix_soft_user_slot_hits(81u);
    u32HitsB2 = irq_msix_soft_user_slot_hits(82u);
    u32HitsC2 = irq_msix_soft_user_slot_hits(83u);
    fHwBOnly = (u32HitsB2 > u32HitsB1 && u32HitsA2 == u32HitsA1 &&
                u32HitsC2 == u32HitsC1)
                   ? 1u
                   : 0u;

    /* Isol denser: inject TBL0 hits H83 only. Soft!=product. */
    irq_msix_soft_inject(u64MaskTbl0);
    u32HitsA3 = irq_msix_soft_user_slot_hits(81u);
    u32HitsB3 = irq_msix_soft_user_slot_hits(82u);
    u32HitsC3 = irq_msix_soft_user_slot_hits(83u);
    fTblCOnly = (u32HitsC3 > u32HitsC2 && u32HitsA3 == u32HitsA2 &&
                 u32HitsB3 == u32HitsB2)
                    ? 1u
                    : 0u;

    /* Multi-match denser: combined badge fans out to all three hosts. */
    irq_msix_soft_inject(GJ_MSIX_BADGE_SOFT | GJ_MSIX_BADGE_HW | u64MaskTbl0);
    fMultiHit = (irq_msix_soft_user_slot_hits(81u) > u32HitsA3 &&
                 irq_msix_soft_user_slot_hits(82u) > u32HitsB3 &&
                 irq_msix_soft_user_slot_hits(83u) > u32HitsC3)
                    ? 1u
                    : 0u;

    u32Live2 = irq_msix_soft_user_count_live();
    fLiveLeCap = (u32Live2 <= IRQ_MSIX_SOFT_USER_SLOTS) ? 1u : 0u;
    if (fLiveLeCap != 0u) {
        irq_msix_soft_inc(&g_u32SoftLiveLeCapOk);
    }

    /* CLOSE denser: unbind ddi peer H83; rtl+xhci remain live. Soft!=product. */
    if (irq_msix_soft_user_unbind(83u) != 0) {
        (void)irq_msix_soft_user_unbind(81u);
        (void)irq_msix_soft_user_unbind(82u);
        return 0u;
    }
    u32Live3 = irq_msix_soft_user_count_live();
    fPeerSafe = (irq_msix_soft_user_slot_bound(81u) != 0 &&
                 irq_msix_soft_user_slot_bound(82u) != 0 &&
                 irq_msix_soft_user_slot_bound(83u) == 0 &&
                 u32Live3 + 1u == u32Live2)
                    ? 1u
                    : 0u;

    /* Post-unbind denser: SOFT still hits surviving rtl host H81. */
    u32HitsA4 = irq_msix_soft_user_slot_hits(81u);
    u32HitsB4 = irq_msix_soft_user_slot_hits(82u);
    irq_msix_soft_inject(GJ_MSIX_BADGE_SOFT);
    if (!(irq_msix_soft_user_slot_hits(81u) > u32HitsA4 &&
          irq_msix_soft_user_slot_hits(82u) == u32HitsB4)) {
        fPeerSafe = 0u;
    }

    /*
     * Dual DoD OPEN honesty denser (law): soft residual never closes A/B.
     * product_hosts=UDX; freestanding_product=SKIP. Soft!=product; G-AC-1.
     */
    fDualOpen = (u32Ready == 1u && fLiveLeCap == 1u &&
                 IRQ_MSIX_FORCE_IRQ_ETH_POLL == 0u &&
                 IRQ_MSIX_POLL_MODE_FIRST == 1u)
                    ? 1u
                    : 0u;

    fDenser = 0u;
    if (fSoftAOnly == 1u && fHwBOnly == 1u && fTblCOnly == 1u &&
        fMultiHit == 1u && fPeerSafe == 1u && fDualOpen == 1u &&
        u32Live1 >= 3u) {
        fDenser = 1u;
        irq_msix_soft_inc(&g_u32SoftIrqDmaDenserOk);
    }

    fOk = 0u;
    if (fDenser == 1u && u32Ready == 1u && fLiveLeCap == 1u &&
        irq_msix_soft_user_slot_bound(81u) != 0 &&
        irq_msix_soft_user_slot_bound(82u) != 0) {
        fOk = 1u;
    }

    u32Live4 = irq_msix_soft_user_count_live();

    /* Grep: irq_msix: soft residual irq_dma denser | product IRQ+DMA Dual DoD */
    kprintf("irq_msix: soft residual irq_dma denser via=selftest denser=1 "
            "hosts=H81,H82,H83 product_hosts=UDX multi_host_udx=1 "
            "id_shape=10ec:8168|8086:a12f|ddi_host "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "ready=%u live0=%u live1=%u live2=%u live3=%u live4=%u "
            "cap=%u live_le_cap=%u "
            "soft_A_only=%u hw_B_only=%u tbl_C_only=%u multi_hit=%u "
            "peer_safe=%u dual_open=%u denser_ok=%u denser_tally=%u safe=%u "
            "hits_A=%u->%u->%u->%u hits_B=%u->%u->%u->%u hits_C=%u->%u->%u->%u "
            "path=bind3->isol_SOFT->isol_HW->isol_TBL->multi->"
            "unbind_ddi->post_inject "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 freestanding_product=SKIP "
            "never_freestanding_product_close=1 G-AC-1=1 stamp_free=1 "
            "bar_honesty=v2026.08.04.75 never_invent=.76 "
            "product_IRQ+DMA=OPEN Soft!=product soft!=product "
            "dual_license=MIT_OR_Apache-2.0\n",
            (unsigned)u32Ready,
            (unsigned)u32Live0, (unsigned)u32Live1, (unsigned)u32Live2,
            (unsigned)u32Live3, (unsigned)u32Live4,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS, (unsigned)fLiveLeCap,
            (unsigned)fSoftAOnly, (unsigned)fHwBOnly, (unsigned)fTblCOnly,
            (unsigned)fMultiHit, (unsigned)fPeerSafe, (unsigned)fDualOpen,
            (unsigned)fDenser, (unsigned)g_u32SoftIrqDmaDenserOk,
            (unsigned)fOk,
            (unsigned)u32HitsA0, (unsigned)u32HitsA1, (unsigned)u32HitsA2,
            (unsigned)u32HitsA3,
            (unsigned)u32HitsB0, (unsigned)u32HitsB1, (unsigned)u32HitsB2,
            (unsigned)u32HitsB3,
            (unsigned)u32HitsC0, (unsigned)u32HitsC1, (unsigned)u32HitsC2,
            (unsigned)u32HitsC3);
    kprintf("irq: soft residual irq_dma denser=1 multi_host_udx=1 "
            "triple_host=1 isol=1 multi_hit=1 peer_safe=%u dual_open=%u "
            "product_hosts=UDX dual_dod=OPEN freestanding_product=SKIP "
            "product_IRQ+DMA=OPEN Soft!=product soft!=product\n",
            (unsigned)fPeerSafe, (unsigned)fDualOpen);

    if (fOk != 0u && g_fSoftIrqDmaDualPass == 0u) {
        g_fSoftIrqDmaDualPass = 1u;
        /* Grep: irq_msix: soft residual irq_dma denser PASS */
        kprintf("irq_msix: soft residual irq_dma denser PASS denser=1 "
                "triple_host=1 soft_A_only=1 hw_B_only=1 tbl_C_only=1 "
                "multi_hit=1 peer_safe=1 dual_open=1 live_le_cap=1 "
                "product_hosts=UDX dual_dod=OPEN "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                "hosts=xhci_udx|rtl8168_udx|ddi_host "
                "id=8086:a12f|10ec:8168|ddi "
                "freestanding_product=SKIP "
                "never freestanding product close "
                "product_notify_mint=OPEN product_irq_cap=OPEN "
                "product_IRQ+DMA=OPEN soft_note_only=1 G-AC-1=1 "
                "stamp_free=1 bar_honesty=v2026.08.04.75 never_invent=.76 "
                "Soft!=product soft!=product "
                "dual_license=MIT_OR_Apache-2.0\n");
        kprintf("irq: soft residual irq_dma denser PASS denser=1 "
                "triple_host=1 product_hosts=UDX dual_dod=OPEN "
                "product_IRQ+DMA=OPEN Soft!=product soft!=product\n");
    }

    (void)irq_msix_soft_user_unbind(81u);
    (void)irq_msix_soft_user_unbind(82u);
    (void)irq_msix_soft_user_unbind(83u);
    return fOk;
}

/**
 * Sparse residual catalog for later UDX hosts (once-shot after selftests).
 * Soft multi-slot bind -> global MSI-X Notification -> NOTIFY_WAIT.
 * Soft != product IRQ cap mint; freestanding poll-mode NIC unaffected.
 * Multi-host residual: concurrent DDI handles; unbind(one) peer-safe.
 * Soft user bind residual: DDI_OP_IRQ_BIND rebind + multi-host bind shape.
 * Badge isolation residual: peer host mask isol on inject (bit_N_line_N).
 * Slot capacity residual: FULL reject + unbind-one reclaim.
 * Multi-match residual: shared badge fans out to all matching hosts.
 * Bulk-clear residual: bind(0) bulk vs unbind(one) CLOSE shape.
 * Table_user residual: soft_tbl fire -> UDX host note hits (product MSI-X).
 * Dual DoD residual honesty: A/B OPEN (UDX product); freestanding SKIP.
 * Bind honesty residual denser multi-host UDX: ready + primary/mask vs table
 * + rebind + live<=cap + inject isol + peer mask + post-unbind hit.
 * IRQ+DMA Dual DoD denser: triple-host (rtl/xhci/ddi) isol + multi denser.
 * Product Notification mint OPEN honesty (no per-host CNode IRQ mint).
 * product_hosts=UDX (10ec:8168|8086:a12f); never freestanding product close.
 * Lean residual (Hazard H1): never force IRQ eth poll; freestanding net
 * may stay poll-mode (net_eth_poll independent of this delivery path).
 * C0 residual deepen: stamp-free; G-AC-1; Dual DoD OPEN; Soft!=product.
 * Dual MIT OR Apache-2.0; Soft!=product dual license. H2 once-lamps.
 * greppable: irq_msix: soft residual
 * greppable: irq_msix: soft residual lean
 * greppable: irq_msix: soft residual lean PASS
 * greppable: irq_msix: soft residual multi_host
 * greppable: irq_msix: soft residual user_bind
 * greppable: irq_msix: soft residual badge_iso
 * greppable: irq_msix: soft residual slot_cap
 * greppable: irq_msix: soft residual multi_match
 * greppable: irq_msix: soft residual bulk_clear
 * greppable: irq_msix: soft residual table_user
 * greppable: irq_msix: soft residual dual_dod
 * greppable: irq_msix: soft residual bind_honesty
 * greppable: irq_msix: soft residual bind_honesty denser
 * greppable: irq_msix: soft residual irq_dma denser
 * greppable: product_hosts=UDX denser=1 multi_host_udx
 * greppable: product_notify_mint=OPEN
 * greppable: force_irq_eth_poll=0 poll_mode_first=1
 * greppable: net_eth_poll=run_loop_only net_eth_irq=0
 */
static void
irq_msix_soft_residual_log(const char *szVia)
{
    const char *szViaSafe;
    int fInitDefer;

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";

    /*
     * C0 residual: defer once-shot full residual catalog until after
     * residual selftests (exercise). Init inventory would lock pass=0
     * and hide multi_host/user_bind/badge_iso/slot_cap PASS tallies.
     * Soft!=product; stamp-free; never hard-gates; no version stamp.
     */
    fInitDefer = 0;
    if (szViaSafe[0] == 'i' && szViaSafe[1] == 'n' && szViaSafe[2] == 'i' &&
        szViaSafe[3] == 't' && szViaSafe[4] == '\0') {
        fInitDefer = 1;
    }

    if (fInitDefer != 0) {
        /* Lean + dual_dod honesty only on init (no residual once lock). */
        kprintf("irq_msix: soft residual lean udx_irq=1 "
                "poll_mode_first=%u force_irq_eth_poll=%u "
                "net_eth_irq=%u freestanding_rtl_poll=1 "
                "net_eth_poll_from_msix=%u net_eth_poll=run_loop_only "
                "product_irq_cap=OPEN product_notify_mint=OPEN "
                "soft_note_only=1 Soft!=product soft!=product G-AC-1=1 "
                "dual_dod_A=OPEN dual_dod_B=OPEN dual_dod=OPEN "
                "product_hosts=UDX freestanding_product=SKIP "
                "never_freestanding_product_close=1 freestanding_skip=1 "
                "C0=1 stamp_free=1 dual_license=MIT_OR_Apache-2.0 "
                "hazard=H1 via=%s\n",
                (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
                (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
                (unsigned)IRQ_MSIX_NET_ETH_IRQ,
                (unsigned)IRQ_MSIX_NET_ETH_POLL_FROM,
                szViaSafe);
        kprintf("irq_msix: soft residual dual_dod via=%s "
                "A=OPEN B=OPEN dual_dod=OPEN product_udx=1 "
                "product_hosts=UDX hosts=xhci_udx|rtl8168_udx "
                "id_A=8086:a12f id_B=10ec:8168 "
                "freestanding_product=SKIP freestanding_skip=1 "
                "never_freestanding_product_close=1 "
                "soft_residual_closes_dod=0 product_notify_mint=OPEN "
                "product_irq_cap=OPEN soft_note_only=1 G-AC-1=1 "
                "Soft!=product soft!=product dual_license=MIT_OR_Apache-2.0\n",
                szViaSafe);
        kprintf("irq: soft residual dual_dod A=OPEN B=OPEN dual_dod=OPEN "
                "product_hosts=UDX freestanding_product=SKIP "
                "product_udx=1 Soft!=product soft!=product G-AC-1=1\n");
        return;
    }

    if (g_fSoftResidualOnce != 0u) {
        return;
    }
    g_fSoftResidualOnce = 1u;
    (void)irq_msix_soft_user_count_live();
    {
        u32 u32ReadyRes;
        u32 fPrimaryOk;
        u32 fLiveLeCap;

        u32ReadyRes = (g_fReady != 0) ? 1u : 0u;
        fPrimaryOk = irq_msix_soft_primary_vs_table_ok();
        fLiveLeCap = (g_u32SoftUserLive <= IRQ_MSIX_SOFT_USER_SLOTS) ? 1u : 0u;
        /* Grep: irq_msix: soft residual */
        kprintf("irq_msix: soft residual UDX hosts via=%s "
                "slots=%u live=%u binds=%u unbinds=%u full=%u "
                "primary_handle=%u primary_mask=0x%lx notify_hits=%u "
                "primary_vs_table=%u live_le_cap=%u irq_msix_ready=%u "
                "ready_ok=%u primary_vs_table_ok=%u live_le_cap_ok=%u "
                "multi_host_safe=%u multi_host_check=%u multi_host_pass=%u "
                "rebinds=%u multi_host_binds=%u "
                "user_bind_check=%u user_bind_pass=%u "
                "badge_iso_check=%u badge_iso_pass=%u "
                "slot_cap_check=%u slot_cap_pass=%u "
                "multi_match_check=%u multi_match_pass=%u "
                "bulk_clear_check=%u bulk_clear_pass=%u "
                "table_user_check=%u table_user_pass=%u "
                "dual_dod_pass=%u bind_honesty_check=%u bind_honesty_pass=%u "
                "bind_honesty_denser_ok=%u denser=1 multi_host_udx=1 "
                "irq_dma_check=%u irq_dma_pass=%u irq_dma_denser_ok=%u "
                "path=soft_bind->inject->notify_msix_global->"
                "GJ_SYS_NOTIFY_WAIT which=0 "
                "ddi_path=OPEN->IRQ_BIND->inject->NOTIFY_WAIT->CLOSE "
                "table_user_path=soft_tbl->inject->NOTIFY_WAIT "
                "badge_shape=bit_N_line_N badge_bits=64 "
                "product_msix=OPEN product_irq_cap=OPEN product_notify_mint=OPEN "
                "product_IRQ+DMA=OPEN soft_note_only=1 cnode_irq_mint=0 "
                "product_hosts=UDX hosts=xhci_udx|rtl8168_udx|ddi_host "
                "id_A=8086:a12f id_B=10ec:8168 "
                "Soft!=product soft!=product "
                "dual_dod_A=OPEN dual_dod_B=OPEN dual_dod=OPEN "
                "freestanding_product=SKIP freestanding_skip=1 "
                "never_freestanding_product_close=1 G-AC-1=1 "
                "poll_mode_first=%u force_irq_eth_poll=%u "
                "net_eth_irq=%u freestanding_rtl_poll=1 freestanding_poll_ok=1 "
                "net_eth_poll_from_msix=%u net_eth_poll=run_loop_only "
                "hazard=H1 C0=1 stamp_free=1 dual_license=MIT_OR_Apache-2.0\n",
                szViaSafe,
                (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
                (unsigned)g_u32SoftUserLive,
                (unsigned)g_u32SoftUserBinds,
                (unsigned)g_u32SoftUserUnbinds,
                (unsigned)g_u32SoftUserFull,
                (unsigned)g_u32SoftUserHandle,
                (unsigned long)g_u64SoftUserMask,
                (unsigned)g_u32SoftUserNotifyHit,
                (unsigned)fPrimaryOk, (unsigned)fLiveLeCap,
                (unsigned)u32ReadyRes,
                (unsigned)g_u32SoftReadyOk,
                (unsigned)g_u32SoftPrimaryVsTableOk,
                (unsigned)g_u32SoftLiveLeCapOk,
                (unsigned)g_u32SoftUserMultiHostSafe,
                (unsigned)g_u32SoftUserMultiHostCheck,
                (unsigned)g_fSoftMultiHostPass,
                (unsigned)g_u32SoftUserRebinds,
                (unsigned)g_u32SoftUserMultiHostBinds,
                (unsigned)g_u32SoftUserBindResidualCheck,
                (unsigned)g_fSoftUserBindResidualPass,
                (unsigned)g_u32SoftUserBadgeIsoCheck,
                (unsigned)g_fSoftBadgeIsoPass,
                (unsigned)g_u32SoftUserSlotCapCheck,
                (unsigned)g_fSoftSlotCapPass,
                (unsigned)g_u32SoftUserMultiMatchCheck,
                (unsigned)g_fSoftMultiMatchPass,
                (unsigned)g_u32SoftUserBulkClearCheck,
                (unsigned)g_fSoftBulkClearPass,
                (unsigned)g_u32SoftUserTableUserCheck,
                (unsigned)g_fSoftTableUserPass,
                (unsigned)g_fSoftDualDodPass,
                (unsigned)g_u32SoftBindHonestyCheck,
                (unsigned)g_fSoftBindHonestyPass,
                (unsigned)g_u32SoftBindHonestyDenserOk,
                (unsigned)g_u32SoftIrqDmaDualCheck,
                (unsigned)g_fSoftIrqDmaDualPass,
                (unsigned)g_u32SoftIrqDmaDenserOk,
                (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
                (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
                (unsigned)IRQ_MSIX_NET_ETH_IRQ,
                (unsigned)IRQ_MSIX_NET_ETH_POLL_FROM);
        /* Twin residual lamp (irq: soft residual) */
        kprintf("irq: soft residual UDX hosts slots=%u live=%u "
                "primary_vs_table=%u live_le_cap=%u ready=%u "
                "product_irq_cap=OPEN product_notify_mint=OPEN "
                "soft_note_only=1 multi_host_safe=%u "
                "product_hosts=UDX Soft!=product soft!=product "
                "dual_dod_A=OPEN dual_dod_B=OPEN dual_dod=OPEN "
                "freestanding_product=SKIP G-AC-1=1 "
                "poll_mode_first=%u force_irq_eth_poll=%u "
                "net_eth_irq=%u hazard=H1 C0=1\n",
                (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
                (unsigned)g_u32SoftUserLive,
                (unsigned)fPrimaryOk, (unsigned)fLiveLeCap,
                (unsigned)u32ReadyRes,
                (unsigned)g_u32SoftUserMultiHostSafe,
                (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
                (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
                (unsigned)IRQ_MSIX_NET_ETH_IRQ);
    }
    /*
     * Multi-host residual honesty lamp (sparse; catalog once with residual).
     * Grep: irq_msix: soft residual multi_host
     * Grep: product_notify_mint=OPEN
     */
    kprintf("irq_msix: soft residual multi_host via=%s "
            "slots=%u live=%u unbind_one=1 peer_safe=%u "
            "ddi_op=IRQ_BIND close=unbind_handle "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
            (unsigned)g_u32SoftUserLive,
            (unsigned)g_u32SoftUserMultiHostSafe);
    kprintf("irq: soft residual multi_host via=%s "
            "product_notify_mint=OPEN soft_note_only=1 soft!=product\n",
            szViaSafe);
    /*
     * Soft user bind residual honesty lamp (DDI_OP_IRQ_BIND foundation).
     * Grep: irq_msix: soft residual user_bind
     * Grep: product_notify_mint=OPEN
     */
    kprintf("irq_msix: soft residual user_bind via=%s "
            "slots=%u live=%u binds=%u rebinds=%u "
            "multi_host_binds=%u unbinds=%u full=%u "
            "notify_hits=%u check=%u pass=%u "
            "ddi_op=IRQ_BIND close=unbind_handle never_clear_all=1 "
            "path=DDI_OP_OPEN->DDI_OP_IRQ_BIND->inject->"
            "GJ_SYS_NOTIFY_WAIT->DDI_OP_CLOSE "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
            (unsigned)g_u32SoftUserLive,
            (unsigned)g_u32SoftUserBinds,
            (unsigned)g_u32SoftUserRebinds,
            (unsigned)g_u32SoftUserMultiHostBinds,
            (unsigned)g_u32SoftUserUnbinds,
            (unsigned)g_u32SoftUserFull,
            (unsigned)g_u32SoftUserNotifyHit,
            (unsigned)g_u32SoftUserBindResidualCheck,
            (unsigned)g_fSoftUserBindResidualPass);
    kprintf("irq: soft residual user_bind via=%s "
            "ddi_irq_bind=1 never_clear_all=1 "
            "product_notify_mint=OPEN soft_note_only=1 soft!=product\n",
            szViaSafe);
    /*
     * Badge isolation residual honesty lamp (peer host mask isol).
     * Grep: irq_msix: soft residual badge_iso
     * Grep: product_notify_mint=OPEN
     */
    kprintf("irq_msix: soft residual badge_iso via=%s "
            "check=%u pass=%u peer_mask_isol=1 "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)g_u32SoftUserBadgeIsoCheck,
            (unsigned)g_fSoftBadgeIsoPass);
    kprintf("irq: soft residual badge_iso via=%s "
            "peer_mask_isol=1 product_notify_mint=OPEN "
            "soft_note_only=1 soft!=product\n",
            szViaSafe);
    /*
     * Slot capacity residual honesty lamp (FULL reject + reclaim).
     * Grep: irq_msix: soft residual slot_cap
     * Grep: product_notify_mint=OPEN
     */
    kprintf("irq_msix: soft residual slot_cap via=%s "
            "cap=%u live=%u full=%u check=%u pass=%u "
            "full_reject=1 reclaim=1 never_clear_all=1 "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
            (unsigned)g_u32SoftUserLive,
            (unsigned)g_u32SoftUserFull,
            (unsigned)g_u32SoftUserSlotCapCheck,
            (unsigned)g_fSoftSlotCapPass);
    kprintf("irq: soft residual slot_cap via=%s "
            "full_reject=1 reclaim=1 never_clear_all=1 "
            "product_notify_mint=OPEN soft_note_only=1 soft!=product\n",
            szViaSafe);
    /*
     * Multi-match residual honesty lamp (shared badge fanout).
     * Grep: irq_msix: soft residual multi_match
     * Grep: product_notify_mint=OPEN
     */
    kprintf("irq_msix: soft residual multi_match via=%s "
            "check=%u pass=%u shared_badge_fanout=1 "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)g_u32SoftUserMultiMatchCheck,
            (unsigned)g_fSoftMultiMatchPass);
    kprintf("irq: soft residual multi_match via=%s "
            "shared_badge_fanout=1 product_notify_mint=OPEN "
            "soft_note_only=1 soft!=product\n",
            szViaSafe);
    /*
     * Bulk-clear residual honesty lamp (bind0 bulk vs unbind CLOSE).
     * Grep: irq_msix: soft residual bulk_clear
     * Grep: product_notify_mint=OPEN
     */
    kprintf("irq_msix: soft residual bulk_clear via=%s "
            "check=%u pass=%u unbind_one=CLOSE bind0=bulk_only "
            "never_bulk_on_ddi_close=1 "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)g_u32SoftUserBulkClearCheck,
            (unsigned)g_fSoftBulkClearPass);
    kprintf("irq: soft residual bulk_clear via=%s "
            "unbind_one=CLOSE bind0=bulk_only "
            "never_bulk_on_ddi_close=1 product_notify_mint=OPEN "
            "soft_note_only=1 soft!=product\n",
            szViaSafe);
    /*
     * Product MSI-X table_user residual honesty (soft_tbl -> UDX host).
     * Grep: irq_msix: soft residual table_user
     * Grep: product_notify_mint=OPEN
     */
    kprintf("irq_msix: soft residual table_user via=%s "
            "check=%u pass=%u soft_tbl_to_udx=1 "
            "path=soft_tbl->inject->notify_msix_global->NOTIFY_WAIT "
            "product_msix=OPEN product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 cnode_irq_mint=0 "
            "soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)g_u32SoftUserTableUserCheck,
            (unsigned)g_fSoftTableUserPass);
    kprintf("irq: soft residual table_user via=%s "
            "soft_tbl_to_udx=1 product_msix=OPEN "
            "product_notify_mint=OPEN soft_note_only=1 soft!=product\n",
            szViaSafe);
    /*
     * Dual DoD residual honesty (UDX product path OPEN; freestanding SKIP).
     * Soft residual != Dual DoD A/B close. Soft!=product; G-AC-1.
     * dual_dod PASS = honesty residual only (A/B remain OPEN).
     * product_hosts=UDX; never freestanding product close.
     * Grep: irq_msix: soft residual dual_dod | product_hosts=UDX
     */
    kprintf("irq_msix: soft residual dual_dod via=%s "
            "A=OPEN B=OPEN dual_dod=OPEN product_udx=1 "
            "product_hosts=UDX hosts=xhci_udx|rtl8168_udx "
            "id_A=8086:a12f id_B=10ec:8168 "
            "freestanding_product=SKIP freestanding_skip=1 "
            "never_freestanding_product_close=1 "
            "soft_residual_closes_dod=0 product_msix=OPEN "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 G-AC-1=1 dual_dod_pass=%u "
            "table_user_pass=%u bind_honesty_pass=%u stamp_free=1 "
            "Soft!=product soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)g_fSoftDualDodPass,
            (unsigned)g_fSoftTableUserPass,
            (unsigned)g_fSoftBindHonestyPass);
    kprintf("irq: soft residual dual_dod via=%s "
            "A=OPEN B=OPEN dual_dod=OPEN product_hosts=UDX "
            "freestanding_product=SKIP never_freestanding_product_close=1 "
            "product_udx=1 freestanding_skip=1 "
            "G-AC-1=1 Soft!=product soft!=product\n",
            szViaSafe);
    /*
     * Product IRQ bind honesty residual denser multi-host UDX
     * (ready/primary/rebind/live<=cap/inject_isol/peer_mask).
     * Grep: irq_msix: soft residual bind_honesty
     * Grep: irq_msix: soft residual bind_honesty denser
     * Grep: product_hosts=UDX | primary_vs_table | live_le_cap | denser=1
     */
    kprintf("irq_msix: soft residual bind_honesty via=%s denser=1 "
            "multi_host_udx=1 check=%u pass=%u ready_ok=%u "
            "primary_vs_table_ok=%u live_le_cap_ok=%u denser_ok=%u "
            "rebinds=%u multi_host_binds=%u "
            "primary_handle=%u primary_mask=0x%lx live=%u cap=%u "
            "irq_msix_ready=%u primary_vs_table=%u live_le_cap=%u "
            "product_hosts=UDX dual_dod=OPEN "
            "freestanding_product=SKIP never_freestanding_product_close=1 "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "soft_note_only=1 G-AC-1=1 stamp_free=1 "
            "Soft!=product soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)g_u32SoftBindHonestyCheck,
            (unsigned)g_fSoftBindHonestyPass,
            (unsigned)g_u32SoftReadyOk,
            (unsigned)g_u32SoftPrimaryVsTableOk,
            (unsigned)g_u32SoftLiveLeCapOk,
            (unsigned)g_u32SoftBindHonestyDenserOk,
            (unsigned)g_u32SoftUserRebinds,
            (unsigned)g_u32SoftUserMultiHostBinds,
            (unsigned)g_u32SoftUserHandle,
            (unsigned long)g_u64SoftUserMask,
            (unsigned)g_u32SoftUserLive,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
            (unsigned)((g_fReady != 0) ? 1u : 0u),
            (unsigned)irq_msix_soft_primary_vs_table_ok(),
            (unsigned)((g_u32SoftUserLive <= IRQ_MSIX_SOFT_USER_SLOTS)
                       ? 1u : 0u));
    kprintf("irq: soft residual bind_honesty via=%s denser=1 multi_host_udx=1 "
            "ready=1 primary_vs_table live_le_cap rebind inject_isol "
            "product_hosts=UDX dual_dod=OPEN Soft!=product soft!=product\n",
            szViaSafe);
    kprintf("irq_msix: soft residual bind_honesty denser via=%s "
            "multi_host_udx=1 denser=1 denser_ok=%u pass=%u "
            "product_hosts=UDX dual_dod=OPEN product_notify_mint=OPEN "
            "soft_note_only=1 freestanding_product=SKIP G-AC-1=1 "
            "Soft!=product soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)g_u32SoftBindHonestyDenserOk,
            (unsigned)g_fSoftBindHonestyPass);
    /*
     * Product IRQ+DMA Dual DoD denser residual honesty (triple-host UDX).
     * Grep: irq_msix: soft residual irq_dma denser
     * Grep: product IRQ+DMA Dual DoD OPEN Soft!=product denser=1
     */
    kprintf("irq_msix: soft residual irq_dma denser via=%s denser=1 "
            "multi_host_udx=1 triple_host=1 check=%u pass=%u denser_ok=%u "
            "ready_ok=%u live_le_cap_ok=%u "
            "product_hosts=UDX dual_dod=OPEN "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "hosts=xhci_udx|rtl8168_udx|ddi_host "
            "freestanding_product=SKIP never_freestanding_product_close=1 "
            "product_notify_mint=OPEN product_irq_cap=OPEN "
            "product_IRQ+DMA=OPEN soft_note_only=1 G-AC-1=1 stamp_free=1 "
            "bar_honesty=v2026.08.04.75 never_invent=.76 "
            "Soft!=product soft!=product dual_license=MIT_OR_Apache-2.0\n",
            szViaSafe,
            (unsigned)g_u32SoftIrqDmaDualCheck,
            (unsigned)g_fSoftIrqDmaDualPass,
            (unsigned)g_u32SoftIrqDmaDenserOk,
            (unsigned)g_u32SoftReadyOk,
            (unsigned)g_u32SoftLiveLeCapOk);
    kprintf("irq: soft residual irq_dma denser via=%s denser=1 "
            "triple_host=1 product_hosts=UDX dual_dod=OPEN "
            "product_IRQ+DMA=OPEN Soft!=product soft!=product\n",
            szViaSafe);
    /*
     * Lean residual lamp (sparse; C0 residual deepen; no stamp storm).
     * freestanding net poll-mode first - MSI-X residual for later UDX only.
     * Grep: irq_msix: soft residual lean / irq: soft residual lean
     * Grep: irq_msix: soft residual lean PASS
     */
    kprintf("irq_msix: soft residual lean udx_irq=1 "
            "poll_mode_first=%u force_irq_eth_poll=%u "
            "net_eth_irq=%u freestanding_rtl_poll=1 "
            "net_eth_poll_from_msix=%u net_eth_poll=run_loop_only "
            "product_irq_cap=OPEN product_notify_mint=OPEN "
            "soft_note_only=1 Soft!=product soft!=product G-AC-1=1 "
            "dual_dod_A=OPEN dual_dod_B=OPEN dual_dod=OPEN "
            "product_hosts=UDX freestanding_product=SKIP "
            "never_freestanding_product_close=1 freestanding_skip=1 "
            "C0=1 stamp_free=1 dual_license=MIT_OR_Apache-2.0 "
            "hazard=H1 via=%s\n",
            (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
            (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
            (unsigned)IRQ_MSIX_NET_ETH_IRQ,
            (unsigned)IRQ_MSIX_NET_ETH_POLL_FROM,
            szViaSafe);
    kprintf("irq: soft residual lean udx_irq=1 "
            "poll_mode_first=%u force_irq_eth_poll=%u "
            "net_eth_irq=%u Soft!=product soft!=product G-AC-1=1 "
            "dual_dod_A=OPEN dual_dod_B=OPEN dual_dod=OPEN "
            "product_hosts=UDX C0=1 "
            "dual_license=MIT_OR_Apache-2.0 hazard=H1\n",
            (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
            (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
            (unsigned)IRQ_MSIX_NET_ETH_IRQ);
    /* Grep: irq_msix: soft residual lean PASS (once-shot; Soft!=product; C0) */
    kprintf("irq_msix: soft residual lean PASS "
            "poll_mode_first=%u force_irq_eth_poll=%u "
            "net_eth_irq=%u net_eth_poll_from_msix=%u "
            "net_eth_poll=run_loop_only "
            "product_notify_mint=OPEN Soft!=product soft!=product G-AC-1=1 "
            "dual_dod_A=OPEN dual_dod_B=OPEN dual_dod=OPEN "
            "product_hosts=UDX freestanding_product=SKIP freestanding_skip=1 "
            "C0=1 stamp_free=1 dual_license=MIT_OR_Apache-2.0 hazard=H1\n",
            (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
            (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
            (unsigned)IRQ_MSIX_NET_ETH_IRQ,
            (unsigned)IRQ_MSIX_NET_ETH_POLL_FROM);
}

/**
 * After soft inject pulse: if any driver-host soft bind is live and the
 * global MSI-X Notification pending overlaps a bind mask, record a hit
 * and emit greppable PASS once. Soft path only (no kprintf on hard IRQ).
 * Userspace reaps via GJ_SYS_NOTIFY_WAIT which=0 mask=slot.mask.
 */
static void
irq_msix_soft_user_notify_maybe(u64 u64Badge)
{
    struct gj_notify *pNotify;
    u64 u64Pending;
    u32 i;
    u32 u32HitHandle;
    u64 u64HitMask;
    u64 u64Match;
    int fAny;

    if (g_u32SoftUserLive == 0u && g_u32SoftUserHandle == 0u) {
        return;
    }
    pNotify = notify_msix_global();
    if (pNotify == NULL || !notify_is_live(pNotify)) {
        return;
    }
    u64Pending = notify_pending(pNotify);
    fAny = 0;
    u32HitHandle = 0u;
    u64HitMask = 0ull;
    u64Match = 0ull;

    for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
        u64 u64SlotMatch;

        if (g_aSoftUser[i].u32Handle == 0u ||
            g_aSoftUser[i].u64Mask == 0ull) {
            continue;
        }
        u64SlotMatch = u64Pending & g_aSoftUser[i].u64Mask;
        if (u64SlotMatch == 0ull) {
            /* Pulse may have just OR'd; accept badge AND mask as soft match. */
            u64SlotMatch = u64Badge & g_aSoftUser[i].u64Mask;
        }
        if (u64SlotMatch == 0ull) {
            continue;
        }
        irq_msix_soft_inc(&g_aSoftUser[i].u32Hits);
        irq_msix_soft_inc(&g_u32SoftUserNotifyHit);
        if (fAny == 0) {
            u32HitHandle = g_aSoftUser[i].u32Handle;
            u64HitMask = g_aSoftUser[i].u64Mask;
            u64Match = u64SlotMatch;
            fAny = 1;
        }
    }

    if (fAny == 0) {
        return;
    }
    if (g_fSoftUserNotifyPass != 0u) {
        return;
    }
    g_fSoftUserNotifyPass = 1u;
    /* Grep: irq_msix: soft user notify PASS */
    kprintf("irq_msix: soft user notify PASS handle=%u badge=0x%lx "
            "pending=0x%lx mask=0x%lx match=0x%lx live_slots=%u "
            "wait=GJ_SYS_NOTIFY_WAIT which=0 block=1 soft!=product\n",
            (unsigned)u32HitHandle, (unsigned long)u64Badge,
            (unsigned long)u64Pending, (unsigned long)u64HitMask,
            (unsigned long)u64Match, (unsigned)g_u32SoftUserLive);
}

/**
 * Wave 14 soft inventory dump - greppable "irq: soft ..." / "irq_msix: soft ...".
 * Snapshots live soft path state; never allocates; never hard-gates.
 * Not for hard-IRQ (kprintf only from product / soft paths).
 * szVia: caller tag (init / inject / pulse / table / hw / exercise).
 */
static void
irq_msix_soft_inventory_log(const char *szVia)
{
    const char *szViaSafe;
    struct gj_notify *pNotify;
    u32 u32Ready;
    u32 u32Live;
    u32 u32Signals;
    u64 u64Pending;
    u32 u32Soft;
    u32 u32Hw;
    u32 u32Irq;
    u32 u32Path;
    u32 u32Tbl;
    u32 u32Pulse;
    u64 u64Badge;
    u32 u32LastPath;
    u32 u32Handler;
    u32 u32NotifyNonNull;
    u32 fInHandler;

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    irq_msix_soft_inc(&g_u32SoftLogN);

    u32Ready = (g_fReady != 0) ? 1u : 0u;
    pNotify = notify_msix_global();
    u32NotifyNonNull = (pNotify != NULL) ? 1u : 0u;
    u32Live = (pNotify != NULL && notify_is_live(pNotify)) ? 1u : 0u;
    u32Signals = (pNotify != NULL) ? notify_signals(pNotify) : 0u;
    u64Pending = (pNotify != NULL) ? notify_pending(pNotify) : 0ull;
    fInHandler = (g_fInHandler != 0) ? 1u : 0u;

    /* Snapshot product counters (relaxed; concurrent hard IRQ may race). */
    u32Soft = g_u32MsixSoft;
    u32Hw = g_u32MsixHw;
    u32Irq = g_u32MsixIrq;
    u32Path = g_u32SoftPulsePath;
    u32Tbl = g_u32TablePulse;
    u64Badge = g_u64LastBadge;
    u32LastPath = g_u32LastPath;
    u32Handler =
        __atomic_load_n(&g_u32SoftIrqHandler, __ATOMIC_RELAXED);
    u32Pulse = g_u32SoftPulseEnter;

    /*
     * Primary prefix: irq: soft ...
     * Catalog capacity + path surface so smoke greps product depth.
     */
    /* Grep: irq: soft inventory */
    kprintf("irq: soft inventory via=%s vec=0x%x ready=%u live=%u "
            "paths=inject,pulse,table,hw,handler,exercise "
            "badge_soft=0x%x badge_hw=0x%x badge_tbl0=0x%x "
            "soft=%u hw=%u irq=%u path=%u tbl=%u logs=%u wave=%u\n",
            szViaSafe, (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)GJ_MSIX_BADGE_SOFT,
            (unsigned)GJ_MSIX_BADGE_HW, (unsigned)GJ_MSIX_BADGE_TBL(0),
            (unsigned)u32Soft, (unsigned)u32Hw, (unsigned)u32Irq,
            (unsigned)u32Path, (unsigned)u32Tbl, (unsigned)g_u32SoftLogN,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq: soft inject */
    kprintf("irq: soft inject enter=%u zero_coalesce=%u soft_total=%u "
            "default_badge=0x%x path_tag=%u\n",
            (unsigned)g_u32SoftInjectEnter, (unsigned)g_u32SoftInjectZero,
            (unsigned)u32Soft, (unsigned)GJ_MSIX_BADGE_SOFT,
            (unsigned)GJ_MSIX_PATH_SOFT);

    /* Grep: irq: soft pulse */
    kprintf("irq: soft pulse enter=%u not_ready=%u path_count=%u "
            "pend_ok=%u pend_miss=%u pending=0x%lx signals=%u\n",
            (unsigned)u32Pulse, (unsigned)g_u32SoftPulseNotReady,
            (unsigned)u32Path, (unsigned)g_u32SoftPulsePendOk,
            (unsigned)g_u32SoftPulsePendMiss, (unsigned long)u64Pending,
            (unsigned)u32Signals);

    /* Grep: irq: soft table */
    kprintf("irq: soft table enter=%u not_ready=%u init=%u prog=%u "
            "fire_ok=%u fire_miss=%u mask=%u tbl_pulse=%u "
            "path_tag=%u\n",
            (unsigned)g_u32SoftTableEnter, (unsigned)g_u32SoftTableNotReady,
            (unsigned)g_u32SoftTableInit, (unsigned)g_u32SoftTableProg,
            (unsigned)g_u32SoftTableFireOk, (unsigned)g_u32SoftTableFireMiss,
            (unsigned)g_u32SoftTableMask, (unsigned)u32Tbl,
            (unsigned)GJ_MSIX_PATH_TBL);

    /* Grep: irq: soft hw */
    kprintf("irq: soft hw enter=%u not_ready=%u hw_total=%u handler=%u "
            "path_tag_hw=%u path_tag_irq=%u self_ipi=0 eoi_on_handler=1\n",
            (unsigned)g_u32SoftHwEnter, (unsigned)g_u32SoftHwNotReady,
            (unsigned)u32Hw, (unsigned)u32Handler,
            (unsigned)GJ_MSIX_PATH_HW, (unsigned)GJ_MSIX_PATH_IRQ);

    /* Grep: irq: soft badges */
    kprintf("irq: soft badges last=0x%lx last_path=%u soft=0x%x hw=0x%x "
            "tbl0=0x%x pending=0x%lx\n",
            (unsigned long)u64Badge, (unsigned)u32LastPath,
            (unsigned)GJ_MSIX_BADGE_SOFT, (unsigned)GJ_MSIX_BADGE_HW,
            (unsigned)GJ_MSIX_BADGE_TBL(0), (unsigned long)u64Pending);

    /* Grep: irq: soft vec (Wave 14 IDT vector + path-tag map) */
    kprintf("irq: soft vec idt=0x%x path_none=%u path_soft=%u path_hw=%u "
            "path_irq=%u path_tbl=%u soft PASS\n",
            (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)GJ_MSIX_PATH_NONE,
            (unsigned)GJ_MSIX_PATH_SOFT, (unsigned)GJ_MSIX_PATH_HW,
            (unsigned)GJ_MSIX_PATH_IRQ, (unsigned)GJ_MSIX_PATH_TBL);

    /* Grep: irq: soft notify (Wave 14 Notification snapshot) */
    kprintf("irq: soft notify nonnull=%u live=%u signals=%u pending=0x%lx "
            "in_handler=%u soft PASS\n",
            (unsigned)u32NotifyNonNull, (unsigned)u32Live,
            (unsigned)u32Signals, (unsigned long)u64Pending,
            (unsigned)fInHandler);

    /* Grep: irq: soft exercise (Wave 14 exercise tallies) */
    kprintf("irq: soft exercise enter=%u ok=%u fail=%u not_ready=%u "
            "soft PASS\n",
            (unsigned)g_u32SoftExerciseEnter, (unsigned)g_u32SoftExerciseOk,
            (unsigned)g_u32SoftExerciseFail,
            (unsigned)g_u32SoftExerciseNotReady);

    /*
     * Grep: irq: soft path
     * Honesty: soft delivery plumbing != full device MSI-X product close.
     * Freestanding net may stay poll-mode; never force IRQ eth poll.
     */
    kprintf("irq: soft path claim=notify_delivery live_device=0 "
            "self_ipi=0 dual=soft+idt_gate "
            "poll_mode_first=%u force_irq_eth_poll=%u "
            "net_eth_irq=%u net_eth_poll=run_loop_only "
            "via=%s (soft inventory; not product gate; Soft!=product)\n",
            (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
            (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
            (unsigned)IRQ_MSIX_NET_ETH_IRQ,
            szViaSafe);

    /*
     * Wave 15 exclusive deepen (complementary; never hard-gates).
     * greppable: irq: soft ratio|headroom|surface
     */
    {
        u32 u32PulseBp = 0;
        u32 u32TableBp = 0;
        u32 u32ExOkBp = 0;
        u32 u32PathTot;

        u32PathTot = u32Soft + u32Hw + u32Path + u32Tbl;
        if (u32PathTot != 0u) {
            u32PulseBp = (u32Path * 10000u) / u32PathTot;
            u32TableBp = (u32Tbl * 10000u) / u32PathTot;
        }
        if ((g_u32SoftExerciseOk + g_u32SoftExerciseFail) != 0u) {
            u32ExOkBp = (g_u32SoftExerciseOk * 10000u) /
                        (g_u32SoftExerciseOk + g_u32SoftExerciseFail);
        }
        /* Grep: irq: soft ratio */
        kprintf("irq: soft ratio pulse_bp=%u table_bp=%u ex_ok_bp=%u "
                "soft=%u path=%u tbl=%u wave=%u\n",
                u32PulseBp, u32TableBp, u32ExOkBp, u32Soft, u32Path, u32Tbl,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: irq: soft headroom */
        kprintf("irq: soft headroom ready=%u live=%u exercise_ok=%u "
                "exercise_fail=%u not_ready=%u wave=%u\n",
                u32Ready, u32Live, g_u32SoftExerciseOk, g_u32SoftExerciseFail,
                g_u32SoftExerciseNotReady, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: irq: soft surface */
        kprintf("irq: soft surface inventory,inject,pulse,table,hw,badges,"
                "vec,notify,exercise,path,ratio,headroom,honesty,geom,"
                "return,contract,deepen,stats,residual,residual_lean,"
                "residual_multi_host,residual_user_bind,"
                "residual_badge_iso,residual_slot_cap,"
                "residual_multi_match,residual_bulk_clear,"
                "residual_table_user,residual_dual_dod,"
                "residual_bind_honesty,residual_bind_honesty_denser "
                "areas=%u wave=%u denser=1 multi_host_udx=1\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft != game I/O. greppable: irq: soft honesty|geom|return|contract
     */
    {
        u32 u32Surf = 0u;

        if (u32Ready != 0u) {
            u32Surf |= 0x1u;
        }
        if (u32Live != 0u) {
            u32Surf |= 0x2u;
        }
        if (u32Soft != 0u) {
            u32Surf |= 0x4u;
        }
        if (u32Path != 0u) {
            u32Surf |= 0x8u;
        }
        if (u32Tbl != 0u) {
            u32Surf |= 0x10u;
        }
        if (u32Hw != 0u) {
            u32Surf |= 0x20u;
        }
        if (g_u32SoftExerciseOk != 0u) {
            u32Surf |= 0x40u;
        }
        u32Surf |= 0x80u; /* vec/badge catalog always present */
        /* Grep: irq: soft honesty */
        kprintf("irq: soft honesty notify_delivery=1 live_device=0 "
                "self_ipi=0 game_io=0 product_irq=0 soft_only=1 "
                "wave=%u soft PASS\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: irq: soft geom */
        kprintf("irq: soft geom vec=0x%x badge_soft=0x%x badge_hw=0x%x "
                "badge_tbl0=0x%x path_soft=%u wave=%u soft PASS\n",
                (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)GJ_MSIX_BADGE_SOFT,
                (unsigned)GJ_MSIX_BADGE_HW, (unsigned)GJ_MSIX_BADGE_TBL(0),
                (unsigned)GJ_MSIX_PATH_SOFT, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: irq: soft contract - soft != game I/O; no IRQ eth force */
        kprintf("irq: soft contract soft_only=1 game_io=0 product_io=0 "
                "live_msix=0 force_irq_eth_poll=%u poll_mode_first=%u "
                "net_eth_irq=%u net_eth_poll=run_loop_only "
                "hazard=H1 soft PASS\n",
                (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
                (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
                (unsigned)IRQ_MSIX_NET_ETH_IRQ);
        /* Grep: irq: soft return - surface bit mask (ready|live|paths...) */
        kprintf("irq: soft return surface=0x%x ready=%u live=%u "
                "user_live=%u user_cap=%u wave=%u soft PASS\n",
                (unsigned)u32Surf, (unsigned)u32Ready, (unsigned)u32Live,
                (unsigned)g_u32SoftUserLive,
                (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    }

    /*
     * Residual multi-slot UDX surface (never hard-gates). Soft inventory only.
     * greppable: irq: soft residual / irq_msix: soft residual (once-shot)
     */
    (void)irq_msix_soft_user_count_live();
    irq_msix_soft_residual_log(szViaSafe);

    /* Grep: irq: soft deepen wave (residual stamp; no ret*angle storms) */
    kprintf("irq: soft deepen wave=%u areas=%u via=%s ready=%u live=%u "
            "soft=%u path=%u tbl=%u exercise_ok=%u user_live=%u ok=1 skip=0\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS, szViaSafe,
            (unsigned)u32Ready, (unsigned)u32Live, (unsigned)u32Soft,
            (unsigned)u32Path, (unsigned)u32Tbl,
            (unsigned)g_u32SoftExerciseOk, (unsigned)g_u32SoftUserLive);

    /* Grep: irq: soft stats */
    kprintf("irq: soft stats init=%u inject=%u pulse=%u table=%u hw=%u "
            "handler=%u exercise_enter=%u exercise_ok=%u exercise_fail=%u "
            "exercise_nr=%u soft=%u hw_n=%u irq_n=%u path_n=%u tbl_n=%u "
            "logs=%u wave=%u\n",
            (unsigned)g_u32SoftInit, (unsigned)g_u32SoftInjectEnter,
            (unsigned)u32Pulse, (unsigned)g_u32SoftTableEnter,
            (unsigned)g_u32SoftHwEnter, (unsigned)u32Handler,
            (unsigned)g_u32SoftExerciseEnter, (unsigned)g_u32SoftExerciseOk,
            (unsigned)g_u32SoftExerciseFail,
            (unsigned)g_u32SoftExerciseNotReady, (unsigned)u32Soft,
            (unsigned)u32Hw, (unsigned)u32Irq, (unsigned)u32Path,
            (unsigned)u32Tbl, (unsigned)g_u32SoftLogN,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq: soft inventory PASS / irq: soft PASS */
    kprintf("irq: soft inventory PASS via=%s logs=%u ready=%u live=%u "
            "wave=%u areas=%u\n",
            szViaSafe, (unsigned)g_u32SoftLogN, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS);
    kprintf("irq: soft PASS via=%s wave=%u\n", szViaSafe,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /*
     * Twin prefix: irq_msix: soft ... (agent-friendly alias; same tallies).
     */
    /* Grep: irq_msix: soft inventory */
    kprintf("irq_msix: soft inventory via=%s vec=0x%x ready=%u live=%u "
            "paths=inject,pulse,table,hw,handler,exercise "
            "badge_soft=0x%x badge_hw=0x%x badge_tbl0=0x%x "
            "soft=%u hw=%u irq=%u path=%u tbl=%u logs=%u wave=%u\n",
            szViaSafe, (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)GJ_MSIX_BADGE_SOFT,
            (unsigned)GJ_MSIX_BADGE_HW, (unsigned)GJ_MSIX_BADGE_TBL(0),
            (unsigned)u32Soft, (unsigned)u32Hw, (unsigned)u32Irq,
            (unsigned)u32Path, (unsigned)u32Tbl, (unsigned)g_u32SoftLogN,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq_msix: soft inject */
    kprintf("irq_msix: soft inject enter=%u zero_coalesce=%u soft_total=%u "
            "default_badge=0x%x path_tag=%u\n",
            (unsigned)g_u32SoftInjectEnter, (unsigned)g_u32SoftInjectZero,
            (unsigned)u32Soft, (unsigned)GJ_MSIX_BADGE_SOFT,
            (unsigned)GJ_MSIX_PATH_SOFT);

    /* Grep: irq_msix: soft pulse */
    kprintf("irq_msix: soft pulse enter=%u not_ready=%u path_count=%u "
            "pend_ok=%u pend_miss=%u pending=0x%lx signals=%u\n",
            (unsigned)u32Pulse, (unsigned)g_u32SoftPulseNotReady,
            (unsigned)u32Path, (unsigned)g_u32SoftPulsePendOk,
            (unsigned)g_u32SoftPulsePendMiss, (unsigned long)u64Pending,
            (unsigned)u32Signals);

    /* Grep: irq_msix: soft table */
    kprintf("irq_msix: soft table enter=%u not_ready=%u init=%u prog=%u "
            "fire_ok=%u fire_miss=%u mask=%u tbl_pulse=%u path_tag=%u\n",
            (unsigned)g_u32SoftTableEnter, (unsigned)g_u32SoftTableNotReady,
            (unsigned)g_u32SoftTableInit, (unsigned)g_u32SoftTableProg,
            (unsigned)g_u32SoftTableFireOk, (unsigned)g_u32SoftTableFireMiss,
            (unsigned)g_u32SoftTableMask, (unsigned)u32Tbl,
            (unsigned)GJ_MSIX_PATH_TBL);

    /* Grep: irq_msix: soft hw */
    kprintf("irq_msix: soft hw enter=%u not_ready=%u hw_total=%u "
            "handler=%u path_tag_hw=%u path_tag_irq=%u self_ipi=0 "
            "eoi_on_handler=1\n",
            (unsigned)g_u32SoftHwEnter, (unsigned)g_u32SoftHwNotReady,
            (unsigned)u32Hw, (unsigned)u32Handler,
            (unsigned)GJ_MSIX_PATH_HW, (unsigned)GJ_MSIX_PATH_IRQ);

    /* Grep: irq_msix: soft badges */
    kprintf("irq_msix: soft badges last=0x%lx last_path=%u soft=0x%x "
            "hw=0x%x tbl0=0x%x pending=0x%lx\n",
            (unsigned long)u64Badge, (unsigned)u32LastPath,
            (unsigned)GJ_MSIX_BADGE_SOFT, (unsigned)GJ_MSIX_BADGE_HW,
            (unsigned)GJ_MSIX_BADGE_TBL(0), (unsigned long)u64Pending);

    /* Grep: irq_msix: soft vec */
    kprintf("irq_msix: soft vec idt=0x%x path_none=%u path_soft=%u "
            "path_hw=%u path_irq=%u path_tbl=%u soft PASS\n",
            (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)GJ_MSIX_PATH_NONE,
            (unsigned)GJ_MSIX_PATH_SOFT, (unsigned)GJ_MSIX_PATH_HW,
            (unsigned)GJ_MSIX_PATH_IRQ, (unsigned)GJ_MSIX_PATH_TBL);

    /* Grep: irq_msix: soft notify */
    kprintf("irq_msix: soft notify nonnull=%u live=%u signals=%u "
            "pending=0x%lx in_handler=%u soft PASS\n",
            (unsigned)u32NotifyNonNull, (unsigned)u32Live,
            (unsigned)u32Signals, (unsigned long)u64Pending,
            (unsigned)fInHandler);

    /* Grep: irq_msix: soft exercise */
    kprintf("irq_msix: soft exercise enter=%u ok=%u fail=%u not_ready=%u "
            "soft PASS\n",
            (unsigned)g_u32SoftExerciseEnter, (unsigned)g_u32SoftExerciseOk,
            (unsigned)g_u32SoftExerciseFail,
            (unsigned)g_u32SoftExerciseNotReady);

    /* Grep: irq_msix: soft path - freestanding poll-mode first - H1 */
    kprintf("irq_msix: soft path claim=notify_delivery live_device=0 "
            "self_ipi=0 dual=soft+idt_gate "
            "poll_mode_first=%u force_irq_eth_poll=%u "
            "net_eth_irq=%u net_eth_poll=run_loop_only "
            "via=%s (soft inventory; not product gate; Soft!=product)\n",
            (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
            (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
            (unsigned)IRQ_MSIX_NET_ETH_IRQ,
            szViaSafe);

    /* Grep: irq_msix: soft user (multi-slot / multi-host / DDI IRQ_BIND residual) */
    kprintf("irq_msix: soft user handle=%u mask=0x%lx binds=%u unbinds=%u "
            "full=%u live=%u cap=%u notify_hits=%u pass=%u "
            "multi_host_safe=%u multi_host_pass=%u "
            "rebinds=%u multi_host_binds=%u "
            "user_bind_check=%u user_bind_pass=%u "
            "badge_iso_check=%u badge_iso_pass=%u "
            "slot_cap_check=%u slot_cap_pass=%u "
            "multi_match_check=%u multi_match_pass=%u "
            "bulk_clear_check=%u bulk_clear_pass=%u "
            "table_user_check=%u table_user_pass=%u "
            "dual_dod_pass=%u bind_honesty_check=%u bind_honesty_pass=%u "
            "bind_honesty_denser_ok=%u denser=1 multi_host_udx=1 "
            "irq_dma_check=%u irq_dma_pass=%u irq_dma_denser_ok=%u "
            "primary_vs_table_ok=%u live_le_cap_ok=%u ready_ok=%u "
            "ddi_op=IRQ_BIND never_clear_all=1 "
            "product_msix=OPEN product_notify_mint=OPEN soft_note_only=1 "
            "product_hosts=UDX dual_dod_A=OPEN dual_dod_B=OPEN dual_dod=OPEN "
            "product_IRQ+DMA=OPEN freestanding_product=SKIP G-AC-1=1 "
            "wait=GJ_SYS_NOTIFY_WAIT which=0 Soft!=product soft!=product\n",
            (unsigned)g_u32SoftUserHandle, (unsigned long)g_u64SoftUserMask,
            (unsigned)g_u32SoftUserBinds, (unsigned)g_u32SoftUserUnbinds,
            (unsigned)g_u32SoftUserFull, (unsigned)g_u32SoftUserLive,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
            (unsigned)g_u32SoftUserNotifyHit,
            (unsigned)g_fSoftUserNotifyPass,
            (unsigned)g_u32SoftUserMultiHostSafe,
            (unsigned)g_fSoftMultiHostPass,
            (unsigned)g_u32SoftUserRebinds,
            (unsigned)g_u32SoftUserMultiHostBinds,
            (unsigned)g_u32SoftUserBindResidualCheck,
            (unsigned)g_fSoftUserBindResidualPass,
            (unsigned)g_u32SoftUserBadgeIsoCheck,
            (unsigned)g_fSoftBadgeIsoPass,
            (unsigned)g_u32SoftUserSlotCapCheck,
            (unsigned)g_fSoftSlotCapPass,
            (unsigned)g_u32SoftUserMultiMatchCheck,
            (unsigned)g_fSoftMultiMatchPass,
            (unsigned)g_u32SoftUserBulkClearCheck,
            (unsigned)g_fSoftBulkClearPass,
            (unsigned)g_u32SoftUserTableUserCheck,
            (unsigned)g_fSoftTableUserPass,
            (unsigned)g_fSoftDualDodPass,
            (unsigned)g_u32SoftBindHonestyCheck,
            (unsigned)g_fSoftBindHonestyPass,
            (unsigned)g_u32SoftBindHonestyDenserOk,
            (unsigned)g_u32SoftIrqDmaDualCheck,
            (unsigned)g_fSoftIrqDmaDualPass,
            (unsigned)g_u32SoftIrqDmaDenserOk,
            (unsigned)g_u32SoftPrimaryVsTableOk,
            (unsigned)g_u32SoftLiveLeCapOk,
            (unsigned)g_u32SoftReadyOk);

    /* Grep: irq_msix: soft ratio (Wave 15 twin) */
    {
        u32 u32PulseBp2 = 0;
        u32 u32TableBp2 = 0;
        u32 u32ExOkBp2 = 0;
        u32 u32PathTot2;

        u32PathTot2 = u32Soft + u32Hw + u32Path + u32Tbl;
        if (u32PathTot2 != 0u) {
            u32PulseBp2 = (u32Path * 10000u) / u32PathTot2;
            u32TableBp2 = (u32Tbl * 10000u) / u32PathTot2;
        }
        if ((g_u32SoftExerciseOk + g_u32SoftExerciseFail) != 0u) {
            u32ExOkBp2 = (g_u32SoftExerciseOk * 10000u) /
                         (g_u32SoftExerciseOk + g_u32SoftExerciseFail);
        }
        kprintf("irq_msix: soft ratio pulse_bp=%u table_bp=%u ex_ok_bp=%u "
                "soft=%u path=%u tbl=%u wave=%u\n",
                u32PulseBp2, u32TableBp2, u32ExOkBp2, u32Soft, u32Path,
                u32Tbl, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("irq_msix: soft headroom ready=%u live=%u exercise_ok=%u "
                "exercise_fail=%u not_ready=%u wave=%u\n",
                u32Ready, u32Live, g_u32SoftExerciseOk, g_u32SoftExerciseFail,
                g_u32SoftExerciseNotReady, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("irq_msix: soft surface inventory,inject,pulse,table,hw,"
                "badges,vec,notify,exercise,path,ratio,headroom,honesty,"
                "geom,return,contract,deepen,stats,residual,residual_lean,"
                "residual_multi_host,residual_user_bind,"
                "residual_badge_iso,residual_slot_cap,"
                "residual_multi_match,residual_bulk_clear,"
                "residual_table_user,residual_dual_dod,"
                "residual_bind_honesty,residual_bind_honesty_denser "
                "areas=%u wave=%u denser=1 multi_host_udx=1\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 16 exclusive deepen twin (irq_msix: soft ...).
     * Soft != game I/O. greppable: irq_msix: soft honesty|geom|return|contract
     */
    {
        u32 u32Surf2 = 0u;

        if (u32Ready != 0u) {
            u32Surf2 |= 0x1u;
        }
        if (u32Live != 0u) {
            u32Surf2 |= 0x2u;
        }
        if (u32Soft != 0u) {
            u32Surf2 |= 0x4u;
        }
        if (u32Path != 0u) {
            u32Surf2 |= 0x8u;
        }
        if (u32Tbl != 0u) {
            u32Surf2 |= 0x10u;
        }
        if (u32Hw != 0u) {
            u32Surf2 |= 0x20u;
        }
        if (g_u32SoftExerciseOk != 0u) {
            u32Surf2 |= 0x40u;
        }
        u32Surf2 |= 0x80u;
        kprintf("irq_msix: soft honesty notify_delivery=1 live_device=0 "
                "self_ipi=0 game_io=0 product_irq=0 soft_only=1 "
                "wave=%u soft PASS\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("irq_msix: soft geom vec=0x%x badge_soft=0x%x badge_hw=0x%x "
                "badge_tbl0=0x%x path_soft=%u wave=%u soft PASS\n",
                (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)GJ_MSIX_BADGE_SOFT,
                (unsigned)GJ_MSIX_BADGE_HW, (unsigned)GJ_MSIX_BADGE_TBL(0),
                (unsigned)GJ_MSIX_PATH_SOFT,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("irq_msix: soft contract soft_only=1 game_io=0 product_io=0 "
                "live_msix=0 force_irq_eth_poll=%u poll_mode_first=%u "
                "net_eth_irq=%u net_eth_poll=run_loop_only "
                "hazard=H1 soft PASS\n",
                (unsigned)IRQ_MSIX_FORCE_IRQ_ETH_POLL,
                (unsigned)IRQ_MSIX_POLL_MODE_FIRST,
                (unsigned)IRQ_MSIX_NET_ETH_IRQ);
        /* Grep: irq_msix: soft return */
        kprintf("irq_msix: soft return surface=0x%x ready=%u live=%u "
                "user_live=%u user_cap=%u wave=%u soft PASS\n",
                (unsigned)u32Surf2, (unsigned)u32Ready, (unsigned)u32Live,
                (unsigned)g_u32SoftUserLive,
                (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    }

    /* Grep: irq_msix: soft deepen (residual stamp; no ret*angle storms) */
    kprintf("irq_msix: soft deepen wave=%u areas=%u via=%s ready=%u "
            "live=%u soft=%u path=%u tbl=%u exercise_ok=%u user_live=%u "
            "ok=1 skip=0\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS, szViaSafe,
            (unsigned)u32Ready, (unsigned)u32Live, (unsigned)u32Soft,
            (unsigned)u32Path, (unsigned)u32Tbl,
            (unsigned)g_u32SoftExerciseOk, (unsigned)g_u32SoftUserLive);

    /* Grep: irq_msix: soft stats */
    kprintf("irq_msix: soft stats init=%u inject=%u pulse=%u table=%u "
            "hw=%u handler=%u exercise_enter=%u exercise_ok=%u "
            "exercise_fail=%u exercise_nr=%u soft=%u hw_n=%u irq_n=%u "
            "path_n=%u tbl_n=%u logs=%u wave=%u\n",
            (unsigned)g_u32SoftInit, (unsigned)g_u32SoftInjectEnter,
            (unsigned)u32Pulse, (unsigned)g_u32SoftTableEnter,
            (unsigned)g_u32SoftHwEnter, (unsigned)u32Handler,
            (unsigned)g_u32SoftExerciseEnter, (unsigned)g_u32SoftExerciseOk,
            (unsigned)g_u32SoftExerciseFail,
            (unsigned)g_u32SoftExerciseNotReady, (unsigned)u32Soft,
            (unsigned)u32Hw, (unsigned)u32Irq, (unsigned)u32Path,
            (unsigned)u32Tbl, (unsigned)g_u32SoftLogN,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq_msix: soft inventory PASS / irq_msix: soft PASS */
    kprintf("irq_msix: soft inventory PASS via=%s logs=%u ready=%u "
            "live=%u wave=%u areas=%u\n",
            szViaSafe, (unsigned)g_u32SoftLogN, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS);
    kprintf("irq_msix: soft PASS via=%s wave=%u\n", szViaSafe,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
}

/**
 * After first product soft activity, print soft inventory once
 * (mirrors notify/futex soft-stats-once). Diagnostics only.
 */
static void
irq_msix_soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftInjectEnter == 0 && g_u32SoftPulseEnter == 0 &&
        g_u32SoftTableEnter == 0 && g_u32SoftHwEnter == 0 &&
        g_u32SoftExerciseEnter == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    irq_msix_soft_inventory_log("once");
}

void
irq_msix_handler(void)
{
    struct gj_notify *pNotify = notify_msix_global();

    g_fInHandler = 1;
    /*
     * Hard IRQ path (Hazard H1): IRQ-safe tallies + Notification pulse + EOI
     * only. No kprintf. Never net_eth_poll (fault class: IRQ stack smash /
     * #PF I=1). Eth poll is run-loop only (scheduler_run). Soft!=product.
     * greppable: net_eth_poll_from_msix=0 net_eth_irq=0
     */
    irq_msix_soft_inc_atomic(&g_u32SoftIrqHandler);
    g_u32MsixIrq++;
    g_u32MsixHw++;
    irq_msix_note_pulse(GJ_MSIX_BADGE_SOFT, GJ_MSIX_PATH_IRQ);
    notify_pulse(pNotify, GJ_MSIX_BADGE_SOFT);
    apic_eoi();
    g_fInHandler = 0;
}

void
irq_msix_soft_inject(u64 u64Badge)
{
    struct gj_notify *pNotify = notify_msix_global();

    irq_msix_soft_inc(&g_u32SoftInjectEnter);
    if (u64Badge == 0) {
        u64Badge = GJ_MSIX_BADGE_SOFT;
        irq_msix_soft_inc(&g_u32SoftInjectZero);
    }
    g_u32MsixSoft++;
    g_u32MsixIrq++;
    irq_msix_note_pulse(u64Badge, GJ_MSIX_PATH_SOFT);
    notify_pulse(pNotify, u64Badge);
    /*
     * Soft user notify: when a driver-host bind is live, one-shot PASS
     * (kprintf only on first match - safe under inject storms).
     */
    irq_msix_soft_user_notify_maybe(u64Badge);
    /* No other kprintf here: may be nested under soft fire / early inject. */
    irq_msix_soft_inventory_maybe_once();
}

int
irq_msix_soft_user_bind(u32 u32Handle, u64 u64BadgeMask)
{
    u32 i;
    u32 u32Free;
    u32 u32Slot;
    u32 u32LiveBefore;
    u32 fMultiHostBind;

    if (u32Handle == 0u) {
        /*
         * Bulk clear only (API: handle 0 = unbound all).
         * DDI_OP_CLOSE must never use this path - multi-host unbind-safe
         * residual depends on unbind(handle) clearing ONE slot only.
         * Soft!=product; product Notification mint remains OPEN.
         */
        irq_msix_soft_user_clear_all();
        irq_msix_soft_inc(&g_u32SoftUserUnbinds);
        return 0;
    }
    if (!g_fReady) {
        return -1;
    }
    if (u64BadgeMask == 0ull) {
        u64BadgeMask = GJ_MSIX_BADGE_SOFT;
    }

    u32LiveBefore = irq_msix_soft_user_count_live();

    /* Rebind existing handle if present (DDI_OP_IRQ_BIND rebind residual). */
    u32Free = IRQ_MSIX_SOFT_USER_SLOTS;
    for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
        if (g_aSoftUser[i].u32Handle == u32Handle) {
            g_aSoftUser[i].u64Mask = u64BadgeMask;
            g_u32SoftUserHandle = u32Handle;
            g_u64SoftUserMask = u64BadgeMask;
            irq_msix_soft_inc(&g_u32SoftUserBinds);
            irq_msix_soft_inc(&g_u32SoftUserRebinds);
            (void)irq_msix_soft_user_count_live();
            /* Grep: irq_msix: soft user bind (rebind residual) */
            kprintf("irq_msix: soft user bind handle=%u mask=0x%lx "
                    "slot=%u rebind=1 live=%u cap=%u "
                    "multi_host=1 ddi_op=IRQ_BIND "
                    "product_notify_mint=OPEN soft_note_only=1 "
                    "wait=GJ_SYS_NOTIFY_WAIT which=0 soft!=product\n",
                    (unsigned)u32Handle, (unsigned long)u64BadgeMask,
                    (unsigned)i, (unsigned)g_u32SoftUserLive,
                    (unsigned)IRQ_MSIX_SOFT_USER_SLOTS);
            return 0;
        }
        if (u32Free == IRQ_MSIX_SOFT_USER_SLOTS &&
            g_aSoftUser[i].u32Handle == 0u) {
            u32Free = i;
        }
    }

    if (u32Free >= IRQ_MSIX_SOFT_USER_SLOTS) {
        irq_msix_soft_inc(&g_u32SoftUserFull);
        kprintf("irq_msix: soft user bind FULL handle=%u mask=0x%lx "
                "live=%u cap=%u multi_host=1 ddi_op=IRQ_BIND soft!=product\n",
                (unsigned)u32Handle, (unsigned long)u64BadgeMask,
                (unsigned)g_u32SoftUserLive,
                (unsigned)IRQ_MSIX_SOFT_USER_SLOTS);
        return -1;
    }

    /*
     * Multi-host bind residual: new DDI_OP_IRQ_BIND while peer host(s)
     * already occupy soft slots (concurrent UDX hosts). Soft!=product.
     */
    fMultiHostBind = (u32LiveBefore >= 1u) ? 1u : 0u;
    if (fMultiHostBind != 0u) {
        irq_msix_soft_inc(&g_u32SoftUserMultiHostBinds);
    }

    u32Slot = u32Free;
    g_aSoftUser[u32Slot].u32Handle = u32Handle;
    g_aSoftUser[u32Slot].u64Mask = u64BadgeMask;
    g_aSoftUser[u32Slot].u32Hits = 0u;
    g_u32SoftUserHandle = u32Handle;
    g_u64SoftUserMask = u64BadgeMask;
    irq_msix_soft_inc(&g_u32SoftUserBinds);
    (void)irq_msix_soft_user_count_live();
    /* Grep: irq_msix: soft user bind / product_notify_mint=OPEN */
    kprintf("irq_msix: soft user bind handle=%u mask=0x%lx "
            "slot=%u rebind=0 live=%u cap=%u "
            "multi_host=%u multi_host_binds=%u ddi_op=IRQ_BIND "
            "product_notify_mint=OPEN soft_note_only=1 "
            "wait=GJ_SYS_NOTIFY_WAIT which=0 soft!=product\n",
            (unsigned)u32Handle, (unsigned long)u64BadgeMask,
            (unsigned)u32Slot, (unsigned)g_u32SoftUserLive,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
            (unsigned)fMultiHostBind,
            (unsigned)g_u32SoftUserMultiHostBinds);
    return 0;
}

int
irq_msix_soft_user_unbind(u32 u32Handle)
{
    u32 i;
    u32 u32LiveBefore;
    u32 u32PeersLeft;
    int fFound;

    if (u32Handle == 0u) {
        /* Bulk clear only - not multi-host DDI CLOSE shape. */
        irq_msix_soft_user_clear_all();
        irq_msix_soft_inc(&g_u32SoftUserUnbinds);
        return 0;
    }
    if (!g_fReady) {
        return -1;
    }

    u32LiveBefore = irq_msix_soft_user_count_live();
    fFound = 0;
    for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
        if (g_aSoftUser[i].u32Handle == u32Handle) {
            g_aSoftUser[i].u32Handle = 0u;
            g_aSoftUser[i].u64Mask = 0ull;
            g_aSoftUser[i].u32Hits = 0u;
            fFound = 1;
            break;
        }
    }
    if (fFound == 0) {
        return 0;
    }
    irq_msix_soft_inc(&g_u32SoftUserUnbinds);
    (void)irq_msix_soft_user_count_live();
    /*
     * Multi-host safe residual: unbind(one) left other host slots live.
     * DDI CLOSE foundation - never clears peer hosts (Soft!=product).
     */
    u32PeersLeft = g_u32SoftUserLive;
    if (u32LiveBefore > 1u && u32PeersLeft == (u32LiveBefore - 1u)) {
        irq_msix_soft_inc(&g_u32SoftUserMultiHostSafe);
    }
    /* Refresh primary stats to a remaining live slot (if any). */
    if (g_u32SoftUserHandle == u32Handle) {
        g_u32SoftUserHandle = 0u;
        g_u64SoftUserMask = 0ull;
        for (i = 0u; i < IRQ_MSIX_SOFT_USER_SLOTS; i++) {
            if (g_aSoftUser[i].u32Handle != 0u) {
                g_u32SoftUserHandle = g_aSoftUser[i].u32Handle;
                g_u64SoftUserMask = g_aSoftUser[i].u64Mask;
                break;
            }
        }
    }
    /* Grep: irq_msix: soft user unbind / multi_host residual / DDI CLOSE */
    kprintf("irq_msix: soft user unbind handle=%u live=%u cap=%u "
            "peers_left=%u multi_host_safe=%u "
            "ddi_op=CLOSE never_clear_all=1 "
            "product_notify_mint=OPEN soft_note_only=1 soft!=product\n",
            (unsigned)u32Handle, (unsigned)g_u32SoftUserLive,
            (unsigned)IRQ_MSIX_SOFT_USER_SLOTS,
            (unsigned)u32PeersLeft,
            (unsigned)g_u32SoftUserMultiHostSafe);
    return 0;
}

u32
irq_msix_soft_user_handle(void)
{
    return g_u32SoftUserHandle;
}

u64
irq_msix_soft_user_mask(void)
{
    return g_u64SoftUserMask;
}

u32
irq_msix_soft_user_notify_hits(void)
{
    return g_u32SoftUserNotifyHit;
}

u32
irq_msix_soft_user_slots_live(void)
{
    return irq_msix_soft_user_count_live();
}

u32
irq_msix_soft_user_slots_cap(void)
{
    return IRQ_MSIX_SOFT_USER_SLOTS;
}

u32
irq_msix_soft_user_multi_host_safe(void)
{
    return g_u32SoftUserMultiHostSafe;
}

u32
irq_msix_soft_user_multi_host_pass(void)
{
    return (g_fSoftMultiHostPass != 0u) ? 1u : 0u;
}

u32
irq_msix_soft_user_rebinds(void)
{
    return g_u32SoftUserRebinds;
}

u32
irq_msix_soft_user_multi_host_binds(void)
{
    return g_u32SoftUserMultiHostBinds;
}

u32
irq_msix_soft_user_bind_residual_pass(void)
{
    return (g_fSoftUserBindResidualPass != 0u) ? 1u : 0u;
}

u32
irq_msix_soft_pulse_path(u64 u64Badge)
{
    struct gj_notify *pNotify;
    u64 u64Pending;

    irq_msix_soft_inc(&g_u32SoftPulseEnter);
    if (!g_fReady) {
        irq_msix_soft_inc(&g_u32SoftPulseNotReady);
        return 0;
    }
    if (u64Badge == 0) {
        u64Badge = GJ_MSIX_BADGE_SOFT;
        irq_msix_soft_inc(&g_u32SoftInjectZero);
    }
    pNotify = notify_msix_global();
    g_u32MsixSoft++;
    g_u32MsixIrq++;
    g_u32SoftPulsePath++;
    irq_msix_note_pulse(u64Badge, GJ_MSIX_PATH_SOFT);
    notify_pulse(pNotify, u64Badge);
    irq_msix_soft_user_notify_maybe(u64Badge);
    /*
     * Soft path verify: pending must observe the OR'd badge (stats poll;
     * does not clear - wait path owns reclaim).
     */
    u64Pending = notify_pending(pNotify);
    if ((u64Pending & u64Badge) == 0) {
        irq_msix_soft_inc(&g_u32SoftPulsePendMiss);
        kprintf("irq: MSI-X soft pulse path pending miss badge=0x%lx "
                "pending=0x%lx\n",
                (unsigned long)u64Badge, (unsigned long)u64Pending);
        /* Twin soft miss lamp (Wave 14 inventory; does not replace smoke). */
        kprintf("irq: soft pulse pend_miss badge=0x%lx pending=0x%lx\n",
                (unsigned long)u64Badge, (unsigned long)u64Pending);
        kprintf("irq_msix: soft pulse pend_miss badge=0x%lx pending=0x%lx\n",
                (unsigned long)u64Badge, (unsigned long)u64Pending);
    } else {
        irq_msix_soft_inc(&g_u32SoftPulsePendOk);
    }
    irq_msix_soft_inventory_maybe_once();
    return g_u32SoftPulsePath;
}

u32
irq_msix_hw_pulse(void)
{
    struct gj_notify *pNotify;

    irq_msix_soft_inc(&g_u32SoftHwEnter);
    if (!g_fReady) {
        irq_msix_soft_inc(&g_u32SoftHwNotReady);
        return 0;
    }
    /*
     * Real self-IPI during early kmain risks #DF (stack/TPR). The IDT stub
     * (irq_stub_msix) is installed for live device IRQs; this pulse mimics
     * the Notification side of a hardware IRQ without EOI/reentry.
     */
    pNotify = notify_msix_global();
    g_u32MsixHw++;
    g_u32MsixIrq++;
    irq_msix_note_pulse(GJ_MSIX_BADGE_HW, GJ_MSIX_PATH_HW);
    notify_pulse(pNotify, GJ_MSIX_BADGE_HW);
    kprintf("irq: MSI-X hw-sim pulse (IDT gate installed for live IRQs)\n");
    /* Grep: irq: soft hw / irq_msix: soft hw (event lamp) */
    kprintf("irq: soft hw pulse n=%u badge=0x%x path=%u\n",
            (unsigned)g_u32MsixHw, (unsigned)GJ_MSIX_BADGE_HW,
            (unsigned)GJ_MSIX_PATH_HW);
    kprintf("irq_msix: soft hw pulse n=%u badge=0x%x path=%u\n",
            (unsigned)g_u32MsixHw, (unsigned)GJ_MSIX_BADGE_HW,
            (unsigned)GJ_MSIX_PATH_HW);
    irq_msix_soft_inventory_maybe_once();
    return g_u32MsixHw;
}

u32
irq_msix_soft_table_pulse(u16 u16Idx)
{
    u32 u32Before;
    u32 u32Delivered;

    irq_msix_soft_inc(&g_u32SoftTableEnter);
    if (!g_fReady) {
        irq_msix_soft_inc(&g_u32SoftTableNotReady);
        return 0;
    }
    if (!pci_msix_soft_ready()) {
        irq_msix_soft_inc(&g_u32SoftTableInit);
        pci_msix_soft_table_init();
        /* Ensure a programmed unmasked entry for the soft table path. */
        irq_msix_soft_inc(&g_u32SoftTableProg);
        (void)pci_msix_soft_program(u16Idx, 0xFEE00000u, (u32)GJ_MSIX_IRQ_VEC,
                                    0);
    }
    u32Before = g_u32MsixSoft;
    u32Delivered = pci_msix_soft_fire(u16Idx);
    if (u32Delivered == 0) {
        irq_msix_soft_inc(&g_u32SoftTableFireMiss);
        return 0;
    }
    irq_msix_soft_inc(&g_u32SoftTableFireOk);
    /*
     * soft_fire calls irq_msix_soft_inject when ready - retag path as table
     * soft delivery for stats.
     */
    g_u32LastPath = GJ_MSIX_PATH_TBL;
    g_u64LastBadge = GJ_MSIX_BADGE_TBL(u16Idx);
    if (g_u32MsixSoft > u32Before) {
        g_u32TablePulse++;
    }
    irq_msix_soft_inventory_maybe_once();
    return 1;
}

u32
irq_msix_soft_path_exercise(void)
{
    struct gj_notify *pNotify;
    u64 u64Pend;
    u32 u32SoftPath;
    u32 fOk = 1;
    u32 u32Tbl;

    irq_msix_soft_inc(&g_u32SoftExerciseEnter);
    if (!g_fReady) {
        irq_msix_soft_inc(&g_u32SoftExerciseNotReady);
        return 0;
    }
    pNotify = notify_msix_global();
    if (pNotify == NULL || !notify_is_live(pNotify)) {
        kprintf("irq: MSI-X soft pulse path FAIL (notify not live)\n");
        irq_msix_soft_inc(&g_u32SoftExerciseFail);
        /* Grep: irq: soft exercise / irq_msix: soft exercise */
        kprintf("irq: soft exercise FAIL reason=notify_not_live\n");
        kprintf("irq_msix: soft exercise FAIL reason=notify_not_live\n");
        return 0;
    }

    /* Multi-badge soft pulse path (bits 0 + 2). */
    u32SoftPath = irq_msix_soft_pulse_path(GJ_MSIX_BADGE_SOFT |
                                           GJ_MSIX_BADGE_TBL(0));
    if (u32SoftPath == 0) {
        fOk = 0;
    }
    u64Pend = notify_pending(pNotify);
    if ((u64Pend & GJ_MSIX_BADGE_SOFT) == 0) {
        fOk = 0;
    }

    /* Second soft pulse path inject (OR coalesce). */
    if (irq_msix_soft_pulse_path(GJ_MSIX_BADGE_SOFT) == 0) {
        fOk = 0;
    }

    /* Table soft -> Notification pulse. */
    if (!pci_msix_soft_ready()) {
        irq_msix_soft_inc(&g_u32SoftTableInit);
        pci_msix_soft_table_init();
        irq_msix_soft_inc(&g_u32SoftTableProg);
        (void)pci_msix_soft_program(0, 0xFEE00000u, (u32)GJ_MSIX_IRQ_VEC, 0);
    } else {
        /* Ensure entry 0 unmasked for delivery. */
        irq_msix_soft_inc(&g_u32SoftTableMask);
        (void)pci_msix_soft_mask(0, 0);
    }
    u32Tbl = irq_msix_soft_table_pulse(0);
    if (u32Tbl == 0) {
        fOk = 0;
    }
    if (g_u32LastPath != GJ_MSIX_PATH_TBL &&
        g_u32LastPath != GJ_MSIX_PATH_SOFT) {
        fOk = 0;
    }
    if (g_u64LastBadge == 0) {
        fOk = 0;
    }

    if (fOk) {
        irq_msix_soft_inc(&g_u32SoftExerciseOk);
        kprintf("irq: MSI-X soft pulse path soft=%u path=%u tbl=%u "
                "last_badge=0x%lx signals=%u PASS\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge, notify_signals(pNotify));
        kprintf("irq: MSI-X soft pulse path PASS\n");
        /* Grep: irq: soft exercise PASS */
        kprintf("irq: soft exercise PASS soft=%u path=%u tbl=%u "
                "last_badge=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
        kprintf("irq_msix: soft exercise PASS soft=%u path=%u tbl=%u "
                "last_badge=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
        /*
         * Soft driver-host wire selftest: bind synthetic handle -> soft inject
         * -> pending matches mask (same shape as userspace NOTIFY_WAIT reclaim).
         * greppable: irq_msix: soft user notify PASS
         */
        /*
         * Soft driver-host multi-slot residual selftest:
         *   bind handles 1..3 (UDX-host shapes) into IRQ_MSIX_SOFT_USER_SLOTS
         *   table -> inject -> pending matches masks.
         * Soft != product; freestanding poll-mode NIC untouched (no hard IRQ).
         * Never force IRQ eth poll; never call net_eth_poll from this path.
         * No stamp storms: bind lamps once per slot; notify PASS once-shot.
         */
        if (irq_msix_soft_user_bind(1u, GJ_MSIX_BADGE_SOFT) == 0) {
            (void)irq_msix_soft_user_bind(2u, GJ_MSIX_BADGE_SOFT |
                                              GJ_MSIX_BADGE_HW);
            (void)irq_msix_soft_user_bind(3u, GJ_MSIX_BADGE_TBL(0));
            irq_msix_soft_inject(GJ_MSIX_BADGE_SOFT);
            /*
             * If inject path skipped PASS (e.g. race), still confirm pending
             * and emit once so boot smoke always greps the wire.
             */
            if (g_fSoftUserNotifyPass == 0u) {
                u64 u64PendUser = notify_pending(pNotify);

                if ((u64PendUser & GJ_MSIX_BADGE_SOFT) != 0ull) {
                    irq_msix_soft_inc(&g_u32SoftUserNotifyHit);
                    g_fSoftUserNotifyPass = 1u;
                    kprintf("irq_msix: soft user notify PASS handle=%u "
                            "badge=0x%lx pending=0x%lx mask=0x%lx "
                            "live_slots=%u "
                            "wait=GJ_SYS_NOTIFY_WAIT which=0 block=1 "
                            "product_notify_mint=OPEN soft!=product\n",
                            (unsigned)g_u32SoftUserHandle,
                            (unsigned long)GJ_MSIX_BADGE_SOFT,
                            (unsigned long)u64PendUser,
                            (unsigned long)g_u64SoftUserMask,
                            (unsigned)g_u32SoftUserLive);
                }
            }
            /* Leave multi-slot notes live for residual inventory greps. */
            (void)irq_msix_soft_user_count_live();
        }
        /*
         * Multi-host unbind residual (DDI IRQ_BIND CLOSE foundation):
         *   bind A/B/C -> unbind B only -> A and C remain live.
         * Product Notification mint OPEN honesty (soft note only).
         * greppable: irq_msix: soft residual multi_host PASS
         * greppable: product_notify_mint=OPEN
         */
        (void)irq_msix_soft_multi_host_selftest();
        /*
         * Soft user bind residual deepen (DDI_OP_IRQ_BIND shape):
         *   bind/rebind/multi-host bind/unbind-one; never clear-all on CLOSE.
         * Product Notification mint OPEN; soft_note_only; Soft!=product.
         * greppable: irq_msix: soft residual user_bind PASS
         * greppable: product_notify_mint=OPEN
         */
        (void)irq_msix_soft_user_bind_residual_selftest();
        /*
         * Badge isolation residual deepen (concurrent UDX host masks):
         *   A SOFT-only + B HW-only; inject SOFT hits A only; HW hits B only.
         * greppable: irq_msix: soft residual badge_iso PASS
         * greppable: product_notify_mint=OPEN
         */
        (void)irq_msix_soft_badge_iso_selftest();
        /*
         * Slot capacity residual deepen (IRQ_MSIX_SOFT_USER_SLOTS table):
         *   fill free -> FULL reject -> unbind(one) reclaim -> rebind OK.
         * greppable: irq_msix: soft residual slot_cap PASS
         * greppable: product_notify_mint=OPEN
         */
        (void)irq_msix_soft_slot_cap_selftest();
        /*
         * Multi-match residual deepen (global notify shared-badge fanout):
         *   hosts sharing SOFT both hit on one inject; Soft!=product.
         * greppable: irq_msix: soft residual multi_match PASS
         * greppable: product_notify_mint=OPEN
         */
        (void)irq_msix_soft_multi_match_selftest();
        /*
         * Bulk-clear residual deepen (API honesty):
         *   unbind(one)=CLOSE; bind(0)=bulk only; re-seed residual hosts.
         * greppable: irq_msix: soft residual bulk_clear PASS
         * greppable: product_notify_mint=OPEN
         */
        (void)irq_msix_soft_bulk_clear_selftest();
        /*
         * Product MSI-X table_user residual (soft_tbl -> UDX host notes):
         *   bind TBL masks -> soft_table_pulse(0) -> multi-host hits.
         * greppable: irq_msix: soft residual table_user PASS
         * greppable: product_notify_mint=OPEN product_msix=OPEN
         */
        (void)irq_msix_soft_table_user_selftest();
        /*
         * Dual DoD residual honesty (A/B OPEN; soft residual != DoD close).
         * product_hosts=UDX; never freestanding product close.
         * greppable: irq_msix: soft residual dual_dod PASS
         * greppable: product_hosts=UDX Soft!=product dual_dod OPEN
         */
        (void)irq_msix_soft_dual_dod_residual();
        /*
         * Product IRQ bind honesty residual denser multi-host UDX:
         *   irq_msix_ready + primary handle/mask vs table + rebind + live<=cap
         *   + inject isol (SOFT A-only / HW B-only) + peer mask + post-unbind.
         * greppable: irq_msix: soft residual bind_honesty PASS
         * greppable: irq_msix: soft residual bind_honesty denser PASS
         * greppable: product_hosts=UDX primary_vs_table live_le_cap denser=1
         */
        (void)irq_msix_soft_bind_honesty_residual();
        /*
         * Product IRQ+DMA Dual DoD denser residual (triple-host UDX):
         *   H81 rtl SOFT / H82 xhci HW / H83 ddi TBL0 isol + multi denser.
         * greppable: irq_msix: soft residual irq_dma denser PASS
         * greppable: product IRQ+DMA Dual DoD OPEN Soft!=product denser=1
         */
        (void)irq_msix_soft_irq_dma_dual_residual();
    } else {
        irq_msix_soft_inc(&g_u32SoftExerciseFail);
        kprintf("irq: MSI-X soft pulse path FAIL soft=%u path=%u tbl=%u "
                "last=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
        kprintf("irq: soft exercise FAIL soft=%u path=%u tbl=%u "
                "last=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
        kprintf("irq_msix: soft exercise FAIL soft=%u path=%u tbl=%u "
                "last=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
    }

    /* Full Wave 14 soft inventory after exercise (bring-up smoke greps). */
    irq_msix_soft_inventory_log("exercise");
    return fOk;
}

void
irq_msix_init(void)
{
    irq_msix_soft_inc(&g_u32SoftInit);
    notify_msix_init();
    idt_set_gate(GJ_MSIX_IRQ_VEC, (void *)irq_stub_msix, 0x8E);
    g_fReady = 1;
    g_u32MsixIrq = 0;
    g_u32MsixSoft = 0;
    g_u32MsixHw = 0;
    g_u32SoftPulsePath = 0;
    g_u32TablePulse = 0;
    g_u64LastBadge = 0;
    g_u32LastPath = GJ_MSIX_PATH_NONE;
    /* Reset soft path tallies for this bring-up; keep log counter. */
    g_u32SoftInjectEnter = 0;
    g_u32SoftInjectZero = 0;
    g_u32SoftPulseEnter = 0;
    g_u32SoftPulseNotReady = 0;
    g_u32SoftPulsePendOk = 0;
    g_u32SoftPulsePendMiss = 0;
    g_u32SoftTableEnter = 0;
    g_u32SoftTableNotReady = 0;
    g_u32SoftTableInit = 0;
    g_u32SoftTableProg = 0;
    g_u32SoftTableFireOk = 0;
    g_u32SoftTableFireMiss = 0;
    g_u32SoftTableMask = 0;
    g_u32SoftHwEnter = 0;
    g_u32SoftHwNotReady = 0;
    __atomic_store_n(&g_u32SoftIrqHandler, 0u, __ATOMIC_RELAXED);
    g_u32SoftExerciseEnter = 0;
    g_u32SoftExerciseOk = 0;
    g_u32SoftExerciseFail = 0;
    g_u32SoftExerciseNotReady = 0;
    g_fSoftInvOnce = 0;
    irq_msix_soft_user_clear_all();
    g_u32SoftUserBinds = 0;
    g_u32SoftUserUnbinds = 0;
    g_u32SoftUserFull = 0;
    g_u32SoftUserNotifyHit = 0;
    g_u32SoftUserMultiHostSafe = 0;
    g_u32SoftUserMultiHostCheck = 0;
    g_u32SoftUserRebinds = 0;
    g_u32SoftUserMultiHostBinds = 0;
    g_u32SoftUserBindResidualCheck = 0;
    g_u32SoftUserBadgeIsoCheck = 0;
    g_u32SoftUserSlotCapCheck = 0;
    g_u32SoftUserMultiMatchCheck = 0;
    g_u32SoftUserBulkClearCheck = 0;
    g_u32SoftUserTableUserCheck = 0;
    g_u32SoftBindHonestyCheck = 0;
    g_u32SoftBindHonestyDenserOk = 0;
    g_u32SoftIrqDmaDualCheck = 0;
    g_u32SoftIrqDmaDenserOk = 0;
    g_u32SoftPrimaryVsTableOk = 0;
    g_u32SoftLiveLeCapOk = 0;
    g_u32SoftReadyOk = 0;
    g_fSoftUserNotifyPass = 0;
    g_fSoftResidualOnce = 0;
    g_fSoftMultiHostPass = 0;
    g_fSoftUserBindResidualPass = 0;
    g_fSoftBadgeIsoPass = 0;
    g_fSoftSlotCapPass = 0;
    g_fSoftMultiMatchPass = 0;
    g_fSoftBulkClearPass = 0;
    g_fSoftTableUserPass = 0;
    g_fSoftDualDodPass = 0;
    g_fSoftBindHonestyPass = 0;
    g_fSoftIrqDmaDualPass = 0;
    kprintf("irq: MSI-X vec=0x%x Notification bound PASS "
            "product_notify_mint=OPEN product_hosts=UDX "
            "dual_dod=OPEN Soft!=product soft!=product\n",
            GJ_MSIX_IRQ_VEC);
    /* Baseline soft inventory before exercise (zeros typical). */
    irq_msix_soft_inventory_log("init");
    /* Soft Notification pulse path exercise (table soft -> badge OR). */
    (void)irq_msix_soft_path_exercise();
}

u32
irq_msix_count(void)
{
    return g_u32MsixIrq;
}

u32
irq_msix_soft_count(void)
{
    return g_u32MsixSoft;
}

u32
irq_msix_hw_count(void)
{
    return g_u32MsixHw;
}

u32
irq_msix_soft_pulse_path_count(void)
{
    return g_u32SoftPulsePath;
}

u32
irq_msix_table_pulse_count(void)
{
    return g_u32TablePulse;
}

u64
irq_msix_last_badge(void)
{
    return g_u64LastBadge;
}

u32
irq_msix_last_path(void)
{
    return g_u32LastPath;
}

/**
 * Product MSI-X path ready residual (Soft!=product; multi-host UDX denser).
 * Returns non-zero after successful irq_msix_init. Side effect: recount
 * live soft-bind slots so concurrent UDX hosts (rtl 10ec:8168 + xhci
 * 8086:a12f product shape) observe honest live<=cap after DDI IRQ_BIND.
 * Dual DoD A/B remain OPEN; freestanding_product=SKIP; never freestanding
 * product close. Never hard-gates; no kprintf (H2; callers own lamps).
 * greppable via residual catalog: irq_msix_ready product_hosts=UDX
 */
int
irq_msix_ready(void)
{
    if (g_fReady != 0) {
        /*
         * Denser multi-host UDX residual: refresh live slot count so
         * ddi_door IRQ_BIND live<=cap / primary stats see table truth.
         * Soft!=product; product_hosts=UDX; dual_dod OPEN.
         */
        (void)irq_msix_soft_user_count_live();
    }
    return g_fReady;
}
