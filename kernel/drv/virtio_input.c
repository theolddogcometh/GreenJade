/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-input: event queue 0 (single outstanding buffer, OASIS).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Greppable product markers (prefix-stable):
 *   virtio-input: ready PASS
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/virtio_input.h>

/* Public OASIS virtio-input event layout */
struct virtio_input_event {
    u16 u16Type;
    u16 u16Code;
    i32 i32Value;
} __attribute__((packed));

static struct gj_virtio_dev *g_pIn;
static struct gj_virtq       g_qEvent;
static int                   g_fReady;
static int                   g_fPosted;
static u32                   g_u32EventCount;
static struct virtio_input_event g_Ev __attribute__((aligned(16)));

static void
post_rx(void)
{
    if (g_fPosted) {
        return;
    }
    memset(&g_Ev, 0, sizeof(g_Ev));
    if (virtio_q_add(&g_qEvent, (gj_paddr_t)(gj_vaddr_t)&g_Ev, sizeof(g_Ev),
                     1) >= 0) {
        g_fPosted = 1;
        virtio_q_kick(&g_qEvent);
    }
}

/*
 * Probe path: find first input → modern PCI caps → V1 features → event q0.
 * Leaves g_fReady=0 and g_pIn=NULL on any hard failure.
 */
int
virtio_input_probe(void)
{
    u32 i;
    u32 c;
    gj_status_t st;

    g_pIn = NULL;
    g_fReady = 0;
    g_fPosted = 0;
    g_u32EventCount = 0;
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
        return -1;
    }
    st = virtio_pci_setup(g_pIn);
    if (st != GJ_OK || g_pIn->pCommon == NULL) {
        kprintf("virtio-input: setup failed %d\n", (int)st);
        g_pIn = NULL;
        return -1;
    }
    st = virtio_negotiate(g_pIn, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        kprintf("virtio-input: negotiate failed %d\n", (int)st);
        g_pIn = NULL;
        return -1;
    }
    st = virtio_q_setup(g_pIn, &g_qEvent, 0, 64);
    if (st != GJ_OK) {
        kprintf("virtio-input: event queue failed %d\n", (int)st);
        g_pIn = NULL;
        return -1;
    }
    virtio_set_status(g_pIn, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                  GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    post_rx();
    g_fReady = 1;
    kprintf("virtio-input: ready PASS bus=%x slot=%x\n",
            (unsigned)g_pIn->u8Bus, (unsigned)g_pIn->u8Slot);
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
    i32 i32Len;

    if (!g_fReady || pOut == NULL) {
        return -1;
    }
    i32Len = virtio_q_poll(&g_qEvent, 100u);
    if (i32Len < 0) {
        return 0;
    }
    g_fPosted = 0;
    pOut->u16Type = g_Ev.u16Type;
    pOut->u16Code = g_Ev.u16Code;
    pOut->i32Value = g_Ev.i32Value;
    g_u32EventCount++;
    post_rx();
    return 1;
}

u32
virtio_input_event_count(void)
{
    return g_u32EventCount;
}
