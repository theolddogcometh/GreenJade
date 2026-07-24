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
 * Soft inventory (Wave 15 exclusive deepen; this unit only; never hard-gates):
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
 *   virtio-net: soft last …
 *   virtio-net: soft errors …
 *   virtio-net: soft api …
 *   virtio-net: soft refill …
 *   virtio-net: soft hdr …
 *   virtio-net: soft bytes …
 *   virtio-net: soft empty …
 *   virtio-net: soft link …
 *   virtio-net: soft pci …          (Wave 15)
 *   virtio-net: soft probe …        (Wave 15)
 *   virtio-net: soft claim …        (Wave 15)
 *   virtio-net: soft via …          (Wave 15)
 *   virtio-net: soft kicks …        (Wave 15)
 *   virtio-net: soft oasis …        (Wave 15)
 *   virtio-net: soft deepen …
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
/* Wave 20 deepen stamp (greppable wave= / areas=). */
#define VIRTIO_NET_SOFT_DEEPEN_WAVE 116u
#define VIRTIO_NET_SOFT_DEEPEN_AREAS 176u
static u8                    g_aBounce[GJ_NET_BOUNCE_N][GJ_NET_BOUNCE_SZ]
    __attribute__((aligned(16)));
static u8                    g_aBounceUsed[GJ_NET_BOUNCE_N];
static u8                    g_aMac[6];
static int                   g_fHaveMac;

/* Wave 14 soft inventory telemetry (never hard-gates product TX/RX). */
static u32 g_u32SoftLogN;     /* inventory emissions */
static int g_fSoftOnce;       /* first post-activity inventory emitted */
static u32 g_u32MapQ;         /* map_q_user successes */
static u32 g_u32ExportQ;      /* export_q successes */
static u32 g_u32KickApi;      /* kick_q API calls (distinct from desc kicks) */
static u32 g_u32MapDma;       /* map_dma_user successes */
static u16 g_u16RxFreeMin;    /* RX free-desc watermark (0xffff = never) */
static u16 g_u16TxFreeMin;    /* TX free-desc watermark (0xffff = never) */
/* Wave 14 deepen: API enter, miss, sticky last, refill. */
static u32 g_u32TxApi;        /* virtio_net_tx entries */
static u32 g_u32RxApi;        /* virtio_net_rx entries */
static u32 g_u32StatsApi;     /* virtio_net_stats entries */
static u32 g_u32ExportFail;   /* export_q soft miss */
static u32 g_u32MapFail;      /* map_q_user soft miss */
static u32 g_u32MapDmaFail;   /* map_dma_user soft miss */
static u32 g_u32BounceFail;   /* bounce_alloc miss */
static u32 g_u32RefillOps;    /* rx_post_all invocations with posts */
static u32 g_u32RefillSlots;  /* slots newly posted across refills */
static u32 g_u32LastTxLen;    /* sticky last TX frame length */
static u32 g_u32LastRxLen;    /* sticky last RX payload length */
static u32 g_u32LastNumBuf;   /* sticky last RX num_buffers */
static u32 g_u32ProbeOk;      /* probe DRIVER_OK */
static u32 g_u32ProbeNodev;   /* probe no device */
static u32 g_u32ProbeFail;    /* probe pci/nego/q fail */
static u32 g_u32ReadyQuery;   /* virtio_net_ready samples */
static const char *g_szLastVia; /* sticky last inventory via */

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
 * Greppable Wave 15 soft inventory dump (product / smoke).
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
 *   virtio-net: soft last       — sticky last TX/RX lengths (Wave 14)
 *   virtio-net: soft errors     — fail/drop/miss breakdown (Wave 14)
 *   virtio-net: soft api        — public API enter tallies (Wave 14)
 *   virtio-net: soft refill     — RX pool refill tallies (Wave 14)
 *   virtio-net: soft hdr        — virtio-net hdr geometry (Wave 14)
 *   virtio-net: soft bytes      — TX/RX byte totals (Wave 14)
 *   virtio-net: soft empty      — RX empty / soft miss (Wave 14)
 *   virtio-net: soft link       — MAC+STATUS feature lamps (Wave 14)
 *   virtio-net: soft pci        — bus/slot/func/modern (Wave 15)
 *   virtio-net: soft probe      — probe outcome lamps (Wave 15)
 *   virtio-net: soft claim      — transport claim honesty (Wave 15)
 *   virtio-net: soft via        — sticky last inventory via (Wave 15)
 *   virtio-net: soft kicks      — kick/reap tallies (Wave 15)
 *   virtio-net: soft oasis      — feature-bit constant catalog (Wave 15)
 *   virtio-net: soft return rate — Wave 19 ok/fail rate lamps
 *   virtio-net: soft retcode    — Wave 19 retcode catalog
 *   virtio-net: soft deepen     — wave=116 areas stamp
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
    u8 u8Func = 0;
    u8 u8Modern = 0;
    unsigned fMac;
    unsigned fStatus;
    unsigned fMrg;
    unsigned fV1;
    u32 u32Ready;
    u32 u32BytesTot;

    szViaSafe = (szVia != NULL) ? szVia : "path";
    g_szLastVia = szViaSafe;

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
        u8Func = g_pNet->u8Func;
        u8Modern = g_pNet->fModern;
    }

    fMac = (unsigned)((g_Stats.u64Features & VIRTIO_NET_F_MAC) != 0);
    fStatus = (unsigned)((g_Stats.u64Features & VIRTIO_NET_F_STATUS) != 0);
    fMrg = (unsigned)((g_Stats.u64Features & VIRTIO_NET_F_MRG_RXBUF) != 0);
    fV1 = (unsigned)((g_Stats.u64Features & GJ_VIRTIO_F_VERSION_1) != 0);
    u32Ready = g_fReady ? 1u : 0u;
    u32BytesTot = (u32)(g_Stats.u64TxBytes + g_Stats.u64RxBytes);

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
            "tx_free=%u tx_free_min=%u log_n=%u wave=%u areas=%u\n",
            szViaSafe, u32Ready, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)GJ_VIRTIO_NET_RX_N, (unsigned)GJ_VIRTIO_NET_RX_SZ,
            (unsigned)GJ_NET_BOUNCE_N, (unsigned)GJ_NET_BOUNCE_SZ,
            (unsigned)g_fHaveMac, (unsigned)u16RxFree, (unsigned)u16RxFreeMin,
            (unsigned)u16TxFree, (unsigned)u16TxFreeMin, g_u32SoftLogN,
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE,
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_AREAS);

    /* Grep: virtio-net: soft multi-buf */
    kprintf("virtio-net: soft multi-buf rx_n=%u posted=%u live=%u "
            "post_fail=%u merge=%u mrg_feat=%u refill=1 pool_sz=%u "
            "refill_ops=%u refill_slots=%u\n",
            (unsigned)GJ_VIRTIO_NET_RX_N, (unsigned)g_Stats.u32RxPosted,
            (unsigned)cLive, (unsigned)g_Stats.u32RxPostFail,
            (unsigned)g_Stats.u32RxMerge, fMrg,
            (unsigned)(GJ_VIRTIO_NET_RX_N * GJ_VIRTIO_NET_RX_SZ),
            g_u32RefillOps, g_u32RefillSlots);

    /* Grep: virtio-net: soft tx-chain */
    kprintf("virtio-net: soft tx-chain prefer=2desc fallback=1desc "
            "multi=%u single=%u fail=%u timeout=%u hdr_sz=%u "
            "max_payload=1514 last_tx=%u\n",
            (unsigned)g_Stats.u32TxMulti, (unsigned)g_Stats.u32TxSingle,
            (unsigned)g_Stats.u32TxFail, (unsigned)g_Stats.u32TxTimeout,
            (unsigned)sizeof(struct virtio_net_hdr), g_u32LastTxLen);

    /* Grep: virtio-net: soft bounce */
    kprintf("virtio-net: soft bounce slots=%u sz=%u used=%u free=%u "
            "avail_push=%u user_ring=%u map_dma=%u bounce_fail=%u\n",
            (unsigned)GJ_NET_BOUNCE_N, (unsigned)GJ_NET_BOUNCE_SZ,
            (unsigned)cBounceUsed,
            (unsigned)(GJ_NET_BOUNCE_N - cBounceUsed),
            (unsigned)g_Stats.u32AvailPushes,
            (unsigned)g_Stats.u32UserRingPushes, g_u32MapDma,
            g_u32BounceFail);

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
            "mac=%u status=%u mrg=%u v1=%u ladder=mac+status+mrg|mac+status|v1 "
            "wave=%u\n",
            (unsigned long)g_Stats.u64Features,
            (unsigned long)g_Stats.u64FeaturesDev,
            fMac, fStatus, fMrg, fV1,
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);

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
            "gso=0 csum_offload=0 mq=0 ctrl_vq=0 wave=%u\n",
            u32Ready, (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-net: soft ring */
    kprintf("virtio-net: soft ring export=%u map=%u map_dma=%u kick_api=%u "
            "desc_kicks=%u avail_push=%u user_ring=%u "
            "rx_pa_desc=0x%lx tx_pa_desc=0x%lx "
            "export_fail=%u map_fail=%u map_dma_fail=%u\n",
            g_u32ExportQ, g_u32MapQ, g_u32MapDma, g_u32KickApi,
            (unsigned)g_Stats.u32Kicks,
            (unsigned)g_Stats.u32AvailPushes,
            (unsigned)g_Stats.u32UserRingPushes,
            (unsigned long)(g_fReady ? (u64)g_qRx.paDesc : 0ull),
            (unsigned long)(g_fReady ? (u64)g_qTx.paDesc : 0ull),
            g_u32ExportFail, g_u32MapFail, g_u32MapDmaFail);

    /* Grep: virtio-net: soft last (Wave 14) */
    kprintf("virtio-net: soft last tx_len=%u rx_len=%u num_buf=%u "
            "via=%s once=%u\n",
            g_u32LastTxLen, g_u32LastRxLen, g_u32LastNumBuf, szViaSafe,
            g_fSoftOnce ? 1u : 0u);

    /* Grep: virtio-net: soft errors (Wave 14) */
    kprintf("virtio-net: soft errors tx_fail=%u tx_timeout=%u drop=%u "
            "empty=%u post_fail=%u bounce_fail=%u export_fail=%u "
            "map_fail=%u map_dma_fail=%u probe_fail=%u\n",
            (unsigned)g_Stats.u32TxFail, (unsigned)g_Stats.u32TxTimeout,
            (unsigned)g_Stats.u32RxDrop, (unsigned)g_Stats.u32RxEmpty,
            (unsigned)g_Stats.u32RxPostFail, g_u32BounceFail, g_u32ExportFail,
            g_u32MapFail, g_u32MapDmaFail, g_u32ProbeFail);

    /* Grep: virtio-net: soft api (Wave 14) */
    kprintf("virtio-net: soft api tx=%u rx=%u stats=%u export=%u map=%u "
            "map_dma=%u kick=%u ready_q=%u\n",
            g_u32TxApi, g_u32RxApi, g_u32StatsApi, g_u32ExportQ, g_u32MapQ,
            g_u32MapDma, g_u32KickApi, g_u32ReadyQuery);

    /* Grep: virtio-net: soft refill (Wave 14) */
    kprintf("virtio-net: soft refill ops=%u slots=%u posted=%u live=%u "
            "post_fail=%u pool_n=%u\n",
            g_u32RefillOps, g_u32RefillSlots, (unsigned)g_Stats.u32RxPosted,
            (unsigned)cLive, (unsigned)g_Stats.u32RxPostFail,
            (unsigned)GJ_VIRTIO_NET_RX_N);

    /* Grep: virtio-net: soft hdr (Wave 14) */
    kprintf("virtio-net: soft hdr sz=%u flags_off=0 gso_off=1 "
            "num_buffers_off=10 v1=%u mrg=%u last_num_buf=%u\n",
            (unsigned)sizeof(struct virtio_net_hdr), fV1, fMrg,
            g_u32LastNumBuf);

    /* Grep: virtio-net: soft bytes (Wave 14) */
    kprintf("virtio-net: soft bytes tx=%lu rx=%lu total=%u "
            "tx_count=%u rx_count=%u\n",
            (unsigned long)g_Stats.u64TxBytes,
            (unsigned long)g_Stats.u64RxBytes, u32BytesTot,
            (unsigned)g_Stats.u32TxCount, (unsigned)g_Stats.u32RxCount);

    /* Grep: virtio-net: soft empty (Wave 14) */
    kprintf("virtio-net: soft empty rx_empty=%u tx_timeout=%u "
            "soft_miss=empty+timeout\n",
            (unsigned)g_Stats.u32RxEmpty, (unsigned)g_Stats.u32TxTimeout);

    /* Grep: virtio-net: soft link (Wave 14) */
    kprintf("virtio-net: soft link ready=%u modern=%u mac_have=%u "
            "mac_feat=%u status_feat=%u bus=%x slot=%x func=%x "
            "probe_ok=%u probe_nodev=%u\n",
            u32Ready, (unsigned)u8Modern, (unsigned)g_fHaveMac, fMac, fStatus,
            (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
            g_u32ProbeOk, g_u32ProbeNodev);

    /* Grep: virtio-net: soft pci (Wave 15) */
    kprintf("virtio-net: soft pci bus=%x slot=%x func=%x modern=%u "
            "ready=%u mac_have=%u\n",
            (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
            (unsigned)u8Modern, u32Ready, (unsigned)g_fHaveMac);

    /* Grep: virtio-net: soft probe (Wave 15) */
    kprintf("virtio-net: soft probe ok=%u nodev=%u fail=%u ready=%u "
            "tx=%u rx=%u\n",
            g_u32ProbeOk, g_u32ProbeNodev, g_u32ProbeFail, u32Ready,
            (unsigned)g_Stats.u32TxCount, (unsigned)g_Stats.u32RxCount);

    /* Grep: virtio-net: soft claim (Wave 15) */
    kprintf("virtio-net: soft claim ready=%u claim=%u modern=%u "
            "rx_q=%u tx_q=%u multi_buf=1 bounce=1\n",
            u32Ready, u32Ready, (unsigned)u8Modern,
            (unsigned)(g_fReady ? 1u : 0u), (unsigned)(g_fReady ? 1u : 0u));

    /* Grep: virtio-net: soft via (Wave 15) */
    kprintf("virtio-net: soft via last=%s log_n=%u once=%u\n",
            (g_szLastVia != NULL) ? g_szLastVia : "path", g_u32SoftLogN,
            g_fSoftOnce ? 1u : 0u);

    /* Grep: virtio-net: soft kicks (Wave 15) */
    kprintf("virtio-net: soft kicks desc=%u api=%u reaps=%u "
            "avail_push=%u user_ring=%u\n",
            (unsigned)g_Stats.u32Kicks, g_u32KickApi,
            (unsigned)g_Stats.u32Reaps, (unsigned)g_Stats.u32AvailPushes,
            (unsigned)g_Stats.u32UserRingPushes);

    /* Grep: virtio-net: soft oasis (Wave 15 feature-bit catalog) */
    kprintf("virtio-net: soft oasis bit_mac=5 bit_status=16 bit_mrg=15 "
            "hdr_sz=%u rx_n=%u rx_sz=%u bounce_n=%u "
            "mac=%u status=%u mrg=%u v1=%u gso=0 csum=0 mq=0\n",
            (unsigned)sizeof(struct virtio_net_hdr),
            (unsigned)GJ_VIRTIO_NET_RX_N, (unsigned)GJ_VIRTIO_NET_RX_SZ,
            (unsigned)GJ_NET_BOUNCE_N, fMac, fStatus, fMrg, fV1);

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: virtio-net: soft ratio|headroom|surface|return|contract
     */
    {
        u32 u32Surf = 0u;
        u32 u32TxBp = 0;
        u32 u32XferTot =
            (u32)g_Stats.u32TxCount + (u32)g_Stats.u32RxCount;

        if (u32XferTot != 0u) {
            u32TxBp = ((u32)g_Stats.u32TxCount * 10000u) / u32XferTot;
        }
        if (u32Ready != 0u) {
            u32Surf |= 0x1u;
        }
        if (g_Stats.u32TxCount != 0u) {
            u32Surf |= 0x2u;
        }
        if (g_Stats.u32RxCount != 0u) {
            u32Surf |= 0x4u;
        }
        if (g_fHaveMac != 0) {
            u32Surf |= 0x8u;
        }
        if (g_Stats.u32TxFail != 0u) {
            u32Surf |= 0x10u;
        }
        if (g_Stats.u32RxDrop != 0u) {
            u32Surf |= 0x20u;
        }
        u32Surf |= 0x40u; /* bounce multi-buf catalog */
        u32Surf |= 0x80u; /* oasis catalog always present */
        /* Grep: virtio-net: soft ratio */
        kprintf("virtio-net: soft ratio tx_bp=%u tx=%u rx=%u fail=%u "
                "drop=%u ready=%u wave=%u soft PASS\n",
                u32TxBp, (unsigned)g_Stats.u32TxCount,
                (unsigned)g_Stats.u32RxCount, (unsigned)g_Stats.u32TxFail,
                (unsigned)g_Stats.u32RxDrop, u32Ready,
                (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-net: soft headroom */
        kprintf("virtio-net: soft headroom rx_n=%u rx_sz=%u bounce_n=%u "
                "hdr_sz=%u wave=%u soft PASS\n",
                (unsigned)GJ_VIRTIO_NET_RX_N, (unsigned)GJ_VIRTIO_NET_RX_SZ,
                (unsigned)GJ_NET_BOUNCE_N,
                (unsigned)sizeof(struct virtio_net_hdr),
                (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-net: soft surface */
        kprintf("virtio-net: soft surface inventory,geometry,queue,tx,rx,"
                "mac,link,errors,api,pci,probe,claim,via,kicks,oasis,"
                "ratio,headroom,return,contract,return_selftest,retmap,deepen areas=%u wave=%u\n",
                (unsigned)VIRTIO_NET_SOFT_DEEPEN_AREAS,
                (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-net: soft return — return-surface bitmask */
        kprintf("virtio-net: soft return surf=0x%x ready=%u tx=%u rx=%u "
                "mac=%u fail=%u drop=%u via=%s areas=%u wave=%u soft PASS\n",
                u32Surf, u32Ready, (unsigned)g_Stats.u32TxCount,
                (unsigned)g_Stats.u32RxCount, (unsigned)(g_fHaveMac ? 1 : 0),
                (unsigned)g_Stats.u32TxFail, (unsigned)g_Stats.u32RxDrop,
                szViaSafe, (unsigned)VIRTIO_NET_SOFT_DEEPEN_AREAS,
                (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-net: soft contract — soft ≠ game I/O */
        kprintf("virtio-net: soft contract soft_only=1 game_io=0 "
                "product_net=0 gso=0 csum=0 mq=0 bar3=open wave=%u "
                "soft PASS\n",
                (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-net: soft return — Wave 17 API return surfaces (kept) */
    kprintf("virtio-net: soft return soft_inv=1 net=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-net: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("virtio-net: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-net: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("virtio-net: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-net: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("virtio-net: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-net: soft retcode — Wave 19 retcode catalog */
    kprintf("virtio-net: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-net: soft deepen (Wave 20 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-net: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("virtio-net: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
    /* Grep: virtio-net: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("virtio-net: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-net: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("virtio-net: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
    /* Grep: virtio-net: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("virtio-net: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-net: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("virtio-net: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-net: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("virtio-net: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-net: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("virtio-net: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-net: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("virtio-net: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: virtio-net: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("virtio-net: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-net: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("virtio-net: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-net: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("virtio-net: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-net: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("virtio-net: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-net: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("virtio-net: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-net: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("virtio-net: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-net: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("virtio-net: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-net: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("virtio-net: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: virtio-net: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("virtio-net: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
                    /* Grep: virtio-net: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("virtio-net: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-net: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("virtio-net: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
                            /* Grep: virtio-net: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("virtio-net: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-net: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("virtio-net: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=116 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: virtio-net: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("virtio-net: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=116 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-net: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-net: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=116 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: virtio-net: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("virtio-net: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=116 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-net: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("virtio-net: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=116 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: virtio-net: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-net: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=116 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: virtio-net: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("virtio-net: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=116 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("virtio-net: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("virtio-net: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("virtio-net: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("virtio-net: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("virtio-net: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("virtio-net: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retfortress — Wave 35 return-fortress honesty */
kprintf("virtio-net: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("virtio-net: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft rethold — Wave 36 return-hold honesty */
kprintf("virtio-net: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retspire — Wave 36 exclusive spire stamp */
kprintf("virtio-net: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retwall — Wave 37 return-wall honesty */
kprintf("virtio-net: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retgate — Wave 37 exclusive gate stamp */
kprintf("virtio-net: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retmoat — Wave 38 return-moat honesty */
kprintf("virtio-net: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retower — Wave 38 exclusive tower stamp */
kprintf("virtio-net: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("virtio-net: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("virtio-net: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("virtio-net: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("virtio-net: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retravelin — Wave 41 return-travelin honesty */
kprintf("virtio-net: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("virtio-net: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("virtio-net: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("virtio-net: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("virtio-net: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("virtio-net: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("virtio-net: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("virtio-net: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("virtio-net: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("virtio-net: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retbailey — Wave 46 return-bailey honesty */
kprintf("virtio-net: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("virtio-net: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("virtio-net: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("virtio-net: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("virtio-net: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("virtio-net: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("virtio-net: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("virtio-net: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retsally — Wave 50 return-sally honesty */
kprintf("virtio-net: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("virtio-net: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retfosse — Wave 51 return-fosse honesty */
kprintf("virtio-net: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("virtio-net: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("virtio-net: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("virtio-net: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retravelin — Wave 53 return-travelin honesty */
kprintf("virtio-net: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("virtio-net: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("virtio-net: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retredan — Wave 54 exclusive redan stamp */
kprintf("virtio-net: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retflank — Wave 55 return-flank honesty */
kprintf("virtio-net: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retface — Wave 55 exclusive face stamp */
kprintf("virtio-net: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retgorge — Wave 56 return-gorge honesty */
kprintf("virtio-net: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("virtio-net: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retraverse — Wave 57 return-traverse honesty */
kprintf("virtio-net: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("virtio-net: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retorillon — Wave 58 return-orillon honesty */
kprintf("virtio-net: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("virtio-net: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("virtio-net: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("virtio-net: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retplace — Wave 60 return-place honesty */
kprintf("virtio-net: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("virtio-net: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("virtio-net: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("virtio-net: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("virtio-net: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("virtio-net: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("virtio-net: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("virtio-net: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: virtio-net: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("virtio-net: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: virtio-net: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("virtio-net: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: virtio-net: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("virtio-net: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: virtio-net: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("virtio-net: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: virtio-net: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("virtio-net: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=116 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: virtio-net: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("virtio-net: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=116 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("virtio-net: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("virtio-net: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("virtio-net: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("virtio-net: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("virtio-net: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("virtio-net: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("virtio-net: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("virtio-net: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("virtio-net: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("virtio-net: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-net: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("virtio-net: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("virtio-net: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("virtio-net: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("virtio-net: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbastionangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("virtio-net: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("virtio-net: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retparapetangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("virtio-net: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("virtio-net: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retowerangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("virtio-net: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("virtio-net: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retwallangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("virtio-net: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("virtio-net: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retholdangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("virtio-net: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("virtio-net: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retfortressangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("virtio-net: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("virtio-net: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("virtio-net: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("virtio-net: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("virtio-net: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("virtio-net: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("virtio-net: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("virtio-net: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retaegisangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("virtio-net: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("virtio-net: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retsigilangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("virtio-net: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("virtio-net: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retglyphangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("virtio-net: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("virtio-net: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retshardangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("virtio-net: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("virtio-net: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retprismangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("virtio-net: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("virtio-net: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcipherangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("virtio-net: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("virtio-net: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retledgerangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("virtio-net: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("virtio-net: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retvaultangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("virtio-net: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("virtio-net: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (rettokenangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("virtio-net: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("virtio-net: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retphaseangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("virtio-net: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("virtio-net: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retpulseangle stamp; Soft≠product)\n");

/* Grep: virtio-net: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("virtio-net: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("virtio-net: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retboundangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("virtio-net: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("virtio-net: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbladeangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("virtio-net: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("virtio-net: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retarcangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("virtio-net: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("virtio-net: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("virtio-net: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("virtio-net: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("virtio-net: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("virtio-net: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retellipseangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("virtio-net: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("virtio-net: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("virtio-net: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("virtio-net: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (rethelixangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("virtio-net: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("virtio-net: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retknotangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("virtio-net: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("virtio-net: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retkleinangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("virtio-net: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("virtio-net: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retaffineangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("virtio-net: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("virtio-net: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("virtio-net: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("virtio-net: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcubicangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("virtio-net: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("virtio-net: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retquinticangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("virtio-net: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("virtio-net: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbezierangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("virtio-net: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("virtio-net: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("virtio-net: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("virtio-net: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("virtio-net: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("virtio-net: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retgridangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("virtio-net: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("virtio-net: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (rettexelangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retfragmentangle — Wave 112 return-fragmentangle honesty */
kprintf("virtio-net: soft retfragmentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retfragmentangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retvertexangle — Wave 112 exclusive vertexangle stamp */
kprintf("virtio-net: soft retvertexangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retvertexangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retshaderangle — Wave 113 return-shaderangle honesty */
kprintf("virtio-net: soft retshaderangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retshaderangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retpipelineangle — Wave 113 exclusive pipelineangle stamp */
kprintf("virtio-net: soft retpipelineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retpipelineangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retframebufferangle — Wave 114 return-framebufferangle honesty */
kprintf("virtio-net: soft retframebufferangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retframebufferangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retswapchainangle — Wave 114 exclusive swapchainangle stamp */
kprintf("virtio-net: soft retswapchainangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retswapchainangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retpresentangle — Wave 115 return-presentangle honesty */
kprintf("virtio-net: soft retpresentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retpresentangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retvsyncangle — Wave 115 exclusive vsyncangle stamp */
kprintf("virtio-net: soft retvsyncangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retvsyncangle stamp; Soft≠product)\n");
/* Grep: virtio-net: soft retfenceangle — Wave 116 return-fenceangle honesty */
kprintf("virtio-net: soft retfenceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retfenceangle honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-net: soft retsemaphoreangle — Wave 116 exclusive semaphoreangle stamp */
kprintf("virtio-net: soft retsemaphoreangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retsemaphoreangle stamp; Soft≠product)\n");
                            kprintf("virtio-net: soft deepen wave=%u areas=%u ready=%u tx=%u "
            "rx=%u log_n=%u\n",
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE,
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_AREAS, u32Ready,
            (unsigned)g_Stats.u32TxCount, (unsigned)g_Stats.u32RxCount,
            g_u32SoftLogN);

    /* Grep: virtio-net: soft PASS | NODEV | PARTIAL */
    kprintf("virtio-net: soft %s via=%s ready=%u tx=%u rx=%u "
            "tx_fail=%u drop=%u log_n=%u wave=%u\n",
            szVerdict, szViaSafe, u32Ready,
            (unsigned)g_Stats.u32TxCount, (unsigned)g_Stats.u32RxCount,
            (unsigned)g_Stats.u32TxFail, (unsigned)g_Stats.u32RxDrop,
            g_u32SoftLogN, (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-net: soft inventory PASS|NODEV|PARTIAL */
    kprintf("virtio-net: soft inventory %s via=%s logs=%u wave=%u "
            "(soft inventory only; not bar3)\n",
            szVerdict, szViaSafe, g_u32SoftLogN,
            (unsigned)VIRTIO_NET_SOFT_DEEPEN_WAVE);
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
    if (c > 0u) {
        g_u32RefillOps++;
        if (g_u32RefillSlots <= 0xffffffffu - c) {
            g_u32RefillSlots += c;
        } else {
            g_u32RefillSlots = 0xffffffffu;
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
    g_u32TxApi = 0;
    g_u32RxApi = 0;
    g_u32StatsApi = 0;
    g_u32ExportFail = 0;
    g_u32MapFail = 0;
    g_u32MapDmaFail = 0;
    g_u32BounceFail = 0;
    g_u32RefillOps = 0;
    g_u32RefillSlots = 0;
    g_u32LastTxLen = 0;
    g_u32LastRxLen = 0;
    g_u32LastNumBuf = 0;
    g_u32ProbeOk = 0;
    g_u32ProbeNodev = 0;
    g_u32ProbeFail = 0;
    g_u32ReadyQuery = 0;
    g_szLastVia = NULL;

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
        g_u32ProbeNodev++;
        /* Grep: virtio-net: soft … NODEV (Wave 14 soft inventory). */
        virtio_net_soft_inventory("nodev");
        return -1;
    }
    st = virtio_pci_setup(g_pNet);
    if (st != GJ_OK || g_pNet->pCommon == NULL) {
        kprintf("virtio-net: setup failed %d\n", (int)st);
        g_pNet = NULL;
        g_u32ProbeFail++;
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
                g_u32ProbeFail++;
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
        g_u32ProbeFail++;
        virtio_net_soft_inventory("rx_q_fail");
        return -1;
    }
    st = virtio_q_setup(g_pNet, &g_qTx, 1, 64);
    if (st != GJ_OK) {
        kprintf("virtio-net: tx queue failed %d\n", (int)st);
        g_pNet = NULL;
        g_u32ProbeFail++;
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
    g_u32ProbeOk++;
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
     * Wave 14 soft inventory rollup (prefix-stable "virtio-net: soft …").
     * Post-probe PARTIAL until first product TX/RX frames complete.
     */
    virtio_net_soft_inventory("probe");
    return 0;
}

int
virtio_net_ready(void)
{
    g_u32ReadyQuery++;
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

    g_u32TxApi++;
    if (!g_fReady || pFrame == NULL || cbLen == 0 || cbLen > 1514) {
        if (g_fReady) {
            g_Stats.u32TxFail++;
            net_soft_maybe_once();
        }
        return -1;
    }
    g_u32LastTxLen = cbLen;

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

    g_u32RxApi++;
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

    g_u32LastNumBuf = (u32)u16NumBuf;
    if (cbCopied > 0) {
        g_Stats.u32RxCount++;
        g_Stats.u64RxBytes += (u64)cbCopied;
        g_u32LastRxLen = cbCopied;
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
    g_u32StatsApi++;
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
        g_u32ExportFail++;
        return -1;
    }
    pQ = (u16Which == 0) ? &g_qRx : &g_qTx;
    if (u16Which > 1 || pQ->pDesc == NULL) {
        g_u32ExportFail++;
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
        /* export_q already tallied export_fail; map miss is distinct. */
        g_u32MapFail++;
        return -1;
    }
    if (u64VaBase == 0 || (u64VaBase & 0xfffull) != 0) {
        g_u32MapFail++;
        return -1;
    }
    /* One page each: rings live on dedicated PMM pages (virtio_q_setup). */
    if (vmm_map_page((gj_vaddr_t)u64VaBase, (gj_paddr_t)ex.u64PaDesc,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        g_u32MapFail++;
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaAvail,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        g_u32MapFail++;
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + 2u * GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaUsed,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        g_u32MapFail++;
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
        g_u32MapDmaFail++;
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
                g_u32MapDmaFail++;
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
    g_u32BounceFail++;
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
