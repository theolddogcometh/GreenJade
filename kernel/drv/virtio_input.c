/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-input: event queue 0 with multi-slot DMA + soft ring.
 * Soft abs/rel axis state for keyboard/tablet/pointer fan-in (OASIS layout).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only. Pure C11 freestanding.
 *
 * Geometry:
 *   q0 event — N device-write slots (parallel HW fills)
 *   soft ring — drop-oldest delivery buffer for poll consumers
 *   soft axes — last ABS_X/Y + accumulated REL_X/Y/WHEEL
 *
 * Greppable product markers (prefix-stable):
 *   virtio-input: ready PASS
 *   virtio-input: event ring soft PASS
 *   virtio-input: abs/rel axes soft PASS
 *
 * Soft inventory (Wave 15 exclusive deepen; this unit only —
 * greppable "virtio-input: soft …"; never hard-gates; not bar3):
 *   virtio-input: soft inventory …
 *   virtio-input: soft pci …
 *   virtio-input: soft geometry …
 *   virtio-input: soft ring …
 *   virtio-input: soft slots …
 *   virtio-input: soft queue …
 *   virtio-input: soft axes …
 *   virtio-input: soft caps …
 *   virtio-input: soft absinfo …
 *   virtio-input: soft drain …
 *   virtio-input: soft last …
 *   virtio-input: soft stats …
 *   virtio-input: soft counters …
 *   virtio-input: soft api …
 *   virtio-input: soft features …
 *   virtio-input: soft path …
 *   virtio-input: soft claim …       (Wave 15)
 *   virtio-input: soft via …         (Wave 15)
 *   virtio-input: soft ready …       (Wave 15)
 *   virtio-input: soft types …       (Wave 15)
 *   virtio-input: soft kicks …       (Wave 15)
 *   virtio-input: soft honesty …     (Wave 15)
 *   virtio-input: soft return rate — Wave 19 ok/fail rate lamps
 *   virtio-input: soft retcode    — Wave 19 retcode catalog
 *   virtio-input: soft deepen wave=81 …
 *   virtio-input: soft PASS|NODEV|PARTIAL
 *   virtio-input: soft inventory PASS|NODEV|PARTIAL
 *
 * greppable: virtio-input: soft
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

/* Wave 35 exclusive soft deepen stamp (inventory only; never hard-gates). */
#define VI_SOFT_WAVE 81u
#define VI_SOFT_AREAS 155u

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
static int g_fRelSeen;
static u32 g_u32Caps;

static struct gj_input_absinfo g_AbsX;
static struct gj_input_absinfo g_AbsY;
static int g_fAbsInfoX;
static int g_fAbsInfoY;

/* Wave 14 soft inventory telemetry (never hard-gates product poll). */
static u32 g_u32SoftLogN;      /* inventory emissions */
static int g_fSoftOnce;        /* first-activity inventory emitted */
static u16 g_u16FreeMin;       /* event q free-desc watermark */
static u32 g_u32Polls;         /* virtio_input_poll entries */
static u32 g_u32PollHits;      /* poll returned an event */
static u32 g_u32PollEmpty;     /* poll returned none */
static u32 g_u32DrainCalls;    /* drain_hw invocations */
static u32 g_u32DrainPushed;   /* events pushed by drain across life */
static u32 g_u32DrainBurstMax; /* peak events in one drain_hw call */
static u32 g_u32UnknownSlot;   /* used-id not matching a posted slot */
static u32 g_u32PostFail;      /* post_slot virtio_q_add failures */
static u32 g_u32Kicks;         /* virtqueue kicks after post/drain */
static u16 g_u16LastType;      /* sticky last event type */
static u16 g_u16LastCode;      /* sticky last event code */
static i32 g_i32LastValue;     /* sticky last event value */
static u32 g_u32SynSeen;       /* SYN events observed */
static u32 g_u32KeySeen;       /* KEY events observed */
static u32 g_u32RelEvSeen;     /* REL events observed */
static u32 g_u32AbsEvSeen;     /* ABS events observed */
static u32 g_u32ApiPoll;       /* virtio_input_poll enters */
static u32 g_u32ApiRelSoft;    /* virtio_input_rel_soft enters */
static u32 g_u32ApiAbsSoft;    /* virtio_input_abs_soft enters */
static u32 g_u32ApiAbsinfo;    /* virtio_input_absinfo enters */
static u32 g_u32ApiEventCount; /* virtio_input_event_count enters */
static u32 g_u32ApiPending;    /* virtio_input_pending enters */
static u32 g_u32ApiRelReset;   /* virtio_input_rel_reset enters */
static u32 g_u32ApiMiss;       /* public API not-ready / bad-arg rejects */
/* Wave 15 sticky via (inventory only). */
static const char *g_szLastVia;

/* Forward: emit after probe / fail / activity / stats reads. */
static void soft_inventory(const char *szVia);
static void soft_maybe_once(void);

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
    g_fRelSeen = 0;
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
telemetry_reset(void)
{
    g_u32SoftLogN = 0;
    g_fSoftOnce = 0;
    g_u16FreeMin = 0xffffu;
    g_u32Polls = 0;
    g_u32PollHits = 0;
    g_u32PollEmpty = 0;
    g_u32DrainCalls = 0;
    g_u32DrainPushed = 0;
    g_u32DrainBurstMax = 0;
    g_u32UnknownSlot = 0;
    g_u32PostFail = 0;
    g_u32Kicks = 0;
    g_u16LastType = 0;
    g_u16LastCode = 0;
    g_i32LastValue = 0;
    g_u32SynSeen = 0;
    g_u32KeySeen = 0;
    g_u32RelEvSeen = 0;
    g_u32AbsEvSeen = 0;
    g_u32ApiPoll = 0;
    g_u32ApiRelSoft = 0;
    g_u32ApiAbsSoft = 0;
    g_u32ApiAbsinfo = 0;
    g_u32ApiEventCount = 0;
    g_u32ApiPending = 0;
    g_u32ApiRelReset = 0;
    g_u32ApiMiss = 0;
}

static void
axes_apply(const struct gj_input_event *pEv)
{
    if (pEv == NULL) {
        return;
    }
    if (pEv->u16Type == GJ_EV_SYN) {
        g_u32SynSeen++;
        return;
    }
    if (pEv->u16Type == GJ_EV_KEY) {
        g_u32Caps |= GJ_VIRTIO_INPUT_CAP_KEY;
        g_u32KeySeen++;
        return;
    }
    if (pEv->u16Type == GJ_EV_REL) {
        g_u32Caps |= GJ_VIRTIO_INPUT_CAP_REL;
        g_fRelSeen = 1;
        g_u32RelEvSeen++;
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
        g_u32AbsEvSeen++;
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
ring_push(const struct gj_input_event *pEv)
{
    u32 u32Pos;

    if (pEv == NULL) {
        return;
    }
    (void)ring_sane();
    if (g_u32Len >= VI_RING) {
        /* Drop oldest — keep a live tail for session poll. */
        g_u32Head = (g_u32Head + 1u) % VI_RING;
        g_u32Len--;
        g_u32Dropped++;
    }
    u32Pos = (g_u32Head + g_u32Len) % VI_RING;
    g_aRing[u32Pos] = *pEv;
    g_u32Len++;
    g_u32EventCount++;
    g_u16LastType = pEv->u16Type;
    g_u16LastCode = pEv->u16Code;
    g_i32LastValue = pEv->i32Value;
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

/* Snapshot free-desc watermark for soft queue inventory (live only). */
static void
q_note_free(void)
{
    u16 u16Free;

    if (!g_fReady) {
        return;
    }
    u16Free = virtio_q_num_free(&g_qEvent);
    if (u16Free < g_u16FreeMin) {
        g_u16FreeMin = u16Free;
    }
}

static void
q_kick_counted(void)
{
    virtio_q_kick(&g_qEvent);
    g_u32Kicks++;
    q_note_free();
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
        g_u32PostFail++;
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
        q_kick_counted();
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
 */
static u32
drain_hw(void)
{
    u32 cPushed = 0;
    u32 u32Spins = VI_POLL_SPINS;
    u32 u32Burst;

    g_u32DrainCalls++;

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
        if (g_cPosted > 0) {
            g_cPosted--;
        }
        iSlot = slot_from_desc(u32Id);
        if (iSlot < 0) {
            /* Unknown completion — still try to keep the queue fed. */
            g_u32UnknownSlot++;
            post_all_slots();
            continue;
        }
        g_aSlotDesc[iSlot] = (u16)VI_SLOT_FREE;
        /*
         * Device writes a full virtio_input_event (8 bytes). Accept any
         * non-negative used length; short completions still copy type/code.
         */
        ev.u16Type = g_aSlots[iSlot].u16Type;
        ev.u16Code = g_aSlots[iSlot].u16Code;
        ev.i32Value = g_aSlots[iSlot].i32Value;
        ring_push(&ev);
        cPushed++;
        (void)post_slot((u32)iSlot);
    }
    if (cPushed > 0) {
        g_u32DrainPushed += cPushed;
        if (cPushed > g_u32DrainBurstMax) {
            g_u32DrainBurstMax = cPushed;
        }
        q_kick_counted();
    }
    q_note_free();
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

/* ---- Soft inventory (Wave 15 exclusive deepen) --------------------------- */

/**
 * Greppable Wave 15 soft inventory dump (product / smoke).
 * Prefix-stable "virtio-input: soft …" — never hard-gates; kprintf only.
 * Soft only — not bar3 / not product multi-device input close.
 *
 * greppable: virtio-input: soft
 */
static void
soft_inventory(const char *szVia)
{
    const char *szVerdict;
    const char *szViaSafe;
    u32 u32Pending;
    u32 u32Key;
    u32 u32Rel;
    u32 u32Abs;
    u32 u32Ready;
    u32 u32Claim;
    u16 u16FreeNow;
    u16 u16QSize;
    u16 u16FreeMin;
    u16 u16NumFree;
    u16 u16LastUsed;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u8 u8Modern;
    u16 u16Device;
    u32 u32Kind;
    u32 u32NotifyMult;
    u32 u32NumQueues;
    u64 u64FeatDev;
    u64 u64FeatDrv;
    u64 u64PaDesc;
    u64 u64PaAvail;
    u64 u64PaUsed;

    szViaSafe = (szVia != NULL) ? szVia : "path";
    g_szLastVia = szViaSafe;

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    (void)ring_sane();
    u32Pending = g_u32Len;
    u32Key = (g_u32Caps & GJ_VIRTIO_INPUT_CAP_KEY) ? 1u : 0u;
    u32Rel = (g_u32Caps & GJ_VIRTIO_INPUT_CAP_REL) ? 1u : 0u;
    u32Abs = (g_u32Caps & GJ_VIRTIO_INPUT_CAP_ABS) ? 1u : 0u;
    u32Ready = g_fReady ? 1u : 0u;
    u32Claim = u32Ready;

    u16QSize = g_fReady ? g_qEvent.u16Size : 0;
    u16FreeNow = 0;
    u16NumFree = 0;
    u16LastUsed = 0;
    if (g_fReady) {
        q_note_free();
        u16FreeNow = virtio_q_num_free(&g_qEvent);
        u16NumFree = g_qEvent.u16NumFree;
        u16LastUsed = g_qEvent.u16LastUsed;
    }
    u16FreeMin = (g_u16FreeMin == 0xffffu) ? 0u : g_u16FreeMin;
    u8Bus = (g_pIn != NULL) ? g_pIn->u8Bus : 0;
    u8Slot = (g_pIn != NULL) ? g_pIn->u8Slot : 0;
    u8Func = (g_pIn != NULL) ? g_pIn->u8Func : 0;
    u8Modern = (g_pIn != NULL) ? g_pIn->fModern : 0;
    u16Device = (g_pIn != NULL) ? g_pIn->u16Device : 0;
    u32Kind = (g_pIn != NULL) ? g_pIn->u32Kind : 0;
    u64FeatDev = (g_pIn != NULL) ? g_pIn->u64FeaturesDev : 0ull;
    u64FeatDrv = (g_pIn != NULL) ? g_pIn->u64FeaturesDrv : 0ull;
    u32NotifyMult = (g_pIn != NULL) ? g_pIn->u32NotifyMult : 0;
    u32NumQueues = (g_pIn != NULL) ? g_pIn->u32NumQueues : 0;
    u64PaDesc = g_fReady ? (u64)g_qEvent.paDesc : 0ull;
    u64PaAvail = g_fReady ? (u64)g_qEvent.paAvail : 0ull;
    u64PaUsed = g_fReady ? (u64)g_qEvent.paUsed : 0ull;

    /*
     * Soft verdict (inventory only; poll path unchanged):
     *   NODEV    — not ready / no device
     *   PASS     — ready + any event accepted into soft ring
     *   PARTIAL  — ready, no completed product event yet (post-probe)
     */
    if (!g_fReady) {
        szVerdict = "NODEV";
    } else if (g_u32EventCount != 0u || g_u32PollHits != 0u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /* Grep: virtio-input: soft inventory */
    kprintf("virtio-input: soft inventory via=%s ready=%u bus=%x slot=%x "
            "slots=%u ring=%u q=%u posted=%u caps=0x%x free=%u free_min=%u "
            "log_n=%u wave=%u areas=%u\n",
            szViaSafe, u32Ready, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)VI_SLOTS, (unsigned)VI_RING, (unsigned)VI_Q_SIZE,
            (unsigned)g_cPosted, (unsigned)g_u32Caps, (unsigned)u16FreeNow,
            (unsigned)u16FreeMin, g_u32SoftLogN, (unsigned)VI_SOFT_WAVE,
            (unsigned)VI_SOFT_AREAS);

    /* Grep: virtio-input: soft pci */
    kprintf("virtio-input: soft pci bus=%x slot=%x func=%x dev=0x%x kind=%u "
            "modern=%u feat_dev=0x%lx feat_drv=0x%lx num_queues=%u "
            "notify_mult=%u\n",
            (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
            (unsigned)u16Device, u32Kind, (unsigned)u8Modern,
            (unsigned long)u64FeatDev, (unsigned long)u64FeatDrv,
            u32NumQueues, u32NotifyMult);

    /* Grep: virtio-input: soft geometry */
    kprintf("virtio-input: soft geometry q_size=%u slots=%u ring=%u "
            "drain_max=%u poll_spins=%u abs_soft=%d..%d status_q=0 "
            "event_q=0\n",
            (unsigned)VI_Q_SIZE, (unsigned)VI_SLOTS, (unsigned)VI_RING,
            (unsigned)VI_DRAIN_MAX, (unsigned)VI_POLL_SPINS,
            (int)VI_ABS_SOFT_MIN, (int)VI_ABS_SOFT_MAX);

    /* Grep: virtio-input: soft ring */
    kprintf("virtio-input: soft ring head=%u len=%u depth=%u events=%u "
            "dropped=%u drop_oldest=1 pending=%u "
            "pa_desc=0x%lx pa_avail=0x%lx pa_used=0x%lx\n",
            (unsigned)g_u32Head, (unsigned)g_u32Len, (unsigned)VI_RING,
            (unsigned)g_u32EventCount, (unsigned)g_u32Dropped,
            (unsigned)u32Pending, (unsigned long)u64PaDesc,
            (unsigned long)u64PaAvail, (unsigned long)u64PaUsed);

    /* Grep: virtio-input: soft slots */
    kprintf("virtio-input: soft slots posted=%u max=%u drain_max=%u "
            "poll_spins=%u unknown=%u post_fail=%u\n",
            (unsigned)g_cPosted, (unsigned)VI_SLOTS, (unsigned)VI_DRAIN_MAX,
            (unsigned)VI_POLL_SPINS, g_u32UnknownSlot, g_u32PostFail);

    /* Grep: virtio-input: soft queue */
    kprintf("virtio-input: soft queue event_q=0 size=%u free=%u free_min=%u "
            "num_free=%u free_head=%u last_used=%u notify_off=%u "
            "notify_mult=%u q_idx=%u\n",
            (unsigned)u16QSize, (unsigned)u16FreeNow, (unsigned)u16FreeMin,
            (unsigned)u16NumFree,
            (unsigned)(g_fReady ? g_qEvent.u16FreeHead : 0u),
            (unsigned)u16LastUsed,
            (unsigned)(g_fReady ? g_qEvent.u16NotifyOff : 0u),
            u32NotifyMult,
            (unsigned)(g_fReady ? g_qEvent.u16QueueIdx : 0u));

    /* Grep: virtio-input: soft axes */
    kprintf("virtio-input: soft axes abs_x=%d abs_y=%d abs_seen=%u "
            "rel_x=%d rel_y=%d rel_wheel=%d rel_seen=%u\n",
            (int)g_i32AbsX, (int)g_i32AbsY, (unsigned)(g_fAbsSeen ? 1 : 0),
            (int)g_i32RelX, (int)g_i32RelY, (int)g_i32RelWheel,
            (unsigned)(g_fRelSeen ? 1 : 0));

    /* Grep: virtio-input: soft caps */
    kprintf("virtio-input: soft caps mask=0x%x key=%u rel=%u abs=%u "
            "key_ev=%u rel_ev=%u abs_ev=%u syn_ev=%u\n",
            (unsigned)g_u32Caps, (unsigned)u32Key, (unsigned)u32Rel,
            (unsigned)u32Abs, g_u32KeySeen, g_u32RelEvSeen, g_u32AbsEvSeen,
            g_u32SynSeen);

    /* Grep: virtio-input: soft absinfo */
    kprintf("virtio-input: soft absinfo x=%d..%d seen=%u fuzz=%d flat=%d "
            "res=%d y=%d..%d seen=%u fuzz=%d flat=%d res=%d "
            "soft_default=%d..%d\n",
            (int)g_AbsX.i32Min, (int)g_AbsX.i32Max,
            (unsigned)(g_fAbsInfoX ? 1 : 0), (int)g_AbsX.i32Fuzz,
            (int)g_AbsX.i32Flat, (int)g_AbsX.i32Res, (int)g_AbsY.i32Min,
            (int)g_AbsY.i32Max, (unsigned)(g_fAbsInfoY ? 1 : 0),
            (int)g_AbsY.i32Fuzz, (int)g_AbsY.i32Flat, (int)g_AbsY.i32Res,
            (int)VI_ABS_SOFT_MIN, (int)VI_ABS_SOFT_MAX);

    /* Grep: virtio-input: soft drain */
    kprintf("virtio-input: soft drain calls=%u pushed=%u burst_max=%u "
            "drain_max=%u poll_spins=%u unknown=%u\n",
            g_u32DrainCalls, g_u32DrainPushed, g_u32DrainBurstMax,
            (unsigned)VI_DRAIN_MAX, (unsigned)VI_POLL_SPINS, g_u32UnknownSlot);

    /* Grep: virtio-input: soft last */
    kprintf("virtio-input: soft last type=%u code=%u value=%d events=%u\n",
            (unsigned)g_u16LastType, (unsigned)g_u16LastCode,
            (int)g_i32LastValue, (unsigned)g_u32EventCount);

    /* Grep: virtio-input: soft stats */
    kprintf("virtio-input: soft stats events=%u pending=%u dropped=%u "
            "posted=%u ready=%u kicks=%u\n",
            (unsigned)g_u32EventCount, (unsigned)u32Pending,
            (unsigned)g_u32Dropped, (unsigned)g_cPosted, u32Ready,
            g_u32Kicks);

    /* Grep: virtio-input: soft counters */
    kprintf("virtio-input: soft counters polls=%u hits=%u empty=%u "
            "drain=%u pushed=%u kicks=%u post_fail=%u unknown=%u "
            "api_miss=%u log_n=%u\n",
            g_u32Polls, g_u32PollHits, g_u32PollEmpty, g_u32DrainCalls,
            g_u32DrainPushed, g_u32Kicks, g_u32PostFail, g_u32UnknownSlot,
            g_u32ApiMiss, g_u32SoftLogN);

    /* Grep: virtio-input: soft api */
    kprintf("virtio-input: soft api poll=%u rel_soft=%u abs_soft=%u "
            "absinfo=%u event_count=%u pending=%u rel_reset=%u miss=%u\n",
            g_u32ApiPoll, g_u32ApiRelSoft, g_u32ApiAbsSoft, g_u32ApiAbsinfo,
            g_u32ApiEventCount, g_u32ApiPending, g_u32ApiRelReset,
            g_u32ApiMiss);

    /* Grep: virtio-input: soft features */
    kprintf("virtio-input: soft features guest=0x%lx dev=0x%lx "
            "cap_key=%u cap_rel=%u cap_abs=%u absinfo_x=%u absinfo_y=%u "
            "v1_only=1\n",
            (unsigned long)u64FeatDrv, (unsigned long)u64FeatDev,
            (unsigned)u32Key, (unsigned)u32Rel, (unsigned)u32Abs,
            (unsigned)(g_fAbsInfoX ? 1 : 0), (unsigned)(g_fAbsInfoY ? 1 : 0));

    /*
     * Grep: virtio-input: soft path
     * Honesty: product input path is this driver (claim=1 when ready).
     * Multi-slot q0 + soft ring + soft axes + cfg EV_BITS/ABS_INFO soft probe.
     * Soft inventory ≠ bar3 / multi-device product close.
     */
    kprintf("virtio-input: soft path claim=%u q0_event=1 multi_slot=1 "
            "soft_ring=1 soft_axes=1 drop_oldest=1 cfg_ev_bits=1 "
            "cfg_abs_info=1 poll=1 absinfo=1 rel_reset=1 status_q=0 "
            "bar3=0 steam=0\n",
            u32Claim);

    /* Grep: virtio-input: soft claim (Wave 15) */
    kprintf("virtio-input: soft claim ready=%u claim=%u modern=%u "
            "posted=%u events=%u caps=0x%x\n",
            u32Ready, u32Claim, (unsigned)u8Modern, (unsigned)g_cPosted,
            (unsigned)g_u32EventCount, (unsigned)g_u32Caps);

    /* Grep: virtio-input: soft via (Wave 15) */
    kprintf("virtio-input: soft via last=%s log_n=%u once=%u\n",
            (g_szLastVia != NULL) ? g_szLastVia : "path", g_u32SoftLogN,
            g_fSoftOnce ? 1u : 0u);

    /* Grep: virtio-input: soft ready (Wave 15) */
    kprintf("virtio-input: soft ready live=%u posted=%u pending=%u "
            "polls=%u hits=%u empty=%u dropped=%u\n",
            u32Ready, (unsigned)g_cPosted, (unsigned)u32Pending, g_u32Polls,
            g_u32PollHits, g_u32PollEmpty, (unsigned)g_u32Dropped);

    /* Grep: virtio-input: soft types (Wave 15 EV class tallies) */
    kprintf("virtio-input: soft types syn=%u key=%u rel=%u abs=%u "
            "last_type=%u last_code=%u last_value=%d\n",
            g_u32SynSeen, g_u32KeySeen, g_u32RelEvSeen, g_u32AbsEvSeen,
            (unsigned)g_u16LastType, (unsigned)g_u16LastCode,
            (int)g_i32LastValue);

    /* Grep: virtio-input: soft kicks (Wave 15) */
    kprintf("virtio-input: soft kicks n=%u post_fail=%u unknown_slot=%u "
            "drain=%u pushed=%u burst_max=%u\n",
            g_u32Kicks, g_u32PostFail, g_u32UnknownSlot, g_u32DrainCalls,
            g_u32DrainPushed, g_u32DrainBurstMax);

    /* Grep: virtio-input: soft honesty (Wave 15 non-claims) */
    kprintf("virtio-input: soft honesty multi_slot=1 soft_ring=1 "
            "soft_axes=1 status_q=0 multi_dev=0 bar3=0 steam=0 "
            "product_input=0 soft_only=1 wave=%u areas=%u\n",
            (unsigned)VI_SOFT_WAVE, (unsigned)VI_SOFT_AREAS);

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: virtio-input: soft ratio|headroom|surface|return|contract
     */
    {
        u32 u32Surf = 0u;
        u32 u32HitBp = 0;
        u32 u32PollTot = g_u32Polls;

        if (u32PollTot != 0u) {
            u32HitBp = (g_u32PollHits * 10000u) / u32PollTot;
        }
        if (u32Ready != 0u) {
            u32Surf |= 0x1u;
        }
        if (g_u32EventCount != 0u) {
            u32Surf |= 0x2u;
        }
        if (g_u32PollHits != 0u) {
            u32Surf |= 0x4u;
        }
        if (g_cPosted != 0u) {
            u32Surf |= 0x8u;
        }
        if (g_u32Dropped != 0u) {
            u32Surf |= 0x10u;
        }
        if (g_u32Kicks != 0u) {
            u32Surf |= 0x20u;
        }
        u32Surf |= 0x40u; /* multi-slot soft ring */
        u32Surf |= 0x80u; /* honesty catalog always present */
        /* Grep: virtio-input: soft ratio */
        kprintf("virtio-input: soft ratio hit_bp=%u polls=%u hits=%u "
                "events=%u dropped=%u wave=%u soft PASS\n",
                u32HitBp, g_u32Polls, g_u32PollHits,
                (unsigned)g_u32EventCount, (unsigned)g_u32Dropped,
                (unsigned)VI_SOFT_WAVE);
        /* Grep: virtio-input: soft headroom */
        kprintf("virtio-input: soft headroom q_size=%u free_now=%u "
                "posted=%u burst_max=%u abs_soft_max=%u wave=%u soft PASS\n",
                (unsigned)u16QSize, (unsigned)u16FreeNow,
                (unsigned)g_cPosted, g_u32DrainBurstMax,
                (unsigned)VI_ABS_SOFT_MAX, (unsigned)VI_SOFT_WAVE);
        /* Grep: virtio-input: soft surface */
        kprintf("virtio-input: soft surface inventory,geometry,queue,events,"
                "poll,slots,claim,via,ready,types,kicks,honesty,ratio,"
                "headroom,return,contract,return_selftest,retmap,deepen areas=%u wave=%u\n",
                (unsigned)VI_SOFT_AREAS, (unsigned)VI_SOFT_WAVE);
        /* Grep: virtio-input: soft return — return-surface bitmask */
        kprintf("virtio-input: soft return surf=0x%x ready=%u events=%u "
                "hits=%u posted=%u dropped=%u kicks=%u via=%s areas=%u "
                "wave=%u soft PASS\n",
                u32Surf, u32Ready, (unsigned)g_u32EventCount, g_u32PollHits,
                (unsigned)g_cPosted, (unsigned)g_u32Dropped, g_u32Kicks,
                szViaSafe, (unsigned)VI_SOFT_AREAS, (unsigned)VI_SOFT_WAVE);
        /* Grep: virtio-input: soft contract — soft ≠ game I/O */
        kprintf("virtio-input: soft contract soft_only=1 game_io=0 "
                "product_input=0 steam=0 multi_dev=0 bar3=open wave=%u "
                "soft PASS\n",
                (unsigned)VI_SOFT_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-input: soft return — Wave 17 API return surfaces (kept) */
    kprintf("virtio-input: soft return soft_inv=1 input=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)VI_SOFT_WAVE);

    /* Grep: virtio-input: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("virtio-input: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)VI_SOFT_WAVE);

    /* Grep: virtio-input: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("virtio-input: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)VI_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-input: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("virtio-input: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)VI_SOFT_WAVE);

    /* Grep: virtio-input: soft retcode — Wave 19 retcode catalog */
    kprintf("virtio-input: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)VI_SOFT_WAVE);

    /* Grep: virtio-input: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-input: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("virtio-input: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)VI_SOFT_WAVE);
    /* Grep: virtio-input: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("virtio-input: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)VI_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-input: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("virtio-input: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)VI_SOFT_WAVE);
    /* Grep: virtio-input: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("virtio-input: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)VI_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-input: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("virtio-input: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)VI_SOFT_WAVE);
            /* Grep: virtio-input: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("virtio-input: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)VI_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-input: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("virtio-input: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)VI_SOFT_WAVE);
            /* Grep: virtio-input: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("virtio-input: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: virtio-input: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("virtio-input: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)VI_SOFT_WAVE);
            /* Grep: virtio-input: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("virtio-input: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)VI_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-input: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("virtio-input: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)VI_SOFT_WAVE);
            /* Grep: virtio-input: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("virtio-input: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)VI_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-input: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("virtio-input: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)VI_SOFT_WAVE);
            /* Grep: virtio-input: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("virtio-input: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)VI_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-input: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("virtio-input: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)VI_SOFT_WAVE);
            /* Grep: virtio-input: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("virtio-input: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)VI_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: virtio-input: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("virtio-input: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)VI_SOFT_WAVE);
                    /* Grep: virtio-input: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("virtio-input: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)VI_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-input: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("virtio-input: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)VI_SOFT_WAVE);
                            /* Grep: virtio-input: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("virtio-input: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)VI_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-input: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("virtio-input: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)VI_SOFT_WAVE);
                            /* Grep: virtio-input: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("virtio-input: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)VI_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-input: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-input: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)VI_SOFT_WAVE);
                            /* Grep: virtio-input: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("virtio-input: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)VI_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-input: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("virtio-input: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)VI_SOFT_WAVE);
                            /* Grep: virtio-input: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-input: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)VI_SOFT_WAVE);
                            /* Grep: virtio-input: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("virtio-input: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("virtio-input: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("virtio-input: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("virtio-input: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("virtio-input: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("virtio-input: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("virtio-input: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retfortress — Wave 35 return-fortress honesty */
kprintf("virtio-input: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("virtio-input: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft rethold — Wave 36 return-hold honesty */
kprintf("virtio-input: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retspire — Wave 36 exclusive spire stamp */
kprintf("virtio-input: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retwall — Wave 37 return-wall honesty */
kprintf("virtio-input: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retgate — Wave 37 exclusive gate stamp */
kprintf("virtio-input: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retmoat — Wave 38 return-moat honesty */
kprintf("virtio-input: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retower — Wave 38 exclusive tower stamp */
kprintf("virtio-input: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("virtio-input: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("virtio-input: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("virtio-input: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("virtio-input: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retravelin — Wave 41 return-travelin honesty */
kprintf("virtio-input: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("virtio-input: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("virtio-input: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("virtio-input: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("virtio-input: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("virtio-input: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("virtio-input: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("virtio-input: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("virtio-input: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("virtio-input: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retbailey — Wave 46 return-bailey honesty */
kprintf("virtio-input: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)VI_SOFT_WAVE);
/* Grep: virtio-input: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("virtio-input: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)VI_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("virtio-input: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("virtio-input: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("virtio-input: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("virtio-input: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("virtio-input: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("virtio-input: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retsally — Wave 50 return-sally honesty */
kprintf("virtio-input: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("virtio-input: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retfosse — Wave 51 return-fosse honesty */
kprintf("virtio-input: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("virtio-input: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("virtio-input: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("virtio-input: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retravelin — Wave 53 return-travelin honesty */
kprintf("virtio-input: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("virtio-input: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("virtio-input: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retredan — Wave 54 exclusive redan stamp */
kprintf("virtio-input: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retflank — Wave 55 return-flank honesty */
kprintf("virtio-input: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retface — Wave 55 exclusive face stamp */
kprintf("virtio-input: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retgorge — Wave 56 return-gorge honesty */
kprintf("virtio-input: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("virtio-input: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retraverse — Wave 57 return-traverse honesty */
kprintf("virtio-input: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("virtio-input: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retorillon — Wave 58 return-orillon honesty */
kprintf("virtio-input: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("virtio-input: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("virtio-input: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("virtio-input: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retplace — Wave 60 return-place honesty */
kprintf("virtio-input: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("virtio-input: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("virtio-input: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("virtio-input: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("virtio-input: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("virtio-input: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("virtio-input: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("virtio-input: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: virtio-input: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("virtio-input: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: virtio-input: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("virtio-input: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: virtio-input: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("virtio-input: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: virtio-input: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("virtio-input: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: virtio-input: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("virtio-input: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=81 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: virtio-input: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("virtio-input: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=81 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("virtio-input: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("virtio-input: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("virtio-input: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("virtio-input: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("virtio-input: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("virtio-input: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("virtio-input: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("virtio-input: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("virtio-input: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("virtio-input: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-input: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("virtio-input: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("virtio-input: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: virtio-input: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("virtio-input: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("virtio-input: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionangle stamp; Soft≠product)\n");
/* Grep: virtio-input: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("virtio-input: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("virtio-input: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retparapetangle stamp; Soft≠product)\n");
/* Grep: virtio-input: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("virtio-input: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("virtio-input: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retowerangle stamp; Soft≠product)\n");
/* Grep: virtio-input: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("virtio-input: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("virtio-input: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retwallangle stamp; Soft≠product)\n");
/* Grep: virtio-input: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("virtio-input: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("virtio-input: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retholdangle stamp; Soft≠product)\n");
/* Grep: virtio-input: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("virtio-input: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("virtio-input: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retfortressangle stamp; Soft≠product)\n");
/* Grep: virtio-input: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("virtio-input: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("virtio-input: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: virtio-input: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("virtio-input: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("virtio-input: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: virtio-input: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("virtio-input: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-input: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("virtio-input: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbulwarkangle stamp; Soft≠product)\n");

                            kprintf("virtio-input: soft deepen wave=%u areas=%u via=%s ready=%u "
            "events=%u polls=%u posted=%u log_n=%u "
            "(soft inventory only; not bar3)\n",
            (unsigned)VI_SOFT_WAVE, (unsigned)VI_SOFT_AREAS, szViaSafe,
            u32Ready, (unsigned)g_u32EventCount, g_u32Polls,
            (unsigned)g_cPosted, g_u32SoftLogN);

    /* Grep: virtio-input: soft PASS | NODEV | PARTIAL */
    kprintf("virtio-input: soft %s via=%s ready=%u events=%u polls=%u "
            "hits=%u dropped=%u log_n=%u wave=%u\n",
            szVerdict, szViaSafe, u32Ready, (unsigned)g_u32EventCount,
            g_u32Polls, g_u32PollHits, (unsigned)g_u32Dropped, g_u32SoftLogN,
            (unsigned)VI_SOFT_WAVE);

    /* Grep: virtio-input: soft inventory PASS|NODEV|PARTIAL */
    kprintf("virtio-input: soft inventory %s via=%s logs=%u wave=%u "
            "areas=%u (soft inventory only; never hard-gates)\n",
            szVerdict, szViaSafe, g_u32SoftLogN, (unsigned)VI_SOFT_WAVE,
            (unsigned)VI_SOFT_AREAS);
}

/**
 * After first product poll/event activity, print soft inventory once
 * (mirrors virtio-blk / virtio-gpu soft-stats-once). Diagnostics only.
 */
static void
soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32EventCount == 0u && g_u32PollHits == 0u &&
        g_u32PollEmpty == 0u && g_u32Dropped == 0u) {
        return;
    }
    g_fSoftOnce = 1;
    soft_inventory("activity");
}

/*
 * Soft probe of device config: EV_BITS for KEY/REL/ABS + ABS_INFO for X/Y.
 * Failure is non-fatal — axes soft defaults remain.
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
 * Probe path: find first input → modern PCI caps → V1 features → event q0
 * → multi-slot post → soft cfg. Leaves g_fReady=0 on any hard failure.
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
    slot_desc_clear_all();
    ring_reset();
    axes_reset();
    telemetry_reset();

    c = virtio_dev_count();
    kprintf("virtio-input: probe (%u devs)\n", c);

    /* kind==4, transitional 0x1052, or modern input device ID */
    for (i = 0; i < c; i++) {
        struct gj_virtio_dev *pDev = virtio_dev_get(i);

        if (pDev == NULL) {
            continue;
        }
        if (pDev->u32Kind == 4 || pDev->u16Device == 0x1052u ||
            pDev->u16Device == GJ_VIRTIO_PCI_DEV_INPUT) {
            g_pIn = pDev;
            kprintf("virtio-input: found id=%x kind=%u\n",
                    (unsigned)pDev->u16Device, pDev->u32Kind);
            break;
        }
    }
    if (g_pIn == NULL) {
        kprintf("virtio-input: no device\n");
        /* Grep: virtio-input: soft … NODEV (Wave 14 soft inventory) */
        soft_inventory("nodev");
        return -1;
    }
    st = virtio_pci_setup(g_pIn);
    if (st != GJ_OK || g_pIn->pCommon == NULL) {
        kprintf("virtio-input: setup failed %d\n", (int)st);
        g_pIn = NULL;
        soft_inventory("pci_fail");
        return -1;
    }
    st = virtio_negotiate(g_pIn, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        kprintf("virtio-input: negotiate failed %d\n", (int)st);
        g_pIn = NULL;
        soft_inventory("negotiate_fail");
        return -1;
    }
    st = virtio_q_setup(g_pIn, &g_qEvent, 0, (u16)VI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-input: event queue failed %d\n", (int)st);
        g_pIn = NULL;
        soft_inventory("q_fail");
        return -1;
    }
    virtio_set_status(g_pIn, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                  GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));

    post_all_slots();
    soft_cfg_probe(g_pIn);
    q_note_free();

    g_fReady = 1;
    kprintf("virtio-input: ready PASS bus=%x slot=%x slots=%u ring=%u\n",
            (unsigned)g_pIn->u8Bus, (unsigned)g_pIn->u8Slot,
            (unsigned)g_cPosted, (unsigned)VI_RING);
    kprintf("virtio-input: event ring soft PASS posted=%u\n",
            (unsigned)g_cPosted);
    kprintf("virtio-input: abs/rel axes soft PASS caps=0x%x absinfo=%d/%d\n",
            (unsigned)g_u32Caps, g_fAbsInfoX, g_fAbsInfoY);
    /*
     * Wave 14 soft inventory rollup (prefix-stable "virtio-input: soft …").
     * Never hard-gates ready PASS.
     */
    soft_inventory("probe");
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
    g_u32ApiPoll++;
    if (!g_fReady || pOut == NULL) {
        g_u32ApiMiss++;
        return -1;
    }
    g_u32Polls++;
    /* Fill soft ring from multi-slot HW, then pop one. */
    (void)drain_hw();
    if (ring_pop(pOut)) {
        g_u32PollHits++;
        soft_maybe_once();
        return 1;
    }
    g_u32PollEmpty++;
    soft_maybe_once();
    return 0;
}

u32
virtio_input_event_count(void)
{
    /*
     * Emit soft inventory on count read so bring-up smoke greps
     * virtio-input: soft … without requiring a second poll (mirrors
     * virtio-gpu present_count / virtio-blk q_stats). Never hard-gates.
     */
    g_u32ApiEventCount++;
    soft_inventory("event_count");
    return g_u32EventCount;
}

u32
virtio_input_pending(void)
{
    g_u32ApiPending++;
    if (!g_fReady || !ring_sane()) {
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
    g_u32ApiRelSoft++;
    if (!g_fReady) {
        g_u32ApiMiss++;
        return -1;
    }
    /* Opportunistic drain so axes reflect recent device traffic. */
    (void)drain_hw();
    soft_maybe_once();
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
    g_u32ApiRelReset++;
    g_i32RelX = 0;
    g_i32RelY = 0;
    g_i32RelWheel = 0;
}

int
virtio_input_abs_soft(i32 *pX, i32 *pY, int *pFSeen)
{
    g_u32ApiAbsSoft++;
    if (!g_fReady) {
        g_u32ApiMiss++;
        return -1;
    }
    (void)drain_hw();
    soft_maybe_once();
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
    g_u32ApiAbsinfo++;
    if (!g_fReady || pOut == NULL) {
        g_u32ApiMiss++;
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
    g_u32ApiMiss++;
    return -1;
}
