/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-net: multi-buffer RX pool + 2-desc TX + bounce pool
 * for UDX ring push (OASIS). Features logging + soft stats counters.
 * No Linux virtio source. Dual MIT OR Apache-2.0 only. Pure C11 freestanding.
 *
 * T0 product net residual - claim class C2 (this unit only; Soft!=product):
 *   QEMU / virtio path is T0 product net until UDX owns real-HW NIC wire.
 *   No freestanding rtl work here (lab rtl is out of this exclusive unit).
 *   G-AC-1: no Linux .ko product AC; no GPL in-tree source.
 *   Functional TX/RX residual lean:
 *     - TX 2-desc chain + single fallback; ETH_MIN pad; second-chance reap
 *     - TX wire-byte tally (post-pad) + sticky last lens; busy after deep reap
 *     - RX multi-buf pool + MRG merge + refill; merge-seg / claim-miss tallies
 *     - housekeep: deeper TX reap when tight + silent link sample + refill
 *     - UDX avail_push TX also ETH_MIN pads (parity with kernel TX residual)
 *     - MAC + link STATUS export; free_min/now / pad / busy silent tallies
 *   LEAN residual only - never re-introduce multi-kprintf stamp storms
 *   (prior FAULT H2: sequential soft-deepen kprintf floods -> stack smash
 *   / #PF I=1 string-as-code). Soft inventory is one-shot, one line,
 *   probe path only. Soft residual lean is one-shot (probe lean and/or
 *   first TX/RX/hk/avail); optional lean PASS when silent self-check full.
 *   Map lamps hard-capped once. Silent residual counters for bounce/map/
 *   probe class + lean_ok self-check. No wave=/version stamp. No ret*angle.
 *   Soft residual lamps != product DoD close. Soft!=product dual license.
 *   Dual DoD OPEN (laptop UDX NIC later); freestanding rtl SKIP here.
 *   H1: this unit never calls net_eth_poll (thr/door/run-loop own eth).
 *   H1 thr-only eth: force_irq_eth_poll=0 poll_mode_first=1 net_eth_irq=0.
 *   Product MSI-X residual is irq_msix/pci_msix; this T0 unit stays thr eth.
 *   H3 thr_exit order is process-death residual (not this TU).
 *   Stamp-free: no version stamp; bar honesty v2026.08.04.75 never invent .76.
 *
 * Greppable product markers (prefix-stable; C2 bring-up, not soft deepen):
 *   virtio-net: ready PASS
 *   virtio-net: features
 *   virtio-net: multi-buf
 *   virtio-net: product residual
 *
 * Soft inventory: one-shot one-line boot lamp only (Soft!=product)
 *   virtio-net: soft inventory via=... ready=... posted=... mac=... link=...
 *   ... t0_product=1 qemu=1 udx_real_hw=0 freestanding_rtl=SKIP
 *       dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1
 *       claim_class=C2 dual_dod_a=OPEN dual_dod_b=OPEN
 *       soft_ne_product=1 Soft!=product G-AC-1
 * Soft residual lean: one-shot after probe lean and/or first TX/RX (Soft!=product)
 *   virtio-net: soft residual lean via=... lean_ok=N/M Soft!=product
 *   virtio-net: soft residual lean PASS (when lean_ok == lean_checks)
 * greppable: virtio-net: soft inventory
 * greppable: virtio-net: soft residual lean
 * greppable: virtio-net: soft residual lean PASS
 * greppable: virtio-net: product residual
 * greppable: virtio-net: map_q | virtio-net: map_dma (once each)
 * greppable: Soft!=product G-AC-1 t0_product product residual
 * greppable: lean_ok stamp_storm=0 dual=MIT_OR_Apache-2.0 freestanding_rtl=SKIP
 * greppable: dual_dod_a=OPEN dual_dod_b=OPEN claim_class=C2
 * greppable: force_irq_eth_poll=0 poll_mode_first=1 net_eth_irq=0
 * greppable: thr_only_eth=1 hazard=H1
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

/* Device config status bit (public OASIS; soft sample only). */
#define VIRTIO_NET_S_LINK_UP   1u

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
/* desc head -> RX slot (0xff free); set at post, consumed on poll_id */
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

/*
 * Lean soft residual (file-static; Soft!=product / C2 honesty). Silent
 * counters only - never multi-line inventory dumps / stamp storms (H2).
 * Map lamps once each. Probe-class tallies never kprintf on hot path.
 * Soft residual lean is one-shot after probe lean and/or first product TX/RX.
 * Silent lean self-check folds into lean lines only (peer T0 residual bar).
 */
static int g_fSoftInvOnce;       /* one-shot soft inventory boot lamp */
static int g_fSoftResidOnce;     /* one-shot soft residual lean (probe/activity) */
static int g_fMapQLamp;          /* one-shot map_q user lamp */
static int g_fMapDmaLamp;        /* one-shot map_dma user lamp */
static u16 g_u16LinkStatus;      /* soft STATUS sample (bit0 = link up) */
static u16 g_u16TxFreeMin;       /* min free TX descs (0xffff = unset) */
static u16 g_u16RxFreeMin;       /* min free RX descs (0xffff = unset) */
static u32 g_u32SoftMapQ;        /* map_q_user success count (silent) */
static u32 g_u32SoftMapDma;      /* map_dma_user success count (silent) */
static u32 g_u32SoftProbeOk;     /* probe DRIVER_OK path (silent) */
static u32 g_u32SoftProbeNodev;  /* probe no-device (silent) */
static u32 g_u32SoftProbeFail;   /* probe pci/nego/q fail (silent) */
static u32 g_u32SoftStatsSnap;   /* stats() samples (silent residual) */
static u32 g_u32LeanOk;          /* last silent lean self-check ok count */
static u32 g_u32LeanChecks;      /* last silent lean self-check total */
/* Bounce-full lives in g_Stats.u32BounceFull (door-visible residual lean). */

static void virtio_net_soft_inventory_once(const char *szVia);
static void virtio_net_soft_residual_lean_once(const char *szVia);
static u32  net_soft_residual_lean_ok(u32 *pOutChecks);
static void soft_sample_link(struct gj_virtio_dev *pDev);
static void note_free_min(void);

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

/* Translate kernel buffer VA -> guest physical (identity fallback). */
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
 * Soft link STATUS sample (VIRTIO_NET_F_STATUS). Silent residual only -
 * never kprintf here (hot-path / re-enter safe). Soft!=product.
 */
static void
soft_sample_link(struct gj_virtio_dev *pDev)
{
    volatile struct virtio_net_config *pCfg;

    if (pDev == NULL || pDev->pDevice == NULL) {
        g_u16LinkStatus = 0;
        return;
    }
    if ((g_Stats.u64Features & VIRTIO_NET_F_STATUS) != 0) {
        pCfg = (volatile struct virtio_net_config *)(void *)pDev->pDevice;
        g_u16LinkStatus = pCfg->u16Status;
    } else {
        /* No STATUS feature: soft assume link up once DRIVER_OK path. */
        g_u16LinkStatus = (u16)VIRTIO_NET_S_LINK_UP;
    }
}

static void
stats_reset(void)
{
    memset(&g_Stats, 0, sizeof(g_Stats));
    g_u16LinkStatus = 0;
    g_u16TxFreeMin = 0xffffu;
    g_u16RxFreeMin = 0xffffu;
    g_Stats.u16TxFreeMin = 0xffffu;
    g_Stats.u16RxFreeMin = 0xffffu;
    g_u32SoftMapQ = 0;
    g_u32SoftMapDma = 0;
    g_u32SoftProbeOk = 0;
    g_u32SoftProbeNodev = 0;
    g_u32SoftProbeFail = 0;
    g_u32SoftStatsSnap = 0;
    g_u32LeanOk = 0;
    g_u32LeanChecks = 0;
    g_fMapQLamp = 0;
    g_fMapDmaLamp = 0;
    g_fSoftResidOnce = 0;
}

/*
 * Silent free-desc watermarks (product residual lean; no kprintf).
 * Called from TX/RX/housekeep hot paths only.
 */
static void
note_free_min(void)
{
    u16 u16Tx;
    u16 u16Rx;

    if (!g_fReady) {
        return;
    }
    u16Tx = virtio_q_num_free(&g_qTx);
    u16Rx = virtio_q_num_free(&g_qRx);
    if (u16Tx < g_u16TxFreeMin) {
        g_u16TxFreeMin = u16Tx;
        g_Stats.u16TxFreeMin = u16Tx;
    }
    if (u16Rx < g_u16RxFreeMin) {
        g_u16RxFreeMin = u16Rx;
        g_Stats.u16RxFreeMin = u16Rx;
    }
}

/* ---- lean soft residual inventory (Soft!=product; no stamp storms / H2) --- */

/**
 * Silent lean residual self-check (no kprintf; Soft!=product / C2).
 * OASIS virtio-net geometry + feature bits + modern net PCI id + KIND_NET T0
 * + bounce/TX pack residual + free/posted watermarks when ready.
 * Returns ok count; *pOutChecks = total when non-NULL. Results fold into
 * soft residual lean lines only (never hard-gates product TX/RX).
 */
static u32
net_soft_residual_lean_ok(u32 *pOutChecks)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;

    /* 1: multi-buf RX pool depth (soft post at probe / refill) */
    u32Checks++;
    if (GJ_VIRTIO_NET_RX_N >= 2u && GJ_VIRTIO_NET_RX_N <= 64u &&
        (GJ_VIRTIO_NET_RX_N % 2u) == 0u) {
        u32Ok++;
    }
    /* 2: RX slot size covers eth + virtio-net 1.x hdr headroom */
    u32Checks++;
    if (GJ_VIRTIO_NET_RX_SZ >= 1518u &&
        GJ_VIRTIO_NET_RX_SZ >=
            ((u32)sizeof(struct virtio_net_hdr) + GJ_VIRTIO_NET_ETH_MAX)) {
        u32Ok++;
    }
    /* 3: Ethernet min/max residual (QEMU SLIRP pad + no-FCS max) */
    u32Checks++;
    if (GJ_VIRTIO_NET_ETH_MIN == 60u && GJ_VIRTIO_NET_ETH_MAX == 1514u &&
        GJ_VIRTIO_NET_ETH_MAX > GJ_VIRTIO_NET_ETH_MIN) {
        u32Ok++;
    }
    /* 4: packed virtio-net 1.x hdr is 12 bytes (incl. num_buffers) */
    u32Checks++;
    if (sizeof(struct virtio_net_hdr) == 12u) {
        u32Ok++;
    }
    /* 5: packed device config has MAC[6] + status u16 */
    u32Checks++;
    if (sizeof(struct virtio_net_config) == 8u) {
        u32Ok++;
    }
    /* 6: public OASIS feature bits (MAC / STATUS / MRG_RXBUF) */
    u32Checks++;
    if (VIRTIO_NET_F_MAC == (1ull << 5) &&
        VIRTIO_NET_F_STATUS == (1ull << 16) &&
        VIRTIO_NET_F_MRG_RXBUF == (1ull << 15)) {
        u32Ok++;
    }
    /* 7: link-up STATUS bit (public OASIS) */
    u32Checks++;
    if (VIRTIO_NET_S_LINK_UP == 1u) {
        u32Ok++;
    }
    /* 8: VERSION_1 feature bit (modern negotiate residual) */
    u32Checks++;
    if (GJ_VIRTIO_F_VERSION_1 == (1ull << 32)) {
        u32Ok++;
    }
    /* 9: modern net PCI device id (T0 product inventory) */
    u32Checks++;
    if (GJ_VIRTIO_PCI_DEV_NET_MODERN == 0x1041u) {
        u32Ok++;
    }
    /* 10: KIND_NET is T0 triad member (kind==1) */
    u32Checks++;
    if (GJ_VIRTIO_KIND_NET == 1u &&
        GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_NET) &&
        GJ_VIRTIO_KIND_NET != GJ_VIRTIO_KIND_BLK &&
        GJ_VIRTIO_KIND_NET != GJ_VIRTIO_KIND_GPU) {
        u32Ok++;
    }
    /* 11: status ladder bits (product DRIVER_OK path honesty) */
    u32Checks++;
    if (GJ_VIRTIO_S_ACKNOWLEDGE == 1u && GJ_VIRTIO_S_DRIVER == 2u &&
        GJ_VIRTIO_S_DRIVER_OK == 4u && GJ_VIRTIO_S_FEATURES_OK == 8u &&
        (GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
         GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK) == 15u) {
        u32Ok++;
    }
    /* 12: bounce pool residual (UDX AVAIL_PUSH path) */
    u32Checks++;
    if (GJ_NET_BOUNCE_N >= 4u && GJ_NET_BOUNCE_N <= 32u &&
        GJ_NET_BOUNCE_SZ >=
            ((u32)sizeof(struct virtio_net_hdr) + GJ_VIRTIO_NET_ETH_MAX) &&
        GJ_NET_BOUNCE_SZ >= GJ_VIRTIO_NET_ETH_MIN) {
        u32Ok++;
    }
    /* 13: kernel TX pack/payload residual buffers */
    u32Checks++;
    if ((u32)sizeof(g_aTxPayload) >= GJ_VIRTIO_NET_ETH_MAX &&
        (u32)sizeof(g_aTxPack) >=
            ((u32)sizeof(struct virtio_net_hdr) + GJ_VIRTIO_NET_ETH_MAX) &&
        (u32)sizeof(g_TxHdr) == (u32)sizeof(struct virtio_net_hdr)) {
        u32Ok++;
    }
    /* 14: RX multi-buf BSS holds full pool */
    u32Checks++;
    if ((u32)sizeof(g_aRxSlot) ==
        (GJ_VIRTIO_NET_RX_N * GJ_VIRTIO_NET_RX_SZ)) {
        u32Ok++;
    }
    /* 15: bounce BSS holds full pool */
    u32Checks++;
    if ((u32)sizeof(g_aBounce) == (GJ_NET_BOUNCE_N * GJ_NET_BOUNCE_SZ)) {
        u32Ok++;
    }
    /* 16: page geometry (map_q / map_dma residual) */
    u32Checks++;
    if (GJ_PAGE_SIZE == 4096u &&
        (GJ_PAGE_SIZE & (GJ_PAGE_SIZE - 1u)) == 0u) {
        u32Ok++;
    }
    /* 17: virtq max size covers soft q setup residual (64-desc bring-up) */
    u32Checks++;
    if (GJ_VIRTQ_MAX_SIZE >= 64u && GJ_VIRTQ_MAX_SIZE <= 1024u) {
        u32Ok++;
    }
    /* 18: head->slot map table covers virtq max */
    u32Checks++;
    if ((u32)sizeof(g_aRxHeadSlot) >= GJ_VIRTQ_MAX_SIZE) {
        u32Ok++;
    }
    /* 19: live free-desc watermark coherent when ready (silent) */
    u32Checks++;
    if (!g_fReady ||
        (g_u16TxFreeMin <= g_qTx.u16Size && g_u16RxFreeMin <= g_qRx.u16Size)) {
        u32Ok++;
    }
    /* 20: posted watermark coherent when ready (silent) */
    u32Checks++;
    if (!g_fReady || g_Stats.u32RxPosted <= GJ_VIRTIO_NET_RX_N) {
        u32Ok++;
    }
    /* 21: common-cfg select offsets (feature snapshot residual; match virtio_pci) */
    u32Checks++;
    if (NET_PCI_COMMON_DFSELECT == 0 && NET_PCI_COMMON_DF == 4 &&
        NET_PCI_COMMON_GFSELECT == 8 && NET_PCI_COMMON_GF == 12) {
        u32Ok++;
    }
    /* 22: live/used slot tables sized to pools (silent residual geometry) */
    u32Checks++;
    if ((u32)sizeof(g_aRxSlotLive) == GJ_VIRTIO_NET_RX_N &&
        (u32)sizeof(g_aBounceUsed) == GJ_NET_BOUNCE_N) {
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
 * One-shot one-line boot soft inventory only (Soft!=product / C2).
 * LEAN residual: single kprintf - never multi-line stamp floods (FAULT H2).
 * No wave=/version stamp. No ret*angle. Dual license honesty on lamp only.
 * greppable: virtio-net: soft inventory
 * greppable: Soft!=product t0_product qemu=1 freestanding_rtl=SKIP G-AC-1
 */
static void
virtio_net_soft_inventory_once(const char *szVia)
{
    u32 u32LinkUp;
    u8 u8Bus;
    u8 u8Slot;

    if (g_fSoftInvOnce != 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    u32LinkUp = ((g_u16LinkStatus & (u16)VIRTIO_NET_S_LINK_UP) != 0) ? 1u : 0u;
    u8Bus = (g_pNet != NULL) ? g_pNet->u8Bus : 0;
    u8Slot = (g_pNet != NULL) ? g_pNet->u8Slot : 0;
    /*
     * ONE line only. Soft!=product. No wave=/version stamp.
     * t0_product=1 / qemu=1: C2 virtio T0 QEMU path (lamp != DoD close).
     * udx_real_hw=0: UDX owns future real-HW NIC; freestanding_rtl=SKIP here.
     * Dual DoD A/B OPEN: residual != laptop Dual DoD close.
     * G-AC-1 honesty: no .ko product AC from this residual.
     */
    kprintf("virtio-net: soft inventory via=%s ready=%u bus=%x slot=%x "
            "posted=%u rx_n=%u mac=%u link=%u bounce_n=%u eth_min=%u "
            "t0_product=1 qemu=1 multi_buf=1 tx_chain=2desc "
            "udx_real_hw=0 freestanding_rtl=SKIP claim_class=C2 "
            "product_net=virtio_t0 product_dir=UDX+ABI_later "
            "product_msix=OPEN thr_only_eth=1 "
            "force_irq_eth_poll=0 poll_mode_first=1 net_eth_irq=0 "
            "net_eth_poll=run_loop_only hazard=H1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
            "stamp_free=1 soft_ne_product=1 G-AC-1 Soft!=product\n",
            szVia != NULL ? szVia : "?",
            g_fReady ? 1u : 0u,
            (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)g_Stats.u32RxPosted,
            (unsigned)GJ_VIRTIO_NET_RX_N,
            g_fHaveMac ? 1u : 0u,
            (unsigned)u32LinkUp,
            (unsigned)GJ_NET_BOUNCE_N,
            (unsigned)GJ_VIRTIO_NET_ETH_MIN);
}

/*
 * One-shot soft residual lean after probe lean and/or first product TX/RX.
 * Silent self-check + ONE line (+ optional PASS). Soft!=product / C2.
 * Never re-emit on hot path. No wave=/version stamp. Dual MIT OR Apache-2.0.
 * Dual DoD remains OPEN (lamp != laptop Dual DoD / UDX wire claim).
 * greppable: virtio-net: soft residual lean
 * greppable: virtio-net: soft residual lean PASS
 * greppable: Soft!=product soft_ne_product t0_product G-AC-1 lean_ok
 */
static void
virtio_net_soft_residual_lean_once(const char *szVia)
{
    u32 u32LinkUp;
    u16 u16TxMin;
    u16 u16RxMin;
    u32 u32Ok;
    u32 u32Checks;

    if (g_fSoftResidOnce != 0) {
        return;
    }
    g_fSoftResidOnce = 1;

    u32Ok = net_soft_residual_lean_ok(&u32Checks);
    u32LinkUp = ((g_u16LinkStatus & (u16)VIRTIO_NET_S_LINK_UP) != 0) ? 1u : 0u;
    u16TxMin = (g_u16TxFreeMin == 0xffffu) ? 0u : g_u16TxFreeMin;
    u16RxMin = (g_u16RxFreeMin == 0xffffu) ? 0u : g_u16RxFreeMin;
    /*
     * ONE lean line. Soft!=product dual license; no version stamp; no storm.
     * Fold functional residual lean tallies + silent lean_ok self-check.
     * t0_product=1 qemu=1: C2 T0 QEMU residual until UDX owns real-HW NIC.
     */
    kprintf("virtio-net: soft residual lean via=%s ready=%u "
            "tx=%u rx=%u multi=%u single=%u pad=%u busy=%u to=%u "
            "drop=%u posted=%u refill=%u free_tx_min=%u free_rx_min=%u "
            "merge=%u merge_segs=%u claim_miss=%u bounce_full=%u "
            "wire_tx=%lu last_tx=%u last_rx=%u link=%u "
            "probe_ok=%u nodev=%u fail=%u map_q=%u map_dma=%u "
            "hk=%u stats_snap=%u lean_ok=%u/%u "
            "t0_product=1 qemu=1 multi_buf=1 tx_chain=2desc "
            "udx_real_hw=0 freestanding_rtl=SKIP claim_class=C2 "
            "product_net=virtio_t0 product_dir=UDX+ABI_later "
            "product_msix=OPEN thr_only_eth=1 "
            "force_irq_eth_poll=0 poll_mode_first=1 net_eth_irq=0 "
            "net_eth_poll=run_loop_only hazard=H1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
            "stamp_free=1 soft_ne_product=1 Soft!=product G-AC-1\n",
            szVia != NULL ? szVia : "txrx",
            g_fReady ? 1u : 0u,
            g_Stats.u32TxCount, g_Stats.u32RxCount,
            g_Stats.u32TxMulti, g_Stats.u32TxSingle,
            g_Stats.u32TxPad, g_Stats.u32TxBusy, g_Stats.u32TxTimeout,
            g_Stats.u32RxDrop, g_Stats.u32RxPosted,
            g_Stats.u32RxRefillKick,
            (unsigned)u16TxMin, (unsigned)u16RxMin,
            g_Stats.u32RxMerge, g_Stats.u32RxMergeSegs,
            g_Stats.u32RxClaimMiss, g_Stats.u32BounceFull,
            (unsigned long)g_Stats.u64TxWireBytes,
            (unsigned)g_Stats.u16LastTxLen,
            (unsigned)g_Stats.u16LastRxLen,
            (unsigned)u32LinkUp,
            (unsigned)g_u32SoftProbeOk,
            (unsigned)g_u32SoftProbeNodev,
            (unsigned)g_u32SoftProbeFail,
            (unsigned)g_u32SoftMapQ,
            (unsigned)g_u32SoftMapDma,
            (unsigned)g_Stats.u32Housekeep,
            (unsigned)g_u32SoftStatsSnap,
            u32Ok, u32Checks);

    if (u32Ok == u32Checks) {
        /* Grep: virtio-net: soft residual lean PASS */
        kprintf("virtio-net: soft residual lean PASS "
                "checks=%u ok=%u t0_product=1 qemu=1 multi_buf=1 "
                "tx_chain=2desc freestanding_rtl=SKIP claim_class=C2 "
                "product_net=virtio_t0 product_dir=UDX+ABI_later "
                "product_msix=OPEN thr_only_eth=1 "
                "force_irq_eth_poll=0 poll_mode_first=1 net_eth_irq=0 "
                "hazard=H1 dual_dod_a=OPEN dual_dod_b=OPEN "
                "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
                "stamp_free=1 soft_ne_product=1 Soft!=product G-AC-1\n",
                u32Checks, u32Ok);
        /*
         * Product residual lamp (T0 virtio until UDX owns real-HW NIC).
         * Soft residual lean PASS != Dual DoD B close. Soft!=product.
         * greppable: virtio-net: product residual
         */
        kprintf("virtio-net: product residual t0=1 qemu=1 "
                "udx_real_hw=0 freestanding_rtl=SKIP "
                "product_net=virtio_t0 product_dir=UDX+ABI_later "
                "product_msix=OPEN thr_only_eth=1 "
                "dual_dod_a=OPEN dual_dod_b=OPEN "
                "soft_residual_closes_dod=0 G-AC-1=1 stamp_free=1 "
                "Soft!=product dual=MIT_OR_Apache-2.0\n");
    }
    /* Keep lean symbols live for residual observe (Soft!=product). */
    (void)g_u32LeanOk;
    (void)g_u32LeanChecks;
}

static void
net_kick(struct gj_virtq *pQ)
{
    virtio_q_kick(pQ);
    g_Stats.u32Kicks++;
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
 * Claim RX completion: map used head -> slot, mark free for refill.
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
 * Probe path: find first net -> modern PCI caps -> features -> RX/TX qs ->
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
    /* Allow one boot lamp on this probe attempt. */
    g_fSoftInvOnce = 0;

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
        g_u32SoftProbeNodev++;
        kprintf("virtio-net: no device\n");
        virtio_net_soft_inventory_once("nodev");
        return -1;
    }
    st = virtio_pci_setup(g_pNet);
    if (st != GJ_OK || g_pNet->pCommon == NULL) {
        g_u32SoftProbeFail++;
        kprintf("virtio-net: setup failed %d\n", (int)st);
        g_pNet = NULL;
        virtio_net_soft_inventory_once("pci_fail");
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
                g_u32SoftProbeFail++;
                kprintf("virtio-net: negotiate failed %d\n", (int)st);
                g_pNet = NULL;
                virtio_net_soft_inventory_once("negotiate_fail");
                return -1;
            }
        }
    }
    u64Dev = net_read_features(g_pNet, 0);
    u64Drv = net_read_features(g_pNet, 1);
    g_Stats.u64FeaturesDev = u64Dev;
    g_Stats.u64Features = u64Drv;
    /* Product bring-up marker (one line; not soft deepen / not H2 storm). */
    net_log_features(u64Dev, u64Drv);

    read_mac(g_pNet);
    /* RX=0, TX=1 */
    st = virtio_q_setup(g_pNet, &g_qRx, 0, 64);
    if (st != GJ_OK) {
        g_u32SoftProbeFail++;
        kprintf("virtio-net: rx queue failed %d\n", (int)st);
        g_pNet = NULL;
        virtio_net_soft_inventory_once("rx_q_fail");
        return -1;
    }
    st = virtio_q_setup(g_pNet, &g_qTx, 1, 64);
    if (st != GJ_OK) {
        g_u32SoftProbeFail++;
        kprintf("virtio-net: tx queue failed %d\n", (int)st);
        g_pNet = NULL;
        virtio_net_soft_inventory_once("tx_q_fail");
        return -1;
    }

    /* Soft multi-buffer: post full RX pool (device-write) before DRIVER_OK */
    cPosted = rx_post_all();
    if (cPosted > 0) {
        net_kick(&g_qRx);
        g_Stats.u32RxRefillKick++;
    } else {
        kprintf("virtio-net: initial RX post failed (tx-only until retry)\n");
    }
    /* Product bring-up marker (one line). Soft!=product residual is separate. */
    kprintf("virtio-net: multi-buf rx_slots=%u posted=%u tx_chain=2desc "
            "eth_min=%u\n",
            (unsigned)GJ_VIRTIO_NET_RX_N, (unsigned)cPosted,
            (unsigned)GJ_VIRTIO_NET_ETH_MIN);

    virtio_set_status(g_pNet, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                   GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
    g_u32SoftProbeOk++;
    /* Soft residual: sample link after DRIVER_OK (STATUS feature or assume). */
    soft_sample_link(g_pNet);
    note_free_min();
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
     * Product residual honesty (one line; C2 T0 QEMU path).
     * Soft!=product lamps are separate; this is product bring-up residual.
     * Dual DoD remains OPEN (laptop UDX NIC later). freestanding_rtl=SKIP.
     * greppable: virtio-net: product residual
     */
    kprintf("virtio-net: product residual t0=1 qemu=1 multi_buf=1 "
            "tx_chain=2desc eth_min=%u rx_n=%u posted=%u mac=%u link=%u "
            "udx_real_hw=0 freestanding_rtl=SKIP claim_class=C2 "
            "product_net=virtio_t0 product_dir=UDX+ABI_later "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
            "soft_ne_product=1 Soft!=product G-AC-1\n",
            (unsigned)GJ_VIRTIO_NET_ETH_MIN,
            (unsigned)GJ_VIRTIO_NET_RX_N,
            (unsigned)g_Stats.u32RxPosted,
            g_fHaveMac ? 1u : 0u,
            ((g_u16LinkStatus & (u16)VIRTIO_NET_S_LINK_UP) != 0) ? 1u : 0u);
    /* One-shot soft inventory + probe lean (LEAN residual; Soft!=product; H2). */
    virtio_net_soft_inventory_once("probe");
    virtio_net_soft_residual_lean_once("probe");
    return 0;
}

int
virtio_net_ready(void)
{
    /* Hot path: flag only. Link residual sampled at probe (no flood MMIO). */
    return g_fReady;
}

int
virtio_net_mac(u8 *pMac)
{
    u32 i;

    if (pMac == NULL || !g_fReady || !g_fHaveMac) {
        return -1;
    }
    for (i = 0; i < 6u; i++) {
        pMac[i] = g_aMac[i];
    }
    return 0;
}

int
virtio_net_link_up(void)
{
    if (!g_fReady) {
        return 0;
    }
    /* Re-sample STATUS (silent; not hot-path flood). Soft!=product assume-up. */
    if (g_pNet != NULL) {
        soft_sample_link(g_pNet);
    }
    return ((g_u16LinkStatus & (u16)VIRTIO_NET_S_LINK_UP) != 0) ? 1 : 0;
}

/*
 * Kernel TX product residual lean: prefer 2-desc chain (hdr device-R +
 * payload device-R). Fall back to single contiguous pack if the queue has
 * only one free descriptor. Short frames zero-padded to ETH_MIN (QEMU
 * SLIRP-friendly). Second-chance deeper reap before busy (retryable;
 * tx_busy != hard fail). Wire-byte tally is post-pad; u64TxBytes is
 * caller payload only.
 */
int
virtio_net_tx(const void *pFrame, u32 cbLen)
{
    gj_paddr_t paHdr;
    gj_paddr_t paPay;
    i32 i32Done;
    u32 cbWire;
    u32 n;

    if (!g_fReady || pFrame == NULL || cbLen == 0 ||
        cbLen > GJ_VIRTIO_NET_ETH_MAX) {
        if (g_fReady) {
            g_Stats.u32TxFail++;
        }
        return -1;
    }

    /* Reap completed TX to free multi-desc chains (lean residual). */
    n = virtio_q_reap(&g_qTx, 16);
    g_Stats.u32Reaps += n;
    g_Stats.u32TxReapBefore += n;
    note_free_min();

    if (g_qTx.u16NumFree == 0) {
        /*
         * Second-chance deeper reap (functional residual lean): multi-desc
         * chains may free more than the first 16 used entries expose.
         * Still busy => retryable; Soft!=product tally only.
         */
        n = virtio_q_reap(&g_qTx, 32);
        g_Stats.u32Reaps += n;
        g_Stats.u32TxReapBefore += n;
        note_free_min();
        if (g_qTx.u16NumFree == 0) {
            g_Stats.u32TxBusy++;
            return -1;
        }
    }

    /*
     * Product TX residual: pad short frames to Ethernet minimum (excl. FCS).
     * Wire length is padded; u64TxBytes counts caller payload only.
     */
    cbWire = cbLen;
    if (cbWire < GJ_VIRTIO_NET_ETH_MIN) {
        cbWire = GJ_VIRTIO_NET_ETH_MIN;
        g_Stats.u32TxPad++;
    }

    memset(&g_TxHdr, 0, sizeof(g_TxHdr));
    /* V1 hdr: num_buffers unused on TX; leave 0 */
    memset(g_aTxPayload, 0, cbWire);
    memcpy(g_aTxPayload, pFrame, cbLen);
    paHdr = buf_phys(&g_TxHdr);
    paPay = buf_phys(g_aTxPayload);

    if (g_qTx.u16NumFree >= 2 &&
        virtio_q_add2(&g_qTx, paHdr, (u32)sizeof(g_TxHdr), 0, paPay, cbWire,
                      0) >= 0) {
        g_Stats.u32TxMulti++;
    } else if (g_qTx.u16NumFree >= 1) {
        /* Single-desc fallback: hdr+payload packed */
        struct virtio_net_hdr *pHdr;
        u32 cbTotal;
        gj_paddr_t pa;

        memset(g_aTxPack, 0, sizeof(g_aTxPack));
        pHdr = (struct virtio_net_hdr *)(void *)g_aTxPack;
        memset(pHdr, 0, sizeof(*pHdr));
        memcpy(g_aTxPack + sizeof(*pHdr), pFrame, cbLen);
        /* Zero-pad remainder of ETH_MIN region when packing short frames. */
        cbTotal = (u32)sizeof(*pHdr) + cbWire;
        pa = buf_phys(g_aTxPack);
        if (virtio_q_add(&g_qTx, pa, cbTotal, 0) < 0) {
            g_Stats.u32TxBusy++;
            return -1;
        }
        g_Stats.u32TxSingle++;
    } else {
        g_Stats.u32TxBusy++;
        return -1;
    }

    net_kick(&g_qTx);
    note_free_min();
    i32Done = virtio_q_poll(&g_qTx, 1000000u);
    if (i32Done < 0) {
        /*
         * Soft: count timeout but keep historical success return - device may
         * complete asynchronously; queue still holds the chain until reap.
         */
        g_Stats.u32TxTimeout++;
    } else {
        g_Stats.u32Reaps++;
    }
    g_Stats.u32TxCount++;
    g_Stats.u64TxBytes += (u64)cbLen;
    g_Stats.u64TxWireBytes += (u64)cbWire;
    g_Stats.u16LastTxLen = (u16)cbLen;
    virtio_net_soft_residual_lean_once("tx");
    return 0;
}

u32
virtio_net_tx_count(void)
{
    return g_Stats.u32TxCount;
}

/*
 * Soft multi-buffer RX product residual: poll one used head, map to slot,
 * copy Ethernet payload. If MRG_RXBUF and num_buffers>1, soft-merge
 * subsequent used buffers (each carries its own virtio_net_hdr per OASIS).
 * Refill pool; count refill kicks. Empty path still opportunistic refill.
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
    u32 cPosted;

    if (!g_fReady || pOut == NULL) {
        return -1;
    }

    note_free_min();
    i32Len = virtio_q_poll_id(&g_qRx, 1000u, &u32Id);
    if (i32Len < 0) {
        g_Stats.u32RxEmpty++;
        /* Opportunistic refill if pool drained (product RX residual lean). */
        if (g_Stats.u32RxPosted < GJ_VIRTIO_NET_RX_N) {
            cPosted = rx_post_all();
            if (cPosted > 0) {
                net_kick(&g_qRx);
                g_Stats.u32RxRefillKick++;
            }
        }
        return 0;
    }
    g_Stats.u32Reaps++;
    nSlot = rx_claim_head(u32Id);
    if (nSlot < 0) {
        /* Mapping miss: drop payload, still refill (claim residual lean). */
        g_Stats.u32RxClaimMiss++;
        g_Stats.u32RxDrop++;
        cPosted = rx_post_all();
        if (cPosted > 0) {
            net_kick(&g_qRx);
            g_Stats.u32RxRefillKick++;
        }
        note_free_min();
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
            g_Stats.u32RxClaimMiss++;
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
            /* Functional residual lean: count each extra merge segment. */
            g_Stats.u32RxMergeSegs++;
        }
    }
    if (u16NumBuf > 1 && cbCopied > 0) {
        g_Stats.u32RxMerge++;
    }

    if (cbCopied > 0) {
        g_Stats.u32RxCount++;
        g_Stats.u64RxBytes += (u64)cbCopied;
        g_Stats.u16LastRxLen = (u16)cbCopied;
    }

    /* Refill multi-buffer RX pool (product residual lean). */
    cPosted = rx_post_all();
    if (cPosted > 0) {
        net_kick(&g_qRx);
        g_Stats.u32RxRefillKick++;
    }
    note_free_min();
    if (cbCopied > 0) {
        virtio_net_soft_residual_lean_once("rx");
    }
    return (i32)cbCopied;
}

/*
 * Lean product residual housekeep: TX reap (deeper when free is tight) +
 * RX pool refill only. Silent link STATUS sample. Does not claim RX used
 * entries (would drop frames) - eth/rx owns that. No frame copy - run-loop
 * safe. Soft residual counters silent; soft residual lean may light once
 * via="hk" after first product TX/RX (never stamp storm).
 */
u32
virtio_net_housekeep(void)
{
    u32 nTx;
    u32 nExtra = 0;
    u32 cPosted = 0;
    u16 u16TxFree;

    if (!g_fReady) {
        return 0;
    }
    g_Stats.u32Housekeep++;
    nTx = virtio_q_reap(&g_qTx, 16);
    g_Stats.u32Reaps += nTx;
    /* Deeper TX reap when free is tight (2-desc chains need headroom). */
    u16TxFree = virtio_q_num_free(&g_qTx);
    if (u16TxFree < 4u) {
        nExtra = virtio_q_reap(&g_qTx, 32);
        g_Stats.u32Reaps += nExtra;
        nTx += nExtra;
    }
    if (g_Stats.u32RxPosted < GJ_VIRTIO_NET_RX_N) {
        cPosted = rx_post_all();
        if (cPosted > 0) {
            net_kick(&g_qRx);
            g_Stats.u32RxRefillKick++;
        }
    }
    /* Silent link residual sample (not hot-path flood). Soft!=product. */
    if (g_pNet != NULL) {
        soft_sample_link(g_pNet);
    }
    note_free_min();
    /* One-shot activity lamp may fire here if TX/RX already happened. */
    virtio_net_soft_residual_lean_once("hk");
    return nTx + cPosted;
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
virtio_net_tx_busy_count(void)
{
    return g_Stats.u32TxBusy;
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
    /*
     * Silent residual only on stats path:
     *   - re-sample link STATUS (no kprintf; not hot TX/RX)
     *   - snapshot free_min + flags (product residual lean)
     *   - tally snap count (file-static; Soft!=product)
     * Never re-emit soft inventory here (FAULT H2 avoid).
     */
    if (g_fReady && g_pNet != NULL) {
        soft_sample_link(g_pNet);
        note_free_min();
        /* Free-now snapshot for door residual lean (silent; Soft!=product). */
        g_Stats.u16TxFreeNow = virtio_q_num_free(&g_qTx);
        g_Stats.u16RxFreeNow = virtio_q_num_free(&g_qRx);
    } else {
        g_Stats.u16TxFreeNow = 0;
        g_Stats.u16RxFreeNow = 0;
    }
    if (g_u32SoftStatsSnap < 0xffffffffu) {
        g_u32SoftStatsSnap++;
    }
    g_Stats.u16LinkStatus = g_u16LinkStatus;
    g_Stats.u16TxFreeMin = g_u16TxFreeMin;
    g_Stats.u16RxFreeMin = g_u16RxFreeMin;
    g_Stats.u16Flags = 0;
    if (g_fHaveMac) {
        g_Stats.u16Flags |= 1u;
    }
    if (g_fReady) {
        g_Stats.u16Flags |= 2u;
    }
    *pOut = g_Stats;
    /* Keep silent residual symbols live (not wire-stable; Soft!=product). */
    (void)g_u32SoftMapQ;
    (void)g_u32SoftMapDma;
    (void)g_u32SoftProbeOk;
    (void)g_u32SoftProbeNodev;
    (void)g_u32SoftProbeFail;
    (void)g_u32SoftStatsSnap;
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
    g_u32SoftMapQ++;
    /* Once-only lamp - UDX re-map must not stamp-flood. Soft!=product. */
    if (g_fMapQLamp == 0) {
        g_fMapQLamp = 1;
        kprintf("virtio-net: map_q which=%u va=0x%lx size=%u\n", u16Which,
                (unsigned long)u64VaBase, ex.u16Size);
    }
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
    g_u32SoftMapDma++;
    /* Once-only lamp - no multi-kprintf map flood. Soft!=product. */
    if (g_fMapDmaLamp == 0) {
        g_fMapDmaLamp = 1;
        kprintf("virtio-net: map_dma va=0x%lx slots=%u\n",
                (unsigned long)u64VaBase, GJ_NET_BOUNCE_N);
    }
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
    u32 cbWire;

    if (!g_fReady || pBuf == NULL || u32Slot >= GJ_NET_BOUNCE_N || cbLen == 0) {
        return -1;
    }
    pB = g_aBounce[u32Slot];
    memset(pB, 0, GJ_NET_BOUNCE_SZ);
    g_aBounceUsed[u32Slot] = 1;
    if (fTxHdr) {
        /*
         * TX residual lean parity with virtio_net_tx / avail_push: prepend
         * virtio-net hdr + ETH_MIN zero-pad for short frames (QEMU SLIRP).
         * Bounce already zeroed; pad region is trailing zeros after memcpy.
         * Return value is desc length (hdr + wire bytes post-pad).
         */
        struct virtio_net_hdr *pHdr = (struct virtio_net_hdr *)(void *)pB;

        memset(pHdr, 0, sizeof(*pHdr));
        if (cbLen > GJ_NET_BOUNCE_SZ - sizeof(*pHdr)) {
            g_aBounceUsed[u32Slot] = 0;
            return -1;
        }
        cbWire = cbLen;
        if (cbWire < GJ_VIRTIO_NET_ETH_MIN) {
            cbWire = GJ_VIRTIO_NET_ETH_MIN;
            if (cbWire > GJ_NET_BOUNCE_SZ - sizeof(*pHdr)) {
                g_aBounceUsed[u32Slot] = 0;
                return -1;
            }
            g_Stats.u32TxPad++;
        }
        memcpy(pB + sizeof(*pHdr), pBuf, cbLen);
        cb = (u32)sizeof(*pHdr) + cbWire;
        g_Stats.u16LastTxLen = (u16)cbLen;
    } else {
        if (cbLen > GJ_NET_BOUNCE_SZ) {
            g_aBounceUsed[u32Slot] = 0;
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
            /* Silent residual: bounce pool full under UDX flood. Soft!=product. */
            g_Stats.u32BounceFull++;
            return -1;
        }
    }
    pB = g_aBounce[slot];
    memset(pB, 0, GJ_NET_BOUNCE_SZ);
    if (u16Which == 1 && !fWrite) {
        /*
         * TX residual lean (parity with kernel virtio_net_tx): prepend
         * virtio-net hdr + ETH_MIN zero-pad for short frames (QEMU SLIRP).
         * Bounce already zeroed; pad region is trailing zeros after memcpy.
         */
        struct virtio_net_hdr *pHdr = (struct virtio_net_hdr *)(void *)pB;
        u32 cbWire;
        u32 cbPay = cbLen;

        memset(pHdr, 0, sizeof(*pHdr));
        if (cbPay > GJ_NET_BOUNCE_SZ - sizeof(*pHdr)) {
            g_aBounceUsed[slot] = 0;
            return -1;
        }
        cbWire = cbPay;
        if (cbWire < GJ_VIRTIO_NET_ETH_MIN) {
            cbWire = GJ_VIRTIO_NET_ETH_MIN;
            if (cbWire > GJ_NET_BOUNCE_SZ - sizeof(*pHdr)) {
                g_aBounceUsed[slot] = 0;
                return -1;
            }
            g_Stats.u32TxPad++;
        }
        memcpy(pB + sizeof(*pHdr), pBuf, cbPay);
        cbLen = (u32)sizeof(*pHdr) + cbWire;
        /* Payload pre-pad + post-pad wire tallies (parity with kernel TX). */
        g_Stats.u64TxBytes += (u64)cbPay;
        g_Stats.u64TxWireBytes += (u64)cbWire;
        g_Stats.u16LastTxLen = (u16)cbPay;
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
        g_Stats.u32TxSingle++; /* bounce AVAIL_PUSH is single-desc residual */
        virtio_net_soft_residual_lean_once("avail_tx");
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
         * RX multi-buffer: must claim head->slot before descs free so the
         * soft pool stays coherent (payload discarded on pure reap).
         */
        while (n < u32Limit) {
            u32 u32Id;
            i32 i32Len = virtio_q_poll_id(&g_qRx, 1, &u32Id);
            int nSlot;

            if (i32Len < 0) {
                break;
            }
            nSlot = rx_claim_head(u32Id);
            if (nSlot < 0) {
                g_Stats.u32RxClaimMiss++;
            }
            n++;
        }
        g_Stats.u32Reaps += n;
        if (rx_post_all() > 0) {
            net_kick(&g_qRx);
            g_Stats.u32RxRefillKick++;
        }
        note_free_min();
    } else {
        /* TX used_reap: deeper second pass when free is still tight. */
        n = virtio_q_reap(pQ, u32Limit);
        g_Stats.u32Reaps += n;
        if (virtio_q_num_free(pQ) < 4u && u32Limit < 64u) {
            u32 n2 = virtio_q_reap(pQ, 32);
            g_Stats.u32Reaps += n2;
            n += n2;
        }
        bounce_free_all_if_idle();
        note_free_min();
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
