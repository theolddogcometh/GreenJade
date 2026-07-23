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
 * Soft notify path → real Notification (gap doc; exclusive deepen)
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
 * Gap (cannot close from userspace UDX sim / this file alone):
 *   - Cannot call kernel notify_pulse / notify_install / notify_msix_global
 *   - Host libc build has no GJ_SYS_NOTIFY_WAIT syscall surface
 *   - Soft multi-waiter, CAS-clear pending, and cap install live only in
 *     kernel/ipc/notify.c (see greppable notify: soft … there)
 *   - Wiring UDX host to a real Notification cap is M4.2 (TODO)
 *
 * Soft deepen catalogs the UDX-side fire_irq → dispatch pulse surface so
 * greps measure readiness without claiming a kernel endpoint.
 * -------------------------------------------------------------------------
 */
#include <udx/irq.h>
#include <udx/udx.h>

#define UDX_IRQ_MAX 256
#define UDX_IRQ_SHARE_MAX 4

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
 */
struct udx_notify_soft_stats {
    u32 u32RequestOk;       /* request_irq bind success */
    u32 u32RequestFail;     /* request_irq rejected */
    u32 u32Free;            /* free_irq matched action */
    u32 u32FreeMiss;        /* free_irq no match / bad line */
    u32 u32Disable;         /* disable_irq depth++ */
    u32 u32Enable;          /* enable_irq depth-- */
    u32 u32Dispatch;        /* udx_irq_dispatch / fire_irq entries */
    u32 u32PulseHit;        /* dispatch ran ≥1 handler (soft pulse hit) */
    u32 u32PulseEmpty;      /* dispatch with no bound actions */
    u32 u32PulseLatch;      /* fire while depth>0 → pending */
    u32 u32PulseUnmask;     /* pending delivered on depth→0 */
    u32 u32PulseBad;        /* line out of range */
    u32 u32HandlerInvoke;   /* individual handler calls */
    u32 u32SoftLog;         /* soft inventory dumps emitted */
    u32 u32LastLine;        /* last soft-pulse line (stats) */
    u32 u32LastBadgeLo;     /* last soft badge word low 32 (bit N set) */
    u32 u32LastBadgeHi;     /* last soft badge word high 32 */
    u32 u32BadgeOutOfWord;  /* dispatch line ≥ 64 (beyond NOTIFY badge) */
};

static struct udx_irq_action g_aIrq[UDX_IRQ_MAX][UDX_IRQ_SHARE_MAX];
static u16 g_aIrqDisableDepth[UDX_IRQ_MAX];
static u8  g_aIrqPending[UDX_IRQ_MAX];

static struct udx_notify_soft_stats g_notifySoft;
static u8 g_fNotifySoftProtocolOnce;
static u8 g_fNotifySoftPulseOnce;
static u8 g_fNotifySoftBindOnce;

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

/*
 * Greppable soft notify inventory (toward real Notification).
 * Pure observation — never gates skeleton PASS or dispatch behavior.
 *
 *   udx: notify soft protocol …
 *   udx: notify soft inventory …
 *   udx: notify soft pulse inventory …
 *   udx: notify soft stats …
 */
static void
notify_soft_log(void)
{
    notify_soft_inc(&g_notifySoft.u32SoftLog);

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
               "soft_log=%u\n",
               (unsigned)UDX_NOTIFY_SOFT_BADGE_BITS,
               (unsigned)UDX_IRQ_MAX,
               g_notifySoft.u32SoftLog);

    /* Grep: udx: notify soft inventory */
    udx_printk("udx: notify soft inventory irq_max=%u share_max=%u "
               "paths=request,free,disable,enable,dispatch,"
               "mask_latch,unmask_deliver "
               "bind_ok=%u bind_fail=%u free=%u free_miss=%u "
               "disable=%u enable=%u soft_log=%u\n",
               (unsigned)UDX_IRQ_MAX, (unsigned)UDX_IRQ_SHARE_MAX,
               g_notifySoft.u32RequestOk, g_notifySoft.u32RequestFail,
               g_notifySoft.u32Free, g_notifySoft.u32FreeMiss,
               g_notifySoft.u32Disable, g_notifySoft.u32Enable,
               g_notifySoft.u32SoftLog);

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
               "badge_out_of_word=%u\n",
               g_notifySoft.u32PulseHit, g_notifySoft.u32PulseLatch,
               g_notifySoft.u32PulseUnmask, g_notifySoft.u32PulseEmpty,
               g_notifySoft.u32PulseBad, g_notifySoft.u32HandlerInvoke,
               g_notifySoft.u32Dispatch, g_notifySoft.u32LastLine,
               g_notifySoft.u32LastBadgeLo, g_notifySoft.u32LastBadgeHi,
               g_notifySoft.u32BadgeOutOfWord);

    /* Grep: udx: notify soft stats */
    udx_printk("udx: notify soft stats request_ok=%u request_fail=%u "
               "free=%u free_miss=%u disable=%u enable=%u dispatch=%u "
               "pulse_hit=%u pulse_latch=%u pulse_unmask=%u "
               "pulse_empty=%u pulse_bad=%u handler_invoke=%u "
               "badge_out_of_word=%u soft_log=%u "
               "last_line=%u last_badge_lo=0x%x last_badge_hi=0x%x\n",
               g_notifySoft.u32RequestOk, g_notifySoft.u32RequestFail,
               g_notifySoft.u32Free, g_notifySoft.u32FreeMiss,
               g_notifySoft.u32Disable, g_notifySoft.u32Enable,
               g_notifySoft.u32Dispatch, g_notifySoft.u32PulseHit,
               g_notifySoft.u32PulseLatch, g_notifySoft.u32PulseUnmask,
               g_notifySoft.u32PulseEmpty, g_notifySoft.u32PulseBad,
               g_notifySoft.u32HandlerInvoke,
               g_notifySoft.u32BadgeOutOfWord, g_notifySoft.u32SoftLog,
               g_notifySoft.u32LastLine, g_notifySoft.u32LastBadgeLo,
               g_notifySoft.u32LastBadgeHi);
}

/** Soft: one-shot protocol + inventory on first successful bind. */
static void
notify_soft_log_bind_once(void)
{
    if (g_fNotifySoftBindOnce != 0) {
        return;
    }
    g_fNotifySoftBindOnce = 1;
    /* greppable: udx: notify soft bind */
    udx_printk("udx: notify soft bind table_only=1 "
               "delivery=fire_irq_or_NOTIFY_WAIT\n");
    if (g_fNotifySoftProtocolOnce == 0) {
        g_fNotifySoftProtocolOnce = 1;
    }
    notify_soft_log();
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
 * greppable: udx: notify soft pulse
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
             * Table-only bind. NOTIFY_WAIT / host fire is pumped from
             * udx_run (core.c) or udx_host_fire_irq — not here.
             * Leave disable depth as-is so a quiesced line stays masked.
             *
             * Real Notification bind (vector → badge bit on a cap) is a
             * kernel/devmgr step; this slot is the UDX-side soft endpoint.
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
                g_aIrqPending[nIrq] = 0;
                g_aIrqDisableDepth[nIrq] = 0;
            }
            /* greppable: udx: notify soft unbind */
            notify_soft_inc(&g_notifySoft.u32Free);
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
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 1;
    }
    return g_aIrqDisableDepth[nIrq] != 0;
}

int
udx_irq_disable_depth(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 0;
    }
    return (int)g_aIrqDisableDepth[nIrq];
}

int
udx_irq_is_pending(int nIrq)
{
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        return 0;
    }
    return g_aIrqPending[nIrq] != 0;
}

void
udx_synchronize_irq(int nIrq)
{
    /*
     * Host / freestanding soft path: handlers run synchronously in
     * dispatch (no separate hard-IRQ thread). Nothing to wait on.
     * Real Notification multi-waiter park is kernel-side only (gap).
     */
    (void)nIrq;
}

const char *
udx_irq_name(int nIrq)
{
    int iSlot;

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
     * Convergence point for:
     *   host:  udx_host_fire_irq(n)     (soft pulse analog)
     *   full:  NOTIFY_WAIT badge bit n  (real Notification claim)
     * greppable: udx: notify soft pulse / fire_irq
     */
    if (nIrq < 0 || nIrq >= UDX_IRQ_MAX) {
        /* greppable: udx: notify soft pulse bad */
        notify_soft_inc(&g_notifySoft.u32PulseBad);
        return;
    }
    notify_soft_inc(&g_notifySoft.u32Dispatch);
    irq_dispatch_unlocked(nIrq);
}
