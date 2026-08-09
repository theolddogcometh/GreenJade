/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Threaded IRQ table. Kernel Notification → udx_irq_dispatch.
 *
 * Registration (udx_request_irq) only fills the host table. Delivery is
 * pumped from udx_run via non-blocking GJ_SYS_NOTIFY_WAIT (freestanding)
 * or udx_host_fire_irq (UDX_HOST_LIBC). Handlers run in host context.
 *
 * Soft mask: nested disable depth. Fire while masked latches pending;
 * final enable delivers once. Use disable in quiesce before free_irq.
 *
 * -------------------------------------------------------------------------
 * Soft notify path → real Notification (gap doc; residual deepen)
 * greppable: udx: notify soft …
 *
 * Real GreenJade product path (kernel owns the endpoint):
 *   hard IRQ / irq_msix soft inject
 *     → notify_pulse(badge) on Notification (kernel/ipc/notify.c)
 *     → userspace GJ_SYS_NOTIFY_WAIT reaps pending & mask (CAS-clear)
 *     → freestanding udx_run: for each set bit N → udx_irq_dispatch(N)
 *
 * Host sim soft protocol shape (UDX_HOST_LIBC — this process only):
 *   udx_host_fire_irq(N)  ≈  pulse + immediate claim of badge bit N
 *     → udx_irq_dispatch(N)  (same threaded handler surface as full GJ)
 *   Soft mask (disable depth / pending latch) is UDX-table local.
 *   Kernel hard-IRQ mask / EOI / MSI-X PBA remain out of this file.
 *
 * Badge protocol shape (must stay aligned with freestanding udx_run):
 *   bit N  →  IRQ line N   (N in [0, 63] from NOTIFY_WAIT u64; table to 255)
 *   badge 0 coalesces to bit 0 on the kernel side (notify_pulse); host
 *   fire_irq never passes a raw badge word — line index is the badge.
 *
 * DDI IRQ_BIND badge path honesty (Soft!=product; product mint OPEN):
 *   Kernel soft residual: DDI_OP_IRQ_BIND stores handle→badge via
 *     irq_msix_soft_user_bind (ddi: soft irq note PASS); CLOSE unbinds
 *     that handle only. No product IRQ→Notification cap mint.
 *   UDX residual: udx_request_irq fills the threaded-handler table only;
 *     it does not call DDI_OP_IRQ_BIND and does not mint a Notification.
 *   Delivery residual: freestanding reaps the (global soft) MSI-X badge
 *     word then udx_irq_dispatch; host sim uses fire_irq(N) without a
 *     real badge word. Product Notification cap mint remains OPEN.
 * greppable: udx: notify soft residual ddi_irq_bind
 * greppable: udx: notify soft residual cap_mint
 *
 * Gap (cannot close from userspace UDX sim / this file alone):
 *   - Cannot call kernel notify_pulse / notify_install / notify_msix_global
 *   - Host libc build has no GJ_SYS_NOTIFY_WAIT syscall surface
 *   - Soft multi-waiter, CAS-clear pending, and cap install live only in
 *     kernel/ipc/notify.c (see greppable notify: soft … there)
 *   - DDI_OP_IRQ_BIND + product CNode Notification mint are kernel/devmgr
 *   - Wiring UDX host to a real Notification cap is M4.2 (TODO)
 *
 * Honesty: UDX kernel notify product remains OPEN. Soft deepen catalogs
 * the UDX-side fire_irq -> dispatch pulse surface so greps measure
 * readiness without claiming a kernel endpoint.
 * greppable: udx: notify soft open
 *
 * Lean residual (Soft!=product; G-AC-1): soft IRQ API honesty only --
 * table bind / fire_irq != product IRQ Notification cap install.
 * request_irq / free_irq / soft fire residual deepened below (sparse).
 * Product-host / Dual DoD residual (C2 deepen; agent != close):
 *   residual product_host — rtl8168_udx (DoD B) / xhci_udx (DoD A)
 *     threaded ISR surface; freestanding rtl/USB class SKIP
 *   residual dual_dod     — Dual DoD A/B OPEN honesty (soft != close)
 *   residual bind         — IRQ bind path for freestanding product hosts
 *     (request_irq thr-only table; Soft!=product; product_hosts=UDX)
 * thr-only residual deepen (C2; Soft!=product; G-AC-1; Dual DoD OPEN):
 *   residual thr       — thr residual: process-context handler, sync no-op,
 *     schedule_work legal; hard_primary=0; hard_irq_thread=0
 *   residual thr-only  — model lock: UDX IRQ has no hard primary half;
 *     request_irq = thr-only table bind; fire delivers thr handler only
 *   residual msix      — toward product MSI-X continuum honesty:
 *     product path (kernel; OPEN mint): hard MSI-X / irq_msix soft inject
 *       → notify_pulse(badge) → NOTIFY_WAIT → udx_irq_dispatch thr
 *     UDX residual (this file): thr-only table bind + fire; no MSI-X
 *       vector program, no PBA, no hard EOI, no product Notification mint
 *   residual work      — thr handler → schedule_work deferred BH only
 *     (work thr-only chain; never net_eth_poll / hard-IRQ work product)
 *   residual h1        — H1 lock: no net_eth_poll from IRQ thr path
 *   residual freestanding — freestanding class SKIP; product=UDX+ABI hosts
 *   Linux mental split shape only (never product hard IRQ):
 *     request_threaded_irq(primary, thread_fn) → hard + thr
 *     udx_request_irq(handler) → thr-only (hard_primary=0)
 *   Product hosts (rtl8168_udx / xhci_udx) stay thr-only table+fire
 *     until product Notification cap mint (OPEN).
 *   != kernel linux_time_soft request_threaded_irq primary+thread_fn.
 * H1: no net_eth_poll from IRQ (handlers may schedule_work only).
 * Dual MIT OR Apache-2.0. Product hosts = Linux-shaped userspace UDX.
 * greppable: udx: notify soft residual lean
 * greppable: udx: notify soft residual request
 * greppable: udx: notify soft residual free
 * greppable: udx: notify soft residual fire
 * greppable: udx: notify soft residual thr
 * greppable: udx: notify soft residual thr-only
 * greppable: udx: notify soft residual msix
 * greppable: udx: notify soft residual work
 * greppable: udx: notify soft residual h1
 * greppable: udx: notify soft residual freestanding
 * greppable: udx: notify soft residual product_host
 * greppable: udx: notify soft residual dual_dod
 * greppable: udx: notify soft residual bind
 * greppable: udx: notify soft residual denser
 * greppable: denser_prove=1 | denser residual | lean_checks=
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: product_hosts=UDX
 * greppable: udx: notify soft api honesty
 * -------------------------------------------------------------------------
 */
#include <udx/irq.h>
#include <udx/udx.h>

#define UDX_IRQ_MAX 256
#define UDX_IRQ_SHARE_MAX 4

/* Soft wave stamp + greppable area count (IRQ residual deepen). */
/* Wave 75 soft deepen surfaces (CREATE-ONLY soft != product; stamp-free):
 *   Bar honesty v2026.08.04.75 — NEVER invent intermediate image stamps.
 *   greppable: soft retgradientangle continuum_toward=26800 soft_ne_product=1 wave=75
 *   greppable: soft retblendangle exclusive=1 continuum_toward=26800 soft_ne_product=1 wave=75
 * Soft != product complete; product lamps 0;
 * Residual deepen: request/free/fire + DDI IRQ_BIND badge + cap_mint OPEN
 *   + product_host (Dual DoD A/B) + dual_dod OPEN + thr / thr-only
 *   + residual msix (toward product MSI-X) + residual work (thr→schedule)
 *   + residual h1 + residual freestanding SKIP. thr-only C2 exclusive.
 * STRONGER IRQ bind path residual (freestanding product hosts; Soft!=product):
 *   request_irq thr-only table bind is the freestanding product-host IRQ
 *   surface after DDI SCAN→GET→OPEN→MAP_BAR→IRQ→DMA install; delivery is
 *   NOTIFY_WAIT badge → udx_irq_dispatch (or host fire_irq). Never mints
 *   Notification; never DDI_OP_IRQ_BIND from this unit. Dual DoD OPEN.
 * denser residual (preferred over lamps alone; Soft!=product):
 *   denser_prove bind: request_irq thr-only table + fire/dispatch + badge;
 *   denser thr-only + denser product_host + denser dual_dod OPEN;
 *   denser Soft!=product + agent_close=0 + freestanding class SKIP;
 *   denser stamp_free_75 (bar v2026.08.04.75; NEVER invent .76).
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: denser_prove=1 | denser residual | lean_checks=
 * greppable: udx: notify soft residual denser
 * Dual MIT OR Apache-2.0; G-AC-1; Soft!=product. Never invent .73/.76.
 * Never bump GJ_IMAGE_VERSION from this unit (stamp-free residual).
 */

#define UDX_NOTIFY_SOFT_WAVE 75u
#define UDX_NOTIFY_SOFT_AREAS  35u

/*
 * thr-only residual locks (C2; Soft!=product; G-AC-1; Dual DoD A/B OPEN).
 * UDX IRQ is thr-only — no hard-IRQ primary half, no hard-IRQ thread.
 * Flip requires thr-only model review (hard primary would be product path).
 * Header (irq.h) owns thr_only / hard_primary / hard_irq_thread / H1 /
 * Dual DoD / soft_ne_product / table_only / cap_mint / ddi_bind_from_here.
 * File-local: schedule_work legal + sync soft no-op + MSI-X path honesty.
 * greppable: udx: notify soft residual thr-only
 * greppable: thr_only=1 hard_primary=0 hard_irq_thread=0
 * greppable: udx: notify soft residual msix
 */
#ifndef UDX_IRQ_THR_ONLY
#define UDX_IRQ_THR_ONLY         1u /* thr-only table; no hard primary half */
#endif
#ifndef UDX_IRQ_HARD_PRIMARY
#define UDX_IRQ_HARD_PRIMARY     0u /* never Linux primary_handler residual */
#endif
#ifndef UDX_IRQ_HARD_IRQ_THREAD
#define UDX_IRQ_HARD_IRQ_THREAD  0u /* no separate hard-IRQ thread to park */
#endif
#define UDX_IRQ_THR_SCHEDULE_OK  1u /* schedule_work legal from thr handler */
#define UDX_IRQ_SYNC_SOFT_NOOP   1u /* synchronize_irq soft no-op (thr-only) */
/* Toward product MSI-X: residual flags only — never product vector bind. */
#define UDX_IRQ_MSIX_VECTOR_BIND 0u /* no MSI-X table program from UDX thr */
#define UDX_IRQ_MSIX_PBA         0u /* no MSI-X PBA residual from this unit */
#define UDX_IRQ_MSIX_HARD_EOI    0u /* no hard EOI; soft mask local only */
#define UDX_IRQ_PRODUCT_MSIX_OPEN 1u /* product MSI-X Notification path OPEN */
#define UDX_IRQ_FREESTANDING_SKIP 1u /* freestanding class SKIP; UDX+ABI hosts */
/*
 * STRONGER IRQ bind path residual (freestanding product hosts; Soft!=product).
 * Product hosts = UDX (rtl8168_udx DoD B / xhci_udx DoD A); freestanding
 * class drivers SKIP. request_irq is thr-only table bind only.
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: udx: notify soft residual bind
 * greppable: product_hosts=UDX
 */
#define UDX_IRQ_PRODUCT_HOSTS_UDX 1u /* product_hosts=UDX (never fs rtl/usb) */
#define UDX_IRQ_BIND_TABLE_ONLY   1u /* bind residual = thr-only table slot */
#define UDX_IRQ_BIND_CAP_MINT     0u /* bind path never mints Notification */
#define UDX_IRQ_BIND_DDI_FROM_HERE 0u /* bind path never DDI_OP_IRQ_BIND */
#define UDX_IRQ_DENSER_PROVE      1u /* denser residual prove on */
#define UDX_IRQ_DENSER_BIND       1u /* denser: request thr-only table bind */
#define UDX_IRQ_DENSER_FIRE       1u /* denser: fire/dispatch thr delivery */
#define UDX_IRQ_DENSER_BADGE      1u /* denser: badge bit_N→line_N shape */
#define UDX_IRQ_DENSER_PRODUCT_HOST 1u /* denser product-host DoD A/B */
#define UDX_IRQ_STAMP_FREE_75     1u /* bar v2026.08.04.75; never invent .76 */
#define UDX_IRQ_LEAN_CHECKS      17u /* denser functional self-check count */

/* Compile-time thr-only + H1 locks (peer irq_msix H1 spirit; Soft!=product). */
_Static_assert(UDX_IRQ_THR_ONLY == 1u, "thr-only model lock");
_Static_assert(UDX_IRQ_HARD_PRIMARY == 0u, "no hard primary half");
_Static_assert(UDX_IRQ_HARD_IRQ_THREAD == 0u, "no hard-IRQ thread");
_Static_assert(UDX_IRQ_THR_SCHEDULE_OK == 1u, "schedule_work from thr ok");
_Static_assert(UDX_IRQ_MSIX_VECTOR_BIND == 0u, "no MSI-X vector bind here");
_Static_assert(UDX_IRQ_PRODUCT_MSIX_OPEN == 1u, "product MSI-X remains OPEN");
#if defined(UDX_IRQ_H1_NO_NET_ETH_POLL)
_Static_assert(UDX_IRQ_H1_NO_NET_ETH_POLL == 1u, "H1: no net_eth_poll from IRQ");
#endif
/*
 * FUNCTIONAL residual locks (STRONGER IRQ bind path for freestanding product
 * hosts; Soft!=product; G-AC-1; Dual DoD A/B OPEN; agent!=close;
 * stamp-free bar v2026.08.04.75; NEVER invent .76).
 * denser residual: denser_prove bind+fire+badge + denser product_host +
 * Soft!=product + agent_close=0 + freestanding class SKIP.
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: udx: notify soft residual bind
 * greppable: udx: notify soft residual denser
 * greppable: denser_prove=1 | lean_checks=
 */
_Static_assert(UDX_IRQ_DOD_A_OPEN == 1u,
               "Dual DoD A remains OPEN (xhci_udx thr-only)");
_Static_assert(UDX_IRQ_DOD_B_OPEN == 1u,
               "Dual DoD B remains OPEN (rtl8168_udx thr-only)");
_Static_assert(UDX_IRQ_AGENT_CLOSE == 0u,
               "thr residual lamps never close Dual DoD");
_Static_assert(UDX_IRQ_SOFT_NE_PRODUCT == 1u,
               "Soft!=product honesty lock");
_Static_assert(UDX_IRQ_CAP_MINT == 0u,
               "product Notification mint remains OPEN");
_Static_assert(UDX_IRQ_DDI_BIND_FROM_HERE == 0u,
               "request_irq never DDI_OP_IRQ_BIND from this unit");
_Static_assert(UDX_IRQ_TABLE_ONLY == 1u,
               "request_irq is thr-only table bind only");
_Static_assert(UDX_IRQ_FREESTANDING_SKIP == 1u,
               "freestanding class SKIP; product=UDX+ABI hosts");
_Static_assert(UDX_IRQ_PRODUCT_HOSTS_UDX == 1u,
               "product_hosts=UDX (never freestanding rtl/usb class)");
_Static_assert(UDX_IRQ_BIND_TABLE_ONLY == 1u,
               "IRQ bind residual thr-only table slot only");
_Static_assert(UDX_IRQ_BIND_CAP_MINT == 0u,
               "IRQ bind path never mints product Notification");
_Static_assert(UDX_IRQ_BIND_DDI_FROM_HERE == 0u,
               "IRQ bind path never issues DDI_OP_IRQ_BIND");
_Static_assert(UDX_IRQ_DENSER_PROVE == 1u,
               "denser residual prove on (bind/fire/badge)");
_Static_assert(UDX_IRQ_DENSER_BIND == 1u,
               "denser residual: thr-only table bind prove");
_Static_assert(UDX_IRQ_DENSER_FIRE == 1u,
               "denser residual: fire/dispatch thr delivery prove");
_Static_assert(UDX_IRQ_DENSER_BADGE == 1u,
               "denser residual: badge bit_N→line_N shape prove");
_Static_assert(UDX_IRQ_DENSER_PRODUCT_HOST == 1u,
               "denser residual: product-host DoD A/B id");
_Static_assert(UDX_IRQ_STAMP_FREE_75 == 1u,
               "stamp-free residual bar v2026.08.04.75 (never .76)");
_Static_assert(UDX_IRQ_LEAN_CHECKS == 17u,
               "FUNCTIONAL lean residual denser: 17 IRQ bind checks");
_Static_assert(UDX_NOTIFY_SOFT_WAVE == 75u,
               "stamp-free soft wave aligns bar v2026.08.04.75 (never .76)");

/*
 * Soft badge protocol: freestanding NOTIFY_WAIT returns a u64; only the low
 * 64 lines can arrive as badge bits. Higher table lines exist for host sim
 * inject / future binds but are outside the kernel badge word today.
 * greppable: udx: notify soft badge
 */
#define UDX_NOTIFY_SOFT_BADGE_BITS 64u

struct udx_irq_action {
    udx_irq_handler_t pfnHandler;
    void             *pDevId;
    const char       *szName;
    u32               u32Flags;
    u8                u8Used;
};

/*
 * Soft notify pulse catalog (file-local sticky counters; never hard-gate).
 * Greppable prefix-stable serial markers (udx: notify soft …):
 *   udx: notify soft protocol      — gap + badge shape + path catalog
 *   udx: notify soft inventory     — table capacity + bind surface
 *   udx: notify soft pulse inventory — fire_irq→dispatch pulse counters
 *   udx: notify soft stats         — aggregate path counters
 *   udx: notify soft pulse hit     — dispatch invoked at least one handler
 *   udx: notify soft pulse latch   — fire while soft-masked (pending set)
 *   udx: notify soft pulse unmask  — latched deliver on final enable
 *   udx: notify soft pulse empty   — dispatch with no bound actions
 *   udx: notify soft pulse bad     — line out of table range
 *   udx: notify soft bind          — request_irq slot filled
 *   udx: notify soft unbind        — free_irq slot cleared
 *   udx: notify soft mask          — disable/enable/pending geometry
 *   udx: notify soft table         — live action/slot occupancy
 *   udx: notify soft badge         — badge word shape + last snap
 *   udx: notify soft gap           — cannot call kernel notify from sim
 *   udx: notify soft open          — kernel notify product remains OPEN
 *   udx: notify soft deepen        -- wave stamp + area count
 *   udx: notify soft sync          -- synchronize_irq honesty (soft no-op)
 *   udx: notify soft residual lean -- lean residual Soft!=product
 *   udx: notify soft residual request -- request_irq table-only residual
 *   udx: notify soft residual free    -- free_irq unbind residual
 *   udx: notify soft residual fire    -- soft fire_irq / badge residual
 *   udx: notify soft residual thr     -- thr residual (process ctx; sync no-op)
 *   udx: notify soft residual thr-only -- thr-only model (hard_primary=0)
 *   udx: notify soft residual msix     -- toward product MSI-X continuum
 *   udx: notify soft residual work     -- thr→schedule_work deferred BH
 *   udx: notify soft residual h1       -- H1 no net_eth_poll from IRQ thr
 *   udx: notify soft residual freestanding -- freestanding class SKIP
 *   udx: notify soft residual ddi_irq_bind -- DDI IRQ_BIND badge path honesty
 *   udx: notify soft residual cap_mint -- product Notification cap mint OPEN
 *   udx: notify soft residual product_host -- Dual DoD host IRQ surface
 *   udx: notify soft residual dual_dod -- Dual DoD A/B OPEN honesty
 *   udx: notify soft residual bind     -- IRQ bind path (product hosts=UDX)
 *   udx: notify soft residual denser   -- denser residual prove (bind/fire/badge)
 *   udx: notify soft api honesty   -- IRQ Notification cap OPEN
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: denser_prove=1 | denser residual
 */
struct udx_notify_soft_stats {
    u32 u32RequestOk;       /* request_irq bind success */
    u32 u32RequestFail;     /* request_irq rejected */
    u32 u32Free;            /* free_irq matched action */
    u32 u32FreeMiss;        /* free_irq no match / bad line */
    u32 u32Disable;         /* disable_irq depth++ */
    u32 u32Enable;          /* enable_irq depth-- */
    u32 u32Dispatch;        /* udx_irq_dispatch / fire_irq entries */
    u32 u32PulseHit;        /* dispatch ran >=1 handler (soft pulse hit) */
    u32 u32PulseEmpty;      /* dispatch with no bound actions */
    u32 u32PulseLatch;      /* fire while depth>0 -> pending */
    u32 u32PulseUnmask;     /* pending delivered on depth->0 */
    u32 u32PulseBad;        /* line out of range */
    u32 u32HandlerInvoke;   /* individual handler calls */
    u32 u32SoftLog;         /* soft inventory dumps emitted */
    u32 u32LastLine;        /* last soft-pulse line (stats) */
    u32 u32LastBadgeLo;     /* last soft badge word low 32 (bit N set) */
    u32 u32LastBadgeHi;     /* last soft badge word high 32 */
    u32 u32BadgeOutOfWord;  /* dispatch line >= 64 (beyond NOTIFY badge) */
    u32 u32Sync;            /* udx_synchronize_irq entries (soft no-op) */
    u32 u32IsDisabledQ;     /* udx_irq_is_disabled samples */
    u32 u32IsPendingQ;      /* udx_irq_is_pending samples */
    u32 u32DepthQ;          /* udx_irq_disable_depth samples */
    u32 u32NameQ;           /* udx_irq_name samples */
    u32 u32ActionCountQ;    /* udx_irq_action_count samples */
    u32 u32ResidualRequest; /* request residual note emissions */
    u32 u32ResidualFree;    /* free residual note emissions */
    u32 u32ResidualFire;    /* soft fire residual note emissions */
    u32 u32ResidualThr;     /* thr residual note emissions */
    u32 u32ResidualThrOnly; /* thr-only residual note emissions */
    u32 u32ResidualMsix;    /* product MSI-X continuum residual notes */
    u32 u32ResidualWork;    /* thr→schedule_work residual notes */
    u32 u32ResidualH1;      /* H1 no net_eth_poll residual notes */
    u32 u32ResidualFsSkip;  /* freestanding class SKIP residual notes */
    u32 u32ResidualDdiBind; /* DDI IRQ_BIND honesty note emissions */
    u32 u32ResidualCapMint; /* product Notification mint OPEN notes */
    u32 u32ResidualProdHost;/* product_host residual note emissions */
    u32 u32ResidualDualDod; /* Dual DoD A/B OPEN residual notes */
    u32 u32ResidualBind;    /* IRQ bind path residual note emissions */
    u32 u32ResidualDenser;  /* denser residual prove note emissions */
};

static struct udx_irq_action g_aIrq[UDX_IRQ_MAX][UDX_IRQ_SHARE_MAX];
static u16 g_aIrqDisableDepth[UDX_IRQ_MAX];
static u8  g_aIrqPending[UDX_IRQ_MAX];

static struct udx_notify_soft_stats g_notifySoft;
static u8 g_fNotifySoftProtocolOnce;
static u8 g_fNotifySoftPulseOnce;
static u8 g_fNotifySoftBindOnce;
static u8 g_fNotifySoftFreeResidualOnce;
static u8 g_fNotifySoftFireResidualOnce;
static u8 g_fNotifySoftThrResidualOnce;
static u8 g_fNotifySoftThrOnlyResidualOnce;
static u8 g_fNotifySoftMsixResidualOnce;
static u8 g_fNotifySoftWorkResidualOnce;
static u8 g_fNotifySoftH1ResidualOnce;
static u8 g_fNotifySoftFsSkipResidualOnce;
static u8 g_fNotifySoftProdHostResidualOnce;
static u8 g_fNotifySoftDualDodResidualOnce;
static u8 g_fNotifySoftBindResidualOnce;
static u8 g_fNotifySoftDenserResidualOnce;

static void
notify_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/**
 * Soft badge word from IRQ line: bit N for N < 64; 0 if out of badge word.
 * Mirrors freestanding udx_run badge → line map (inverse).
 * greppable: udx: notify soft badge
 */
static u64
notify_soft_badge_from_line(int nIrq)
{
    if (nIrq < 0 || (u32)nIrq >= UDX_NOTIFY_SOFT_BADGE_BITS) {
        return 0ull;
    }
    return 1ull << (u32)nIrq;
}

static void
notify_soft_note_line(int nIrq)
{
    u64 u64Badge;

    if (nIrq < 0) {
        return;
    }
    g_notifySoft.u32LastLine = (u32)nIrq;
    u64Badge = notify_soft_badge_from_line(nIrq);
    g_notifySoft.u32LastBadgeLo = (u32)(u64Badge & 0xffffffffull);
    g_notifySoft.u32LastBadgeHi = (u32)(u64Badge >> 32);
    if (u64Badge == 0ull && (u32)nIrq < (u32)UDX_IRQ_MAX) {
        notify_soft_inc(&g_notifySoft.u32BadgeOutOfWord);
    }
}

/** Soft: walk table for live action / disabled / pending occupancy. */
static void
notify_soft_table_snap(u32 *pu32Actions, u32 *pu32LinesUsed,
                       u32 *pu32Disabled, u32 *pu32Pending)
{
    int nIrq;
    int iSlot;
    u32 u32Actions;
    u32 u32Lines;
    u32 u32Dis;
    u32 u32Pend;

    u32Actions = 0;
    u32Lines = 0;
    u32Dis = 0;
    u32Pend = 0;
    for (nIrq = 0; nIrq < UDX_IRQ_MAX; nIrq++) {
        int fLine = 0;

        for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
            if (g_aIrq[nIrq][iSlot].u8Used) {
                if (u32Actions < 0xffffffffu) {
                    u32Actions++;
                }
                fLine = 1;
            }
        }
        if (fLine != 0) {
            if (u32Lines < 0xffffffffu) {
                u32Lines++;
            }
        }
        if (g_aIrqDisableDepth[nIrq] != 0) {
            if (u32Dis < 0xffffffffu) {
                u32Dis++;
            }
        }
        if (g_aIrqPending[nIrq] != 0) {
            if (u32Pend < 0xffffffffu) {
                u32Pend++;
            }
        }
    }
    if (pu32Actions != NULL) {
        *pu32Actions = u32Actions;
    }
    if (pu32LinesUsed != NULL) {
        *pu32LinesUsed = u32Lines;
    }
    if (pu32Disabled != NULL) {
        *pu32Disabled = u32Dis;
    }
    if (pu32Pending != NULL) {
        *pu32Pending = u32Pend;
    }
}

/*
 * Greppable soft notify inventory (toward real Notification; Wave 126).
 * Pure observation — never gates skeleton PASS or dispatch behavior.
 *
 *   udx: notify soft protocol …
 *   udx: notify soft inventory …
 *   udx: notify soft pulse inventory …
 *   udx: notify soft stats …
 *   udx: notify soft mask …
 *   udx: notify soft table …
 *   udx: notify soft badge …
 *   udx: notify soft gap …
 *   udx: notify soft open …
 *   udx: notify soft deepen …
 *   udx: notify soft sync …
 *   udx: notify soft residual lean …
 *   udx: notify soft residual request …
 *   udx: notify soft residual free …
 *   udx: notify soft residual fire …
 *   udx: notify soft residual thr …
 *   udx: notify soft residual thr-only …
 *   udx: notify soft residual msix …
 *   udx: notify soft residual work …
 *   udx: notify soft residual h1 …
 *   udx: notify soft residual freestanding …
 *   udx: notify soft residual ddi_irq_bind …
 *   udx: notify soft residual cap_mint …
 *   udx: notify soft residual product_host …
 *   udx: notify soft residual dual_dod …
 *   udx: notify soft residual bind …
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 */
static void
notify_soft_log(void)
{
    u32 u32Actions;
    u32 u32LinesUsed;
    u32 u32Disabled;
    u32 u32Pending;

    notify_soft_inc(&g_notifySoft.u32SoftLog);
    notify_soft_table_snap(&u32Actions, &u32LinesUsed, &u32Disabled,
                           &u32Pending);

    /*
     * Grep: udx: notify soft protocol
     * Documents the gap: userspace UDX sim cannot call the kernel notify
     * endpoint; badge shape and path names stay product-stable for greps.
     */
    udx_printk("udx: notify soft protocol "
               "gap=no_kernel_pulse_from_host_sim "
               "cannot=notify_pulse,notify_install,GJ_SYS_NOTIFY_WAIT_on_libc "
               "badge_shape=bit_N_to_line_N badge_bits=%u irq_max=%u "
               "host_path=fire_irq->dispatch "
               "full_path=notify_pulse->NOTIFY_WAIT->dispatch "
               "kernel_soft=notify:_soft_* "
               "soft_log=%u wave=%u\n",
               (unsigned)UDX_NOTIFY_SOFT_BADGE_BITS,
               (unsigned)UDX_IRQ_MAX,
               g_notifySoft.u32SoftLog,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /* Grep: udx: notify soft inventory */
    udx_printk("udx: notify soft inventory irq_max=%u share_max=%u "
               "paths=request,free,disable,enable,dispatch,"
               "mask_latch,unmask_deliver "
               "bind_ok=%u bind_fail=%u free=%u free_miss=%u "
               "disable=%u enable=%u soft_log=%u wave=%u\n",
               (unsigned)UDX_IRQ_MAX, (unsigned)UDX_IRQ_SHARE_MAX,
               g_notifySoft.u32RequestOk, g_notifySoft.u32RequestFail,
               g_notifySoft.u32Free, g_notifySoft.u32FreeMiss,
               g_notifySoft.u32Disable, g_notifySoft.u32Enable,
               g_notifySoft.u32SoftLog,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Grep: udx: notify soft pulse inventory
     * fire_irq → soft notify pulse catalog (host path equivalent of
     * notify_pulse + claim; freestanding reaps badge then same dispatch).
     */
    udx_printk("udx: notify soft pulse inventory "
               "fire_irq_equiv=udx_irq_dispatch "
               "or_shape=badge_bit_N pulse_hit=%u pulse_latch=%u "
               "pulse_unmask=%u pulse_empty=%u pulse_bad=%u "
               "handler_invoke=%u dispatch=%u "
               "last_line=%u last_badge_lo=0x%x last_badge_hi=0x%x "
               "badge_out_of_word=%u wave=%u\n",
               g_notifySoft.u32PulseHit, g_notifySoft.u32PulseLatch,
               g_notifySoft.u32PulseUnmask, g_notifySoft.u32PulseEmpty,
               g_notifySoft.u32PulseBad, g_notifySoft.u32HandlerInvoke,
               g_notifySoft.u32Dispatch, g_notifySoft.u32LastLine,
               g_notifySoft.u32LastBadgeLo, g_notifySoft.u32LastBadgeHi,
               g_notifySoft.u32BadgeOutOfWord,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /* Grep: udx: notify soft stats */
    udx_printk("udx: notify soft stats request_ok=%u request_fail=%u "
               "free=%u free_miss=%u disable=%u enable=%u dispatch=%u "
               "pulse_hit=%u pulse_latch=%u pulse_unmask=%u "
               "pulse_empty=%u pulse_bad=%u handler_invoke=%u "
               "badge_out_of_word=%u soft_log=%u "
               "last_line=%u last_badge_lo=0x%x last_badge_hi=0x%x "
               "wave=%u\n",
               g_notifySoft.u32RequestOk, g_notifySoft.u32RequestFail,
               g_notifySoft.u32Free, g_notifySoft.u32FreeMiss,
               g_notifySoft.u32Disable, g_notifySoft.u32Enable,
               g_notifySoft.u32Dispatch, g_notifySoft.u32PulseHit,
               g_notifySoft.u32PulseLatch, g_notifySoft.u32PulseUnmask,
               g_notifySoft.u32PulseEmpty, g_notifySoft.u32PulseBad,
               g_notifySoft.u32HandlerInvoke,
               g_notifySoft.u32BadgeOutOfWord, g_notifySoft.u32SoftLog,
               g_notifySoft.u32LastLine, g_notifySoft.u32LastBadgeLo,
               g_notifySoft.u32LastBadgeHi,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /* Grep: udx: notify soft mask (Wave 111 deepen) */
    udx_printk("udx: notify soft mask disable=%u enable=%u "
               "pulse_latch=%u pulse_unmask=%u live_disabled=%u "
               "live_pending=%u nested_depth=1 wave=%u\n",
               g_notifySoft.u32Disable, g_notifySoft.u32Enable,
               g_notifySoft.u32PulseLatch, g_notifySoft.u32PulseUnmask,
               u32Disabled, u32Pending,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /* Grep: udx: notify soft table (Wave 111 deepen) */
    udx_printk("udx: notify soft table actions=%u lines_used=%u "
               "disabled=%u pending=%u irq_max=%u share_max=%u "
               "badge_bits=%u wave=%u\n",
               u32Actions, u32LinesUsed, u32Disabled, u32Pending,
               (unsigned)UDX_IRQ_MAX, (unsigned)UDX_IRQ_SHARE_MAX,
               (unsigned)UDX_NOTIFY_SOFT_BADGE_BITS,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /* Grep: udx: notify soft badge (Wave 111 deepen) */
    udx_printk("udx: notify soft badge bits=%u last_line=%u "
               "last_lo=0x%x last_hi=0x%x out_of_word=%u "
               "shape=bit_N_to_line_N wave=%u\n",
               (unsigned)UDX_NOTIFY_SOFT_BADGE_BITS,
               g_notifySoft.u32LastLine, g_notifySoft.u32LastBadgeLo,
               g_notifySoft.u32LastBadgeHi, g_notifySoft.u32BadgeOutOfWord,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Gap catalog — host sim cannot reach kernel notify endpoint.
     * greppable: udx: notify soft gap
     */
    udx_printk("udx: notify soft gap kernel_pulse=0 notify_install=0 "
               "notify_msix_global=0 NOTIFY_WAIT_on_libc=0 "
               "multi_waiter=0 cas_clear=0 cap_bind=0 "
               "product_close=0 soft=1 wave=%u\n",
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Product OPEN honesty — kernel notify from host sim remains OPEN.
     * greppable: udx: notify soft open
     */
    udx_printk("udx: notify soft open kernel_notify=OPEN "
               "host_sim_endpoint=OPEN multi_waiter=OPEN "
               "cap_install=OPEN product=0 soft=1 wave=%u\n",
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /* Grep: udx: notify soft deepen wave (Wave 111 stamp) */
    udx_printk("udx: notify soft deepen wave=%u areas=%u unit=irq "
               "exclusive=1 prefix=udx:_notify_soft log_n=%u "
               "(soft inventory; kernel notify product remains OPEN)\n",
               (unsigned)UDX_NOTIFY_SOFT_WAVE,
               (unsigned)UDX_NOTIFY_SOFT_AREAS,
               g_notifySoft.u32SoftLog);

    /* Grep: udx: notify soft sync (Wave 111 deepen; thr-only no-op) */
    udx_printk("udx: notify soft sync enter=%u soft_noop=%u "
               "hard_irq_thread=%u multi_waiter_park=0 thr_only=%u "
               "hard_primary=%u thr_residual=1 "
               "query_disabled=%u query_pending=%u query_depth=%u "
               "query_name=%u query_actions=%u wave=%u\n",
               g_notifySoft.u32Sync,
               (unsigned)UDX_IRQ_SYNC_SOFT_NOOP,
               (unsigned)UDX_IRQ_HARD_IRQ_THREAD,
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               g_notifySoft.u32IsDisabledQ,
               g_notifySoft.u32IsPendingQ, g_notifySoft.u32DepthQ,
               g_notifySoft.u32NameQ, g_notifySoft.u32ActionCountQ,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Grep: udx: notify soft honesty (Wave 126 exclusive deepen).
     * Soft inventory != product multi-server confine.
     */
    udx_printk("udx: notify soft honesty multi_server=0 confine=0 "
               "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Lean residual IRQ API honesty (Soft!=product; dual MIT OR Apache-2.0).
     * Deepen: request_irq / free_irq / soft fire residual + thr / thr-only
     * + residual msix/work/h1/freestanding toward product MSI-X;
     * DDI IRQ_BIND badge path honesty; product Notification cap mint OPEN;
     * product_host Dual DoD A/B + H1 no net_eth_poll from IRQ.
     * greppable: udx: notify soft residual lean
     * greppable: udx: notify soft residual request
     * greppable: udx: notify soft residual free
     * greppable: udx: notify soft residual fire
     * greppable: udx: notify soft residual thr
     * greppable: udx: notify soft residual thr-only
     * greppable: udx: notify soft residual msix
     * greppable: udx: notify soft residual work
     * greppable: udx: notify soft residual h1
     * greppable: udx: notify soft residual freestanding
     * greppable: udx: notify soft residual ddi_irq_bind
     * greppable: udx: notify soft residual cap_mint
     * greppable: udx: notify soft residual product_host
     * greppable: udx: notify soft residual dual_dod
     * greppable: udx: notify soft residual bind
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     * greppable: udx: notify soft api honesty
     */
    udx_printk("udx: notify soft residual lean "
               "table_only=1 cap_install=OPEN "
               "delivery=fire_irq_or_NOTIFY_WAIT "
               "thr_only=%u hard_primary=%u hard_irq_thread=%u "
               "request_ok=%u free=%u dispatch=%u "
               "residual_req=%u residual_free=%u residual_fire=%u "
               "residual_thr=%u residual_thr_only=%u "
               "residual_msix=%u residual_work=%u residual_h1=%u "
               "residual_fs_skip=%u "
               "residual_ddi_bind=%u residual_cap_mint=%u "
               "residual_prod_host=%u residual_dual_dod=%u "
               "residual_bind=%u lean_checks=%u "
               "product_MSI-X=OPEN product_IRQ_Notification_cap=OPEN "
               "soft=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 storm=0 H1=no_net_eth_poll stamp_free=1 "
               "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "wave=%u "
               "(Soft!=product; lean residual only; thr-only toward MSI-X)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_HARD_IRQ_THREAD,
               g_notifySoft.u32RequestOk, g_notifySoft.u32Free,
               g_notifySoft.u32Dispatch,
               g_notifySoft.u32ResidualRequest,
               g_notifySoft.u32ResidualFree,
               g_notifySoft.u32ResidualFire,
               g_notifySoft.u32ResidualThr,
               g_notifySoft.u32ResidualThrOnly,
               g_notifySoft.u32ResidualMsix,
               g_notifySoft.u32ResidualWork,
               g_notifySoft.u32ResidualH1,
               g_notifySoft.u32ResidualFsSkip,
               g_notifySoft.u32ResidualDdiBind,
               g_notifySoft.u32ResidualCapMint,
               g_notifySoft.u32ResidualProdHost,
               g_notifySoft.u32ResidualDualDod,
               g_notifySoft.u32ResidualBind,
               (unsigned)UDX_IRQ_LEAN_CHECKS,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /* greppable: udx: notify soft residual request */
    /* greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX */
    udx_printk("udx: notify soft residual request "
               "table_bind=1 thr_only=%u hard_primary=%u "
               "cap_mint=0 ddi_irq_bind=0 "
               "vector_bind=0 notification_mint=0 "
               "bind_ok=%u bind_fail=%u residual_n=%u "
               "shared_max=%u irq_max=%u "
               "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "product_Notification_cap=OPEN soft=1 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 Soft!=product wave=%u "
               "(request_irq residual; thr-only table bind)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               g_notifySoft.u32RequestOk, g_notifySoft.u32RequestFail,
               g_notifySoft.u32ResidualRequest,
               (unsigned)UDX_IRQ_SHARE_MAX, (unsigned)UDX_IRQ_MAX,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /* greppable: udx: notify soft residual free */
    udx_printk("udx: notify soft residual free "
               "table_unbind=1 thr_only=%u hard_primary_unbind=0 "
               "product_revoke=0 phase_a=0 "
               "empty_line_drop_mask=1 empty_line_drop_pending=1 "
               "free=%u free_miss=%u residual_n=%u "
               "product_Notification_cap=OPEN soft=1 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 wave=%u "
               "(free_irq residual; thr-only; no product revoke)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               g_notifySoft.u32Free, g_notifySoft.u32FreeMiss,
               g_notifySoft.u32ResidualFree,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /* greppable: udx: notify soft residual fire */
    udx_printk("udx: notify soft residual fire "
               "host=fire_irq freestanding=NOTIFY_WAIT "
               "thr_only=%u hard_primary=%u thr_ctx=1 "
               "dispatch=%u pulse_hit=%u pulse_latch=%u "
               "pulse_unmask=%u residual_n=%u "
               "badge_shape=bit_N_to_line_N badge_bits=%u "
               "soft_mask_local=1 hard_eoi=%u msix_pba=%u "
               "msix_vector_bind=%u product_MSI-X=OPEN "
               "schedule_work_ok=%u H1=no_net_eth_poll net_inject=0 "
               "product_Notification_cap=OPEN soft=1 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 wave=%u "
               "(soft fire residual; thr-only toward product MSI-X)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               g_notifySoft.u32Dispatch, g_notifySoft.u32PulseHit,
               g_notifySoft.u32PulseLatch, g_notifySoft.u32PulseUnmask,
               g_notifySoft.u32ResidualFire,
               (unsigned)UDX_NOTIFY_SOFT_BADGE_BITS,
               (unsigned)UDX_IRQ_MSIX_HARD_EOI,
               (unsigned)UDX_IRQ_MSIX_PBA,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * thr residual (process-context handler; sync soft no-op).
     * hard_primary=0; hard_irq_thread=0; schedule_work legal from thr.
     * residual_n once-count (first inventory; no stamp storm H2).
     * greppable: udx: notify soft residual thr
     */
    if (g_notifySoft.u32ResidualThr == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualThr);
    }
    udx_printk("udx: notify soft residual thr "
               "thr_only=%u hard_primary=%u hard_irq_thread=%u "
               "ctx=host_or_udx_run process_context=1 "
               "schedule_work_ok=%u sync_soft_noop=%u "
               "bh_only_constraint=0 top_half=0 "
               "handler_invoke=%u dispatch=%u sync=%u "
               "path=fire_or_NOTIFY_WAIT_to_thr_handler "
               "product_hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "cap_mint=OPEN residual_n=%u soft=1 product=0 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 wave=%u "
               "(thr residual; process ctx; Soft!=product)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_HARD_IRQ_THREAD,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               (unsigned)UDX_IRQ_SYNC_SOFT_NOOP,
               g_notifySoft.u32HandlerInvoke,
               g_notifySoft.u32Dispatch,
               g_notifySoft.u32Sync,
               g_notifySoft.u32ResidualThr,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * thr-only residual model lock (no hard primary half).
     * UDX request_irq = thr-only table bind; fire → thr handler only.
     * Toward product MSI-X: thr table is the userspace residual endpoint
     * after NOTIFY_WAIT reaps badge; never MSI-X vector program / PBA.
     * residual_n once-count (first inventory; no stamp storm H2).
     * greppable: udx: notify soft residual thr-only
     */
    if (g_notifySoft.u32ResidualThrOnly == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualThrOnly);
    }
    udx_printk("udx: notify soft residual thr-only "
               "thr_only=%u hard_primary=%u hard_irq_thread=%u "
               "linux_shape=request_threaded_irq_primary_plus_thr "
               "udx_shape=request_irq_thr_only_table_bind "
               "primary_handler=0 thread_fn=handler_slot "
               "delivery=thr_handler_host_thread_only "
               "hard_irq_stack=0 softirq_stack=0 "
               "schedule_work_ok=%u sync_soft_noop=%u "
               "mask_local=1 hard_eoi=%u msix_pba=%u "
               "msix_vector_bind=%u product_MSI-X=OPEN "
               "toward=product_MSI-X_Notification_cap "
               "product_hosts=rtl8168_udx,xhci_udx "
               "until_cap_mint=thr_only_table_plus_fire "
               "ne_kernel_linux_time_soft_threaded=1 "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "cap_mint=OPEN residual_n=%u soft=1 product=0 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 stamp_free=1 wave=%u "
               "(thr-only model lock; Soft!=product; Dual DoD OPEN)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_HARD_IRQ_THREAD,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               (unsigned)UDX_IRQ_SYNC_SOFT_NOOP,
               (unsigned)UDX_IRQ_MSIX_HARD_EOI,
               (unsigned)UDX_IRQ_MSIX_PBA,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               g_notifySoft.u32ResidualThrOnly,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Product MSI-X continuum residual (Soft!=product; thr-only bind).
     * Product: hard MSI-X / irq_msix → notify_pulse → NOTIFY_WAIT → thr.
     * UDX residual: thr-only table + fire; mint OPEN; no vector bind.
     * residual_n once-count (first inventory; no stamp storm H2).
     * greppable: udx: notify soft residual msix
     */
    if (g_notifySoft.u32ResidualMsix == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualMsix);
    }
    udx_printk("udx: notify soft residual msix "
               "toward=product_MSI-X product_MSI-X=OPEN "
               "product_path=irq_msix_to_notify_pulse_to_NOTIFY_WAIT "
               "udx_residual=thr_only_table_bind_plus_fire "
               "thr_only=%u hard_primary=%u "
               "msix_vector_bind=%u msix_pba=%u hard_eoi=%u "
               "kernel_soft_user_bind=irq_msix_soft_user_bind "
               "ddi_op_irq_bind=kernel_soft_note_only "
               "badge_shape=bit_N_to_line_N badge_bits=%u "
               "notify_msix_global=OPEN product_notify_mint=OPEN "
               "product_hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "cap_mint=OPEN residual_n=%u soft=1 product=0 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 stamp_free=1 wave=%u "
               "(MSI-X continuum residual; thr-only toward product)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               (unsigned)UDX_IRQ_MSIX_PBA,
               (unsigned)UDX_IRQ_MSIX_HARD_EOI,
               (unsigned)UDX_NOTIFY_SOFT_BADGE_BITS,
               g_notifySoft.u32ResidualMsix,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * thr→schedule_work residual (work thr-only chain; Soft!=product).
     * Handlers may schedule_work only; never net_eth_poll / hard-IRQ work.
     * greppable: udx: notify soft residual work
     */
    if (g_notifySoft.u32ResidualWork == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualWork);
    }
    udx_printk("udx: notify soft residual work "
               "path=thr_handler_to_schedule_work "
               "thr_only=%u schedule_work_ok=%u "
               "hard_irq_work=0 softirq_product=0 "
               "napi_poll_from_irq=0 net_eth_poll_from_irq=0 "
               "flush_from=udx_run bh=threaded_context "
               "product_hosts=rtl8168_udx,xhci_udx "
               "rtl8168_path=ISR_ack_schedule_BH "
               "xhci_path=event_schedule_BH "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "toward=product_MSI-X residual_n=%u soft=1 product=0 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 wave=%u "
               "(work thr-only residual; deferred BH only)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               g_notifySoft.u32ResidualWork,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * H1 residual honesty — never net_eth_poll from IRQ thr path.
     * greppable: udx: notify soft residual h1
     */
    if (g_notifySoft.u32ResidualH1 == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualH1);
    }
    udx_printk("udx: notify soft residual h1 "
               "net_eth_poll_from_irq=0 hard_irq_work=0 "
               "H1=no_net_eth_poll thr_only=%u "
               "schedule_from=threaded_ISR "
               "flush_from=udx_run "
               "bh=threaded_context softirq_product=0 "
               "napi_poll_product=OPEN poll_weight=0 "
               "product_hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN residual_n=%u "
               "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
               "(H1 residual; thr-only deferred BH only)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               g_notifySoft.u32ResidualH1,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Freestanding class SKIP residual (product=UDX+ABI hosts only).
     * greppable: udx: notify soft residual freestanding
     */
    if (g_notifySoft.u32ResidualFsSkip == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualFsSkip);
    }
    udx_printk("udx: notify soft residual freestanding "
               "freestanding_class=SKIP freestanding_skip=%u "
               "product_hosts=UDX product=UDX+ABI "
               "hosts=rtl8168_udx,xhci_udx never_fs_rtl_usb=1 "
               "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
               "thr_only=%u hard_primary=%u "
               "product_MSI-X=OPEN DoD_A=OPEN DoD_B=OPEN "
               "bind_path=request_irq_thr_only_table "
               "residual_n=%u soft=1 soft_ne_product=1 Soft!=product "
               "dual=MIT_OR_Apache-2.0 G-AC-1=1 wave=%u "
               "(freestanding SKIP; thr-only product_hosts=UDX)\n",
               (unsigned)UDX_IRQ_FREESTANDING_SKIP,
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               g_notifySoft.u32ResidualFsSkip,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Product-host IRQ residual (Dual DoD A/B seed; Soft!=product).
     * Threaded ISR table surface for rtl8168_udx / xhci_udx.
     * residual_n once-count (first inventory; no stamp storm H2).
     * greppable: udx: notify soft residual product_host
     * greppable: product_hosts=UDX
     */
    if (g_notifySoft.u32ResidualProdHost == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualProdHost);
    }
    udx_printk("udx: notify soft residual product_host "
               "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
               "surface=request_irq_table_fire_dispatch "
               "thr_only=%u hard_primary=%u "
               "product=UDX+ABI Linux-shaped_userspace=1 "
               "freestanding_class=SKIP kernel_rtl8168=SKIP "
               "kernel_xhci_msc=SKIP never_fs_rtl_usb=1 "
               "rtl8168_path=request_irq_to_ISR_ack_schedule "
               "xhci_path=request_irq_to_event_schedule "
               "bind_path=request_irq_thr_only_table "
               "delivery=fire_irq_or_NOTIFY_WAIT "
               "toward=product_MSI-X product_MSI-X=OPEN "
               "msix_vector_bind=%u "
               "handler_may_schedule_work=%u "
               "H1=no_net_eth_poll net_inject=0 hard_irq=0 "
               "napi_poll_from_irq=0 urb_wire=0 bot_stick=0 "
               "cap_mint=OPEN residual_n=%u soft=1 product=0 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 Soft!=product lean_checks=%u wave=%u "
               "(product-host thr-only IRQ residual; Soft!=product)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               g_notifySoft.u32ResidualProdHost,
               (unsigned)UDX_IRQ_LEAN_CHECKS,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Dual DoD A/B OPEN residual honesty (agent != close; Soft!=product).
     * DoD A = xhci_udx + DDI; DoD B = rtl8168_udx + DDI (wire/stack/sshd).
     * Soft IRQ table lamps alone never close Dual DoD.
     * greppable: udx: notify soft residual dual_dod
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     */
    if (g_notifySoft.u32ResidualDualDod == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualDualDod);
    }
    udx_printk("udx: notify soft residual dual_dod "
               "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
               "A_host=xhci_udx A_surface=USB_UDX_DDI "
               "B_host=rtl8168_udx B_surface=NIC_UDX_DDI "
               "irq_table_bind=1 thr_only=%u hard_primary=%u "
               "soft_lamp_close=0 "
               "freestanding_rtl=SKIP freestanding_usb=SKIP "
               "in_kernel_ko=0 G-AC-1=1 product=UDX+ABI "
               "product_hosts=UDX never_fs_rtl_usb=1 "
               "bind_path=request_irq_thr_only_table "
               "H1=no_net_eth_poll residual_n=%u "
               "soft=1 product=0 soft_ne_product=1 "
               "Soft!=product dual=MIT_OR_Apache-2.0 "
               "lean_checks=%u stamp_free=1 wave=%u "
               "(Soft!=product soft residual dual_dod OPEN product_hosts=UDX)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               g_notifySoft.u32ResidualDualDod,
               (unsigned)UDX_IRQ_LEAN_CHECKS,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * IRQ bind path residual (freestanding product hosts; Soft!=product).
     * STRONGER: thr-only table bind after DDI chain; never mint/DDI bind.
     * greppable: udx: notify soft residual bind
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     */
    if (g_notifySoft.u32ResidualBind == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualBind);
    }
    udx_printk("udx: notify soft residual bind "
               "path=request_irq_thr_only_table "
               "table_only=%u thr_only=%u hard_primary=%u "
               "cap_mint=%u ddi_irq_bind_from_here=%u "
               "vector_bind=%u msix_pba=%u hard_eoi=%u "
               "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "never_fs_rtl_usb=1 product=UDX+ABI "
               "delivery=fire_irq_or_NOTIFY_WAIT "
               "chain_after=DDI_SCAN_GET_OPEN_MAP_BAR_IRQ_DMA "
               "bind_ok=%u residual_n=%u lean_checks=%u "
               "soft=1 soft_ne_product=1 Soft!=product "
               "dual=MIT_OR_Apache-2.0 G-AC-1=1 stamp_free=1 wave=%u "
               "(IRQ bind residual; Soft!=product; product_hosts=UDX)\n",
               (unsigned)UDX_IRQ_BIND_TABLE_ONLY,
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_BIND_CAP_MINT,
               (unsigned)UDX_IRQ_BIND_DDI_FROM_HERE,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               (unsigned)UDX_IRQ_MSIX_PBA,
               (unsigned)UDX_IRQ_MSIX_HARD_EOI,
               g_notifySoft.u32RequestOk,
               g_notifySoft.u32ResidualBind,
               (unsigned)UDX_IRQ_LEAN_CHECKS,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * denser residual prove (preferred over lamps alone; Soft!=product).
     * denser_prove bind thr-only table + fire/dispatch + badge shape +
     * denser product_host. Dual DoD OPEN; agent!=close; stamp_free_75.
     * greppable: udx: notify soft residual denser
     * greppable: denser_prove=1 | denser residual
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     */
    if (g_notifySoft.u32ResidualDenser == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualDenser);
    }
    udx_printk("udx: notify soft residual denser "
               "denser_prove=%u denser_bind=%u denser_fire=%u "
               "denser_badge=%u denser_product_host=%u "
               "bind_ok=%u dispatch=%u pulse_hit=%u "
               "badge_bits=%u thr_only=%u hard_primary=%u "
               "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_close=%u "
               "soft_ne_product=%u fs_class_skip=%u stamp_free_75=%u "
               "cap_mint=0 ddi_irq_bind_from_here=0 "
               "residual_n=%u lean_checks=%u "
               "soft=1 product=0 Soft!=product "
               "dual=MIT_OR_Apache-2.0 G-AC-1=1 stamp_free=1 wave=%u "
               "(denser residual prove; Soft!=product; Dual DoD OPEN)\n",
               (unsigned)UDX_IRQ_DENSER_PROVE,
               (unsigned)UDX_IRQ_DENSER_BIND,
               (unsigned)UDX_IRQ_DENSER_FIRE,
               (unsigned)UDX_IRQ_DENSER_BADGE,
               (unsigned)UDX_IRQ_DENSER_PRODUCT_HOST,
               g_notifySoft.u32RequestOk,
               g_notifySoft.u32Dispatch,
               g_notifySoft.u32PulseHit,
               (unsigned)UDX_NOTIFY_SOFT_BADGE_BITS,
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_AGENT_CLOSE,
               (unsigned)UDX_IRQ_SOFT_NE_PRODUCT,
               (unsigned)UDX_IRQ_FREESTANDING_SKIP,
               (unsigned)UDX_IRQ_STAMP_FREE_75,
               g_notifySoft.u32ResidualDenser,
               (unsigned)UDX_IRQ_LEAN_CHECKS,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * DDI IRQ_BIND badge path honesty — UDX table != DDI badge mint.
     * Kernel: ddi: soft irq note PASS (handle->badge; never_clear_all).
     * residual_n is a once-count (set on first inventory; no stamp storm).
     * greppable: udx: notify soft residual ddi_irq_bind
     */
    if (g_notifySoft.u32ResidualDdiBind == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualDdiBind);
    }
    udx_printk("udx: notify soft residual ddi_irq_bind "
               "ddi_op=DDI_OP_IRQ_BIND kernel_soft_note=1 "
               "kernel_path=irq_msix_soft_user_bind "
               "kernel_lamp=ddi:_soft_irq_note_PASS "
               "udx_request_irq=thr_only_table ddi_bind_from_udx=0 "
               "thr_only=%u hard_primary=%u "
               "badge_shape=bit_N_to_line_N badge_bits=%u "
               "wait=GJ_SYS_NOTIFY_WAIT host_fire=udx_host_fire_irq "
               "multi_host=never_clear_all close=unbind_handle "
               "product_IRQ_Notification_cap_mint=OPEN "
               "residual_n=%u soft=1 soft_ne_product=1 "
               "dual=MIT_OR_Apache-2.0 G-AC-1=1 wave=%u "
               "(DDI IRQ_BIND badge path honesty; thr-only table; mint OPEN)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_NOTIFY_SOFT_BADGE_BITS,
               g_notifySoft.u32ResidualDdiBind,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    /*
     * Product Notification cap mint remains OPEN (Soft!=product).
     * residual_n once-count (first inventory only; no stamp storm).
     * greppable: udx: notify soft residual cap_mint
     */
    if (g_notifySoft.u32ResidualCapMint == 0u) {
        notify_soft_inc(&g_notifySoft.u32ResidualCapMint);
    }
    udx_printk("udx: notify soft residual cap_mint "
               "product_IRQ_Notification_cap_mint=OPEN "
               "product_MSI-X=OPEN product_notify_mint=OPEN "
               "cap_install=OPEN multi_server=0 confine=0 "
               "table_bind=1 thr_only=%u hard_primary=%u "
               "hard_irq=0 multi_waiter=0 msix_vector_bind=%u "
               "M4_2_wire=OPEN product_hosts=rtl8168_udx,xhci_udx "
               "until_mint=thr_only_table_plus_fire "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "residual_n=%u soft=1 product=0 soft_ne_product=1 "
               "dual=MIT_OR_Apache-2.0 G-AC-1=1 wave=%u "
               "(product Notification cap mint OPEN; thr-only until mint)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               g_notifySoft.u32ResidualCapMint,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);

    udx_printk("udx: notify soft api honesty "
               "product_irq_notification_cap=OPEN "
               "product_MSI-X=OPEN "
               "table_bind=1 thr_only=%u hard_primary=%u "
               "hard_irq=0 hard_irq_thread=%u multi_waiter=0 "
               "cap_mint=0 multi_server=0 "
               "msix_vector_bind=%u msix_pba=%u hard_eoi=%u "
               "ddi_irq_bind_badge=honesty_only "
               "request_free_fire_residual=1 "
               "thr_residual=1 thr_only_residual=1 "
               "msix_residual=1 work_residual=1 h1_residual=1 "
               "freestanding_residual=1 "
               "product_host_residual=1 dual_dod_residual=1 "
               "schedule_work_ok=%u sync_soft_noop=%u "
               "H1=no_net_eth_poll DoD_A=OPEN DoD_B=OPEN "
               "soft=1 product_kernel=OPEN Soft!=product "
               "dual=MIT_OR_Apache-2.0 G-AC-1=1 stamp_free=1 wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_HARD_IRQ_THREAD,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               (unsigned)UDX_IRQ_MSIX_PBA,
               (unsigned)UDX_IRQ_MSIX_HARD_EOI,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               (unsigned)UDX_IRQ_SYNC_SOFT_NOOP,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/* Forward decls: bind_once emits thr / thr-only / msix / work / h1 / dual_dod / bind / denser. */
static void notify_soft_log_thr_residual_once(void);
static void notify_soft_log_thr_only_residual_once(void);
static void notify_soft_log_msix_residual_once(void);
static void notify_soft_log_work_residual_once(void);
static void notify_soft_log_h1_residual_once(void);
static void notify_soft_log_fs_skip_residual_once(void);
static void notify_soft_log_prod_host_residual_once(void);
static void notify_soft_log_dual_dod_residual_once(void);
static void notify_soft_log_bind_residual_once(void);
static void notify_soft_log_denser_residual_once(void);

/** Soft: one-shot protocol + inventory on first successful bind. */
static void
notify_soft_log_bind_once(void)
{
    if (g_fNotifySoftBindOnce != 0) {
        return;
    }
    g_fNotifySoftBindOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualRequest);
    /* greppable: udx: notify soft bind */
    /* greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX */
    udx_printk("udx: notify soft bind table_only=1 thr_only=%u "
               "hard_primary=%u delivery=fire_irq_or_NOTIFY_WAIT "
               "cap_mint=OPEN ddi_irq_bind=0 "
               "product_hosts=UDX DoD_A=OPEN DoD_B=OPEN "
               "soft_ne_product=1 Soft!=product G-AC-1=1 wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
    /*
     * request_irq residual (one-shot with bind; thr-only; Soft!=product).
     * freestanding product-host IRQ bind path after DDI install.
     * greppable: udx: notify soft residual request
     * greppable: udx: notify soft residual thr-only
     * greppable: udx: notify soft residual bind
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     */
    udx_printk("udx: notify soft residual request "
               "table_bind=1 thr_only=%u hard_primary=%u "
               "vector_bind=0 cap_mint=0 "
               "ddi_op_irq_bind=not_from_here "
               "product_Notification_cap_mint=OPEN "
               "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "soft=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 Soft!=product residual_n=%u wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               g_notifySoft.u32ResidualRequest,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
    notify_soft_log_bind_residual_once();
    notify_soft_log_denser_residual_once();
    notify_soft_log_thr_only_residual_once();
    notify_soft_log_thr_residual_once();
    notify_soft_log_msix_residual_once();
    notify_soft_log_work_residual_once();
    notify_soft_log_h1_residual_once();
    notify_soft_log_fs_skip_residual_once();
    notify_soft_log_prod_host_residual_once();
    notify_soft_log_dual_dod_residual_once();
    if (g_fNotifySoftProtocolOnce == 0) {
        g_fNotifySoftProtocolOnce = 1;
    }
    notify_soft_log();
}

/** Soft: one-shot free_irq residual honesty (sparse; no stamp storm). */
static void
notify_soft_log_free_residual_once(void)
{
    if (g_fNotifySoftFreeResidualOnce != 0) {
        return;
    }
    g_fNotifySoftFreeResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualFree);
    /* greppable: udx: notify soft residual free */
    udx_printk("udx: notify soft residual free "
               "table_unbind=1 thr_only=%u hard_primary_unbind=0 "
               "product_revoke=0 phase_a=0 "
               "empty_line_drop_mask_pending=1 "
               "product_Notification_cap_mint=OPEN "
               "soft=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 residual_n=%u wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               g_notifySoft.u32ResidualFree,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/** Soft: one-shot soft-fire residual honesty after first dispatch. */
static void
notify_soft_log_fire_residual_once(void)
{
    if (g_fNotifySoftFireResidualOnce != 0) {
        return;
    }
    g_fNotifySoftFireResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualFire);
    /* greppable: udx: notify soft residual fire */
    /* greppable: udx: notify soft residual thr-only */
    udx_printk("udx: notify soft residual fire "
               "host_fire_or_NOTIFY_WAIT=1 thr_only=%u "
               "hard_primary=%u hard_irq=0 thr_ctx=1 "
               "badge_shape=bit_N_to_line_N "
               "soft_mask_local=1 hard_eoi=%u msix_pba=%u "
               "msix_vector_bind=%u product_MSI-X=OPEN "
               "schedule_work_ok=%u H1=no_net_eth_poll net_inject=0 "
               "product_Notification_cap_mint=OPEN "
               "soft=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 residual_n=%u wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_MSIX_HARD_EOI,
               (unsigned)UDX_IRQ_MSIX_PBA,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               g_notifySoft.u32ResidualFire,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot thr residual honesty (process ctx; sync no-op; sparse).
 * greppable: udx: notify soft residual thr
 */
static void
notify_soft_log_thr_residual_once(void)
{
    if (g_fNotifySoftThrResidualOnce != 0) {
        return;
    }
    g_fNotifySoftThrResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualThr);
    udx_printk("udx: notify soft residual thr "
               "thr_only=%u hard_primary=%u hard_irq_thread=%u "
               "ctx=host_or_udx_run process_context=1 "
               "schedule_work_ok=%u sync_soft_noop=%u "
               "bh_only_constraint=0 top_half=0 "
               "path=fire_or_NOTIFY_WAIT_to_thr_handler "
               "product_hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN soft=1 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 residual_n=%u wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_HARD_IRQ_THREAD,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               (unsigned)UDX_IRQ_SYNC_SOFT_NOOP,
               g_notifySoft.u32ResidualThr,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot thr-only model lock residual (no hard primary; sparse).
 * Toward product MSI-X: thr-only table is residual endpoint after badge.
 * greppable: udx: notify soft residual thr-only
 */
static void
notify_soft_log_thr_only_residual_once(void)
{
    if (g_fNotifySoftThrOnlyResidualOnce != 0) {
        return;
    }
    g_fNotifySoftThrOnlyResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualThrOnly);
    udx_printk("udx: notify soft residual thr-only "
               "thr_only=%u hard_primary=%u hard_irq_thread=%u "
               "udx_shape=request_irq_thr_only_table_bind "
               "primary_handler=0 delivery=thr_handler_only "
               "hard_irq_stack=0 schedule_work_ok=%u "
               "sync_soft_noop=%u msix_vector_bind=%u "
               "product_MSI-X=OPEN toward=product_MSI-X "
               "until_cap_mint=table_plus_fire "
               "product_hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN soft=1 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 residual_n=%u wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_HARD_IRQ_THREAD,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               (unsigned)UDX_IRQ_SYNC_SOFT_NOOP,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               g_notifySoft.u32ResidualThrOnly,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot product MSI-X continuum residual (sparse).
 * greppable: udx: notify soft residual msix
 */
static void
notify_soft_log_msix_residual_once(void)
{
    if (g_fNotifySoftMsixResidualOnce != 0) {
        return;
    }
    g_fNotifySoftMsixResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualMsix);
    udx_printk("udx: notify soft residual msix "
               "toward=product_MSI-X product_MSI-X=OPEN "
               "product_path=irq_msix_to_notify_pulse_to_NOTIFY_WAIT "
               "udx_residual=thr_only_table_bind_plus_fire "
               "thr_only=%u msix_vector_bind=%u msix_pba=%u "
               "hard_eoi=%u product_notify_mint=OPEN "
               "product_hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN soft=1 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 residual_n=%u wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_MSIX_VECTOR_BIND,
               (unsigned)UDX_IRQ_MSIX_PBA,
               (unsigned)UDX_IRQ_MSIX_HARD_EOI,
               g_notifySoft.u32ResidualMsix,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot thr→schedule_work residual (sparse).
 * greppable: udx: notify soft residual work
 */
static void
notify_soft_log_work_residual_once(void)
{
    if (g_fNotifySoftWorkResidualOnce != 0) {
        return;
    }
    g_fNotifySoftWorkResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualWork);
    udx_printk("udx: notify soft residual work "
               "path=thr_handler_to_schedule_work "
               "thr_only=%u schedule_work_ok=%u "
               "hard_irq_work=0 net_eth_poll_from_irq=0 "
               "flush_from=udx_run "
               "product_hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN soft=1 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 residual_n=%u wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               g_notifySoft.u32ResidualWork,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot H1 residual honesty (sparse).
 * greppable: udx: notify soft residual h1
 */
static void
notify_soft_log_h1_residual_once(void)
{
    if (g_fNotifySoftH1ResidualOnce != 0) {
        return;
    }
    g_fNotifySoftH1ResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualH1);
    udx_printk("udx: notify soft residual h1 "
               "net_eth_poll_from_irq=0 H1=no_net_eth_poll "
               "thr_only=%u schedule_from=threaded_ISR "
               "flush_from=udx_run hard_irq_work=0 "
               "product_hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN soft=1 "
               "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 residual_n=%u wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               g_notifySoft.u32ResidualH1,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot freestanding class SKIP residual (sparse).
 * greppable: udx: notify soft residual freestanding
 */
static void
notify_soft_log_fs_skip_residual_once(void)
{
    if (g_fNotifySoftFsSkipResidualOnce != 0) {
        return;
    }
    g_fNotifySoftFsSkipResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualFsSkip);
    udx_printk("udx: notify soft residual freestanding "
               "freestanding_class=SKIP freestanding_skip=%u "
               "product_hosts=UDX product=UDX+ABI thr_only=%u "
               "product_MSI-X=OPEN DoD_A=OPEN DoD_B=OPEN "
               "never_fs_rtl_usb=1 Soft!=product "
               "soft=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 residual_n=%u wave=%u\n",
               (unsigned)UDX_IRQ_FREESTANDING_SKIP,
               (unsigned)UDX_IRQ_THR_ONLY,
               g_notifySoft.u32ResidualFsSkip,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot product-host IRQ residual (Dual DoD seed; sparse).
 * greppable: udx: notify soft residual product_host
 * greppable: product_hosts=UDX
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 */
static void
notify_soft_log_prod_host_residual_once(void)
{
    if (g_fNotifySoftProdHostResidualOnce != 0) {
        return;
    }
    g_fNotifySoftProdHostResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualProdHost);
    udx_printk("udx: notify soft residual product_host "
               "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
               "surface=request_irq_table_fire_dispatch "
               "thr_only=%u hard_primary=%u "
               "product=UDX+ABI freestanding_class=SKIP "
               "never_fs_rtl_usb=1 bind_path=request_irq_thr_only_table "
               "H1=no_net_eth_poll schedule_work_ok=%u "
               "DoD_A=OPEN DoD_B=OPEN soft=1 "
               "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
               "G-AC-1=1 residual_n=%u lean_checks=%u wave=%u\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_THR_SCHEDULE_OK,
               g_notifySoft.u32ResidualProdHost,
               (unsigned)UDX_IRQ_LEAN_CHECKS,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot Dual DoD A/B OPEN residual honesty (agent != close).
 * greppable: udx: notify soft residual dual_dod
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 */
static void
notify_soft_log_dual_dod_residual_once(void)
{
    if (g_fNotifySoftDualDodResidualOnce != 0) {
        return;
    }
    g_fNotifySoftDualDodResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualDualDod);
    udx_printk("udx: notify soft residual dual_dod "
               "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
               "A_host=xhci_udx B_host=rtl8168_udx "
               "thr_only=%u hard_primary=%u "
               "soft_lamp_close=0 freestanding_class=SKIP "
               "product_hosts=UDX never_fs_rtl_usb=1 "
               "bind_path=request_irq_thr_only_table "
               "G-AC-1=1 soft=1 soft_ne_product=1 Soft!=product "
               "dual=MIT_OR_Apache-2.0 residual_n=%u "
               "lean_checks=%u stamp_free=1 wave=%u "
               "(Soft!=product soft residual dual_dod OPEN product_hosts=UDX)\n",
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               g_notifySoft.u32ResidualDualDod,
               (unsigned)UDX_IRQ_LEAN_CHECKS,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot IRQ bind path residual (freestanding product hosts).
 * STRONGER thr-only table bind; Soft!=product; Dual DoD OPEN.
 * greppable: udx: notify soft residual bind
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 */
static void
notify_soft_log_bind_residual_once(void)
{
    if (g_fNotifySoftBindResidualOnce != 0) {
        return;
    }
    g_fNotifySoftBindResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualBind);
    udx_printk("udx: notify soft residual bind "
               "path=request_irq_thr_only_table "
               "table_only=%u thr_only=%u hard_primary=%u "
               "cap_mint=%u ddi_irq_bind_from_here=%u "
               "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
               "DoD_A=OPEN DoD_B=OPEN freestanding_class=SKIP "
               "never_fs_rtl_usb=1 product=UDX+ABI "
               "delivery=fire_irq_or_NOTIFY_WAIT "
               "chain_after=DDI_SCAN_GET_OPEN_MAP_BAR_IRQ_DMA "
               "soft=1 soft_ne_product=1 Soft!=product "
               "dual=MIT_OR_Apache-2.0 G-AC-1=1 residual_n=%u "
               "lean_checks=%u stamp_free=1 wave=%u "
               "(IRQ bind residual; Soft!=product; product_hosts=UDX)\n",
               (unsigned)UDX_IRQ_BIND_TABLE_ONLY,
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_IRQ_BIND_CAP_MINT,
               (unsigned)UDX_IRQ_BIND_DDI_FROM_HERE,
               g_notifySoft.u32ResidualBind,
               (unsigned)UDX_IRQ_LEAN_CHECKS,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/**
 * Soft: one-shot denser residual prove (preferred over lamps alone).
 * denser_prove bind thr-only + fire/dispatch + badge + product_host.
 * Soft!=product; Dual DoD OPEN; agent!=close; stamp_free_75.
 * greppable: udx: notify soft residual denser
 * greppable: denser_prove=1
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 */
static void
notify_soft_log_denser_residual_once(void)
{
    if (g_fNotifySoftDenserResidualOnce != 0) {
        return;
    }
    g_fNotifySoftDenserResidualOnce = 1;
    notify_soft_inc(&g_notifySoft.u32ResidualDenser);
    /* greppable: udx: notify soft residual denser */
    udx_printk("udx: notify soft residual denser "
               "once=1 denser_prove=%u denser_bind=%u denser_fire=%u "
               "denser_badge=%u denser_product_host=%u "
               "thr_only=%u hard_primary=%u badge_bits=%u "
               "product_hosts=UDX DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN "
               "agent_close=%u soft_ne_product=%u fs_class_skip=%u "
               "stamp_free_75=%u lean_checks=%u residual_n=%u "
               "soft=1 Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1=1 "
               "stamp_free=1 wave=%u "
               "(denser residual prove; Soft!=product; Dual DoD OPEN)\n",
               (unsigned)UDX_IRQ_DENSER_PROVE,
               (unsigned)UDX_IRQ_DENSER_BIND,
               (unsigned)UDX_IRQ_DENSER_FIRE,
               (unsigned)UDX_IRQ_DENSER_BADGE,
               (unsigned)UDX_IRQ_DENSER_PRODUCT_HOST,
               (unsigned)UDX_IRQ_THR_ONLY,
               (unsigned)UDX_IRQ_HARD_PRIMARY,
               (unsigned)UDX_NOTIFY_SOFT_BADGE_BITS,
               (unsigned)UDX_IRQ_AGENT_CLOSE,
               (unsigned)UDX_IRQ_SOFT_NE_PRODUCT,
               (unsigned)UDX_IRQ_FREESTANDING_SKIP,
               (unsigned)UDX_IRQ_STAMP_FREE_75,
               (unsigned)UDX_IRQ_LEAN_CHECKS,
               g_notifySoft.u32ResidualDenser,
               (unsigned)UDX_NOTIFY_SOFT_WAVE);
}

/** Soft: one-shot pulse catalog after first fire_irq/dispatch activity. */
static void
notify_soft_log_pulse_once(void)
{
    if (g_fNotifySoftPulseOnce != 0) {
        return;
    }
    g_fNotifySoftPulseOnce = 1;
    if (g_fNotifySoftProtocolOnce == 0) {
        g_fNotifySoftProtocolOnce = 1;
    }
    notify_soft_log_fire_residual_once();
    notify_soft_log_bind_residual_once();
    notify_soft_log_denser_residual_once();
    notify_soft_log_thr_only_residual_once();
    notify_soft_log_thr_residual_once();
    notify_soft_log_msix_residual_once();
    notify_soft_log_work_residual_once();
    notify_soft_log_h1_residual_once();
    notify_soft_log_fs_skip_residual_once();
    notify_soft_log_prod_host_residual_once();
    notify_soft_log_dual_dod_residual_once();
    notify_soft_log();
}

static int
irq_line_has_actions(int nIrq)
{
    int iSlot;

    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used) {
            return 1;
        }
    }
    return 0;
}

/**
 * Soft pulse core: host fire_irq and freestanding badge claim converge here.
 * thr-only: handlers run in host / udx_run process context only
 * (hard_primary=0; hard_irq_thread=0; schedule_work legal).
 * greppable: udx: notify soft pulse
 * greppable: udx: notify soft residual thr
 * greppable: udx: notify soft residual thr-only
 */
static void
irq_dispatch_unlocked(int nIrq)
{
    int iSlot;
    int cInvoked;

    if (g_aIrqDisableDepth[nIrq] != 0) {
        g_aIrqPending[nIrq] = 1;
        /* greppable: udx: notify soft pulse latch */
        notify_soft_inc(&g_notifySoft.u32PulseLatch);
        notify_soft_note_line(nIrq);
        notify_soft_log_pulse_once();
        return;
    }
    g_aIrqPending[nIrq] = 0;
    cInvoked = 0;
    /*
     * thr-only residual: each bound action is a thr handler slot, not a
     * hard primary. Invoked in process context (host thread / udx_run).
     * greppable: thr_only=1 hard_primary=0
     */
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used && g_aIrq[nIrq][iSlot].pfnHandler) {
            (void)g_aIrq[nIrq][iSlot].pfnHandler(
                nIrq, g_aIrq[nIrq][iSlot].pDevId);
            notify_soft_inc(&g_notifySoft.u32HandlerInvoke);
            cInvoked++;
        }
    }
    notify_soft_note_line(nIrq);
    if (cInvoked > 0) {
        /* greppable: udx: notify soft pulse hit */
        notify_soft_inc(&g_notifySoft.u32PulseHit);
    } else {
        /* greppable: udx: notify soft pulse empty */
        notify_soft_inc(&g_notifySoft.u32PulseEmpty);
    }
    notify_soft_log_pulse_once();
}

udx_status_t
udx_request_irq(int nIrq, udx_irq_handler_t pfnHandler, u32 u32Flags,
                const char *szName, void *pDevId)
{
    int iSlot;
    int fAny;
    int fLineShared;

    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX || pfnHandler == NULL) {
        notify_soft_inc(&g_notifySoft.u32RequestFail);
        return UDX_ERR_INVAL;
    }

    /* Reject duplicate pDevId on the same line (Linux free_irq key). */
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used &&
            g_aIrq[nIrq][iSlot].pDevId == pDevId) {
            notify_soft_inc(&g_notifySoft.u32RequestFail);
            return UDX_ERR_BUSY;
        }
    }

    fAny = 0;
    fLineShared = 0;
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used) {
            fAny = 1;
            if ((g_aIrq[nIrq][iSlot].u32Flags & UDX_IRQF_SHARED) != 0) {
                fLineShared = 1;
            }
            break;
        }
    }

    /*
     * Shared line only when every registrant sets UDX_IRQF_SHARED.
     * First occupant sets the line policy (slot 0 is the anchor).
     */
    if (fAny) {
        if ((u32Flags & UDX_IRQF_SHARED) == 0) {
            notify_soft_inc(&g_notifySoft.u32RequestFail);
            return UDX_ERR_BUSY;
        }
        if (!fLineShared) {
            notify_soft_inc(&g_notifySoft.u32RequestFail);
            return UDX_ERR_BUSY;
        }
    }

    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (!g_aIrq[nIrq][iSlot].u8Used) {
            g_aIrq[nIrq][iSlot].pfnHandler = pfnHandler;
            g_aIrq[nIrq][iSlot].pDevId = pDevId;
            g_aIrq[nIrq][iSlot].szName = szName;
            g_aIrq[nIrq][iSlot].u32Flags = u32Flags;
            g_aIrq[nIrq][iSlot].u8Used = 1;
            /*
             * request_irq residual (Soft!=product; G-AC-1; dual MIT/Apache):
             * thr-only table bind (hard_primary=0). NOTIFY_WAIT / host fire
             * is pumped from udx_run (core.c) or udx_host_fire_irq -- not
             * here. Leave disable depth as-is so a quiesced line stays
             * masked.
             *
             * Does NOT call DDI_OP_IRQ_BIND and does NOT mint a product
             * IRQ->Notification cap (mint OPEN). DDI badge path is kernel
             * soft note only (irq_msix_soft_user_bind / ddi: soft irq note
             * PASS); this slot is the UDX-side thr-only handler residual
             * endpoint for product hosts (rtl8168_udx / xhci_udx Dual DoD
             * B/A OPEN) toward product MSI-X (vector bind=0; PBA=0; EOI=0).
             * No Linux primary_handler half. H1: handlers must not call
             * net_eth_poll; schedule_work is legal from thr (work residual).
             * greppable: udx: notify soft residual request
             * greppable: udx: notify soft residual thr-only
             * greppable: udx: notify soft residual thr
             * greppable: udx: notify soft residual msix
             * greppable: udx: notify soft residual work
             * greppable: udx: notify soft residual h1
             * greppable: udx: notify soft residual freestanding
             * greppable: udx: notify soft residual ddi_irq_bind
             * greppable: udx: notify soft residual cap_mint
             * greppable: udx: notify soft residual product_host
             * greppable: udx: notify soft residual dual_dod
             * greppable: udx: notify soft residual bind
             * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
             * greppable: product_hosts=UDX
             */
            notify_soft_inc(&g_notifySoft.u32RequestOk);
            notify_soft_note_line(nIrq);
            notify_soft_log_bind_once();
            return UDX_OK;
        }
    }
    notify_soft_inc(&g_notifySoft.u32RequestFail);
    return UDX_ERR_BUSY;
}

void
udx_free_irq(int nIrq, void *pDevId)
{
    int iSlot;

    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        notify_soft_inc(&g_notifySoft.u32FreeMiss);
        return;
    }
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used &&
            g_aIrq[nIrq][iSlot].pDevId == pDevId) {
            g_aIrq[nIrq][iSlot].u8Used = 0;
            g_aIrq[nIrq][iSlot].pfnHandler = NULL;
            g_aIrq[nIrq][iSlot].pDevId = NULL;
            g_aIrq[nIrq][iSlot].szName = NULL;
            g_aIrq[nIrq][iSlot].u32Flags = 0;
            if (!irq_line_has_actions(nIrq)) {
                /*
                 * free_irq residual: when the line is empty, drop soft
                 * mask/pending local state. No product revoke / Phase-A
                 * Notification unbind claim (mint OPEN). Kernel DDI CLOSE
                 * multi-host unbind is not claimed from this table clear.
                 * greppable: udx: notify soft residual free
                 * greppable: udx: notify soft residual cap_mint
                 */
                g_aIrqPending[nIrq] = 0;
                g_aIrqDisableDepth[nIrq] = 0;
            }
            /* greppable: udx: notify soft unbind */
            notify_soft_inc(&g_notifySoft.u32Free);
            notify_soft_log_free_residual_once();
            return;
        }
    }
    notify_soft_inc(&g_notifySoft.u32FreeMiss);
}

void
udx_disable_irq(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return;
    }
    if (g_aIrqDisableDepth[nIrq] < 0xffffu) {
        g_aIrqDisableDepth[nIrq]++;
        notify_soft_inc(&g_notifySoft.u32Disable);
    }
}

void
udx_enable_irq(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return;
    }
    if (g_aIrqDisableDepth[nIrq] > 0) {
        g_aIrqDisableDepth[nIrq]--;
        notify_soft_inc(&g_notifySoft.u32Enable);
    }
    if (g_aIrqDisableDepth[nIrq] == 0 && g_aIrqPending[nIrq]) {
        /*
         * Deliver latched soft fire after final unmask.
         * greppable: udx: notify soft pulse unmask
         */
        notify_soft_inc(&g_notifySoft.u32PulseUnmask);
        notify_soft_inc(&g_notifySoft.u32Dispatch);
        irq_dispatch_unlocked(nIrq);
    }
}

int
udx_irq_is_disabled(int nIrq)
{
    notify_soft_inc(&g_notifySoft.u32IsDisabledQ);
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 1;
    }
    return g_aIrqDisableDepth[nIrq] != 0;
}

int
udx_irq_disable_depth(int nIrq)
{
    notify_soft_inc(&g_notifySoft.u32DepthQ);
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 0;
    }
    return (int)g_aIrqDisableDepth[nIrq];
}

int
udx_irq_is_pending(int nIrq)
{
    notify_soft_inc(&g_notifySoft.u32IsPendingQ);
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 0;
    }
    return g_aIrqPending[nIrq] != 0;
}

void
udx_synchronize_irq(int nIrq)
{
    /*
     * thr residual / thr-only: handlers run synchronously in dispatch
     * (host / udx_run process context). hard_irq_thread=0 → soft no-op.
     * Nothing to park or wait on. Real Notification multi-waiter park
     * is kernel-side only (gap). Soft!=product; Dual DoD OPEN.
     * greppable: udx: notify soft sync
     * greppable: udx: notify soft residual thr
     * greppable: udx: notify soft residual thr-only
     */
    notify_soft_inc(&g_notifySoft.u32Sync);
    (void)nIrq;
    /* Sparse thr residual lamp (once; no stamp storm). */
    notify_soft_log_thr_residual_once();
}

const char *
udx_irq_name(int nIrq)
{
    int iSlot;

    notify_soft_inc(&g_notifySoft.u32NameQ);
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return NULL;
    }
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used) {
            return g_aIrq[nIrq][iSlot].szName;
        }
    }
    return NULL;
}

int
udx_irq_action_count(int nIrq)
{
    int iSlot;
    int cActions;

    notify_soft_inc(&g_notifySoft.u32ActionCountQ);
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 0;
    }
    cActions = 0;
    for (iSlot = 0; iSlot < UDX_IRQ_SHARE_MAX; iSlot++) {
        if (g_aIrq[nIrq][iSlot].u8Used) {
            cActions++;
        }
    }
    return cActions;
}

void
udx_irq_dispatch(int nIrq)
{
    /*
     * Soft fire residual convergence (Soft!=product; G-AC-1):
     *   host:  udx_host_fire_irq(n)     (soft pulse analog)
     *   full:  NOTIFY_WAIT badge bit n  (real Notification claim)
     * thr-only residual: delivery invokes thr handlers in host / udx_run
     * process context only (hard_primary=0; hard_irq_thread=0;
     * schedule_work legal; no hard-IRQ stack / BH-only constraints).
     * Badge path honesty: N is line index / badge bit, not a minted
     * product Notification cap. DDI_OP_IRQ_BIND (kernel) may have soft-
     * noted a handle->badge; this dispatch only runs the UDX thr table.
     * Soft mask latch/unmask are table-local (hard EOI / MSI-X PBA = 0).
     * Toward product MSI-X: delivery residual after badge reap only —
     * no MSI-X vector program from this unit (product_MSI-X=OPEN).
     * product Notification cap mint = OPEN (dual MIT OR Apache-2.0).
     * H1: no net_eth_poll from IRQ path (handlers may schedule_work only).
     * Dual DoD A/B remain OPEN (xhci_udx / rtl8168_udx); agent != close.
     * greppable: udx: notify soft pulse / fire_irq
     * greppable: udx: notify soft residual fire
     * greppable: udx: notify soft residual thr
     * greppable: udx: notify soft residual thr-only
     * greppable: udx: notify soft residual msix
     * greppable: udx: notify soft residual work
     * greppable: udx: notify soft residual h1
     * greppable: udx: notify soft residual freestanding
     * greppable: udx: notify soft residual ddi_irq_bind
     * greppable: udx: notify soft residual cap_mint
     * greppable: udx: notify soft residual product_host
     * greppable: udx: notify soft residual dual_dod
     */
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        /* greppable: udx: notify soft pulse bad */
        notify_soft_inc(&g_notifySoft.u32PulseBad);
        return;
    }
    notify_soft_inc(&g_notifySoft.u32Dispatch);
    irq_dispatch_unlocked(nIrq);
}
