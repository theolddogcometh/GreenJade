/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-net: RX q0 + TX q1 + bounce pool for UDX ring push (OASIS).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Greppable product markers (prefix-stable):
 *   virtio-net: ready PASS
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/virtio_net.h>
#include <gj/vmm.h>

/* Feature bits we accept (public OASIS virtio-net) */
#define VIRTIO_NET_F_MAC       (1ull << 5)
#define VIRTIO_NET_F_STATUS    (1ull << 16)
#define VIRTIO_NET_F_MRG_RXBUF (1ull << 15)

/* Device config (MAC + link status); only first fields used */
struct virtio_net_config {
    u8  aMac[6];
    u16 u16Status;
} __attribute__((packed));

/* virtio 1.x hdr always includes num_buffers when VERSION_1 is negotiated */
struct virtio_net_hdr {
    u8  u8Flags;
    u8  u8GsoType;
    u16 u16HdrLen;
    u16 u16GsoSize;
    u16 u16CsumStart;
    u16 u16CsumOffset;
    u16 u16NumBuffers;
} __attribute__((packed));

static struct gj_virtio_dev *g_pNet;
static struct gj_virtq       g_qRx; /* queue 0 */
static struct gj_virtq       g_qTx; /* queue 1 */
static int                   g_fReady;
static u32                   g_u32TxCount;
static u32                   g_u32RxCount;
static u8                    g_aTxBuf[2048] __attribute__((aligned(16)));
static u8                    g_aRxBuf[2048] __attribute__((aligned(16)));
/* Bounce pool for userspace AVAIL_PUSH (ring programming path) */
#define GJ_NET_BOUNCE_N 8u
#define GJ_NET_BOUNCE_SZ 2048u
static u8                    g_aBounce[GJ_NET_BOUNCE_N][GJ_NET_BOUNCE_SZ]
    __attribute__((aligned(16)));
static u8                    g_aBounceUsed[GJ_NET_BOUNCE_N];
static u32                   g_u32AvailPushes;
static u32                   g_u32UserRingPushes;
static int                   g_fRxPosted;
static u8                    g_aMac[6];
static int                   g_fHaveMac;

/* Translate kernel buffer VA → guest physical (identity fallback). */
static gj_paddr_t
buf_phys(const void *pBuf)
{
    gj_paddr_t pa;

    if (pBuf == NULL) {
        return 0;
    }
    pa = vmm_virt_to_phys((gj_vaddr_t)(uintptr_t)pBuf);
    if (pa == 0) {
        /* Low identity map (Multiboot BSS) */
        pa = (gj_paddr_t)(uintptr_t)pBuf;
    }
    return pa;
}

static void
read_mac(struct gj_virtio_dev *pDev)
{
    volatile struct virtio_net_config *pCfg;
    u32 i;

    g_fHaveMac = 0;
    memset(g_aMac, 0, sizeof(g_aMac));
    if (pDev == NULL || pDev->pDevice == NULL) {
        return;
    }
    pCfg = (volatile struct virtio_net_config *)(void *)pDev->pDevice;
    for (i = 0; i < 6; i++) {
        g_aMac[i] = pCfg->aMac[i];
    }
    g_fHaveMac = 1;
}

/*
 * Probe path: find first net → modern PCI caps → features → RX/TX qs → post RX.
 * Leaves g_fReady=0 and g_pNet=NULL on any hard failure.
 */
int
virtio_net_probe(void)
{
    u32 i;
    u32 c;
    gj_status_t st;
    u64 u64Want;

    g_pNet = NULL;
    g_fReady = 0;
    g_fRxPosted = 0;
    g_fHaveMac = 0;
    c = virtio_dev_count();
    /* kind==1, transitional 0x1000, or modern net device ID */
    for (i = 0; i < c; i++) {
        struct gj_virtio_dev *pDev = virtio_dev_get(i);

        if (pDev == NULL) {
            continue;
        }
        if (pDev->u32Kind == 1 || pDev->u16Device == 0x1000u ||
            pDev->u16Device == GJ_VIRTIO_PCI_DEV_NET_MODERN) {
            g_pNet = pDev;
            break;
        }
    }
    if (g_pNet == NULL) {
        kprintf("virtio-net: no device\n");
        return -1;
    }
    st = virtio_pci_setup(g_pNet);
    if (st != GJ_OK || g_pNet->pCommon == NULL) {
        kprintf("virtio-net: setup failed %d\n", (int)st);
        g_pNet = NULL;
        return -1;
    }
    /* Prefer MAC+STATUS+V1; fall back to V1 only */
    u64Want = GJ_VIRTIO_F_VERSION_1 | VIRTIO_NET_F_MAC | VIRTIO_NET_F_STATUS;
    st = virtio_negotiate(g_pNet, u64Want);
    if (st != GJ_OK) {
        st = virtio_negotiate(g_pNet, GJ_VIRTIO_F_VERSION_1);
        if (st != GJ_OK) {
            kprintf("virtio-net: negotiate failed %d\n", (int)st);
            g_pNet = NULL;
            return -1;
        }
    }
    read_mac(g_pNet);
    /* RX=0, TX=1 */
    st = virtio_q_setup(g_pNet, &g_qRx, 0, 64);
    if (st != GJ_OK) {
        kprintf("virtio-net: rx queue failed %d\n", (int)st);
        g_pNet = NULL;
        return -1;
    }
    st = virtio_q_setup(g_pNet, &g_qTx, 1, 64);
    if (st != GJ_OK) {
        kprintf("virtio-net: tx queue failed %d\n", (int)st);
        g_pNet = NULL;
        return -1;
    }
    /* Post one RX buffer (device-write); TX still works if this fails */
    memset(g_aRxBuf, 0, sizeof(g_aRxBuf));
    if (virtio_q_add(&g_qRx, buf_phys(g_aRxBuf), sizeof(g_aRxBuf), 1) >= 0) {
        g_fRxPosted = 1;
        virtio_q_kick(&g_qRx);
    } else {
        kprintf("virtio-net: initial RX post failed (tx-only until retry)\n");
    }

    virtio_set_status(g_pNet, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                   GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
    if (g_fHaveMac) {
        kprintf("virtio-net: ready PASS bus=%x slot=%x "
                "mac=%x:%x:%x:%x:%x:%x\n",
                (unsigned)g_pNet->u8Bus, (unsigned)g_pNet->u8Slot,
                (unsigned)g_aMac[0], (unsigned)g_aMac[1], (unsigned)g_aMac[2],
                (unsigned)g_aMac[3], (unsigned)g_aMac[4], (unsigned)g_aMac[5]);
    } else {
        kprintf("virtio-net: ready PASS bus=%x slot=%x\n",
                (unsigned)g_pNet->u8Bus, (unsigned)g_pNet->u8Slot);
    }
    return 0;
}

int
virtio_net_ready(void)
{
    return g_fReady;
}

int
virtio_net_tx(const void *pFrame, u32 cbLen)
{
    struct virtio_net_hdr *pHdr;
    u32 cbTotal;
    gj_paddr_t pa;

    if (!g_fReady || pFrame == NULL || cbLen == 0 || cbLen > 1514) {
        return -1;
    }
    memset(g_aTxBuf, 0, sizeof(g_aTxBuf));
    pHdr = (struct virtio_net_hdr *)(void *)g_aTxBuf;
    memset(pHdr, 0, sizeof(*pHdr));
    memcpy(g_aTxBuf + sizeof(*pHdr), pFrame, cbLen);
    cbTotal = (u32)sizeof(*pHdr) + cbLen;
    pa = buf_phys(g_aTxBuf);
    if (virtio_q_add(&g_qTx, pa, cbTotal, 0) < 0) {
        return -1;
    }
    virtio_q_kick(&g_qTx);
    (void)virtio_q_poll(&g_qTx, 1000000u);
    g_u32TxCount++;
    return 0;
}

u32
virtio_net_tx_count(void)
{
    return g_u32TxCount;
}

i32
virtio_net_rx(void *pOut, u32 cbMax)
{
    i32 i32Len;
    u32 cbPayload;
    struct virtio_net_hdr *pHdr;

    if (!g_fReady || pOut == NULL) {
        return -1;
    }
    i32Len = virtio_q_poll(&g_qRx, 1000u);
    if (i32Len < 0) {
        return 0;
    }
    g_fRxPosted = 0;
    g_u32RxCount++;
    pHdr = (struct virtio_net_hdr *)(void *)g_aRxBuf;
    if ((u32)i32Len <= sizeof(*pHdr)) {
        cbPayload = 0;
    } else {
        cbPayload = (u32)i32Len - (u32)sizeof(*pHdr);
    }
    if (cbPayload > cbMax) {
        cbPayload = cbMax;
    }
    if (cbPayload > 0) {
        memcpy(pOut, g_aRxBuf + sizeof(*pHdr), cbPayload);
    }
    /* Repost RX buffer */
    memset(g_aRxBuf, 0, sizeof(g_aRxBuf));
    if (virtio_q_add(&g_qRx, buf_phys(g_aRxBuf), sizeof(g_aRxBuf), 1) >= 0) {
        g_fRxPosted = 1;
        virtio_q_kick(&g_qRx);
    }
    (void)pHdr;
    return (i32)cbPayload;
}

int
virtio_net_export_q(u16 u16Which, struct gj_virtq_export *pOut)
{
    struct gj_virtq *pQ;

    if (!g_fReady || pOut == NULL) {
        return -1;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    if (u16Which > 1 || pQ->pDesc == NULL) {
        return -1;
    }
    memset(pOut, 0, sizeof(*pOut));
    pOut->u16Which = u16Which;
    pOut->u16Size = pQ->u16Size;
    pOut->u16QueueIdx = pQ->u16QueueIdx;
    pOut->u16NotifyOff = pQ->u16NotifyOff;
    pOut->u64PaDesc = (u64)pQ->paDesc;
    pOut->u64PaAvail = (u64)pQ->paAvail;
    pOut->u64PaUsed = (u64)pQ->paUsed;
    pOut->u32NotifyMult = g_pNet != NULL ? g_pNet->u32NotifyMult : 0;
    pOut->u32Ready = 1;
    /* MAP_RING layout: one page each for desc / avail / used (see map_q_user) */
    pOut->u32OffDesc = 0;
    pOut->u32OffAvail = GJ_PAGE_SIZE;
    pOut->u32OffUsed = GJ_PAGE_SIZE * 2u;
    pOut->u16FreeHead = pQ->u16FreeHead;
    pOut->u16NumFree = pQ->u16NumFree;
    return 0;
}

int
virtio_net_kick_q(u16 u16Which)
{
    struct gj_virtq *pQ;

    if (!g_fReady) {
        return -1;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    if (u16Which > 1) {
        return -1;
    }
    virtio_q_kick(pQ);
    return 0;
}

int
virtio_net_map_q_user(u16 u16Which, u64 u64VaBase, struct gj_virtq_export *pOut)
{
    struct gj_virtq_export ex;

    if (virtio_net_export_q(u16Which, &ex) != 0) {
        return -1;
    }
    if (u64VaBase == 0 || (u64VaBase & 0xfffull) != 0) {
        return -1;
    }
    /* One page each: rings live on dedicated PMM pages (virtio_q_setup). */
    if (vmm_map_page((gj_vaddr_t)u64VaBase, (gj_paddr_t)ex.u64PaDesc,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaAvail,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + 2u * GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaUsed,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        return -1;
    }
    ex.u32OffDesc = 0;
    ex.u32OffAvail = GJ_PAGE_SIZE;
    ex.u32OffUsed = GJ_PAGE_SIZE * 2u;
    if (pOut != NULL) {
        *pOut = ex;
    }
    kprintf("virtio-net: map_q which=%u va=0x%lx size=%u\n", u16Which,
            (unsigned long)u64VaBase, ex.u16Size);
    return 0;
}

static gj_paddr_t
bounce_phys(u32 u32Slot)
{
    if (u32Slot >= GJ_NET_BOUNCE_N) {
        return 0;
    }
    return buf_phys(&g_aBounce[u32Slot][0]);
}

int
virtio_net_map_dma_user(u64 u64VaBase, struct gj_virtq_dma_export *pOut)
{
    u32 i;
    u32 nPages;
    u32 s;
    gj_paddr_t pa;

    if (!g_fReady || u64VaBase == 0 || (u64VaBase & 0xfffull) != 0) {
        return -1;
    }
    nPages = (GJ_NET_BOUNCE_SZ + GJ_PAGE_SIZE - 1) / GJ_PAGE_SIZE;
    for (s = 0; s < GJ_NET_BOUNCE_N; s++) {
        pa = bounce_phys(s) & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1);
        for (i = 0; i < nPages; i++) {
            if (vmm_map_page((gj_vaddr_t)(u64VaBase +
                                          (u64)s * nPages * GJ_PAGE_SIZE +
                                          (u64)i * GJ_PAGE_SIZE),
                             pa + (gj_paddr_t)i * GJ_PAGE_SIZE,
                             GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                 GJ_VMM_PROT_USER) != GJ_OK) {
                return -1;
            }
        }
    }
    if (pOut != NULL) {
        memset(pOut, 0, sizeof(*pOut));
        pOut->u32NSlots = GJ_NET_BOUNCE_N;
        pOut->u32SlotSz = GJ_NET_BOUNCE_SZ;
        pOut->u64VaBase = u64VaBase;
        for (s = 0; s < GJ_NET_BOUNCE_N; s++) {
            pOut->aPa[s] = (u64)bounce_phys(s);
        }
        pOut->u32Ready = 1;
    }
    kprintf("virtio-net: map_dma va=0x%lx slots=%u\n", (unsigned long)u64VaBase,
            GJ_NET_BOUNCE_N);
    return 0;
}

int
virtio_net_desc_alloc(u16 u16Which)
{
    struct gj_virtq *pQ;
    int head;

    if (!g_fReady || u16Which > 1) {
        return -1;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    (void)virtio_q_reap(pQ, 8);
    head = virtio_q_alloc_desc(pQ);
    return head;
}

int
virtio_net_user_avail(u16 u16Which, u16 u16Head, int fFlags)
{
    struct gj_virtq *pQ;
    int fKick = (fFlags & 1) != 0;
    int fKernelPush = (fFlags & 2) != 0;

    if (!g_fReady || u16Which > 1) {
        return -1;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    if (u16Head >= pQ->u16Size) {
        return -1;
    }
    /*
     * fFlags bit1: kernel writes avail (desc must already be filled).
     * Clear: userspace already wrote avail ring on MAP_RING VA.
     */
    if (fKernelPush) {
        if (virtio_q_push_head(pQ, u16Head) < 0) {
            return -1;
        }
    }
    g_u32UserRingPushes++;
    g_u32AvailPushes++;
    if (fKick) {
        virtio_q_kick(pQ);
    }
    if (u16Which == 1) {
        g_u32TxCount++;
    }
    return 0;
}

int
virtio_net_bounce_fill(u32 u32Slot, const void *pBuf, u32 cbLen, int fTxHdr)
{
    u8 *pB;
    u32 cb;

    if (!g_fReady || pBuf == NULL || u32Slot >= GJ_NET_BOUNCE_N || cbLen == 0) {
        return -1;
    }
    pB = g_aBounce[u32Slot];
    memset(pB, 0, GJ_NET_BOUNCE_SZ);
    g_aBounceUsed[u32Slot] = 1;
    if (fTxHdr) {
        struct virtio_net_hdr *pHdr = (struct virtio_net_hdr *)(void *)pB;

        memset(pHdr, 0, sizeof(*pHdr));
        if (cbLen > GJ_NET_BOUNCE_SZ - sizeof(*pHdr)) {
            return -1;
        }
        memcpy(pB + sizeof(*pHdr), pBuf, cbLen);
        cb = (u32)sizeof(*pHdr) + cbLen;
    } else {
        if (cbLen > GJ_NET_BOUNCE_SZ) {
            return -1;
        }
        memcpy(pB, pBuf, cbLen);
        cb = cbLen;
    }
    return (int)cb;
}

gj_paddr_t
virtio_net_bounce_pa(u32 u32Slot)
{
    return bounce_phys(u32Slot);
}

u32
virtio_net_rx_count(void)
{
    return g_u32RxCount;
}

static int
bounce_alloc(void)
{
    u32 i;

    for (i = 0; i < GJ_NET_BOUNCE_N; i++) {
        if (!g_aBounceUsed[i]) {
            g_aBounceUsed[i] = 1;
            return (int)i;
        }
    }
    return -1;
}

static void
bounce_free_all_if_idle(void)
{
    /* After reaps free descs, clear bounce slots when queue has free slots */
    u32 i;

    if (g_qTx.u16NumFree >= g_qTx.u16Size / 2) {
        for (i = 0; i < GJ_NET_BOUNCE_N; i++) {
            g_aBounceUsed[i] = 0;
        }
    }
}

int
virtio_net_avail_push(u16 u16Which, const void *pBuf, u32 cbLen, int fWrite,
                      int fKick)
{
    struct gj_virtq *pQ;
    int slot;
    u8 *pB;
    gj_paddr_t pa;
    int head;

    if (!g_fReady || pBuf == NULL || cbLen == 0 || cbLen > GJ_NET_BOUNCE_SZ) {
        return -1;
    }
    if (u16Which > 1) {
        return -1;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    /* Reap completed to free descs */
    (void)virtio_q_reap(pQ, 8);
    bounce_free_all_if_idle();
    slot = bounce_alloc();
    if (slot < 0) {
        (void)virtio_q_reap(pQ, 16);
        bounce_free_all_if_idle();
        slot = bounce_alloc();
        if (slot < 0) {
            return -1;
        }
    }
    pB = g_aBounce[slot];
    memset(pB, 0, GJ_NET_BOUNCE_SZ);
    if (u16Which == 1 && !fWrite) {
        /* TX: prepend virtio-net hdr */
        struct virtio_net_hdr *pHdr = (struct virtio_net_hdr *)(void *)pB;

        memset(pHdr, 0, sizeof(*pHdr));
        if (cbLen > GJ_NET_BOUNCE_SZ - sizeof(*pHdr)) {
            g_aBounceUsed[slot] = 0;
            return -1;
        }
        memcpy(pB + sizeof(*pHdr), pBuf, cbLen);
        cbLen = (u32)sizeof(*pHdr) + cbLen;
    } else {
        memcpy(pB, pBuf, cbLen);
    }
    pa = bounce_phys((u32)slot);
    head = virtio_q_add(pQ, pa, cbLen, fWrite);
    if (head < 0) {
        g_aBounceUsed[slot] = 0;
        return -1;
    }
    g_u32AvailPushes++;
    if (fKick) {
        virtio_q_kick(pQ);
    }
    if (u16Which == 1) {
        g_u32TxCount++;
    }
    return 0;
}

u32
virtio_net_used_reap(u16 u16Which, u32 u32Max)
{
    struct gj_virtq *pQ;
    u32 n;

    if (!g_fReady || u16Which > 1) {
        return 0;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    n = virtio_q_reap(pQ, u32Max ? u32Max : 8u);
    bounce_free_all_if_idle();
    return n;
}

u16
virtio_net_q_free(u16 u16Which)
{
    struct gj_virtq *pQ;

    if (!g_fReady || u16Which > 1) {
        return 0;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    return virtio_q_num_free(pQ);
}

u32
virtio_net_avail_pushes(void)
{
    return g_u32AvailPushes;
}

u32
virtio_net_user_ring_pushes(void)
{
    return g_u32UserRingPushes;
}
