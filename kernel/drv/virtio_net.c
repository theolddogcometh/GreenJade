/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-net: multi-buffer RX pool + 2-desc TX soft + bounce pool
 * for UDX ring push (OASIS). Features logging + soft stats counters.
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Greppable product markers (prefix-stable):
 *   virtio-net: ready PASS
 *   virtio-net: features
 *   virtio-net: multi-buf
 *
 * Soft inventory (Wave 12 exclusive deepen; this unit only; never hard-gates):
 * greppable: "virtio-net: soft …"
 *   virtio-net: soft inventory …
 *   virtio-net: soft multi-buf …
 *   virtio-net: soft tx-chain …
 *   virtio-net: soft bounce …
 *   virtio-net: soft queue …
 *   virtio-net: soft features …
 *   virtio-net: soft stats …
 *   virtio-net: soft mac …
 *   virtio-net: soft path …
 *   virtio-net: soft ring …
 *   virtio-net: soft PASS|NODEV|PARTIAL
 *   virtio-net: soft inventory PASS|NODEV|PARTIAL
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

/* Local common-cfg offsets (read negotiated features; match virtio_pci) */
#define NET_PCI_COMMON_DFSELECT 0
#define NET_PCI_COMMON_DF       4
#define NET_PCI_COMMON_GFSELECT 8
#define NET_PCI_COMMON_GF       12

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
static struct gj_virtio_net_stats g_Stats;

/* Soft multi-buffer RX: N slots, one desc each, all posted at probe */
static u8  g_aRxSlot[GJ_VIRTIO_NET_RX_N][GJ_VIRTIO_NET_RX_SZ]
    __attribute__((aligned(16)));
static u8  g_aRxSlotLive[GJ_VIRTIO_NET_RX_N]; /* 1 = on device */
/* desc head → RX slot (0xff free); set at post, consumed on poll_id */
static u8  g_aRxHeadSlot[GJ_VIRTQ_MAX_SIZE];

/* Kernel TX: 2-desc soft chain (hdr | payload) + single-desc fallback pack */
static struct virtio_net_hdr g_TxHdr __attribute__((aligned(16)));
static u8                    g_aTxPayload[1518] __attribute__((aligned(16)));
static u8                    g_aTxPack[2048] __attribute__((aligned(16)));

/* Bounce pool for userspace AVAIL_PUSH (ring programming path) */
#define GJ_NET_BOUNCE_N 8u
#define GJ_NET_BOUNCE_SZ 2048u
static u8                    g_aBounce[GJ_NET_BOUNCE_N][GJ_NET_BOUNCE_SZ]
    __attribute__((aligned(16)));
static u8                    g_aBounceUsed[GJ_NET_BOUNCE_N];
static u8                    g_aMac[6];
static int                   g_fHaveMac;

/* Wave 12 soft inventory telemetry (never hard-gates product TX/RX). */
static u32 g_u32SoftLogN;     /* inventory emissions */
static int g_fSoftOnce;       /* first post-activity inventory emitted */
static u32 g_u32MapQ;         /* map_q_user successes */
static u32 g_u32ExportQ;      /* export_q successes */
static u32 g_u32KickApi;      /* kick_q API calls (distinct from desc kicks) */
static u32 g_u32MapDma;       /* map_dma_user successes */
static u16 g_u16RxFreeMin;    /* RX free-desc watermark (0xffff = never) */
static u16 g_u16TxFreeMin;    /* TX free-desc watermark (0xffff = never) */

static void virtio_net_soft_inventory(const char *szVia);
static void net_soft_maybe_once(void);

/* ---- tiny MMIO helpers (feature snapshot only; no virtio_pci edits) ---- */
static u32
net_mmio_r32(volatile u8 *p)
{
    return p ? *(volatile u32 *)p : 0;
}

static void
net_mmio_w32(volatile u8 *p, u32 u32V)
{
    if (p) {
        *(volatile u32 *)p = u32V;
    }
}

static u64
net_read_features(struct gj_virtio_dev *pDev, int fGuest)
{
    volatile u8 *pCommon;
    u32 u32Lo;
    u32 u32Hi;
    u32 u32Sel;
    u32 u32Val;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    pCommon = pDev->pCommon;
    if (fGuest) {
        u32Sel = NET_PCI_COMMON_GFSELECT;
        u32Val = NET_PCI_COMMON_GF;
    } else {
        u32Sel = NET_PCI_COMMON_DFSELECT;
        u32Val = NET_PCI_COMMON_DF;
    }
    net_mmio_w32(pCommon + u32Sel, 0);
    u32Lo = net_mmio_r32(pCommon + u32Val);
    net_mmio_w32(pCommon + u32Sel, 1);
    u32Hi = net_mmio_r32(pCommon + u32Val);
    return ((u64)u32Hi << 32) | (u64)u32Lo;
}

static void
net_log_features(u64 u64Dev, u64 u64Drv)
{
    kprintf("virtio-net: features dev=0x%lx drv=0x%lx"
            " mac=%u status=%u mrg=%u v1=%u\n",
            (unsigned long)u64Dev, (unsigned long)u64Drv,
            (unsigned)((u64Drv & VIRTIO_NET_F_MAC) != 0),
            (unsigned)((u64Drv & VIRTIO_NET_F_STATUS) != 0),
            (unsigned)((u64Drv & VIRTIO_NET_F_MRG_RXBUF) != 0),
            (unsigned)((u64Drv & GJ_VIRTIO_F_VERSION_1) != 0));
}

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

static void
stats_reset(void)
{
    memset(&g_Stats, 0, sizeof(g_Stats));
}

/* Snapshot free-desc watermarks for soft queue inventory (live only). */
static void
net_q_note_free(void)
{
    u16 u16RxFree;
    u16 u16TxFree;

    if (!g_fReady) {
        return;
    }
    u16RxFree = virtio_q_num_free(&g_qRx);
    u16TxFree = virtio_q_num_free(&g_qTx);
    if (u16RxFree < g_u16RxFreeMin) {
        g_u16RxFreeMin = u16RxFree;
    }
    if (u16TxFree < g_u16TxFreeMin) {
        g_u16TxFreeMin = u16TxFree;
    }
}

/**
 * Greppable Wave 12 soft inventory dump (product / smoke).
 * Prefix-stable "virtio-net: soft …" — never hard-gates; kprintf only.
 *
 *   virtio-net: soft inventory  — ready + PCI + geometry + free watermarks
 *   virtio-net: soft multi-buf  — RX pool post/live/merge tallies
 *   virtio-net: soft tx-chain   — 2-desc prefer + 1-desc fallback tallies
 *   virtio-net: soft bounce     — UDX bounce pool + avail_push surface
 *   virtio-net: soft queue      — RX/TX size / free / free_min / notify
 *   virtio-net: soft features   — guest/dev negotiate snapshot bits
 *   virtio-net: soft stats      — tx/rx bytes + drop/empty
 *   virtio-net: soft mac        — soft MAC presence + octets
 *   virtio-net: soft path       — product surface catalog (claim honesty)
 *   virtio-net: soft ring       — export/map/kick/dma soft tallies
 *   virtio-net: soft PASS|NODEV|PARTIAL
 *
 * greppable: virtio-net: soft
 */
static void
virtio_net_soft_inventory(const char *szVia)
{
    const char *szVerdict;
    const char *szViaSafe;
    u32 i;
    u32 cLive = 0;
    u32 cBounceUsed = 0;
    u16 u16RxSize = 0;
    u16 u16TxSize = 0;
    u16 u16RxFree = 0;
    u16 u16TxFree = 0;
    u16 u16RxFreeMin;
    u16 u16TxFreeMin;
    u8 u8Bus = 0;
    u8 u8Slot = 0;
    unsigned fMac;
    unsigned fStatus;
    unsigned fMrg;
    unsigned fV1;

    szViaSafe = (szVia != NULL) ? szVia : "path";

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    for (i = 0; i < GJ_VIRTIO_NET_RX_N; i++) {
        if (g_aRxSlotLive[i] != 0) {
            cLive++;
        }
    }
    for (i = 0; i < GJ_NET_BOUNCE_N; i++) {
        if (g_aBounceUsed[i] != 0) {
            cBounceUsed++;
        }
    }
    if (g_fReady) {
        net_q_note_free();
        u16RxSize = g_qRx.u16Size;
        u16TxSize = g_qTx.u16Size;
        u16RxFree = virtio_q_num_free(&g_qRx);
        u16TxFree = virtio_q_num_free(&g_qTx);
    }
    u16RxFreeMin = (g_u16RxFreeMin == 0xffffu) ? 0 : g_u16RxFreeMin;
    u16TxFreeMin = (g_u16TxFreeMin == 0xffffu) ? 0 : g_u16TxFreeMin;
    if (g_pNet != NULL) {
        u8Bus = g_pNet->u8Bus;
        u8Slot = g_pNet->u8Slot;
    }

    fMac = (unsigned)((g_Stats.u64Features & VIRTIO_NET_F_MAC) != 0);
    fStatus = (unsigned)((g_Stats.u64Features & VIRTIO_NET_F_STATUS) != 0);
    fMrg = (unsigned)((g_Stats.u64Features & VIRTIO_NET_F_MRG_RXBUF) != 0);
    fV1 = (unsigned)((g_Stats.u64Features & GJ_VIRTIO_F_VERSION_1) != 0);

    /*
     * Soft verdict (inventory only; TX/RX path unchanged):
     *   NODEV    — not ready / no device
     *   PASS     — ready + any completed kernel TX/RX
     *   PARTIAL  — ready, no completed product frame yet (post-probe)
     */
    if (!g_fReady) {
        szVerdict = "NODEV";
    } else if (g_Stats.u32TxCount != 0u || g_Stats.u32RxCount != 0u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /*
     * Grep: virtio-net: soft inventory
     * Aggregated presence / geometry snapshot for product smoke.
     */
    kprintf("virtio-net: soft inventory via=%s ready=%u bus=%x slot=%x "
            "rx_slots=%u rx_sz=%u bounce_n=%u bounce_sz=%u "
            "tx_chain=2desc mac_have=%u rx_free=%u rx_free_min=%u "
            "tx_free=%u tx_free_min=%u log_n=%u\n",
            szViaSafe, g_fReady ? 1u : 0u, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)GJ_VIRTIO_NET_RX_N, (unsigned)GJ_VIRTIO_NET_RX_SZ,
            (unsigned)GJ_NET_BOUNCE_N, (unsigned)GJ_NET_BOUNCE_SZ,
            (unsigned)g_fHaveMac, (unsigned)u16RxFree, (unsigned)u16RxFreeMin,
            (unsigned)u16TxFree, (unsigned)u16TxFreeMin, g_u32SoftLogN);

    /* Grep: virtio-net: soft multi-buf */
    kprintf("virtio-net: soft multi-buf rx_n=%u posted=%u live=%u "
            "post_fail=%u merge=%u mrg_feat=%u refill=1 pool_sz=%u\n",
            (unsigned)GJ_VIRTIO_NET_RX_N, (unsigned)g_Stats.u32RxPosted,
            (unsigned)cLive, (unsigned)g_Stats.u32RxPostFail,
            (unsigned)g_Stats.u32RxMerge, fMrg,
            (unsigned)(GJ_VIRTIO_NET_RX_N * GJ_VIRTIO_NET_RX_SZ));

    /* Grep: virtio-net: soft tx-chain */
    kprintf("virtio-net: soft tx-chain prefer=2desc fallback=1desc "
            "multi=%u single=%u fail=%u timeout=%u hdr_sz=%u "
            "max_payload=1514\n",
            (unsigned)g_Stats.u32TxMulti, (unsigned)g_Stats.u32TxSingle,
            (unsigned)g_Stats.u32TxFail, (unsigned)g_Stats.u32TxTimeout,
            (unsigned)sizeof(struct virtio_net_hdr));

    /* Grep: virtio-net: soft bounce */
    kprintf("virtio-net: soft bounce slots=%u sz=%u used=%u free=%u "
            "avail_push=%u user_ring=%u map_dma=%u\n",
            (unsigned)GJ_NET_BOUNCE_N, (unsigned)GJ_NET_BOUNCE_SZ,
            (unsigned)cBounceUsed,
            (unsigned)(GJ_NET_BOUNCE_N - cBounceUsed),
            (unsigned)g_Stats.u32AvailPushes,
            (unsigned)g_Stats.u32UserRingPushes, g_u32MapDma);

    /* Grep: virtio-net: soft queue */
    kprintf("virtio-net: soft queue rx_size=%u rx_free=%u rx_free_min=%u "
            "rx_idx=%u rx_notify=%u tx_size=%u tx_free=%u tx_free_min=%u "
            "tx_idx=%u tx_notify=%u kicks=%u reaps=%u\n",
            (unsigned)u16RxSize, (unsigned)u16RxFree, (unsigned)u16RxFreeMin,
            (unsigned)(g_fReady ? g_qRx.u16QueueIdx : 0u),
            (unsigned)(g_fReady ? g_qRx.u16NotifyOff : 0u),
            (unsigned)u16TxSize, (unsigned)u16TxFree, (unsigned)u16TxFreeMin,
            (unsigned)(g_fReady ? g_qTx.u16QueueIdx : 0u),
            (unsigned)(g_fReady ? g_qTx.u16NotifyOff : 0u),
            (unsigned)g_Stats.u32Kicks, (unsigned)g_Stats.u32Reaps);

    /* Grep: virtio-net: soft features */
    kprintf("virtio-net: soft features guest=0x%lx dev=0x%lx "
            "mac=%u status=%u mrg=%u v1=%u ladder=mac+status+mrg|mac+status|v1\n",
            (unsigned long)g_Stats.u64Features,
            (unsigned long)g_Stats.u64FeaturesDev,
            fMac, fStatus, fMrg, fV1);

    /* Grep: virtio-net: soft stats */
    kprintf("virtio-net: soft stats tx=%u rx=%u tx_bytes=%lu rx_bytes=%lu "
            "drop=%u empty=%u post_fail=%u tx_fail=%u tx_timeout=%u "
            "merge=%u kicks=%u reaps=%u\n",
            (unsigned)g_Stats.u32TxCount, (unsigned)g_Stats.u32RxCount,
            (unsigned long)g_Stats.u64TxBytes,
            (unsigned long)g_Stats.u64RxBytes,
            (unsigned)g_Stats.u32RxDrop, (unsigned)g_Stats.u32RxEmpty,
            (unsigned)g_Stats.u32RxPostFail, (unsigned)g_Stats.u32TxFail,
            (unsigned)g_Stats.u32TxTimeout, (unsigned)g_Stats.u32RxMerge,
            (unsigned)g_Stats.u32Kicks, (unsigned)g_Stats.u32Reaps);

    /* Grep: virtio-net: soft mac */
    kprintf("virtio-net: soft mac have=%u feat=%u "
            "addr=%x:%x:%x:%x:%x:%x\n",
            (unsigned)g_fHaveMac, fMac,
            (unsigned)g_aMac[0], (unsigned)g_aMac[1], (unsigned)g_aMac[2],
            (unsigned)g_aMac[3], (unsigned)g_aMac[4], (unsigned)g_aMac[5]);

    /*
     * Grep: virtio-net: soft path
     * Honesty catalog: product surfaces this driver exposes (net_door /
     * netstackd / UDX). claim=1 only when DRIVER_OK + RX/TX qs live.
     */
    kprintf("virtio-net: soft path claim=%u tx=1 rx=1 multi_buf=1 "
            "tx_chain=1 bounce=1 export_q=1 map_q=1 map_dma=1 kick_q=1 "
            "avail_push=1 user_ring=1 used_reap=1 q_free=1 stats=1 "
            "gso=0 csum_offload=0 mq=0 ctrl_vq=0\n",
            g_fReady ? 1u : 0u);

    /* Grep: virtio-net: soft ring */
    kprintf("virtio-net: soft ring export=%u map=%u map_dma=%u kick_api=%u "
            "desc_kicks=%u avail_push=%u user_ring=%u "
            "rx_pa_desc=0x%lx tx_pa_desc=0x%lx\n",
            g_u32ExportQ, g_u32MapQ, g_u32MapDma, g_u32KickApi,
            (unsigned)g_Stats.u32Kicks,
            (unsigned)g_Stats.u32AvailPushes,
            (unsigned)g_Stats.u32UserRingPushes,
            (unsigned long)(g_fReady ? (u64)g_qRx.paDesc : 0ull),
            (unsigned long)(g_fReady ? (u64)g_qTx.paDesc : 0ull));

    /* Grep: virtio-net: soft PASS | NODEV | PARTIAL */
    kprintf("virtio-net: soft %s via=%s ready=%u tx=%u rx=%u "
            "tx_fail=%u drop=%u log_n=%u\n",
            szVerdict, szViaSafe, g_fReady ? 1u : 0u,
            (unsigned)g_Stats.u32TxCount, (unsigned)g_Stats.u32RxCount,
            (unsigned)g_Stats.u32TxFail, (unsigned)g_Stats.u32RxDrop,
            g_u32SoftLogN);

    /* Grep: virtio-net: soft inventory PASS|NODEV|PARTIAL */
    kprintf("virtio-net: soft inventory %s via=%s logs=%u "
            "(soft inventory only; not bar3)\n",
            szVerdict, szViaSafe, g_u32SoftLogN);
}

/**
 * After first product TX/RX activity, print soft inventory once
 * (mirrors virtio-blk / virtio-scsi soft-stats-once). Diagnostics only.
 */
static void
net_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_Stats.u32TxCount == 0u && g_Stats.u32RxCount == 0u &&
        g_Stats.u32TxFail == 0u && g_Stats.u32RxDrop == 0u) {
        return;
    }
    g_fSoftOnce = 1;
    virtio_net_soft_inventory("activity");
}

static void
net_kick(struct gj_virtq *pQ)
{
    virtio_q_kick(pQ);
    g_Stats.u32Kicks++;
    net_q_note_free();
}

/*
 * Post one free RX slot (device-write). Returns 0 on success, -1 on fail.
 * Soft multi-buffer: caller refills until pool or free descs exhaust.
 */
static int
rx_post_slot(u32 u32Slot)
{
    int head;
    gj_paddr_t pa;

    if (u32Slot >= GJ_VIRTIO_NET_RX_N || g_aRxSlotLive[u32Slot]) {
        return -1;
    }
    memset(g_aRxSlot[u32Slot], 0, GJ_VIRTIO_NET_RX_SZ);
    pa = buf_phys(g_aRxSlot[u32Slot]);
    head = virtio_q_add(&g_qRx, pa, GJ_VIRTIO_NET_RX_SZ, 1);
    if (head < 0) {
        g_Stats.u32RxPostFail++;
        return -1;
    }
    if ((u32)head < GJ_VIRTQ_MAX_SIZE) {
        g_aRxHeadSlot[head] = (u8)u32Slot;
    }
    g_aRxSlotLive[u32Slot] = 1;
    g_Stats.u32RxPosted++;
    return 0;
}

/* Post every free slot; returns number newly posted. */
static u32
rx_post_all(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < GJ_VIRTIO_NET_RX_N; i++) {
        if (!g_aRxSlotLive[i] && rx_post_slot(i) == 0) {
            c++;
        }
    }
    return c;
}

/*
 * Claim RX completion: map used head → slot, mark free for refill.
 * Returns slot index or -1 if mapping missing (still free descs via poll).
 */
static int
rx_claim_head(u32 u32Head)
{
    u8 u8Slot;

    if (u32Head >= GJ_VIRTQ_MAX_SIZE) {
        return -1;
    }
    u8Slot = g_aRxHeadSlot[u32Head];
    g_aRxHeadSlot[u32Head] = 0xff;
    if (u8Slot >= GJ_VIRTIO_NET_RX_N) {
        return -1;
    }
    if (g_aRxSlotLive[u8Slot]) {
        g_aRxSlotLive[u8Slot] = 0;
        if (g_Stats.u32RxPosted > 0) {
            g_Stats.u32RxPosted--;
        }
    }
    return (int)u8Slot;
}

/*
 * Probe path: find first net → modern PCI caps → features → RX/TX qs →
 * multi-buffer RX post. Leaves g_fReady=0 and g_pNet=NULL on any hard failure.
 */
int
virtio_net_probe(void)
{
    u32 i;
    u32 c;
    u32 cPosted;
    gj_status_t st;
    u64 u64Want;
    u64 u64Dev;
    u64 u64Drv;

    g_pNet = NULL;
    g_fReady = 0;
    g_fHaveMac = 0;
    stats_reset();
    memset(g_aRxSlotLive, 0, sizeof(g_aRxSlotLive));
    memset(g_aRxHeadSlot, 0xff, sizeof(g_aRxHeadSlot));
    memset(g_aBounceUsed, 0, sizeof(g_aBounceUsed));
    /* Preserve g_u32SoftLogN across re-probe so log_n stays monotonic. */
    g_fSoftOnce = 0;
    g_u32MapQ = 0;
    g_u32ExportQ = 0;
    g_u32KickApi = 0;
    g_u32MapDma = 0;
    g_u16RxFreeMin = 0xffffu;
    g_u16TxFreeMin = 0xffffu;

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
        /* Grep: virtio-net: soft … NODEV (Wave 12 soft inventory). */
        virtio_net_soft_inventory("nodev");
        return -1;
    }
    st = virtio_pci_setup(g_pNet);
    if (st != GJ_OK || g_pNet->pCommon == NULL) {
        kprintf("virtio-net: setup failed %d\n", (int)st);
        g_pNet = NULL;
        virtio_net_soft_inventory("pci_fail");
        return -1;
    }

    /*
     * Soft feature ladder: prefer MAC+STATUS+MRG_RXBUF+V1, then without MRG,
     * then V1 only. Snapshot dev/guest features after a successful negotiate.
     */
    u64Want = GJ_VIRTIO_F_VERSION_1 | VIRTIO_NET_F_MAC | VIRTIO_NET_F_STATUS |
              VIRTIO_NET_F_MRG_RXBUF;
    st = virtio_negotiate(g_pNet, u64Want);
    if (st != GJ_OK) {
        u64Want = GJ_VIRTIO_F_VERSION_1 | VIRTIO_NET_F_MAC | VIRTIO_NET_F_STATUS;
        st = virtio_negotiate(g_pNet, u64Want);
        if (st != GJ_OK) {
            st = virtio_negotiate(g_pNet, GJ_VIRTIO_F_VERSION_1);
            if (st != GJ_OK) {
                kprintf("virtio-net: negotiate failed %d\n", (int)st);
                g_pNet = NULL;
                virtio_net_soft_inventory("negotiate_fail");
                return -1;
            }
        }
    }
    u64Dev = net_read_features(g_pNet, 0);
    u64Drv = net_read_features(g_pNet, 1);
    g_Stats.u64FeaturesDev = u64Dev;
    g_Stats.u64Features = u64Drv;
    net_log_features(u64Dev, u64Drv);

    read_mac(g_pNet);
    /* RX=0, TX=1 */
    st = virtio_q_setup(g_pNet, &g_qRx, 0, 64);
    if (st != GJ_OK) {
        kprintf("virtio-net: rx queue failed %d\n", (int)st);
        g_pNet = NULL;
        virtio_net_soft_inventory("rx_q_fail");
        return -1;
    }
    st = virtio_q_setup(g_pNet, &g_qTx, 1, 64);
    if (st != GJ_OK) {
        kprintf("virtio-net: tx queue failed %d\n", (int)st);
        g_pNet = NULL;
        virtio_net_soft_inventory("tx_q_fail");
        return -1;
    }

    /* Soft multi-buffer: post full RX pool (device-write) before DRIVER_OK */
    cPosted = rx_post_all();
    if (cPosted > 0) {
        net_kick(&g_qRx);
    } else {
        kprintf("virtio-net: initial RX post failed (tx-only until retry)\n");
    }
    kprintf("virtio-net: multi-buf rx_slots=%u posted=%u tx_chain=2desc\n",
            (unsigned)GJ_VIRTIO_NET_RX_N, (unsigned)cPosted);

    virtio_set_status(g_pNet, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                   GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
    g_u16RxFreeMin = virtio_q_num_free(&g_qRx);
    g_u16TxFreeMin = virtio_q_num_free(&g_qTx);
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
    /*
     * Wave 12 soft inventory rollup (prefix-stable "virtio-net: soft …").
     * Post-probe PARTIAL until first product TX/RX frames complete.
     */
    virtio_net_soft_inventory("probe");
    return 0;
}

int
virtio_net_ready(void)
{
    return g_fReady;
}

/*
 * Kernel TX soft multi-buffer: prefer 2-desc chain (hdr device-R + payload
 * device-R). Fall back to single contiguous pack if the queue has only one
 * free descriptor.
 */
int
virtio_net_tx(const void *pFrame, u32 cbLen)
{
    gj_paddr_t paHdr;
    gj_paddr_t paPay;
    i32 i32Done;
    int fMulti = 0;

    if (!g_fReady || pFrame == NULL || cbLen == 0 || cbLen > 1514) {
        if (g_fReady) {
            g_Stats.u32TxFail++;
            net_soft_maybe_once();
        }
        return -1;
    }

    /* Reap completed TX to free multi-desc chains */
    {
        u32 n = virtio_q_reap(&g_qTx, 8);

        g_Stats.u32Reaps += n;
        net_q_note_free();
    }

    memset(&g_TxHdr, 0, sizeof(g_TxHdr));
    /* V1 hdr: num_buffers unused on TX; leave 0 */
    memcpy(g_aTxPayload, pFrame, cbLen);
    paHdr = buf_phys(&g_TxHdr);
    paPay = buf_phys(g_aTxPayload);

    if (g_qTx.u16NumFree >= 2 &&
        virtio_q_add2(&g_qTx, paHdr, (u32)sizeof(g_TxHdr), 0, paPay, cbLen,
                      0) >= 0) {
        fMulti = 1;
        g_Stats.u32TxMulti++;
    } else {
        /* Single-desc fallback: hdr+payload packed */
        struct virtio_net_hdr *pHdr;
        u32 cbTotal;
        gj_paddr_t pa;

        memset(g_aTxPack, 0, sizeof(g_aTxPack));
        pHdr = (struct virtio_net_hdr *)(void *)g_aTxPack;
        memset(pHdr, 0, sizeof(*pHdr));
        memcpy(g_aTxPack + sizeof(*pHdr), pFrame, cbLen);
        cbTotal = (u32)sizeof(*pHdr) + cbLen;
        pa = buf_phys(g_aTxPack);
        if (virtio_q_add(&g_qTx, pa, cbTotal, 0) < 0) {
            g_Stats.u32TxFail++;
            net_soft_maybe_once();
            return -1;
        }
        g_Stats.u32TxSingle++;
    }

    net_kick(&g_qTx);
    i32Done = virtio_q_poll(&g_qTx, 1000000u);
    if (i32Done < 0) {
        /*
         * Soft: count timeout but keep historical success return — device may
         * complete asynchronously; queue still holds the chain until reap.
         */
        g_Stats.u32TxTimeout++;
    } else {
        g_Stats.u32Reaps++;
    }
    (void)fMulti;
    g_Stats.u32TxCount++;
    g_Stats.u64TxBytes += (u64)cbLen;
    net_q_note_free();
    net_soft_maybe_once();
    return 0;
}

u32
virtio_net_tx_count(void)
{
    return g_Stats.u32TxCount;
}

/*
 * Soft multi-buffer RX: poll one used head, map to slot, copy Ethernet
 * payload. If MRG_RXBUF and num_buffers>1, soft-merge subsequent used
 * buffers (each carries its own virtio_net_hdr per OASIS). Refill pool.
 */
i32
virtio_net_rx(void *pOut, u32 cbMax)
{
    i32 i32Len;
    u32 u32Id;
    int nSlot;
    u32 cbPayload;
    u32 cbCopied = 0;
    u16 u16NumBuf = 1;
    struct virtio_net_hdr *pHdr;
    u8 *pSlot;
    u32 iMerge;

    if (!g_fReady || pOut == NULL) {
        return -1;
    }

    i32Len = virtio_q_poll_id(&g_qRx, 1000u, &u32Id);
    if (i32Len < 0) {
        g_Stats.u32RxEmpty++;
        /* Opportunistic refill if pool drained */
        if (g_Stats.u32RxPosted < GJ_VIRTIO_NET_RX_N) {
            if (rx_post_all() > 0) {
                net_kick(&g_qRx);
            }
        }
        return 0;
    }
    g_Stats.u32Reaps++;
    nSlot = rx_claim_head(u32Id);
    if (nSlot < 0) {
        /* Mapping miss: drop payload, still refill */
        g_Stats.u32RxDrop++;
        if (rx_post_all() > 0) {
            net_kick(&g_qRx);
        }
        net_soft_maybe_once();
        return 0;
    }

    pSlot = g_aRxSlot[nSlot];
    pHdr = (struct virtio_net_hdr *)(void *)pSlot;
    if ((u32)i32Len <= sizeof(*pHdr)) {
        cbPayload = 0;
        g_Stats.u32RxDrop++;
    } else {
        cbPayload = (u32)i32Len - (u32)sizeof(*pHdr);
    }

    /* Soft MRG_RXBUF: first buffer carries total buffer count */
    if ((g_Stats.u64Features & VIRTIO_NET_F_MRG_RXBUF) != 0 &&
        pHdr->u16NumBuffers > 1) {
        u16NumBuf = pHdr->u16NumBuffers;
        if (u16NumBuf > GJ_VIRTIO_NET_RX_N) {
            u16NumBuf = (u16)GJ_VIRTIO_NET_RX_N;
        }
    } else if (pHdr->u16NumBuffers > 1) {
        /* V1 may still set num_buffers=1; tolerate soft multi without feature */
        u16NumBuf = pHdr->u16NumBuffers;
        if (u16NumBuf > GJ_VIRTIO_NET_RX_N) {
            u16NumBuf = (u16)GJ_VIRTIO_NET_RX_N;
        }
    }

    if (cbPayload > cbMax) {
        /* Truncate first segment; count as soft drop fragment */
        g_Stats.u32RxDrop++;
        cbPayload = cbMax;
    }
    if (cbPayload > 0) {
        memcpy(pOut, pSlot + sizeof(*pHdr), cbPayload);
        cbCopied = cbPayload;
    }

    /* Merge remaining buffers of the same frame (soft multi-buffer) */
    for (iMerge = 1; iMerge < (u32)u16NumBuf && cbCopied < cbMax; iMerge++) {
        i32 i32Seg;
        u32 u32SegId;
        int nSeg;
        u32 cbSeg;
        u8 *pSeg;
        struct virtio_net_hdr *pSegHdr;

        i32Seg = virtio_q_poll_id(&g_qRx, 10000u, &u32SegId);
        if (i32Seg < 0) {
            g_Stats.u32RxDrop++;
            break;
        }
        g_Stats.u32Reaps++;
        nSeg = rx_claim_head(u32SegId);
        if (nSeg < 0) {
            g_Stats.u32RxDrop++;
            continue;
        }
        pSeg = g_aRxSlot[nSeg];
        pSegHdr = (struct virtio_net_hdr *)(void *)pSeg;
        if ((u32)i32Seg <= sizeof(*pSegHdr)) {
            /* Empty or hdr-only segment */
            (void)pSegHdr;
            continue;
        }
        cbSeg = (u32)i32Seg - (u32)sizeof(*pSegHdr);
        if (cbSeg > cbMax - cbCopied) {
            g_Stats.u32RxDrop++;
            cbSeg = cbMax - cbCopied;
        }
        if (cbSeg > 0) {
            memcpy((u8 *)pOut + cbCopied, pSeg + sizeof(*pSegHdr), cbSeg);
            cbCopied += cbSeg;
        }
    }
    if (u16NumBuf > 1 && cbCopied > 0) {
        g_Stats.u32RxMerge++;
    }

    if (cbCopied > 0) {
        g_Stats.u32RxCount++;
        g_Stats.u64RxBytes += (u64)cbCopied;
    }

    /* Refill multi-buffer RX pool */
    if (rx_post_all() > 0) {
        net_kick(&g_qRx);
    }
    net_q_note_free();
    net_soft_maybe_once();
    return (i32)cbCopied;
}

u32
virtio_net_rx_count(void)
{
    return g_Stats.u32RxCount;
}

u32
virtio_net_tx_fail_count(void)
{
    return g_Stats.u32TxFail;
}

u32
virtio_net_tx_timeout_count(void)
{
    return g_Stats.u32TxTimeout;
}

u32
virtio_net_rx_drop_count(void)
{
    return g_Stats.u32RxDrop;
}

u32
virtio_net_kick_count(void)
{
    return g_Stats.u32Kicks;
}

u32
virtio_net_rx_posted(void)
{
    return g_Stats.u32RxPosted;
}

u64
virtio_net_tx_bytes(void)
{
    return g_Stats.u64TxBytes;
}

u64
virtio_net_rx_bytes(void)
{
    return g_Stats.u64RxBytes;
}

u64
virtio_net_features(void)
{
    return g_fReady ? g_Stats.u64Features : 0;
}

u64
virtio_net_features_dev(void)
{
    return g_fReady ? g_Stats.u64FeaturesDev : 0;
}

int
virtio_net_stats(struct gj_virtio_net_stats *pOut)
{
    if (pOut == NULL) {
        return -1;
    }
    *pOut = g_Stats;
    /*
     * Emit soft inventory on stats read so bring-up smoke also greps
     * virtio-net: soft … lines (mirrors virtio_blk q_stats path).
     * greppable: virtio-net: soft
     */
    virtio_net_soft_inventory("stats");
    return 0;
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
    g_u32ExportQ++;
    net_q_note_free();
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
    net_kick(pQ);
    g_u32KickApi++;
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
    g_u32MapQ++;
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
    g_u32MapDma++;
    kprintf("virtio-net: map_dma va=0x%lx slots=%u\n", (unsigned long)u64VaBase,
            GJ_NET_BOUNCE_N);
    return 0;
}

int
virtio_net_desc_alloc(u16 u16Which)
{
    struct gj_virtq *pQ;
    int head;
    u32 n;

    if (!g_fReady || u16Which > 1) {
        return -1;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    n = virtio_q_reap(pQ, 8);
    g_Stats.u32Reaps += n;
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
    g_Stats.u32UserRingPushes++;
    g_Stats.u32AvailPushes++;
    if (fKick) {
        net_kick(pQ);
    }
    if (u16Which == 1) {
        g_Stats.u32TxCount++;
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
    u32 n;

    if (!g_fReady || pBuf == NULL || cbLen == 0 || cbLen > GJ_NET_BOUNCE_SZ) {
        return -1;
    }
    if (u16Which > 1) {
        return -1;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    /* Reap completed to free descs */
    n = virtio_q_reap(pQ, 8);
    g_Stats.u32Reaps += n;
    bounce_free_all_if_idle();
    slot = bounce_alloc();
    if (slot < 0) {
        n = virtio_q_reap(pQ, 16);
        g_Stats.u32Reaps += n;
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
    g_Stats.u32AvailPushes++;
    if (fKick) {
        net_kick(pQ);
    }
    if (u16Which == 1) {
        g_Stats.u32TxCount++;
    }
    return 0;
}

u32
virtio_net_used_reap(u16 u16Which, u32 u32Max)
{
    struct gj_virtq *pQ;
    u32 n = 0;
    u32 u32Limit;

    if (!g_fReady || u16Which > 1) {
        return 0;
    }
    u32Limit = u32Max ? u32Max : 8u;
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    if (u16Which == 0) {
        /*
         * RX multi-buffer: must claim head→slot before descs free so the
         * soft pool stays coherent (payload discarded on pure reap).
         */
        while (n < u32Limit) {
            u32 u32Id;
            i32 i32Len = virtio_q_poll_id(&g_qRx, 1, &u32Id);

            if (i32Len < 0) {
                break;
            }
            (void)rx_claim_head(u32Id);
            n++;
        }
        g_Stats.u32Reaps += n;
        if (rx_post_all() > 0) {
            net_kick(&g_qRx);
        }
    } else {
        n = virtio_q_reap(pQ, u32Limit);
        g_Stats.u32Reaps += n;
        bounce_free_all_if_idle();
    }
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
    return g_Stats.u32AvailPushes;
}

u32
virtio_net_user_ring_pushes(void)
{
    return g_Stats.u32UserRingPushes;
}
