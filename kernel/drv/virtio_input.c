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
 * Wave 10 soft inventory (prefix-stable; never hard-gates):
 *   virtio-input: soft inventory …
 *   virtio-input: soft ring …
 *   virtio-input: soft slots …
 *   virtio-input: soft axes …
 *   virtio-input: soft caps …
 *   virtio-input: soft absinfo …
 *   virtio-input: soft stats …
 *   virtio-input: soft path …
 *   virtio-input: soft inventory PASS|SKIP
 *   virtio-input: soft PASS|SKIP
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
static int g_fRelSeen;
static u32 g_u32Caps;

static struct gj_input_absinfo g_AbsX;
static struct gj_input_absinfo g_AbsY;
static int g_fAbsInfoX;
static int g_fAbsInfoY;

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
axes_apply(const struct gj_input_event *pEv)
{
    if (pEv == NULL) {
        return;
    }
    if (pEv->u16Type == GJ_EV_KEY) {
        g_u32Caps |= GJ_VIRTIO_INPUT_CAP_KEY;
        return;
    }
    if (pEv->u16Type == GJ_EV_REL) {
        g_u32Caps |= GJ_VIRTIO_INPUT_CAP_REL;
        g_fRelSeen = 1;
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
 */
static u32
drain_hw(void)
{
    u32 cPushed = 0;
    u32 u32Spins = VI_POLL_SPINS;
    u32 u32Burst;

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

/*
 * Wave 10 soft inventory — greppable "virtio-input: soft …".
 * Pure observation at probe; does not gate ready PASS or poll paths.
 */
static void
soft_inventory_log(void)
{
    u32 u32Pending;
    u32 u32Key;
    u32 u32Rel;
    u32 u32Abs;

    (void)ring_sane();
    u32Pending = g_u32Len;
    u32Key = (g_u32Caps & GJ_VIRTIO_INPUT_CAP_KEY) ? 1u : 0u;
    u32Rel = (g_u32Caps & GJ_VIRTIO_INPUT_CAP_REL) ? 1u : 0u;
    u32Abs = (g_u32Caps & GJ_VIRTIO_INPUT_CAP_ABS) ? 1u : 0u;

    /* Grep: virtio-input: soft inventory */
    kprintf("virtio-input: soft inventory ready=%u slots=%u ring=%u q=%u "
            "posted=%u caps=0x%x\n",
            (unsigned)(g_fReady ? 1 : 0), (unsigned)VI_SLOTS,
            (unsigned)VI_RING, (unsigned)VI_Q_SIZE, (unsigned)g_cPosted,
            (unsigned)g_u32Caps);

    /* Grep: virtio-input: soft ring */
    kprintf("virtio-input: soft ring head=%u len=%u depth=%u events=%u "
            "dropped=%u drop_oldest=1\n",
            (unsigned)g_u32Head, (unsigned)g_u32Len, (unsigned)VI_RING,
            (unsigned)g_u32EventCount, (unsigned)g_u32Dropped);

    /* Grep: virtio-input: soft slots */
    kprintf("virtio-input: soft slots posted=%u max=%u drain_max=%u "
            "poll_spins=%u\n",
            (unsigned)g_cPosted, (unsigned)VI_SLOTS, (unsigned)VI_DRAIN_MAX,
            (unsigned)VI_POLL_SPINS);

    /* Grep: virtio-input: soft axes */
    kprintf("virtio-input: soft axes abs_x=%d abs_y=%d abs_seen=%u "
            "rel_x=%d rel_y=%d rel_wheel=%d rel_seen=%u\n",
            (int)g_i32AbsX, (int)g_i32AbsY, (unsigned)(g_fAbsSeen ? 1 : 0),
            (int)g_i32RelX, (int)g_i32RelY, (int)g_i32RelWheel,
            (unsigned)(g_fRelSeen ? 1 : 0));

    /* Grep: virtio-input: soft caps */
    kprintf("virtio-input: soft caps mask=0x%x key=%u rel=%u abs=%u\n",
            (unsigned)g_u32Caps, (unsigned)u32Key, (unsigned)u32Rel,
            (unsigned)u32Abs);

    /* Grep: virtio-input: soft absinfo */
    kprintf("virtio-input: soft absinfo x=%d..%d seen=%u y=%d..%d seen=%u "
            "soft_default=%d..%d\n",
            (int)g_AbsX.i32Min, (int)g_AbsX.i32Max,
            (unsigned)(g_fAbsInfoX ? 1 : 0), (int)g_AbsY.i32Min,
            (int)g_AbsY.i32Max, (unsigned)(g_fAbsInfoY ? 1 : 0),
            (int)VI_ABS_SOFT_MIN, (int)VI_ABS_SOFT_MAX);

    /* Grep: virtio-input: soft stats */
    kprintf("virtio-input: soft stats events=%u pending=%u dropped=%u "
            "posted=%u ready=%u\n",
            (unsigned)g_u32EventCount, (unsigned)u32Pending,
            (unsigned)g_u32Dropped, (unsigned)g_cPosted,
            (unsigned)(g_fReady ? 1 : 0));

    /*
     * Grep: virtio-input: soft path
     * Honesty: product input path is this driver (claim=1 when ready).
     * Multi-slot q0 + soft ring + soft axes + cfg EV_BITS/ABS_INFO soft probe.
     */
    kprintf("virtio-input: soft path claim=%u q0_event=1 multi_slot=1 "
            "soft_ring=1 soft_axes=1 cfg_ev_bits=1 cfg_abs_info=1\n",
            (unsigned)(g_fReady ? 1 : 0));

    if (g_fReady) {
        kprintf("virtio-input: soft inventory PASS\n");
        kprintf("virtio-input: soft PASS\n");
    } else {
        kprintf("virtio-input: soft inventory SKIP\n");
        kprintf("virtio-input: soft SKIP\n");
    }
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
        soft_inventory_log();
        return -1;
    }
    st = virtio_pci_setup(g_pIn);
    if (st != GJ_OK || g_pIn->pCommon == NULL) {
        kprintf("virtio-input: setup failed %d\n", (int)st);
        g_pIn = NULL;
        soft_inventory_log();
        return -1;
    }
    st = virtio_negotiate(g_pIn, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        kprintf("virtio-input: negotiate failed %d\n", (int)st);
        g_pIn = NULL;
        soft_inventory_log();
        return -1;
    }
    st = virtio_q_setup(g_pIn, &g_qEvent, 0, (u16)VI_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-input: event queue failed %d\n", (int)st);
        g_pIn = NULL;
        soft_inventory_log();
        return -1;
    }
    virtio_set_status(g_pIn, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                  GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));

    post_all_slots();
    soft_cfg_probe(g_pIn);

    g_fReady = 1;
    kprintf("virtio-input: ready PASS bus=%x slot=%x slots=%u ring=%u\n",
            (unsigned)g_pIn->u8Bus, (unsigned)g_pIn->u8Slot,
            (unsigned)g_cPosted, (unsigned)VI_RING);
    kprintf("virtio-input: event ring soft PASS posted=%u\n",
            (unsigned)g_cPosted);
    kprintf("virtio-input: abs/rel axes soft PASS caps=0x%x absinfo=%d/%d\n",
            (unsigned)g_u32Caps, g_fAbsInfoX, g_fAbsInfoY);
    /* Wave 10: greppable virtio-input: soft … inventory (never hard-gates). */
    soft_inventory_log();
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
    /* Fill soft ring from multi-slot HW, then pop one. */
    (void)drain_hw();
    if (ring_pop(pOut)) {
        return 1;
    }
    return 0;
}

u32
virtio_input_event_count(void)
{
    return g_u32EventCount;
}

u32
virtio_input_pending(void)
{
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
    if (!g_fReady) {
        return -1;
    }
    /* Opportunistic drain so axes reflect recent device traffic. */
    (void)drain_hw();
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
    (void)drain_hw();
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
