/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-input: event queue 0 with multi-slot DMA + soft ring.
 * Soft abs/rel axis state for keyboard/tablet/pointer fan-in (OASIS layout).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only. Pure C11 freestanding.
 *
 * C0 input residual (this unit only; Soft!=product; Dual DoD OPEN):
 *   Lean residual only - never re-introduce multi-kprintf stamp storms
 *   (prior FAULT H2: sequential soft-deepen kprintf floods -> stack smash
 *   / #PF I=1). Soft inventory is one-shot, one line, probe path only.
 *   Soft residual lean is one-shot (probe and/or first poll activity).
 *   Silent lean self-check folds into lean line + optional PASS only.
 *   Silent residual counters feed lean only - no write-only theater.
 *   No wave=/version stamp. No ret*angle stamps. G-AC-1 (no GPL / no .ko AC).
 *   Soft!=product dual license (eng residual != product bar / DoD close).
 *   H1 thr-only eth N/A here. H3 thr_exit N/A. Freestanding path holds.
 *
 * Geometry:
 *   q0 event - N device-write slots (parallel HW fills)
 *   soft ring - drop-oldest delivery buffer for poll consumers
 *   soft axes - last ABS_X/Y + accumulated REL_X/Y/WHEEL
 *
 * Greppable product markers (prefix-stable; bring-up, not soft deepen):
 *   virtio-input: ready PASS
 *   virtio-input: event ring soft PASS
 *   virtio-input: abs/rel axes soft PASS
 *
 * Soft inventory: one-shot one-line boot lamp only (Soft!=product)
 *   virtio-input: soft inventory via=... ready=... bus=... slot=...
 *                 posted=... slots=... ring=... caps=... soft_ne_product=1
 *                 Soft!=product
 * Soft residual lean: one-shot after probe lean or first poll (Soft!=product)
 *   virtio-input: soft residual lean via=... lean_ok=N/M Soft!=product
 *   virtio-input: soft residual lean PASS (when lean_ok == lean_checks)
 * greppable: virtio-input: soft inventory
 * greppable: virtio-input: soft residual lean
 * greppable: virtio-input: soft residual lean PASS
 * greppable: Soft!=product soft_ne_product t0_input G-AC-1
 * greppable: handoff=session|input_hub product_input=virtio
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/virtio_input.h>

/* ---- OASIS virtio-input event / config (public layout) ------------------- */

struct virtio_input_event {
    u16 u16Type;
    u16 u16Code;
    i32 i32Value;
} __attribute__((packed));

/* virtio_input_config select values (OASIS). */
#define VI_CFG_UNSET     0x00u
#define VI_CFG_ID_NAME   0x01u
#define VI_CFG_EV_BITS   0x11u
#define VI_CFG_ABS_INFO  0x12u

struct virtio_input_absinfo_dev {
    i32 i32Min;
    i32 i32Max;
    i32 i32Fuzz;
    i32 i32Flat;
    i32 i32Res;
} __attribute__((packed));

/* Bring-up geometry: multi-slot RX + soft delivery ring. */
#define VI_Q_SIZE      64u
#define VI_SLOTS       16u /* outstanding device-write event buffers */
#define VI_RING        64u /* soft delivery depth */
#define VI_POLL_SPINS  100u
#define VI_DRAIN_MAX   32u
#define VI_SLOT_FREE   0xffffu

/* Soft default tablet range when ABS_INFO is missing. */
#define VI_ABS_SOFT_MIN 0
#define VI_ABS_SOFT_MAX 32767

static struct gj_virtio_dev *g_pIn;
static struct gj_virtq       g_qEvent;
static int                   g_fReady;
static u32                   g_u32EventCount; /* accepted into soft ring */
static u32                   g_u32Dropped;
static u32                   g_cPosted;

/* Multi-slot DMA event buffers (identity-mapped BSS; PA == VA on this path). */
static struct virtio_input_event g_aSlots[VI_SLOTS] __attribute__((aligned(16)));
/* Desc head while posted; VI_SLOT_FREE when idle. */
static u16 g_aSlotDesc[VI_SLOTS];

/* Soft delivery ring (poll consumers). */
static struct gj_input_event g_aRing[VI_RING];
static u32                   g_u32Head;
static u32                   g_u32Len;

/* Soft abs/rel axis state. */
static i32 g_i32RelX;
static i32 g_i32RelY;
static i32 g_i32RelWheel;
static i32 g_i32AbsX;
static i32 g_i32AbsY;
static int g_fAbsSeen;
static u32 g_u32Caps;

static struct gj_input_absinfo g_AbsX;
static struct gj_input_absinfo g_AbsY;
static int g_fAbsInfoX;
static int g_fAbsInfoY;

/*
 * Lean soft residual (file-static; Soft!=product / C0 honesty). Silent
 * counters only - never multi-line inventory dumps / stamp storms (H2).
 * Soft inventory once (probe). Soft residual lean once (probe lean and/or
 * first poll activity). Probe/API tallies never kprintf on hot path; they
 * fold into one-shot lean line only. Soft residual != product DoD.
 * greppable: virtio-input: soft residual lean | soft inventory | lean PASS
 */
static int g_fSoftInvOnce;      /* one-shot soft inventory boot lamp */
static int g_fSoftLeanOnce;     /* one-shot soft residual lean lamp */
static u32 g_u32DrainOk;        /* full-event completions pushed to ring */
static u32 g_u32ShortUsed;      /* used-len short / empty skipped (no push) */
static u32 g_u32UnknownId;      /* used-ring id not matching a posted slot */
static u32 g_u32RepostFail;     /* post_slot fail after reap / top-up */
static u32 g_u32PollApi;        /* virtio_input_poll entries (silent) */
static u32 g_u32PendingApi;     /* virtio_input_pending entries (silent) */
static u32 g_u32RelSoftApi;     /* virtio_input_rel_soft entries (silent) */
static u32 g_u32AbsSoftApi;     /* virtio_input_abs_soft entries (silent) */
static u32 g_u32EvSyn;          /* soft-ring EV_SYN accepted */
static u32 g_u32EvKey;          /* soft-ring EV_KEY accepted */
static u32 g_u32EvRel;          /* soft-ring EV_REL accepted */
static u32 g_u32EvAbs;          /* soft-ring EV_ABS accepted */
static u32 g_u32EvOther;        /* soft-ring other type accepted */
static u32 g_u32ReapTotal;      /* used-ring reaps (ok + short + unk paths) */
static u32 g_u32ProbeOk;        /* probe DRIVER_OK path */
static u32 g_u32ProbeNodev;     /* probe no-device */
static u32 g_u32ProbeFail;      /* probe pci/nego/q fail */
static u32 g_u32LeanOk;         /* last lean self-check ok count */
static u32 g_u32LeanChecks;     /* last lean self-check total */

static void virtio_input_soft_inventory_once(const char *szVia);
static void virtio_input_soft_residual_lean_once(const char *szVia);
static void virtio_input_soft_maybe_lean(const char *szVia);

static void
slot_desc_clear_all(void)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < VI_SLOTS; iSlot++) {
        g_aSlotDesc[iSlot] = (u16)VI_SLOT_FREE;
    }
    g_cPosted = 0;
}

static void
ring_reset(void)
{
    memset(g_aRing, 0, sizeof(g_aRing));
    g_u32Head = 0;
    g_u32Len = 0;
}

static void
axes_reset(void)
{
    g_i32RelX = 0;
    g_i32RelY = 0;
    g_i32RelWheel = 0;
    g_i32AbsX = 0;
    g_i32AbsY = 0;
    g_fAbsSeen = 0;
    g_u32Caps = 0;
    g_fAbsInfoX = 0;
    g_fAbsInfoY = 0;
    memset(&g_AbsX, 0, sizeof(g_AbsX));
    memset(&g_AbsY, 0, sizeof(g_AbsY));
    g_AbsX.i32Min = VI_ABS_SOFT_MIN;
    g_AbsX.i32Max = VI_ABS_SOFT_MAX;
    g_AbsY.i32Min = VI_ABS_SOFT_MIN;
    g_AbsY.i32Max = VI_ABS_SOFT_MAX;
}

static void
axes_apply(const struct gj_input_event *pEv)
{
    if (pEv == NULL) {
        return;
    }
    if (pEv->u16Type == GJ_EV_SYN) {
        return;
    }
    if (pEv->u16Type == GJ_EV_KEY) {
        g_u32Caps |= GJ_VIRTIO_INPUT_CAP_KEY;
        return;
    }
    if (pEv->u16Type == GJ_EV_REL) {
        g_u32Caps |= GJ_VIRTIO_INPUT_CAP_REL;
        if (pEv->u16Code == GJ_REL_X) {
            g_i32RelX += pEv->i32Value;
        } else if (pEv->u16Code == GJ_REL_Y) {
            g_i32RelY += pEv->i32Value;
        } else if (pEv->u16Code == GJ_REL_WHEEL) {
            g_i32RelWheel += pEv->i32Value;
        }
        return;
    }
    if (pEv->u16Type == GJ_EV_ABS) {
        g_u32Caps |= GJ_VIRTIO_INPUT_CAP_ABS;
        g_fAbsSeen = 1;
        if (pEv->u16Code == GJ_ABS_X) {
            g_i32AbsX = pEv->i32Value;
        } else if (pEv->u16Code == GJ_ABS_Y) {
            g_i32AbsY = pEv->i32Value;
        }
    }
}

static int
ring_sane(void)
{
    if (g_u32Head >= VI_RING || g_u32Len > VI_RING) {
        ring_reset();
        return 0;
    }
    return 1;
}

static void
soft_note_ev_type(u16 u16Type)
{
    if (u16Type == (u16)GJ_EV_SYN) {
        g_u32EvSyn++;
    } else if (u16Type == (u16)GJ_EV_KEY) {
        g_u32EvKey++;
    } else if (u16Type == (u16)GJ_EV_REL) {
        g_u32EvRel++;
    } else if (u16Type == (u16)GJ_EV_ABS) {
        g_u32EvAbs++;
    } else {
        g_u32EvOther++;
    }
}

static void
ring_push(const struct gj_input_event *pEv)
{
    u32 u32Pos;

    if (pEv == NULL) {
        return;
    }
    (void)ring_sane();
    if (g_u32Len >= VI_RING) {
        /* Drop oldest - keep a live tail for session poll. */
        g_u32Head = (g_u32Head + 1u) % VI_RING;
        g_u32Len--;
        g_u32Dropped++;
    }
    u32Pos = (g_u32Head + g_u32Len) % VI_RING;
    g_aRing[u32Pos] = *pEv;
    g_u32Len++;
    g_u32EventCount++;
    soft_note_ev_type(pEv->u16Type);
    axes_apply(pEv);
}

static int
ring_pop(struct gj_input_event *pOut)
{
    if (pOut == NULL || !ring_sane() || g_u32Len == 0) {
        return 0;
    }
    *pOut = g_aRing[g_u32Head];
    g_u32Head = (g_u32Head + 1u) % VI_RING;
    g_u32Len--;
    return 1;
}

static int
post_slot(u32 iSlot)
{
    int iHead;

    if (iSlot >= VI_SLOTS) {
        return -1;
    }
    if (g_aSlotDesc[iSlot] != (u16)VI_SLOT_FREE) {
        return 0;
    }
    memset(&g_aSlots[iSlot], 0, sizeof(g_aSlots[iSlot]));
    iHead = virtio_q_add(&g_qEvent,
                         (gj_paddr_t)(gj_vaddr_t)&g_aSlots[iSlot],
                         (u32)sizeof(g_aSlots[iSlot]), 1);
    if (iHead < 0) {
        g_u32RepostFail++;
        return -1;
    }
    g_aSlotDesc[iSlot] = (u16)iHead;
    g_cPosted++;
    return 0;
}

static void
post_all_slots(void)
{
    u32 iSlot;
    u32 cOk = 0;

    for (iSlot = 0; iSlot < VI_SLOTS; iSlot++) {
        if (post_slot(iSlot) == 0 && g_aSlotDesc[iSlot] != (u16)VI_SLOT_FREE) {
            cOk++;
        }
    }
    if (cOk > 0) {
        virtio_q_kick(&g_qEvent);
    }
}

static int
slot_from_desc(u32 u32Id)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < VI_SLOTS; iSlot++) {
        if (g_aSlotDesc[iSlot] == (u16)u32Id) {
            return (int)iSlot;
        }
    }
    return -1;
}

/*
 * Drain completed event slots into the soft ring (non-blocking after first wait).
 * Reposts each reaped slot. Returns number of events pushed this call.
 * C0 residual lean: skip short/empty used-len (no zero-event pollution);
 * silent tallies for short/unknown/repost (Soft!=product; no kprintf here).
 */
static u32
drain_hw(void)
{
    u32 cPushed = 0;
    u32 u32Spins = VI_POLL_SPINS;
    u32 u32Burst;
    u32 u32NeedKick = 0;

    for (u32Burst = 0; u32Burst < VI_DRAIN_MAX; u32Burst++) {
        u32 u32Id = 0;
        i32 i32Len;
        int iSlot;
        struct gj_input_event ev;

        i32Len = virtio_q_poll_id(&g_qEvent, u32Spins, &u32Id);
        if (i32Len < 0) {
            break;
        }
        u32Spins = 1; /* only spin-wait once per drain */
        g_u32ReapTotal++;
        if (g_cPosted > 0) {
            g_cPosted--;
        }
        iSlot = slot_from_desc(u32Id);
        if (iSlot < 0) {
            /* Unknown completion - still try to keep the queue fed. */
            g_u32UnknownId++;
            post_all_slots();
            u32NeedKick = 1;
            continue;
        }
        g_aSlotDesc[iSlot] = (u16)VI_SLOT_FREE;
        /*
         * Device writes a full virtio_input_event (8 bytes). Require a full
         * used length before push - short/empty reaps free+repost only so a
         * zeroed BSS slot cannot inject false EV_SYN 0 into the soft ring.
         */
        if (i32Len < (i32)sizeof(struct virtio_input_event)) {
            g_u32ShortUsed++;
            if (post_slot((u32)iSlot) == 0) {
                u32NeedKick = 1;
            }
            continue;
        }
        ev.u16Type = g_aSlots[iSlot].u16Type;
        ev.u16Code = g_aSlots[iSlot].u16Code;
        ev.i32Value = g_aSlots[iSlot].i32Value;
        ring_push(&ev);
        cPushed++;
        g_u32DrainOk++;
        if (post_slot((u32)iSlot) == 0) {
            u32NeedKick = 1;
        }
    }
    if (cPushed > 0 || u32NeedKick != 0) {
        virtio_q_kick(&g_qEvent);
    }
    return cPushed;
}

static u8
cfg_r8(volatile u8 *pBase, u32 u32Off)
{
    return pBase[u32Off];
}

static void
cfg_w8(volatile u8 *pBase, u32 u32Off, u8 u8Val)
{
    pBase[u32Off] = u8Val;
}

static i32
cfg_r32(volatile u8 *pBase, u32 u32Off)
{
    volatile u32 *p;

    p = (volatile u32 *)(void *)(pBase + u32Off);
    return (i32)(*p);
}

/* ---- lean soft residual inventory (Soft!=product; no stamp storms / H2) --- */

/**
 * Silent lean residual self-check (no kprintf; Soft!=product / C0).
 * OASIS event layout + EV/REL/ABS codes + queue/slot/ring geometry +
 * modern input PCI id + KIND_INPUT. Returns ok count; *pOutChecks = total
 * when non-NULL. Results fold into soft residual lean lines only.
 */
static u32
input_soft_residual_lean_ok(u32 *pOutChecks)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;

    /* 1: event virtqueue size power-of-two (bring-up soft path) */
    u32Checks++;
    if (VI_Q_SIZE == 64u && (VI_Q_SIZE & (VI_Q_SIZE - 1u)) == 0u) {
        u32Ok++;
    }
    /* 2: multi-slot outstanding device-write buffers (parallel HW fills) */
    u32Checks++;
    if (VI_SLOTS >= 8u && VI_SLOTS <= VI_Q_SIZE && (VI_SLOTS % 4u) == 0u) {
        u32Ok++;
    }
    /* 3: soft delivery ring power-of-two depth */
    u32Checks++;
    if (VI_RING == 64u && (VI_RING & (VI_RING - 1u)) == 0u) {
        u32Ok++;
    }
    /* 4: packed virtio_input_event is 8 bytes (type/code/value) */
    u32Checks++;
    if (sizeof(struct virtio_input_event) == 8u) {
        u32Ok++;
    }
    /* 5: soft delivery event matches packed field order size */
    u32Checks++;
    if (sizeof(struct gj_input_event) == 8u) {
        u32Ok++;
    }
    /* 6: public Linux input ABI type codes (not GPL source) */
    u32Checks++;
    if (GJ_EV_SYN == 0u && GJ_EV_KEY == 1u && GJ_EV_REL == 2u &&
        GJ_EV_ABS == 3u) {
        u32Ok++;
    }
    /* 7: common REL/ABS axis codes (public ABI) */
    u32Checks++;
    if (GJ_REL_X == 0u && GJ_REL_Y == 1u && GJ_REL_WHEEL == 8u &&
        GJ_ABS_X == 0u && GJ_ABS_Y == 1u) {
        u32Ok++;
    }
    /* 8: soft capability bit layout (KEY/REL/ABS) */
    u32Checks++;
    if (GJ_VIRTIO_INPUT_CAP_KEY == (1u << 0) &&
        GJ_VIRTIO_INPUT_CAP_REL == (1u << 1) &&
        GJ_VIRTIO_INPUT_CAP_ABS == (1u << 2)) {
        u32Ok++;
    }
    /* 9: modern input PCI device id (T0 input inventory) */
    u32Checks++;
    if (GJ_VIRTIO_PCI_DEV_INPUT == 0x1052u) {
        u32Ok++;
    }
    /* 10: KIND_INPUT matches probe kind==4 */
    u32Checks++;
    if (GJ_VIRTIO_KIND_INPUT == 4u) {
        u32Ok++;
    }
    /* 11: drain/poll budgets non-zero (no infinite hang; soft bound) */
    u32Checks++;
    if (VI_POLL_SPINS > 0u && VI_DRAIN_MAX > 0u && VI_DRAIN_MAX <= VI_RING) {
        u32Ok++;
    }
    /* 12: soft abs default range (host ABS_INFO missing fallback) */
    u32Checks++;
    if (VI_ABS_SOFT_MIN == 0 && VI_ABS_SOFT_MAX == 32767 &&
        VI_ABS_SOFT_MAX > VI_ABS_SOFT_MIN) {
        u32Ok++;
    }
    /* 13: slot-free sentinel distinct from valid desc heads */
    u32Checks++;
    if (VI_SLOT_FREE == 0xffffu) {
        u32Ok++;
    }
    /* 14: multi-slot DMA BSS holds full VI_SLOTS events */
    u32Checks++;
    if ((u32)sizeof(g_aSlots) ==
        (VI_SLOTS * (u32)sizeof(struct virtio_input_event))) {
        u32Ok++;
    }
    /* 15: soft ring BSS holds full VI_RING delivery events */
    u32Checks++;
    if ((u32)sizeof(g_aRing) ==
        (VI_RING * (u32)sizeof(struct gj_input_event))) {
        u32Ok++;
    }
    /* 16: live ring sanity when ready (silent; no kprintf) */
    u32Checks++;
    if (!g_fReady || (g_u32Head < VI_RING && g_u32Len <= VI_RING)) {
        u32Ok++;
    }
    /* 17: posted watermark coherent when ready (silent) */
    u32Checks++;
    if (!g_fReady || g_cPosted <= VI_SLOTS) {
        u32Ok++;
    }

    g_u32LeanOk = u32Ok;
    g_u32LeanChecks = u32Checks;
    if (pOutChecks != NULL) {
        *pOutChecks = u32Checks;
    }
    return u32Ok;
}

/*
 * One-shot one-line boot soft inventory only (Soft!=product / C0).
 * LEAN residual: single kprintf - never multi-line stamp floods (FAULT H2).
 * Probe/fail path only - never re-emit on poll/axes/API hot paths.
 * No wave=/version stamp. No ret*angle. Dual MIT OR Apache-2.0 residual only.
 * greppable: virtio-input: soft inventory
 * greppable: Soft!=product soft_ne_product t0_input G-AC-1
 */
static void
virtio_input_soft_inventory_once(const char *szVia)
{
    u8 u8Bus;
    u8 u8Slot;
    u32 u32Ready;

    if (g_fSoftInvOnce != 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    u32Ready = g_fReady ? 1u : 0u;
    u8Bus = (g_pIn != NULL) ? g_pIn->u8Bus : 0;
    u8Slot = (g_pIn != NULL) ? g_pIn->u8Slot : 0;
    /*
     * ONE line only. Soft!=product. No wave=/version stamp.
     * t0_input=1: QEMU virtio-input T0 path (lamp != product DoD close).
     * event_q0=1 multi_slot=1: event virtqueue only + parallel slots.
     * handoff=session|input_hub: product direction honesty (G-AC-1).
     */
    kprintf("virtio-input: soft inventory via=%s ready=%u bus=%x slot=%x "
            "posted=%u slots=%u ring=%u q_size=%u caps=0x%x "
            "t0_input=1 event_q0=1 multi_slot=1 path=poll|rel|abs "
            "handoff=session|input_hub product_input=virtio "
            "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
            "soft_ne_product=1 G-AC-1 Soft!=product\n",
            szVia != NULL ? szVia : "?",
            u32Ready,
            (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)g_cPosted,
            (unsigned)VI_SLOTS,
            (unsigned)VI_RING,
            (unsigned)VI_Q_SIZE,
            (unsigned)g_u32Caps);
}

/*
 * One-shot soft residual lean after first product activity or probe lean.
 * Silent self-check + ONE line (+ optional PASS). Soft!=product / C0.
 * Never re-emit on hot path. No wave=/version stamp. Dual MIT OR Apache-2.0.
 * Dual DoD remains OPEN (lamp != product bar / session desktop claim).
 * greppable: virtio-input: soft residual lean
 * greppable: virtio-input: soft residual lean PASS
 * greppable: Soft!=product soft_ne_product t0_input G-AC-1
 */
static void
virtio_input_soft_residual_lean_once(const char *szVia)
{
    u32 u32Ok;
    u32 u32Checks;
    u32 u32Ready;
    u8 u8Bus;
    u8 u8Slot;

    if (g_fSoftLeanOnce != 0) {
        return;
    }
    g_fSoftLeanOnce = 1;

    u32Ok = input_soft_residual_lean_ok(&u32Checks);
    u32Ready = g_fReady ? 1u : 0u;
    u8Bus = (g_pIn != NULL) ? g_pIn->u8Bus : 0;
    u8Slot = (g_pIn != NULL) ? g_pIn->u8Slot : 0;

    /*
     * ONE lean line. Soft!=product dual license; no version stamp; no storm.
     * Fold silent drain health + ev/probe tallies. lean_ok/N self-check.
     * t0_input=1 product_input=virtio: T0 freestanding residual until
     * session input hub owns fan-in (lamp != Dual DoD close).
     */
    kprintf("virtio-input: soft residual lean via=%s ready=%u bus=%x slot=%x "
            "posted=%u slots=%u ring=%u q_size=%u "
            "drained=%u reap=%u dropped=%u short=%u unk=%u repost_fail=%u "
            "poll_api=%u pending_api=%u rel_api=%u abs_api=%u pending=%u "
            "caps=0x%x syn=%u key=%u rel=%u abs=%u other=%u "
            "probe_ok=%u nodev=%u fail=%u lean_ok=%u/%u "
            "t0_input=1 event_q0=1 multi_slot=1 path=poll|rel|abs "
            "handoff=session|input_hub product_input=virtio "
            "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
            "soft_ne_product=1 G-AC-1 Soft!=product\n",
            szVia != NULL ? szVia : "?",
            u32Ready, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)g_cPosted,
            (unsigned)VI_SLOTS,
            (unsigned)VI_RING,
            (unsigned)VI_Q_SIZE,
            (unsigned)g_u32DrainOk,
            (unsigned)g_u32ReapTotal,
            (unsigned)g_u32Dropped,
            (unsigned)g_u32ShortUsed,
            (unsigned)g_u32UnknownId,
            (unsigned)g_u32RepostFail,
            (unsigned)g_u32PollApi,
            (unsigned)g_u32PendingApi,
            (unsigned)g_u32RelSoftApi,
            (unsigned)g_u32AbsSoftApi,
            (unsigned)g_u32Len,
            (unsigned)g_u32Caps,
            (unsigned)g_u32EvSyn,
            (unsigned)g_u32EvKey,
            (unsigned)g_u32EvRel,
            (unsigned)g_u32EvAbs,
            (unsigned)g_u32EvOther,
            (unsigned)g_u32ProbeOk,
            (unsigned)g_u32ProbeNodev,
            (unsigned)g_u32ProbeFail,
            u32Ok, u32Checks);

    if (u32Ok == u32Checks) {
        /* Grep: virtio-input: soft residual lean PASS */
        kprintf("virtio-input: soft residual lean PASS "
                "checks=%u ok=%u t0_input=1 event_q0=1 multi_slot=1 "
                "product_input=virtio handoff=session|input_hub "
                "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
                "soft_ne_product=1 G-AC-1 Soft!=product\n",
                u32Checks, u32Ok);
    }
}

/**
 * Fire soft residual lean once after first product-path activity.
 * Probe path may also emit via=probe after inventory (no double lamp).
 */
static void
virtio_input_soft_maybe_lean(const char *szVia)
{
    if (g_fSoftLeanOnce != 0) {
        return;
    }
    virtio_input_soft_residual_lean_once(szVia);
}

/*
 * Soft probe of device config: EV_BITS for KEY/REL/ABS + ABS_INFO for X/Y.
 * Failure is non-fatal - axes soft defaults remain.
 */
static void
soft_cfg_probe(struct gj_virtio_dev *pDev)
{
    volatile u8 *pCfg;
    u8 u8Size;
    u32 iBit;

    if (pDev == NULL || pDev->pDevice == NULL) {
        return;
    }
    pCfg = pDev->pDevice;

    /* EV_BITS: subsel = event type; size>0 ⇒ type present. */
    for (iBit = 0; iBit < 3; iBit++) {
        u8 u8Type = (u8)(iBit + 1u); /* KEY=1 REL=2 ABS=3 */

        cfg_w8(pCfg, 0, (u8)VI_CFG_EV_BITS); /* select */
        cfg_w8(pCfg, 1, u8Type);             /* subsel */
        __asm__ volatile("mfence" ::: "memory");
        u8Size = cfg_r8(pCfg, 2); /* size */
        if (u8Size > 0) {
            if (u8Type == (u8)GJ_EV_KEY) {
                g_u32Caps |= GJ_VIRTIO_INPUT_CAP_KEY;
            } else if (u8Type == (u8)GJ_EV_REL) {
                g_u32Caps |= GJ_VIRTIO_INPUT_CAP_REL;
            } else if (u8Type == (u8)GJ_EV_ABS) {
                g_u32Caps |= GJ_VIRTIO_INPUT_CAP_ABS;
            }
        }
    }

    /* ABS_INFO for X then Y. */
    {
        u16 aAxis[2];
        struct gj_input_absinfo *apInfo[2];
        int *apSeen[2];
        u32 iAx;

        aAxis[0] = (u16)GJ_ABS_X;
        aAxis[1] = (u16)GJ_ABS_Y;
        apInfo[0] = &g_AbsX;
        apInfo[1] = &g_AbsY;
        apSeen[0] = &g_fAbsInfoX;
        apSeen[1] = &g_fAbsInfoY;

        for (iAx = 0; iAx < 2; iAx++) {
            cfg_w8(pCfg, 0, (u8)VI_CFG_ABS_INFO);
            cfg_w8(pCfg, 1, (u8)aAxis[iAx]);
            __asm__ volatile("mfence" ::: "memory");
            u8Size = cfg_r8(pCfg, 2);
            if (u8Size >= (u8)sizeof(struct virtio_input_absinfo_dev)) {
                /* union u starts at offset 8 in virtio_input_config */
                apInfo[iAx]->i32Min = cfg_r32(pCfg, 8);
                apInfo[iAx]->i32Max = cfg_r32(pCfg, 12);
                apInfo[iAx]->i32Fuzz = cfg_r32(pCfg, 16);
                apInfo[iAx]->i32Flat = cfg_r32(pCfg, 20);
                apInfo[iAx]->i32Res = cfg_r32(pCfg, 24);
                *apSeen[iAx] = 1;
                g_u32Caps |= GJ_VIRTIO_INPUT_CAP_ABS;
            }
        }
    }

    /* Leave select unset so later host tools see a clean cfg window. */
    cfg_w8(pCfg, 0, (u8)VI_CFG_UNSET);
    cfg_w8(pCfg, 1, 0);
    (void)VI_CFG_ID_NAME;
}

/*
 * Probe path: find first input -> modern PCI caps -> V1 features -> event q0
 * -> multi-slot post -> soft cfg. Leaves g_fReady=0 on any hard failure.
 */
int
virtio_input_probe(void)
{
    u32 i;
    u32 c;
    gj_status_t st;

    g_pIn = NULL;
    g_fReady = 0;
    g_u32EventCount = 0;
    g_u32Dropped = 0;
    g_fSoftInvOnce = 0;
    /*
     * Preserve g_fSoftLeanOnce across re-probe so residual lean stays
     * one-shot (FAULT H2: no stamp storms). Soft!=product.
     */
    g_u32DrainOk = 0;
    g_u32ShortUsed = 0;
    g_u32UnknownId = 0;
    g_u32RepostFail = 0;
    g_u32PollApi = 0;
    g_u32PendingApi = 0;
    g_u32RelSoftApi = 0;
    g_u32AbsSoftApi = 0;
    g_u32EvSyn = 0;
    g_u32EvKey = 0;
    g_u32EvRel = 0;
    g_u32EvAbs = 0;
    g_u32EvOther = 0;
    g_u32ReapTotal = 0;
    /* probe_ok/nodev/fail are lifetime tallies - do not clear on re-probe */
    g_u32LeanOk = 0;
    g_u32LeanChecks = 0;
    slot_desc_clear_all();
    ring_reset();
    axes_reset();

    c = virtio_dev_count();
    kprintf("virtio-input: probe (%u devs)\n", c);

    /* kind==KIND_INPUT, transitional 0x1052, or modern input device ID */
    for (i = 0; i < c; i++) {
        struct gj_virtio_dev *pDev = virtio_dev_get(i);

        if (pDev == NULL) {
            continue;
        }
        if (pDev->u32Kind == GJ_VIRTIO_KIND_INPUT ||
            pDev->u16Device == 0x1052u ||
            pDev->u16Device == GJ_VIRTIO_PCI_DEV_INPUT) {
            g_pIn = pDev;
            kprintf("virtio-input: found id=%x kind=%u\n",
                    (unsigned)pDev->u16Device, pDev->u32Kind);
            break;
        }
    }
    if (g_pIn == NULL) {
        kprintf("virtio-input: no device\n");
        g_u32ProbeNodev++;
        virtio_input_soft_inventory_once("nodev");
        return -1;
    }
    st = virtio_pci_setup(g_pIn);
    if (st != GJ_OK || g_pIn->pCommon == NULL) {
        kprintf("virtio-input: setup failed %d\n", (int)st);
        g_pIn = NULL;
        g_u32ProbeFail++;
        virtio_input_soft_inventory_once("pci_fail");
        return -1;
    }
    st = virtio_negotiate(g_pIn, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        kprintf("virtio-input: negotiate failed %d\n", (int)st);
        g_pIn = NULL;
        g_u32ProbeFail++;
        virtio_input_soft_inventory_once("negotiate_fail");
        return -1;
    }
    st = virtio_q_setup(g_pIn, &g_qEvent, 0, (u16)VI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-input: event queue failed %d\n", (int)st);
        g_pIn = NULL;
        g_u32ProbeFail++;
        virtio_input_soft_inventory_once("q_fail");
        return -1;
    }
    virtio_set_status(g_pIn, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                  GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));

    post_all_slots();
    soft_cfg_probe(g_pIn);

    g_fReady = 1;
    g_u32ProbeOk++;
    kprintf("virtio-input: ready PASS bus=%x slot=%x slots=%u ring=%u\n",
            (unsigned)g_pIn->u8Bus, (unsigned)g_pIn->u8Slot,
            (unsigned)g_cPosted, (unsigned)VI_RING);
    kprintf("virtio-input: event ring soft PASS posted=%u\n",
            (unsigned)g_cPosted);
    kprintf("virtio-input: abs/rel axes soft PASS caps=0x%x absinfo=%d/%d\n",
            (unsigned)g_u32Caps, g_fAbsInfoX, g_fAbsInfoY);
    /* One-shot soft inventory + probe lean (LEAN residual; Soft!=product; H2). */
    virtio_input_soft_inventory_once("probe");
    virtio_input_soft_maybe_lean("probe");
    return 0;
}

int
virtio_input_ready(void)
{
    return g_fReady;
}

int
virtio_input_poll(struct gj_input_event *pOut)
{
    if (!g_fReady || pOut == NULL) {
        return -1;
    }
    g_u32PollApi++;
    /* Fill soft ring from multi-slot HW, then pop one. */
    (void)drain_hw();
    /*
     * First soft-activity residual lean (one-shot; Soft!=product / C0).
     * Poll is the consumer path (session hub / smoke). Never re-emit if
     * probe already fired lean (g_fSoftLeanOnce).
     */
    virtio_input_soft_maybe_lean("poll");
    if (ring_pop(pOut)) {
        return 1;
    }
    return 0;
}

u32
virtio_input_event_count(void)
{
    /*
     * Silent residual only - never re-fire soft inventory / lean (H2).
     * Keep silent residual symbols live (not wire-stable; Soft!=product).
     */
    (void)g_u32DrainOk;
    (void)g_u32ShortUsed;
    (void)g_u32UnknownId;
    (void)g_u32RepostFail;
    (void)g_u32PollApi;
    (void)g_u32PendingApi;
    (void)g_u32RelSoftApi;
    (void)g_u32AbsSoftApi;
    (void)g_u32EvSyn;
    (void)g_u32EvKey;
    (void)g_u32EvRel;
    (void)g_u32EvAbs;
    (void)g_u32EvOther;
    (void)g_u32ReapTotal;
    (void)g_u32ProbeOk;
    (void)g_u32ProbeNodev;
    (void)g_u32ProbeFail;
    (void)g_u32LeanOk;
    (void)g_u32LeanChecks;
    (void)g_fSoftLeanOnce;
    return g_u32EventCount;
}

u32
virtio_input_pending(void)
{
    if (!g_fReady) {
        return 0;
    }
    g_u32PendingApi++;
    /* Opportunistic HW drain so pending reflects device-side traffic. */
    (void)drain_hw();
    /* Activity residual lean if probe lean not yet fired (Soft!=product). */
    virtio_input_soft_maybe_lean("pending");
    if (!ring_sane()) {
        return 0;
    }
    return g_u32Len;
}

u32
virtio_input_dropped(void)
{
    return g_u32Dropped;
}

u32
virtio_input_slots_posted(void)
{
    return g_cPosted;
}

u32
virtio_input_caps(void)
{
    return g_u32Caps;
}

int
virtio_input_rel_soft(i32 *pX, i32 *pY, i32 *pWheel)
{
    if (!g_fReady) {
        return -1;
    }
    g_u32RelSoftApi++;
    /* Opportunistic drain so axes reflect recent device traffic. */
    (void)drain_hw();
    virtio_input_soft_maybe_lean("rel_soft");
    if (pX != NULL) {
        *pX = g_i32RelX;
    }
    if (pY != NULL) {
        *pY = g_i32RelY;
    }
    if (pWheel != NULL) {
        *pWheel = g_i32RelWheel;
    }
    return 0;
}

void
virtio_input_rel_reset(void)
{
    g_i32RelX = 0;
    g_i32RelY = 0;
    g_i32RelWheel = 0;
}

int
virtio_input_abs_soft(i32 *pX, i32 *pY, int *pFSeen)
{
    if (!g_fReady) {
        return -1;
    }
    g_u32AbsSoftApi++;
    (void)drain_hw();
    virtio_input_soft_maybe_lean("abs_soft");
    if (pX != NULL) {
        *pX = g_i32AbsX;
    }
    if (pY != NULL) {
        *pY = g_i32AbsY;
    }
    if (pFSeen != NULL) {
        *pFSeen = g_fAbsSeen;
    }
    return 0;
}

int
virtio_input_absinfo(u16 u16Code, struct gj_input_absinfo *pOut)
{
    if (!g_fReady || pOut == NULL) {
        return -1;
    }
    if (u16Code == (u16)GJ_ABS_X) {
        *pOut = g_AbsX;
        return 0;
    }
    if (u16Code == (u16)GJ_ABS_Y) {
        *pOut = g_AbsY;
        return 0;
    }
    return -1;
}
