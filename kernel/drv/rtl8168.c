/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room RTL8111/8168 (10ec:8168) poll-mode Ethernet.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL paste.
 *
 * *** SOFT SCAFFOLD / LAB ONLY (ABI-first pivot) ***
 * Product net on T0 = virtio-net + netstackd/sshd. Real NIC product path =
 * userspace/DDI later (clean-room or out-of-tree dual-license) — not this
 * file as T1 bar. Soft counters (t/f/b/r) are bring-up honesty only.
 *
 * Scope (MVP honest):
 *   - First matching PCI function, BAR0 MMIO (or first MEM BAR)
 *   - Soft reset, MAC read, TX/RX descriptor rings (64 entries)
 *   - Poll TX own-bit free + RX own-bit done
 *   - No MSI/MSI-X first cut (idle poll from net_eth_poll)
 *
 * G752VT inventory: 03:00.0 10ec:8168 RTL8168gu/8111gu.
 * Soft SKIP on QEMU without this device (virtio-net remains product path).
 *
 * greppable: rtl8168: probe | rtl8168: ready | rtl8168: tx | rtl8168: rx
 * greppable: rtl8168: dma | rtl8168: iommu
 */
#include <gj/config.h>
#include <gj/dma_buf.h>
#include <gj/error.h>
#include <gj/iommu.h>
#include <gj/klog.h>
#include <gj/rtl8168.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* ---- PCI ---------------------------------------------------------------- */
#define PCI_CFG_ADDR 0xCF8u
#define PCI_CFG_DATA 0xCFCu
#define PCI_CMD      0x04u
#define PCI_CMD_IO   0x1u
#define PCI_CMD_MEM  0x2u
#define PCI_CMD_BUS  0x4u
#define RTL_VID      0x10ecu
#define RTL_DID_8168 0x8168u

/* ---- MMIO (public RTL81xx map) ------------------------------------------ */
#define RTL_MAC0        0x00u
#define RTL_MAR0        0x08u
#define RTL_TNPDS_LO    0x20u /* TX Normal Priority Desc Start */
#define RTL_TNPDS_HI    0x24u
#define RTL_CHIPCMD     0x37u
#define RTL_CHIPCMD_RST (1u << 4)
#define RTL_CHIPCMD_RE  (1u << 3)
#define RTL_CHIPCMD_TE  (1u << 2)
#define RTL_TXCFG       0x40u
#define RTL_RXCFG       0x44u
#define RTL_CFG9346     0x50u /* EEPROM cmd / config unlock */
#define RTL_CFG9346_UNLOCK 0xC0u
#define RTL_CFG9346_LOCK   0x00u
#define RTL_CONFIG1     0x52u
#define RTL_PHYAR       0x60u
#define RTL_PHYSTATUS   0x6Cu
#define RTL_PHYSTATUS_LINK (1u << 1)
#define RTL_CPLUSCMD    0xE0u
#define RTL_RXMAXSIZE   0xDAu
#define RTL_RDSAR_LO    0xE4u /* RX Desc Start */
#define RTL_RDSAR_HI    0xE8u
#define RTL_TPPOLL      0xD9u /* TX poll: NPQ=bit6 */
#define RTL_TPPOLL_NPQ  0x40u
#define RTL_INTRMASK    0x3Cu
#define RTL_INTRSTATUS  0x3Eu
#define RTL_RXMISS      0x4Cu
#define RTL_CONFIG2     0x53u /* PME / ClkReq class (public map) */
#define RTL_CONFIG5     0xD8u /* ASPM / PME status class */
#define RTL_DLLPR       0xD0u /* power / PFM class */
#define RTL_MISC        0xF0u /* dword; RxDv gate class on 8168G */
#define RTL_INTRMITI    0xE2u /* IntrMitigate — 0 for poll mode */

/*
 * CPlusCmd (public RTL81xx / OpenBSD rtl81x9reg map).
 * 8168G/GU class: TXENB|PCI_MRW|MACSTAT_DIS first — C+ before rings/RxCfg.
 * Photo 3291 R0 EMPTY: prior path only ORd PCIMulRW late → RX never left OWN.
 */
#define RTL_CPLUS_TXENB       0x0001u
#define RTL_CPLUS_RXENB       0x0002u /* older 8169; 8168G uses MACSTAT_DIS */
#define RTL_CPLUS_PCI_MRW     0x0008u
#define RTL_CPLUS_PCI_DAC     0x0010u
#define RTL_CPLUS_RXCSUM      0x0020u
#define RTL_CPLUS_VLANSTRIP   0x0040u
#define RTL_CPLUS_MACSTAT_DIS 0x0080u /* 8168B+ / 8168G required */
/* G752 8168GU-class C+ bring-up (no RxCSUM — avoids opts1 noise). */
#define RTL_CPLUS_8168G_BRINGUP \
    (RTL_CPLUS_TXENB | RTL_CPLUS_PCI_MRW | RTL_CPLUS_MACSTAT_DIS)

/* RxConfig accept bits (public RTL81xx) */
#define RTL_RX_ACCEPT_ERR   (1u << 5)
#define RTL_RX_ACCEPT_RUNT  (1u << 4)
#define RTL_RX_ACCEPT_BCAST (1u << 3)
#define RTL_RX_ACCEPT_MCAST (1u << 2)
#define RTL_RX_ACCEPT_MY    (1u << 1)
#define RTL_RX_ACCEPT_ALL   (1u << 0)
/* DMA burst unlimited + FIFO none — common bring-up mask in bits 15:8 */
#define RTL_RX_DMA_UNLIM    0x0000E000u
#define RTL_TX_DMA_IFG      0x03000700u
/* 8168G/GU: EarlyOffV2 (public map bit 11) — without it RX can stall OWN. */
#define RTL_RX_EARLYOFFV2   0x00000800u
/*
 * Bring-up RxConfig: FIFO none (15:13=7) + MXDMA unlim (10:8=7) + EarlyOffV2
 * + accept all/bcast/mcast/my/runt/err. Soft≠product.
 */
#define RTL_RXCFG_BRINGUP   (0x0000E700u | RTL_RX_EARLYOFFV2 | RTL_RX_ACCEPT_ERR | \
                             RTL_RX_ACCEPT_RUNT | RTL_RX_ACCEPT_BCAST |           \
                             RTL_RX_ACCEPT_MCAST | RTL_RX_ACCEPT_MY |             \
                             RTL_RX_ACCEPT_ALL)
#define RTL_CFG1_DRVLOAD    0x20u /* Config1: driver loaded (public map) */

/* Desc flags (public; own + eor + fs/ls) */
#define RTL_DESC_OWN (1u << 31)
#define RTL_DESC_EOR (1u << 30)
#define RTL_DESC_FS  (1u << 29)
#define RTL_DESC_LS  (1u << 28)
/* RX post buffer size field (public CMD_BUFLEN). */
#define RTL_RX_BUFLEN_MASK 0x1fffu
/* 8168G DESCV2 completed frame length (public GFRAGLEN). */
#define RTL_RX_FRAMELEN_MASK 0x3fffu

#define RTL_RING_N   32u
#define RTL_BUF_SZ   2048u
#define RTL_MAP_CB   (64u * 1024u)
/* Prefer MMIO BAR at least 4 KiB (skip 256 B dump BARs on 8168 PCIe). */
#define RTL_BAR_MIN  0x1000u

struct rtl_desc {
    u32 u32Opts1;
    u32 u32Opts2;
    u32 u32AddrLo;
    u32 u32AddrHi;
} __attribute__((packed));

static volatile u8 *g_pMmio;
static u8 g_fReady;
/** Soft MMIO handoff: phase-1 prepare done (gate-on path only). */
static u8 g_fSoftHandoffPrepared;
static u8 g_aMac[6];
/** BDF of bound 10ec:8168 — re-assert BM/ASPM after soft REAL probe. */
static u8 g_u8PciBus;
static u8 g_u8PciSlot;
static u8 g_u8PciFunc;
static u8 g_fPciBdf;

/* Forward: quiet BM rearm for TX wait salvage (no kprintf). */
static void rtl_pci_rearm_bm_quiet(void);
/* Forward: gentle RX re-post without soft-reset (SOFT hybrid / empty). */
static void rtl_rx_gentle_refresh(void);
static u32 g_u32TxCount;
static u32 g_u32RxCount;
static u32 g_u32TxFail;   /* bad args / not ready only */
static u32 g_u32TxBusy;   /* ring full: OWN set after brief wait */
static u32 g_u32RxDrop;   /* OWN free but bad len / RES */
static u32 g_u32RxEmpty;  /* poll saw full ring still OWN */
static u32 g_u32RxAuditN; /* rate-limited empty-with-link dumps */
static u32 g_u32TxIdx;
static u32 g_u32RxIdx;

static struct rtl_desc *g_pTxDesc;
static struct rtl_desc *g_pRxDesc;
static gj_paddr_t g_paTxDesc;
static gj_paddr_t g_paRxDesc;
static gj_paddr_t g_aPaTxBuf[RTL_RING_N];
static gj_paddr_t g_aPaRxBuf[RTL_RING_N];
static void *g_aVaTxBuf[RTL_RING_N];
static void *g_aVaRxBuf[RTL_RING_N];

static inline void
outl(u16 u16Port, u32 u32Val)
{
    __asm__ volatile("outl %0, %1" : : "a"(u32Val), "Nd"(u16Port));
}

static inline u32
inl(u16 u16Port)
{
    u32 u32Val;

    __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"(u16Port));
    return u32Val;
}

static u32
pci_cfg_read32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);

    outl(PCI_CFG_ADDR, u32Addr);
    return inl(PCI_CFG_DATA);
}

static void
pci_cfg_write32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u32 u32Val)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);

    outl(PCI_CFG_ADDR, u32Addr);
    outl(PCI_CFG_DATA, u32Val);
}

static u16
pci_cfg_read16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Word = pci_cfg_read32(u8Bus, u8Slot, u8Func, (u8)(u8Off & 0xfcu));
    u32 u32Shift = (u32)(u8Off & 2u) * 8u;

    return (u16)((u32Word >> u32Shift) & 0xffffu);
}

static void
pci_cfg_write16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u16 u16Val)
{
    u32 u32Word = pci_cfg_read32(u8Bus, u8Slot, u8Func, (u8)(u8Off & 0xfcu));
    u32 u32Shift = (u32)(u8Off & 2u) * 8u;
    u32 u32Mask = 0xffffu << u32Shift;

    u32Word = (u32Word & ~u32Mask) | (((u32)u16Val << u32Shift) & u32Mask);
    pci_cfg_write32(u8Bus, u8Slot, u8Func, (u8)(u8Off & 0xfcu), u32Word);
}

static u8
mmio_r8(u32 u32Off)
{
    return g_pMmio[u32Off];
}

static void
mmio_w8(u32 u32Off, u8 u8Val)
{
    g_pMmio[u32Off] = u8Val;
    __asm__ volatile("mfence" ::: "memory");
}

static u16
mmio_r16(u32 u32Off)
{
    return *(volatile u16 *)(void *)(g_pMmio + u32Off);
}

static void
mmio_w16(u32 u32Off, u16 u16Val)
{
    *(volatile u16 *)(void *)(g_pMmio + u32Off) = u16Val;
    __asm__ volatile("mfence" ::: "memory");
}

static void
mmio_w32(u32 u32Off, u32 u32Val)
{
    *(volatile u32 *)(void *)(g_pMmio + u32Off) = u32Val;
    __asm__ volatile("mfence" ::: "memory");
}

static u32 __attribute__((unused))
mmio_r32(u32 u32Off)
{
    return *(volatile u32 *)(void *)(g_pMmio + u32Off);
}

static void
rtl_clflush(const void *p)
{
    __asm__ volatile("clflush (%0)" : : "r"(p) : "memory");
    __asm__ volatile("mfence" ::: "memory");
}

static int
rtl_soft_reset(void)
{
    u32 i;

    mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RST);
    for (i = 0; i < 1000000u; i++) {
        if ((mmio_r8(RTL_CHIPCMD) & RTL_CHIPCMD_RST) == 0) {
            return 0;
        }
        __asm__ volatile("pause");
    }
    return -1;
}

/*
 * Force 32-bit / VT-d identity DMA PA for one page.
 * Retries dma_buf_alloc_page until PA is inside bring-up identity [0, 1 GiB)
 * (and thus <4 GiB so addr_hi=0). Under TE, PA outside identity → OWN never
 * clears (RX killer). Soft≠product.
 * Grep: rtl8168: dma
 */
static gj_paddr_t
rtl_dma_alloc_low32(void **ppVa)
{
    gj_paddr_t aBad[8];
    u32 cBad = 0;
    u32 iTry;
    gj_paddr_t pa = 0;
    void *pVa = NULL;

    if (ppVa != NULL) {
        *ppVa = NULL;
    }
    for (iTry = 0; iTry < 24u; iTry++) {
        pa = dma_buf_alloc_page(&pVa);
        if (pa == 0) {
            break;
        }
        /* Prefer VT-d identity cover [0,1 GiB). */
        if (dma_buf_in_vtd_identity(pa, (u64)GJ_PAGE_SIZE) != 0) {
            while (cBad > 0u) {
                cBad--;
                dma_buf_free_page(aBad[cBad]);
            }
            if (ppVa != NULL) {
                *ppVa = pVa;
            }
            return pa;
        }
        /* Soft fallback: any PA < 4 GiB (force32 / non-DAC). */
        if (dma_buf_is_low(pa) != 0) {
            while (cBad > 0u) {
                cBad--;
                dma_buf_free_page(aBad[cBad]);
            }
            if (ppVa != NULL) {
                *ppVa = pVa;
            }
            /* Grep: rtl8168: dma */
            kprintf("rtl8168: dma soft pa=0x%lx low32 but outside "
                    "vtd_identity (OWN risk under TE)\n",
                    (unsigned long)pa);
            return pa;
        }
        if (cBad < 8u) {
            aBad[cBad++] = pa;
        } else {
            dma_buf_free_page(pa);
        }
        pa = 0;
        pVa = NULL;
    }
    /* Last resort: first high held page (likely OWN-stuck under TE). */
    if (cBad > 0u) {
        pa = aBad[0];
        for (iTry = 1u; iTry < cBad; iTry++) {
            dma_buf_free_page(aBad[iTry]);
        }
        /* Grep: rtl8168: dma */
        kprintf("rtl8168: dma PA high/unmapped pa=0x%lx "
                "(force32 fail; OWN may stick under TE)\n",
                (unsigned long)pa);
        if (ppVa != NULL) {
            *ppVa = (void *)hhdm_to_virt(pa);
        }
        return pa;
    }
    /* Grep: rtl8168: dma */
    kprintf("rtl8168: dma alloc FAIL (no page)\n");
    return 0;
}

static int
rtl_alloc_rings(void)
{
    u32 i;
    void *pVa = NULL;
    u32 cOutId = 0;

    g_paTxDesc = rtl_dma_alloc_low32(&pVa);
    g_pTxDesc = (struct rtl_desc *)pVa;
    g_paRxDesc = rtl_dma_alloc_low32(&pVa);
    g_pRxDesc = (struct rtl_desc *)pVa;
    if (g_paTxDesc == 0 || g_paRxDesc == 0 || g_pTxDesc == NULL ||
        g_pRxDesc == NULL) {
        /* Grep: rtl8168: dma */
        kprintf("rtl8168: dma desc FAIL\n");
        return -1;
    }
    memset(g_pTxDesc, 0, GJ_PAGE_SIZE);
    memset(g_pRxDesc, 0, GJ_PAGE_SIZE);

    for (i = 0; i < RTL_RING_N; i++) {
        g_aPaTxBuf[i] = rtl_dma_alloc_low32(&g_aVaTxBuf[i]);
        g_aPaRxBuf[i] = rtl_dma_alloc_low32(&g_aVaRxBuf[i]);
        if (g_aPaTxBuf[i] == 0 || g_aPaRxBuf[i] == 0) {
            /* Grep: rtl8168: dma */
            kprintf("rtl8168: dma buf FAIL i=%u\n", (unsigned)i);
            return -1;
        }
        memset(g_aVaTxBuf[i], 0, GJ_PAGE_SIZE);
        memset(g_aVaRxBuf[i], 0, GJ_PAGE_SIZE);

        if (dma_buf_in_vtd_identity(g_aPaTxBuf[i], (u64)GJ_PAGE_SIZE) == 0 ||
            dma_buf_in_vtd_identity(g_aPaRxBuf[i], (u64)GJ_PAGE_SIZE) == 0) {
            cOutId++;
        }

        /*
         * Force 32-bit DMA: addr_hi always 0 for PA < 4 GiB (DAC off path).
         * Identity cover is subset of 32-bit; high PA already warned above.
         */
        g_pTxDesc[i].u32AddrLo = (u32)(g_aPaTxBuf[i] & 0xffffffffu);
        g_pTxDesc[i].u32AddrHi = 0;
        g_pTxDesc[i].u32Opts1 = 0;
        g_pTxDesc[i].u32Opts2 = 0;

        g_pRxDesc[i].u32AddrLo = (u32)(g_aPaRxBuf[i] & 0xffffffffu);
        g_pRxDesc[i].u32AddrHi = 0;
        g_pRxDesc[i].u32Opts1 =
            RTL_DESC_OWN | (RTL_BUF_SZ & RTL_RX_BUFLEN_MASK);
        g_pRxDesc[i].u32Opts2 = 0;
        if (i + 1u == RTL_RING_N) {
            g_pRxDesc[i].u32Opts1 |= RTL_DESC_EOR;
            g_pTxDesc[i].u32Opts1 = RTL_DESC_EOR; /* EOR when idle */
        }
        rtl_clflush(&g_pTxDesc[i]);
        rtl_clflush(&g_pRxDesc[i]);
        rtl_clflush(g_aVaRxBuf[i]);
    }
    g_u32TxIdx = 0;
    g_u32RxIdx = 0;
    /* Grep: rtl8168: dma */
    kprintf("rtl8168: dma rings ok force32=1 out_id=%u desc_tx=0x%lx "
            "desc_rx=0x%lx\n",
            (unsigned)cOutId, (unsigned long)g_paTxDesc,
            (unsigned long)g_paRxDesc);
    if (cOutId != 0u) {
        /* Grep: rtl8168: dma */
        kprintf("rtl8168: dma WARN %u bufs outside vtd_identity "
                "(RX OWN risk under TE)\n",
                (unsigned)cOutId);
    }
    return 0;
}

/*
 * Full HW program for 8168G/GU-class (G752). Public map only.
 * Order: stop → CPlusCmd FIRST → MAC/MAR → rings (HI=0) → RxDv → Tx/RxCfg
 * → TE|RE → force RxCfg+CPlus after RE. No chip soft-reset here. Soft≠product.
 */
static void
rtl_program_hw(void)
{
    u32 i;
    u32 u32RxCfg;
    u16 u16Cp;
    u32 u32Spin;
    u32 u32HwRev;
    u32 u32RdsarLo;
    u32 u32RdsarHi;

    /* Stop engines before programming rings (required on many 8168G). */
    mmio_w8(RTL_CHIPCMD, 0);
    for (u32Spin = 0; u32Spin < 10000u; u32Spin++) {
        __asm__ volatile("pause");
    }

    /* Unlock config space (required before MAC/Rx/Tx cfg on many 8168). */
    mmio_w8(RTL_CFG9346, RTL_CFG9346_UNLOCK);

    /*
     * CPlusCmd FIRST (public re_init rule): TXENB|PCI_MRW|MACSTAT_DIS for
     * 8168G/GU. Prior path only ORd PCIMulRW late → RX OWN forever (3291).
     * Soft≠product.
     */
    u16Cp = (u16)RTL_CPLUS_8168G_BRINGUP;
    mmio_w16(RTL_CPLUSCMD, u16Cp);

    /* Clear interrupts — poll mode (no MSI yet). */
    mmio_w16(RTL_INTRMASK, 0);
    mmio_w16(RTL_INTRSTATUS, 0xffffu);

    /* Re-write station address after reset. */
    for (i = 0; i < 6u; i++) {
        mmio_w8(RTL_MAC0 + i, g_aMac[i]);
    }
    for (i = 0; i < 8u; i++) {
        mmio_w8(RTL_MAR0 + i, 0xffu);
    }

    /*
     * Desc rings — force 32-bit DMA (HI=0). PA must sit in VT-d identity
     * [0,1 GiB) so TE does not fault and OWN can clear. Soft≠product.
     * Grep: rtl8168: dma
     */
    mmio_w32(RTL_TNPDS_LO, (u32)(g_paTxDesc & 0xffffffffu));
    mmio_w32(RTL_TNPDS_HI, 0);
    mmio_w32(RTL_RDSAR_LO, (u32)(g_paRxDesc & 0xffffffffu));
    mmio_w32(RTL_RDSAR_HI, 0);
    /* High-priority TX ring = same (some revs poll HPQ). */
    mmio_w32(0x28u, (u32)(g_paTxDesc & 0xffffffffu)); /* THPDS lo */
    mmio_w32(0x2Cu, 0);

    /* TxConfig: IFG normal + DMA unlimited (classic bring-up). */
    mmio_w32(RTL_TXCFG, 0x03000700u);
    /* Early TX threshold: store-and-forward class. */
    mmio_w8(0xECu, 0x3Fu);
    /* Max TX packet size (8168G): 0x3F ≈ no limit class. */
    mmio_w8(0xEEu, 0x3Fu);

    /*
     * RxConfig: promisc + EarlyOffV2 + unlimited DMA/FIFO none.
     * 8168GU needs EarlyOffV2 (public flag) or RX can stay OWN forever.
     */
    u32RxCfg = RTL_RXCFG_BRINGUP;
    mmio_w32(RTL_RXCFG, u32RxCfg);
    /* Max RX packet — standard MTU class (16383 public 8169 path). */
    mmio_w16(RTL_RXMAXSIZE, 16383u);

    /* Config1: clear bit0 (PM-class), set DRVLOAD so chip treats host live. */
    mmio_w8(RTL_CONFIG1,
            (u8)((mmio_r8(RTL_CONFIG1) & (u8)~0x01u) | RTL_CFG1_DRVLOAD));
    /*
     * Soft power quirks (public RTL81xx map; no GPL paste):
     *   Config2: clear ClkReqEn-class (bit 7 on many maps)
     *   Config5: clear ASPM_en / LanWake sticky (low bits)
     *   DLLPR: clear PFM_EN-class (bit 6 on many maps)
     *   MISC: clear RxDv_Gated_En (bit 19 of dword @ 0xF0) — 8168G RX dead
     *     if gate left set after soft-reset / hybrid path. Soft≠product.
     */
    mmio_w8(RTL_CONFIG2, (u8)(mmio_r8(RTL_CONFIG2) & (u8)~0x80u));
    mmio_w8(RTL_CONFIG5, (u8)(mmio_r8(RTL_CONFIG5) & (u8)~0x19u));
    mmio_w8(RTL_DLLPR, (u8)(mmio_r8(RTL_DLLPR) & (u8)~0x40u));
    {
        u32 u32Misc = mmio_r32(RTL_MISC);

        u32Misc &= ~(1u << 19); /* RxDV gate off so RX can leave PHY */
        mmio_w32(RTL_MISC, u32Misc);
    }
    /* Poll mode: no interrupt coalescing. */
    mmio_w16(RTL_INTRMITI, 0);
    /* Clear missed-packet counter (some revs sticky until read). */
    (void)mmio_r32(RTL_RXMISS);
    mmio_w16(RTL_INTRSTATUS, 0xffffu);
    mmio_w8(RTL_CFG9346, RTL_CFG9346_LOCK);

    /* Enable RX+TX engines together. */
    mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    for (u32Spin = 0; u32Spin < 10000u; u32Spin++) {
        __asm__ volatile("pause");
    }
    /* Re-assert RE after a beat — 8168G can drop RE if rings race enable. */
    mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    /*
     * Re-write RxConfig + CPlus with engines on — 8168G samples accept /
     * C+ after RE; without this, RX stays OWN-stuck (EMPTY). Soft≠product.
     */
    mmio_w16(RTL_CPLUSCMD, u16Cp);
    mmio_w32(RTL_RXCFG, u32RxCfg);

    u32HwRev = mmio_r32(RTL_TXCFG) & 0x7c800000u;
    u32RdsarLo = mmio_r32(RTL_RDSAR_LO);
    u32RdsarHi = mmio_r32(RTL_RDSAR_HI);
    kprintf("rtl8168: hw cmd=0x%x rxcfg=0x%x txcfg=0x%x cplus=0x%x phy=0x%x "
            "hwrev=0x%x tnpds=0x%x:%x rdsar=0x%x:%x (rdback 0x%x:%x)\n",
            (unsigned)mmio_r8(RTL_CHIPCMD), (unsigned)mmio_r32(RTL_RXCFG),
            (unsigned)mmio_r32(RTL_TXCFG), (unsigned)mmio_r16(RTL_CPLUSCMD),
            (unsigned)mmio_r8(RTL_PHYSTATUS), (unsigned)(u32HwRev >> 16),
            (unsigned)0u, (unsigned)(u32)g_paTxDesc, (unsigned)0u,
            (unsigned)(u32)g_paRxDesc, (unsigned)u32RdsarHi,
            (unsigned)u32RdsarLo);
}

/* Keep RX engine alive; clear sticky ISR. Call from net_eth_poll. */
void
rtl8168_poll_hw(void)
{
    u16 u16Isr;
    u8 u8Cmd;
    static u32 s_u32EmptyKick;
    static u32 s_u32GentleN;

    /*
     * Phase-1/2 handoff: after prepare, g_fReady=0 and g_fSoftHandoffPrepared=1.
     * Never re-assert TE|RE (dual-drive). g_pMmio may stay mapped but idle —
     * do not unmap (dangerous mid-soft claim). Soft≠product.
     */
    if (g_fSoftHandoffPrepared != 0 || g_fReady == 0 || g_pMmio == NULL) {
        return;
    }
    /*
     * Only re-assert TE|RE if cleared — thrashing CHIPCMD every poll made
     * TX OWN stick (G752 t4/f36). Clear ISR always.
     */
    u8Cmd = mmio_r8(RTL_CHIPCMD);
    if ((u8Cmd & (RTL_CHIPCMD_RE | RTL_CHIPCMD_TE)) !=
        (RTL_CHIPCMD_RE | RTL_CHIPCMD_TE)) {
        mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    }
    u16Isr = mmio_r16(RTL_INTRSTATUS);
    if (u16Isr != 0) {
        mmio_w16(RTL_INTRSTATUS, u16Isr);
    }
    /*
     * Soft: RX starved with link up — RE reassert + clflush, then rare gentle
     * RX re-post (no soft-reset; photo 3283 reclaim→R0). Cap gentle refreshes.
     * Soft≠product.
     */
    s_u32EmptyKick++;
    if ((s_u32EmptyKick & 255u) == 0u &&
        (mmio_r8(RTL_PHYSTATUS) & RTL_PHYSTATUS_LINK) != 0) {
        mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
        if (g_pRxDesc != NULL) {
            rtl_clflush(&g_pRxDesc[g_u32RxIdx % RTL_RING_N]);
        }
        /* Every 1024 empty-ish polls, at most 4 gentle RX refreshes. */
        if ((s_u32EmptyKick & 1023u) == 0u && s_u32GentleN < 4u &&
            g_u32RxCount == 0u) {
            s_u32GentleN++;
            rtl_rx_gentle_refresh();
        }
    }
}

/* Kick both TX poll registers used across 8168 revisions. */
static void
rtl_tx_kick(void)
{
    u8 u8Cmd;

    /* Handoff prepared: freestanding must not program CHIPCMD (belt). */
    if (g_fSoftHandoffPrepared != 0 || g_fReady == 0 || g_pMmio == NULL) {
        return;
    }
    /*
     * Only re-assert TE|RE if cleared. Rewriting CHIPCMD every TPPOLL kick
     * made TX OWN stick on G752 (t4/f36 / B### storms) — engines restart
     * mid-desc without clearing OWN.
     */
    u8Cmd = mmio_r8(RTL_CHIPCMD);
    if ((u8Cmd & (RTL_CHIPCMD_RE | RTL_CHIPCMD_TE)) !=
        (RTL_CHIPCMD_RE | RTL_CHIPCMD_TE)) {
        mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    }
    /* TPPOLL 0xD9: bit0=HPQ, bit6=NPQ (public 8168 map). */
    mmio_w8(RTL_TPPOLL, (u8)(0x01u | RTL_TPPOLL_NPQ));
    mmio_w8(0x38u, 0x40u); /* older TxPoll */
    /* Ack TOK/TER so sticky ISR does not hold the TX unit (poll mode). */
    mmio_w16(RTL_INTRSTATUS, 0xffffu);
}

/*
 * Wait until desc OWN clears (NIC finished / aborted). Returns 0 if free,
 * -1 if still OWN after budget. Kicks TPPOLL sparsely; no CHIPCMD thrash.
 * Soft≠product.
 */
static int
rtl_tx_wait_own_clear(struct rtl_desc *pD, u32 u32Budget)
{
    u32 u32Spin;
    u16 u16Isr;

    if (pD == NULL) {
        return -1;
    }
    for (u32Spin = 0; u32Spin < u32Budget; u32Spin++) {
        rtl_clflush(pD);
        __asm__ volatile("mfence" ::: "memory");
        if ((pD->u32Opts1 & RTL_DESC_OWN) == 0) {
            return 0;
        }
        /* Sparse kick: every 16k spins — not every packet thrash. */
        if ((u32Spin & 0x3fffu) == 0u) {
            rtl_tx_kick();
            u16Isr = mmio_r16(RTL_INTRSTATUS);
            if (u16Isr != 0) {
                mmio_w16(RTL_INTRSTATUS, u16Isr);
            }
        }
        /* Mid-budget quiet BM salvage once (no serial flood). */
        if (u32Spin == (u32Budget / 2u) && g_fPciBdf != 0u) {
            rtl_pci_rearm_bm_quiet();
            rtl_tx_kick();
        }
        __asm__ volatile("pause");
    }
    rtl_clflush(pD);
    return ((pD->u32Opts1 & RTL_DESC_OWN) == 0) ? 0 : -1;
}

/*
 * Collect MEM BARs for 10ec:8168. Prefer larger regions but NEVER reject the
 * device if size-probe is unreliable (G752 NET NONE regression).
 */
struct rtl_bar_cand {
    u64 u64Pa;
    u32 u32Sz;
    u8  u8Off;
};

static int
rtl_collect_bars(u8 u8Bus, u8 u8Slot, u8 u8Func, struct rtl_bar_cand *pOut,
                 u32 u32Cap)
{
    u8 u8Off;
    u32 n = 0;

    for (u8Off = 0x10u; u8Off <= 0x24u && n < u32Cap;
         u8Off = (u8)(u8Off + 4u)) {
        u32 u32Bar0;
        u32 u32Save;
        u32 u32Raw;
        u32 u32Sz;
        u8 u8Type;
        u64 paBar;

        u32Bar0 = pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
        if ((u32Bar0 & 1u) != 0) {
            continue; /* I/O */
        }
        if ((u32Bar0 & ~0xfu) == 0) {
            continue;
        }
        u8Type = (u8)((u32Bar0 >> 1) & 3u);
        paBar = (u64)(u32Bar0 & ~0xfu);
        if (u8Type == 2u) {
            u32 u32Hi =
                pci_cfg_read32(u8Bus, u8Slot, u8Func, (u8)(u8Off + 4u));

            paBar |= ((u64)u32Hi << 32);
        }

        /* Size probe — always restore. Treat size 0 as "unknown, try 64K". */
        u32Save = u32Bar0;
        pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, 0xffffffffu);
        u32Raw = pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
        pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, u32Save);
        u32Raw &= ~0xfu;
        if (u32Raw == 0) {
            u32Sz = RTL_MAP_CB; /* unknown — still try */
        } else {
            u32Sz = (~u32Raw) + 1u;
            if (u32Sz == 0) {
                u32Sz = RTL_MAP_CB;
            }
        }

        pOut[n].u64Pa = paBar;
        pOut[n].u32Sz = u32Sz;
        pOut[n].u8Off = u8Off;
        kprintf("rtl8168: BAR@%02x pa=0x%lx size=0x%x type=%u\n",
                (unsigned)u8Off, (unsigned long)paBar, (unsigned)u32Sz,
                (unsigned)u8Type);
        n++;
        if (u8Type == 2u) {
            u8Off = (u8)(u8Off + 4u);
        }
    }
    return (int)n;
}

/* Sort candidates: larger size first (insertion). */
static void
rtl_sort_bars(struct rtl_bar_cand *pA, u32 n)
{
    u32 i;
    u32 j;

    for (i = 1; i < n; i++) {
        struct rtl_bar_cand t = pA[i];

        j = i;
        while (j > 0u && pA[j - 1u].u32Sz < t.u32Sz) {
            pA[j] = pA[j - 1u];
            j--;
        }
        pA[j] = t;
    }
}

static int
rtl_find_pci(u8 *pBus, u8 *pSlot, u8 *pFunc)
{
    u32 u32Bus;
    u8 u8Slot;
    u8 u8Func;

    for (u32Bus = 0; u32Bus < 256u; u32Bus++) {
        for (u8Slot = 0; u8Slot < 32u; u8Slot++) {
            for (u8Func = 0; u8Func < 8u; u8Func++) {
                u8 u8Bus = (u8)u32Bus;
                u32 u32Id = pci_cfg_read32(u8Bus, u8Slot, u8Func, 0);
                u16 u16Vid;
                u16 u16Did;
                u16 u16Cmd;

                if (u32Id == 0xffffffffu || u32Id == 0u) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                u16Vid = (u16)(u32Id & 0xffffu);
                u16Did = (u16)(u32Id >> 16);
                if (u16Vid != RTL_VID || u16Did != RTL_DID_8168) {
                    continue;
                }

                u16Cmd = pci_cfg_read16(u8Bus, u8Slot, u8Func, PCI_CMD);
                u16Cmd |= (u16)(PCI_CMD_MEM | PCI_CMD_BUS);
                u16Cmd &= (u16)~0x400u;
                pci_cfg_write16(u8Bus, u8Slot, u8Func, PCI_CMD, u16Cmd);

                /*
                 * Soft: clear PCIe ASPM (LNKCTL bits 1:0) if PCIe cap present.
                 * Many 8168 revs fail RX/TX OWN under ASPM (Linux r8169 quirk).
                 */
                {
                    u8 u8Cap = (u8)(pci_cfg_read32(u8Bus, u8Slot, u8Func,
                                                    0x34u) &
                                    0xffu);
                    u32 cGuard = 0;

                    while (u8Cap >= 0x40u && u8Cap < 0xfcu && cGuard < 48u) {
                        u32 u32Cap =
                            pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Cap);
                        u8 u8Id = (u8)(u32Cap & 0xffu);
                        u8 u8Next = (u8)((u32Cap >> 8) & 0xffu);

                        if (u8Id == 0x10u) {
                            /* PCIe: LNKCTL at cap+0x10 */
                            u16 u16Lnk = pci_cfg_read16(
                                u8Bus, u8Slot, u8Func, (u8)(u8Cap + 0x10u));
                            if ((u16Lnk & 3u) != 0) {
                                u16Lnk = (u16)(u16Lnk & ~3u);
                                pci_cfg_write16(u8Bus, u8Slot, u8Func,
                                                (u8)(u8Cap + 0x10u), u16Lnk);
                                kprintf("rtl8168: ASPM off lnkctl=0x%x\n",
                                        (unsigned)u16Lnk);
                            }
                            break;
                        }
                        if (u8Next == 0 || u8Next == u8Cap) {
                            break;
                        }
                        u8Cap = u8Next;
                        cGuard++;
                    }
                }

                *pBus = u8Bus;
                *pSlot = u8Slot;
                *pFunc = u8Func;
                g_u8PciBus = u8Bus;
                g_u8PciSlot = u8Slot;
                g_u8PciFunc = u8Func;
                g_fPciBdf = 1u;
                kprintf("rtl8168: found %02x:%02x.%u cmd=0x%x\n",
                        (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
                        (unsigned)u16Cmd);
                return 0;
            }
        }
    }
    return -1;
}

/*
 * True if MAC looks like a station address (not all0/all1).
 * Wrong BAR often yields zeros — try next BAR.
 */
static int
rtl_mac_ok(const u8 *pM)
{
    u32 i;
    u8 u8Or = 0;
    u8 u8And = 0xffu;

    if (pM == NULL) {
        return 0;
    }
    for (i = 0; i < 6u; i++) {
        u8Or = (u8)(u8Or | pM[i]);
        u8And = (u8)(u8And & pM[i]);
    }
    if (u8Or == 0 || u8And == 0xffu) {
        return 0;
    }
    /* Multicast bit set on station addr is unusual for onboard NIC. */
    if ((pM[0] & 1u) != 0) {
        return 0;
    }
    return 1;
}

int
rtl8168_probe(void)
{
    u8 u8Bus = 0;
    u8 u8Slot = 0;
    u8 u8Func = 0;
    struct rtl_bar_cand aBar[6];
    int nBar;
    int iBar;
    gj_vaddr_t vaMap = 0;
    gj_status_t st;
    u32 i;

    g_fReady = 0;
    g_pMmio = NULL;

    if (rtl_find_pci(&u8Bus, &u8Slot, &u8Func) != 0) {
        kprintf("rtl8168: probe SKIP (no 10ec:8168)\n");
        return 0;
    }

    /*
     * Soft VT-d identity for this BDF (G752: 03:00.0 10ec:8168, Translated).
     * Root tables cover all buses; attach DID=0 + window in [0,1 GiB).
     * Without this (or with PA outside identity), RX OWN never clears under TE.
     * Grep: rtl8168: iommu
     */
    {
        int nId;
        u32 u32Did;

        nId = iommu_vtd_xhci_identity(u8Bus, u8Slot, u8Func);
        u32Did = iommu_vtd_domain_lookup(u8Bus, u8Slot, u8Func);
        /* Grep: rtl8168: iommu */
        kprintf("rtl8168: iommu identity bdf=%02x:%02x.%u ret=%d did=%u "
                "te_mode=%d tables=%d cover_1g=%d (10ec:8168)\n",
                (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func, nId,
                u32Did, iommu_vtd_te_mode(), iommu_vtd_ready(),
                iommu_vtd_identity_covers(0, GJ_DMA_VTD_IDENTITY_LIMIT));
        if (nId < 0 || (iommu_vtd_ready() != 0 && u32Did != 0u)) {
            /* Grep: rtl8168: iommu — did!=0 includes INVALID (unattached) */
            kprintf("rtl8168: iommu WARN bdf not on identity DID0 "
                    "(OWN may stick)\n");
        }
    }

    nBar = rtl_collect_bars(u8Bus, u8Slot, u8Func, aBar, 6u);
    if (nBar <= 0) {
        kprintf("rtl8168: no MEM BAR at %02x:%02x.%u\n", (unsigned)u8Bus,
                (unsigned)u8Slot, (unsigned)u8Func);
        return 0;
    }
    rtl_sort_bars(aBar, (u32)nBar);

    /*
     * Try each MEM BAR until soft-reset + MAC look real.
     * Prefer large size first, but fall back so we never NET NONE when the
     * chip is present (previous ≥4K-only filter caused NET NONE on G752).
     */
    for (iBar = 0; iBar < nBar; iBar++) {
        u64 paBar = aBar[iBar].u64Pa;
        u32 u32Map = aBar[iBar].u32Sz;

        if (u32Map < 0x100u) {
            u32Map = 0x1000u;
        }
        if (u32Map > RTL_MAP_CB) {
            u32Map = RTL_MAP_CB;
        }

        st = vmm_map_device_uc((gj_paddr_t)paBar, u32Map, &vaMap);
        if (st != GJ_OK) {
            kprintf("rtl8168: map FAIL BAR@%02x st=%d pa=0x%lx\n",
                    (unsigned)aBar[iBar].u8Off, (int)st,
                    (unsigned long)paBar);
            continue;
        }
        g_pMmio = (volatile u8 *)vaMap;

        if (rtl_soft_reset() != 0) {
            kprintf("rtl8168: soft reset FAIL BAR@%02x\n",
                    (unsigned)aBar[iBar].u8Off);
            g_pMmio = NULL;
            continue;
        }
        for (i = 0; i < 100000u; i++) {
            __asm__ volatile("pause");
        }

        for (i = 0; i < 6u; i++) {
            g_aMac[i] = mmio_r8(RTL_MAC0 + i);
        }
        if (rtl_mac_ok(g_aMac) == 0) {
            kprintf("rtl8168: skip BAR@%02x bad mac=%02x:%02x:%02x:%02x:%02x:"
                    "%02x\n",
                    (unsigned)aBar[iBar].u8Off, g_aMac[0], g_aMac[1],
                    g_aMac[2], g_aMac[3], g_aMac[4], g_aMac[5]);
            g_pMmio = NULL;
            continue;
        }

        kprintf("rtl8168: using BAR@%02x pa=0x%lx size=0x%x\n",
                (unsigned)aBar[iBar].u8Off, (unsigned long)paBar,
                (unsigned)aBar[iBar].u32Sz);

        if (rtl_alloc_rings() != 0) {
            g_pMmio = NULL;
            return 0;
        }
        rtl_program_hw();
        g_fReady = 1;

        /*
         * A4 — one-shot DMA PA dump: greppable VT-d identity cover check.
         * desc rings are full pages; first RX buf is representative.
         * force32: addr_hi / RDSAR_HI / TNPDS_HI programmed 0.
         * Grep: rtl8168: dma
         */
        {
            int fTxId =
                dma_buf_in_vtd_identity(g_paTxDesc, (u64)GJ_PAGE_SIZE);
            int fRxId =
                dma_buf_in_vtd_identity(g_paRxDesc, (u64)GJ_PAGE_SIZE);
            int fBufId =
                dma_buf_in_vtd_identity(g_aPaRxBuf[0], (u64)GJ_PAGE_SIZE);
            int fAllId = (fTxId != 0 && fRxId != 0 && fBufId != 0) ? 1 : 0;

            kprintf("rtl8168: dma desc_tx=0x%lx in_id=%d desc_rx=0x%lx "
                    "in_id=%d buf0=0x%lx in_id=%d force32=1 all_id=%d\n",
                    (unsigned long)g_paTxDesc, fTxId,
                    (unsigned long)g_paRxDesc, fRxId,
                    (unsigned long)g_aPaRxBuf[0], fBufId, fAllId);
            if (fAllId == 0) {
                /* Grep: rtl8168: dma */
                kprintf("rtl8168: dma FAIL high/unmapped PA "
                        "(IOMMU identity RX killer risk)\n");
            } else {
                /* Grep: rtl8168: dma */
                kprintf("rtl8168: dma PASS low32 in identity cover\n");
            }
        }

        /* Boot TX self-test: minimal broadcast frame must clear OWN. */
        {
            u8 aTest[64];
            int nTx;

            memset(aTest, 0xff, 6); /* dst bcast */
            memcpy(aTest + 6, g_aMac, 6);
            aTest[12] = 0x08;
            aTest[13] = 0x06; /* ARP ethertype — harmless on wire */
            nTx = rtl8168_tx(aTest, 60u);
            kprintf("rtl8168: TX selftest %s tx=%u fail=%u busy=%u\n",
                    nTx == 0 ? "PASS" : "FAIL", (unsigned)g_u32TxCount,
                    (unsigned)g_u32TxFail, (unsigned)g_u32TxBusy);
        }

        kprintf("rtl8168: probe PASS %02x:%02x.%u bar=0x%lx mac=%02x:%02x:"
                "%02x:%02x:%02x:%02x link=%u\n",
                (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
                (unsigned long)paBar, g_aMac[0], g_aMac[1], g_aMac[2],
                g_aMac[3], g_aMac[4], g_aMac[5],
                (mmio_r8(RTL_PHYSTATUS) & RTL_PHYSTATUS_LINK) != 0 ? 1u : 0u);
        kprintf("rtl8168: ready PASS (poll TX/RX; promiscuous lab RX)\n");
        return 0;
    }

    kprintf("rtl8168: probe FAIL all BARs tried n=%d (NET none)\n", nBar);
    g_pMmio = NULL;
    return 0;
}

int
rtl8168_ready(void)
{
    return g_fReady != 0 ? 1 : 0;
}

/*
 * Soft freestanding→r8169 MMIO handoff prepare (phase 1 → phase-2 readiness).
 * Default gate off: log SKIP once, no MMIO touch.
 * Gate on: stop TE/RE, mask IRQ, clear ready, mark net_l2 pending.
 * Leaves g_pMmio mapped but idle (no unmap — dual-map / reclaim unsafe).
 * Soft path sole documented BAR owner = hostish pci_dev resource[] (phase 2).
 * Does not call soft/.ko open; does not set g_fMmioHandoff. Soft≠product.
 * Grep: rtl8168: soft mmio handoff
 */
void
rtl8168_soft_handoff_prepare(void)
{
    static int s_fSkipLogged;

#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    if (s_fSkipLogged == 0) {
        s_fSkipLogged = 1;
        /* Grep: rtl8168: soft mmio handoff SKIP (gate off) */
        kprintf("rtl8168: soft mmio handoff SKIP (gate off)\n");
    }
    (void)g_fSoftHandoffPrepared;
    return;
#else
    u32 u32Spin;

    (void)s_fSkipLogged;

    if (g_fSoftHandoffPrepared != 0) {
        /* Grep: rtl8168: soft mmio handoff PREPARE (idempotent) */
        kprintf("rtl8168: soft mmio handoff PREPARE (idempotent)\n");
        return;
    }

    if (g_pMmio == NULL) {
        /* Grep: rtl8168: soft mmio handoff SKIP (no mmio) */
        kprintf("rtl8168: soft mmio handoff SKIP (no mmio)\n");
        return;
    }

    /* Stop TX/RX engines; mask interrupts (poll mode already 0, reassert). */
    mmio_w8(RTL_CHIPCMD, 0);
    for (u32Spin = 0; u32Spin < 10000u; u32Spin++) {
        __asm__ volatile("pause");
    }
    mmio_w16(RTL_INTRMASK, 0);
    mmio_w16(RTL_INTRSTATUS, 0xffffu);

    /*
     * Clear ready so poll_hw / tx / rx / tx_kick will not re-assert TE|RE.
     * Keep g_pMmio mapped (idle): unmap would race soft hostish claim;
     * freestanding must stop using it. Phase 2: hostish resource[] is
     * sole documented soft-path BAR owner (see linux_netdev_soft sole-owner).
     */
    g_fReady = 0;
    g_fSoftHandoffPrepared = 1;

    {
        extern void net_l2_soft_handoff_mark_pending(void);

        net_l2_soft_handoff_mark_pending();
    }

    /* Grep: rtl8168: soft mmio handoff PREPARE */
    kprintf("rtl8168: soft mmio handoff PREPARE (TE/RE off, IRQ masked; "
            "g_pMmio idle mapped; Soft≠product; no .ko open)\n");
#endif
}

int
rtl8168_soft_handoff_prepared(void)
{
    return g_fSoftHandoffPrepared != 0 ? 1 : 0;
}

/*
 * Quiet MEM|BM reassert for TX wait salvage (hot path — no kprintf).
 * Soft≠product.
 */
static void
rtl_pci_rearm_bm_quiet(void)
{
    u16 u16Cmd;

    if (g_fPciBdf == 0u) {
        return;
    }
    u16Cmd = pci_cfg_read16(g_u8PciBus, g_u8PciSlot, g_u8PciFunc, PCI_CMD);
    if ((u16Cmd & (u16)(PCI_CMD_MEM | PCI_CMD_BUS)) ==
        (u16)(PCI_CMD_MEM | PCI_CMD_BUS)) {
        return; /* already armed */
    }
    u16Cmd |= (u16)(PCI_CMD_MEM | PCI_CMD_BUS);
    pci_cfg_write16(g_u8PciBus, g_u8PciSlot, g_u8PciFunc, PCI_CMD, u16Cmd);
}

/*
 * Re-assert PCI MEM|BM and clear PCIe ASPM (REAL .ko probe may clear BM).
 * Soft≠product. Greppable — reclaim / kick_wire only (not TX hot path).
 */
static void
rtl_pci_rearm_bus(void)
{
    u16 u16Cmd;
    u8 u8Cap;
    u32 cGuard;

    if (g_fPciBdf == 0u) {
        return;
    }
    u16Cmd = pci_cfg_read16(g_u8PciBus, g_u8PciSlot, g_u8PciFunc, PCI_CMD);
    u16Cmd |= (u16)(PCI_CMD_MEM | PCI_CMD_BUS);
    u16Cmd &= (u16)~0x400u; /* INTx disable clear not required; keep BM */
    pci_cfg_write16(g_u8PciBus, g_u8PciSlot, g_u8PciFunc, PCI_CMD, u16Cmd);

    u8Cap = (u8)(pci_cfg_read32(g_u8PciBus, g_u8PciSlot, g_u8PciFunc, 0x34u) &
                 0xffu);
    cGuard = 0;
    while (u8Cap >= 0x40u && u8Cap < 0xfcu && cGuard < 48u) {
        u32 u32Cap =
            pci_cfg_read32(g_u8PciBus, g_u8PciSlot, g_u8PciFunc, u8Cap);
        u8 u8Id = (u8)(u32Cap & 0xffu);
        u8 u8Next = (u8)((u32Cap >> 8) & 0xffu);

        if (u8Id == 0x10u) {
            u16 u16Lnk = pci_cfg_read16(g_u8PciBus, g_u8PciSlot, g_u8PciFunc,
                                        (u8)(u8Cap + 0x10u));
            if ((u16Lnk & 3u) != 0) {
                u16Lnk = (u16)(u16Lnk & ~3u);
                pci_cfg_write16(g_u8PciBus, g_u8PciSlot, g_u8PciFunc,
                                (u8)(u8Cap + 0x10u), u16Lnk);
            }
            break;
        }
        if (u8Next == 0 || u8Next == u8Cap) {
            break;
        }
        u8Cap = u8Next;
        cGuard++;
    }
    kprintf("rtl8168: soft reclaim pci cmd=0x%x bdf=%02x:%02x.%u\n",
            (unsigned)pci_cfg_read16(g_u8PciBus, g_u8PciSlot, g_u8PciFunc,
                                     PCI_CMD),
            (unsigned)g_u8PciBus, (unsigned)g_u8PciSlot,
            (unsigned)g_u8PciFunc);
}

static void
rtl_rearm_rings_only(void)
{
    u32 i;

    memset(g_pTxDesc, 0, GJ_PAGE_SIZE);
    memset(g_pRxDesc, 0, GJ_PAGE_SIZE);
    for (i = 0; i < RTL_RING_N; i++) {
        g_pTxDesc[i].u32AddrLo = (u32)(g_aPaTxBuf[i] & 0xffffffffu);
        g_pTxDesc[i].u32AddrHi = 0; /* force32 */
        g_pTxDesc[i].u32Opts1 = 0;
        g_pTxDesc[i].u32Opts2 = 0;

        g_pRxDesc[i].u32AddrLo = (u32)(g_aPaRxBuf[i] & 0xffffffffu);
        g_pRxDesc[i].u32AddrHi = 0; /* force32 */
        g_pRxDesc[i].u32Opts1 =
            RTL_DESC_OWN | (RTL_BUF_SZ & RTL_RX_BUFLEN_MASK);
        g_pRxDesc[i].u32Opts2 = 0;
        if (i + 1u == RTL_RING_N) {
            g_pRxDesc[i].u32Opts1 |= RTL_DESC_EOR;
            g_pTxDesc[i].u32Opts1 = RTL_DESC_EOR;
        }
        rtl_clflush(&g_pTxDesc[i]);
        rtl_clflush(&g_pRxDesc[i]);
        if (g_aVaRxBuf[i] != NULL) {
            rtl_clflush(g_aVaRxBuf[i]);
        }
    }
    g_u32TxIdx = 0;
    g_u32RxIdx = 0;
}

static int
rtl_tx_selftest_once(void)
{
    u8 aTest[64];

    memset(aTest, 0xff, 6);
    memcpy(aTest + 6, g_aMac, 6);
    aTest[12] = 0x08;
    aTest[13] = 0x06;
    return rtl8168_tx(aTest, 60u);
}

/*
 * Re-post RX descriptors OWN only — leave TX ring alone (in-flight TX safe).
 * Soft≠product. Used by gentle refresh / kick_wire after SOFT hybrid.
 */
static void
rtl_rx_repost_only(void)
{
    u32 i;

    if (g_pRxDesc == NULL) {
        return;
    }
    for (i = 0; i < RTL_RING_N; i++) {
        g_pRxDesc[i].u32AddrLo = (u32)(g_aPaRxBuf[i] & 0xffffffffu);
        g_pRxDesc[i].u32AddrHi = 0; /* force32 / identity */
        g_pRxDesc[i].u32Opts2 = 0;
        g_pRxDesc[i].u32Opts1 =
            RTL_DESC_OWN | (RTL_BUF_SZ & RTL_RX_BUFLEN_MASK) |
            ((i + 1u == RTL_RING_N) ? RTL_DESC_EOR : 0u);
        rtl_clflush(&g_pRxDesc[i]);
        if (g_aVaRxBuf[i] != NULL) {
            rtl_clflush(g_aVaRxBuf[i]);
        }
    }
    g_u32RxIdx = 0;
    __asm__ volatile("mfence" ::: "memory");
}

/*
 * Gentle RX re-arm: no chip soft-reset, no TX ring wipe (photo 3283 reclaim
 * after SOFT → R0). Stop engines briefly, CPlus first, re-point RDSAR,
 * re-post RX OWN, EarlyOffV2 RxConfig, clear RxDv, RE|TE, force RxCfg after
 * RE. Soft≠product.
 */
static void
rtl_rx_gentle_refresh(void)
{
    u32 u32RxCfg;
    u32 u32Spin;
    u32 u32Misc;
    u16 u16Cp;
    u32 i;

    if (g_fSoftHandoffPrepared != 0 || g_pMmio == NULL || g_pRxDesc == NULL ||
        g_paRxDesc == 0) {
        return;
    }

    rtl_pci_rearm_bm_quiet();

    /* Stop engines only — no soft-reset (keeps EEPROM MAC / PHY state). */
    mmio_w8(RTL_CHIPCMD, 0);
    for (u32Spin = 0; u32Spin < 20000u; u32Spin++) {
        __asm__ volatile("pause");
    }

    mmio_w8(RTL_CFG9346, RTL_CFG9346_UNLOCK);
    /* CPlus first — same 8168G bring-up as program_hw. */
    u16Cp = (u16)RTL_CPLUS_8168G_BRINGUP;
    mmio_w16(RTL_CPLUSCMD, u16Cp);
    /* Station MAC + promisc MAR (accept filter). */
    for (i = 0; i < 6u; i++) {
        mmio_w8(RTL_MAC0 + i, g_aMac[i]);
    }
    for (i = 0; i < 8u; i++) {
        mmio_w8(RTL_MAR0 + i, 0xffu);
    }

    /* Re-point rings (HW may have lost them after soft path side-effects). */
    mmio_w32(RTL_TNPDS_LO, (u32)(g_paTxDesc & 0xffffffffu));
    mmio_w32(RTL_TNPDS_HI, 0); /* force32 */
    mmio_w32(RTL_RDSAR_LO, (u32)(g_paRxDesc & 0xffffffffu));
    mmio_w32(RTL_RDSAR_HI, 0); /* force32 */
    mmio_w32(0x28u, (u32)(g_paTxDesc & 0xffffffffu));
    mmio_w32(0x2Cu, 0); /* force32 */

    rtl_rx_repost_only();

    u32RxCfg = RTL_RXCFG_BRINGUP;
    mmio_w32(RTL_RXCFG, u32RxCfg);
    mmio_w16(RTL_RXMAXSIZE, 16383u);
    mmio_w16(RTL_INTRMITI, 0);
    mmio_w8(RTL_CONFIG1,
            (u8)((mmio_r8(RTL_CONFIG1) & (u8)~0x01u) | RTL_CFG1_DRVLOAD));
    mmio_w8(RTL_CONFIG2, (u8)(mmio_r8(RTL_CONFIG2) & (u8)~0x80u));
    mmio_w8(RTL_CONFIG5, (u8)(mmio_r8(RTL_CONFIG5) & (u8)~0x19u));
    mmio_w8(RTL_DLLPR, (u8)(mmio_r8(RTL_DLLPR) & (u8)~0x40u));
    u32Misc = mmio_r32(RTL_MISC);
    u32Misc &= ~(1u << 19);
    mmio_w32(RTL_MISC, u32Misc);
    (void)mmio_r32(RTL_RXMISS);
    mmio_w16(RTL_INTRSTATUS, 0xffffu);
    mmio_w8(RTL_CFG9346, RTL_CFG9346_LOCK);

    mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    for (u32Spin = 0; u32Spin < 10000u; u32Spin++) {
        __asm__ volatile("pause");
    }
    mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    /* CPlus + accept bits after RE (8168G sample order). */
    mmio_w16(RTL_CPLUSCMD, u16Cp);
    mmio_w32(RTL_RXCFG, u32RxCfg);
}

/*
 * Soft hybrid (EMU): BM/ASPM + one-shot full ring re-init + program_hw
 * (no chip soft-reset — that path after SOFT killed RX, photo 3283).
 * Stronger than gentle-only: re-OWNs TX/RX + full CPlus/RxCfg. Soft≠product.
 * Grep: rtl8168: soft kick wire
 */
void
rtl8168_kick_wire(void)
{
    u8 u8Cmd;
    u8 u8Re;
    u32 u32Spin;
    u32 u32Rdsar;
    u32 u32FirstOpts;

    if (g_fSoftHandoffPrepared != 0 || g_pMmio == NULL || g_fReady == 0) {
        kprintf("rtl8168: soft kick wire SKIP\n");
        return;
    }
    rtl_pci_rearm_bus();

    /*
     * One-shot: stop engines, rearm both rings, full program_hw (CPlus first,
     * EarlyOffV2, force RxCfg after RE). No soft-reset. Soft≠product.
     */
    mmio_w8(RTL_CHIPCMD, 0);
    for (u32Spin = 0; u32Spin < 20000u; u32Spin++) {
        __asm__ volatile("pause");
    }
    rtl_rearm_rings_only();
    rtl_program_hw();
    g_fReady = 1;

    u8Cmd = mmio_r8(RTL_CHIPCMD);
    u8Re = (u8)((u8Cmd & RTL_CHIPCMD_RE) != 0 ? 1u : 0u);
    u32Rdsar = mmio_r32(RTL_RDSAR_LO);
    u32FirstOpts = (g_pRxDesc != NULL) ? g_pRxDesc[0].u32Opts1 : 0u;
    kprintf("rtl8168: soft kick wire PASS cmd=0x%x re=%u cplus=0x%x "
            "rxcfg=0x%x rdsar=0x%x first_opts1=0x%x link=%u "
            "(full program_hw no soft-reset; Soft≠product)\n",
            (unsigned)u8Cmd, (unsigned)u8Re,
            (unsigned)mmio_r16(RTL_CPLUSCMD), (unsigned)mmio_r32(RTL_RXCFG),
            (unsigned)u32Rdsar, (unsigned)u32FirstOpts,
            (mmio_r8(RTL_PHYSTATUS) & RTL_PHYSTATUS_LINK) != 0 ? 1u : 0u);
}

/*
 * Re-arm freestanding rings after soft REAL r8169 probe stole MMIO.
 * Soft≠product; hybrid 4a only (gate0 freestanding wire).
 * Grep: rtl8168: soft reclaim wire
 */
int
rtl8168_reclaim_wire(void)
{
    u32 i;
    int nTx;
    u32 u32BusyBefore;
    u8 aMacSave[6];
    int fMacOk;

    if (g_fSoftHandoffPrepared != 0) {
        /* Grep: rtl8168: soft reclaim wire SKIP (handoff prepared) */
        kprintf("rtl8168: soft reclaim wire SKIP (handoff prepared)\n");
        return -1;
    }
    if (g_pMmio == NULL || g_pTxDesc == NULL || g_pRxDesc == NULL ||
        g_paTxDesc == 0 || g_paRxDesc == 0) {
        /* Grep: rtl8168: soft reclaim wire SKIP (no rings) */
        kprintf("rtl8168: soft reclaim wire SKIP (no rings)\n");
        return -1;
    }
    for (i = 0; i < RTL_RING_N; i++) {
        if (g_aPaTxBuf[i] == 0 || g_aPaRxBuf[i] == 0 ||
            g_aVaTxBuf[i] == NULL || g_aVaRxBuf[i] == NULL) {
            kprintf("rtl8168: soft reclaim wire FAIL (buf i=%u)\n",
                    (unsigned)i);
            return -1;
        }
    }

    u32BusyBefore = g_u32TxBusy;
    memcpy(aMacSave, g_aMac, 6);

    /* 1) PCI BM + ASPM — .ko probe often leaves BM cleared / ASPM on. */
    rtl_pci_rearm_bus();

    /*
     * 2) Gentle reclaim first (no chip soft-reset): stop engines, re-point
     * rings, program_hw. Full soft-reset can blank EEPROM MAC and leave R0
     * if we re-read zeros (photo 3271). Soft≠product.
     */
    mmio_w8(RTL_CHIPCMD, 0);
    for (i = 0; i < 50000u; i++) {
        __asm__ volatile("pause");
    }
    mmio_w16(RTL_INTRMASK, 0);
    mmio_w16(RTL_INTRSTATUS, 0xffffu);

    rtl_rearm_rings_only();
    /* Prefer saved station MAC (pre-REAL) over post-stop read. */
    if (rtl_mac_ok(aMacSave) != 0) {
        memcpy(g_aMac, aMacSave, 6);
    } else {
        for (i = 0; i < 6u; i++) {
            g_aMac[i] = mmio_r8(RTL_MAC0 + i);
        }
    }
    rtl_program_hw();
    g_fReady = 1;
    g_u32TxBusy = 0;

    nTx = rtl_tx_selftest_once();
    if (nTx != 0) {
        /* Brief settle + second kick. */
        for (i = 0; i < 200000u; i++) {
            __asm__ volatile("pause");
        }
        rtl_program_hw();
        g_fReady = 1;
        nTx = rtl_tx_selftest_once();
    }

    /*
     * 3) If gentle path still fails: full soft-reset + rearm (keep MAC save).
     */
    if (nTx != 0) {
        kprintf("rtl8168: soft reclaim wire gentle FAIL → full soft-reset\n");
        rtl_pci_rearm_bus();
        if (rtl_soft_reset() != 0) {
            kprintf("rtl8168: soft reclaim wire FAIL (soft reset)\n");
            return -1;
        }
        for (i = 0; i < 500000u; i++) {
            __asm__ volatile("pause");
        }
        for (i = 0; i < 6u; i++) {
            g_aMac[i] = mmio_r8(RTL_MAC0 + i);
        }
        fMacOk = rtl_mac_ok(g_aMac);
        if (fMacOk == 0 && rtl_mac_ok(aMacSave) != 0) {
            memcpy(g_aMac, aMacSave, 6);
            kprintf("rtl8168: soft reclaim wire MAC restore "
                    "%02x:%02x:%02x:%02x:%02x:%02x\n",
                    g_aMac[0], g_aMac[1], g_aMac[2], g_aMac[3], g_aMac[4],
                    g_aMac[5]);
        }
        rtl_rearm_rings_only();
        rtl_program_hw();
        g_fReady = 1;
        g_u32TxBusy = 0;
        nTx = rtl_tx_selftest_once();
        if (nTx != 0) {
            for (i = 0; i < 200000u; i++) {
                __asm__ volatile("pause");
            }
            nTx = rtl_tx_selftest_once();
        }
    }

    /* Grep: rtl8168: soft reclaim wire PASS|FAIL */
    kprintf("rtl8168: soft reclaim wire %s busy_before=%u selftest=%s "
            "tx=%u busy=%u link=%u mac=%02x:%02x:%02x:%02x:%02x:%02x "
            "(hybrid freestanding after REAL probe; Soft≠product)\n",
            nTx == 0 ? "PASS" : "FAIL", (unsigned)u32BusyBefore,
            nTx == 0 ? "PASS" : "FAIL", (unsigned)g_u32TxCount,
            (unsigned)g_u32TxBusy,
            (mmio_r8(RTL_PHYSTATUS) & RTL_PHYSTATUS_LINK) != 0 ? 1u : 0u,
            g_aMac[0], g_aMac[1], g_aMac[2], g_aMac[3], g_aMac[4], g_aMac[5]);

    return nTx == 0 ? 0 : -1;
}

int
rtl8168_link_up(void)
{
    if (g_fReady == 0 || g_pMmio == NULL) {
        return 0;
    }
    return (mmio_r8(RTL_PHYSTATUS) & RTL_PHYSTATUS_LINK) != 0 ? 1 : 0;
}

int
rtl8168_tx(const void *pFrame, u32 cbLen)
{
    struct rtl_desc *pD;
    u32 u32Idx;
    u32 u32Opts;
    u32 u32Flush;
    u32 u32Scan;
    u32 u32Alt;
    u8 u8Cmd;

    /* Handoff: never TE|RE re-assert after prepare (even if g_fReady corrupt). */
    if (g_fSoftHandoffPrepared != 0 || g_fReady == 0 || g_pMmio == NULL ||
        pFrame == NULL || cbLen < 14u || cbLen > 1514u) {
        if (g_u32TxFail < 0xffffffffu) {
            g_u32TxFail++;
        }
        return -1;
    }

    /* Ensure engines are on without thrashing (see rtl_tx_kick). */
    u8Cmd = mmio_r8(RTL_CHIPCMD);
    if ((u8Cmd & (RTL_CHIPCMD_RE | RTL_CHIPCMD_TE)) !=
        (RTL_CHIPCMD_RE | RTL_CHIPCMD_TE)) {
        mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    }

    /*
     * Find a free TX slot. Prefer head (g_u32TxIdx). If OWN still set after
     * a bounded wait, scan for another free slot — do NOT soft-clear OWN
     * (race with NIC DMA; photo-class B### after soft OWN poke). Soft≠product.
     */
    u32Idx = g_u32TxIdx;
    pD = &g_pTxDesc[u32Idx];
    rtl_clflush(pD);
    __asm__ volatile("mfence" ::: "memory");
    if ((pD->u32Opts1 & RTL_DESC_OWN) != 0) {
        rtl_tx_kick();
        if (rtl_tx_wait_own_clear(pD, 120000u) != 0) {
            for (u32Scan = 1u; u32Scan < RTL_RING_N; u32Scan++) {
                u32Alt = (u32Idx + u32Scan) % RTL_RING_N;
                pD = &g_pTxDesc[u32Alt];
                rtl_clflush(pD);
                __asm__ volatile("mfence" ::: "memory");
                if ((pD->u32Opts1 & RTL_DESC_OWN) == 0) {
                    u32Idx = u32Alt;
                    break;
                }
            }
            pD = &g_pTxDesc[u32Idx];
            rtl_clflush(pD);
            if ((pD->u32Opts1 & RTL_DESC_OWN) != 0) {
                /* Ring full — one salvage kick+BM+wait on head only. */
                rtl_pci_rearm_bm_quiet();
                rtl_tx_kick();
                if (rtl_tx_wait_own_clear(pD, 200000u) != 0) {
                    if (g_u32TxBusy < 0xffffffffu) {
                        g_u32TxBusy++;
                    }
                    return -1;
                }
            }
        }
    }

    memcpy(g_aVaTxBuf[u32Idx], pFrame, cbLen);
    if (cbLen < 60u) {
        memset((u8 *)g_aVaTxBuf[u32Idx] + cbLen, 0, 60u - cbLen);
        cbLen = 60u;
    }
    for (u32Flush = 0; u32Flush < cbLen; u32Flush += 64u) {
        rtl_clflush((u8 *)g_aVaTxBuf[u32Idx] + u32Flush);
    }

    /*
     * TX desc: OWN|FS|LS|len. Force addr_hi=0 when PA < 4GiB so chips
     * without DAC still fetch the buffer.
     */
    pD->u32AddrLo = (u32)(g_aPaTxBuf[u32Idx] & 0xffffffffu);
    pD->u32AddrHi = 0; /* force32 / VT-d identity */
    pD->u32Opts2 = 0;
    u32Opts = RTL_DESC_OWN | RTL_DESC_FS | RTL_DESC_LS | (cbLen & 0x3fffu);
    if (u32Idx + 1u == RTL_RING_N) {
        u32Opts |= RTL_DESC_EOR;
    }
    __asm__ volatile("mfence" ::: "memory");
    pD->u32Opts1 = u32Opts;
    rtl_clflush(pD);
    __asm__ volatile("mfence" ::: "memory");

    rtl_tx_kick();

    /*
     * Wait for OWN clear so the ring does not fill with never-completing
     * descs (G752 b90 / photo 3287 B###). Budget is generous; kicks are
     * sparse (no CHIPCMD thrash). Soft≠product.
     */
    if (rtl_tx_wait_own_clear(pD, 300000u) != 0) {
        /* Last chance: quiet BM rearm + one more wait. */
        rtl_pci_rearm_bm_quiet();
        rtl_tx_kick();
        if (rtl_tx_wait_own_clear(pD, 200000u) != 0) {
            if (g_u32TxBusy < 0xffffffffu) {
                g_u32TxBusy++;
            }
            /* Leave OWN for NIC; do not advance producer (retry same slot). */
            return -1;
        }
    }

    g_u32TxIdx = (u32Idx + 1u) % RTL_RING_N;
    if (g_u32TxCount < 0xffffffffu) {
        g_u32TxCount++;
    }
    return 0;
}

/*
 * RX status bits (public RTL81xx class — overlap length field carefully):
 *   bit 21 RWT / RES class, bit 22 ROR (FIFO overflow) → drop + re-post.
 * 8168G DESCV2 frame length = bits 13:0 (GFRAGLEN 0x3FFF); often includes FCS.
 */
#define RTL_RX_RES (1u << 21)
#define RTL_RX_ROR (1u << 22)
#define RTL_RX_LEN_MASK RTL_RX_FRAMELEN_MASK

i32
rtl8168_rx(void *pOut, u32 cbMax)
{
    struct rtl_desc *pD;
    u32 u32Idx;
    u32 u32Opts;
    u32 u32Len;
    u32 u32Scan;
    u32 u32Flush;
    int fGood = 0;

    /* Handoff: freestanding rings idle after prepare (no dual-drive). */
    if (g_fSoftHandoffPrepared != 0 || g_fReady == 0 || pOut == NULL ||
        cbMax < 14u) {
        return -1;
    }

    /*
     * Walk up to a full ring from g_u32RxIdx so a stuck/bad slot cannot
     * permanently hide later completions (G752 r stuck at 1).
     */
    for (u32Scan = 0; u32Scan < RTL_RING_N; u32Scan++) {
        u32Idx = (g_u32RxIdx + u32Scan) % RTL_RING_N;
        pD = &g_pRxDesc[u32Idx];
        rtl_clflush(pD);
        /* Force re-read of both dwords after invalidate. */
        __asm__ volatile("mfence" ::: "memory");
        u32Opts = pD->u32Opts1;
        if ((u32Opts & RTL_DESC_OWN) != 0) {
            if (u32Scan == 0u) {
                if (g_u32RxEmpty < 0xffffffffu) {
                    g_u32RxEmpty++;
                }
                /*
                 * One-shot lamp when empty with link (OWN-stuck audit).
                 * Grep: rtl8168: soft rx empty n=… cmd=… re=… first_opts1=… rdsar=…
                 */
                if (g_u32RxAuditN < 1u &&
                    (mmio_r8(RTL_PHYSTATUS) & RTL_PHYSTATUS_LINK) != 0) {
                    u8 u8CmdA = mmio_r8(RTL_CHIPCMD);
                    u8 u8ReA = (u8)((u8CmdA & RTL_CHIPCMD_RE) != 0 ? 1u : 0u);
                    u32 u32RdsarLo = mmio_r32(RTL_RDSAR_LO);
                    u32 u32First = (g_pRxDesc != NULL) ? g_pRxDesc[0].u32Opts1
                                                      : u32Opts;

                    g_u32RxAuditN++;
                    /* Grep: rtl8168: soft rx empty */
                    kprintf("rtl8168: soft rx empty n=%u cmd=0x%x re=%u "
                            "first_opts1=0x%x rdsar=0x%x cplus=0x%x "
                            "rxcfg=0x%x (Soft≠product)\n",
                            (unsigned)g_u32RxEmpty, (unsigned)u8CmdA,
                            (unsigned)u8ReA, (unsigned)u32First,
                            (unsigned)u32RdsarLo,
                            (unsigned)mmio_r16(RTL_CPLUSCMD),
                            (unsigned)mmio_r32(RTL_RXCFG));
                }
            }
            continue; /* this slot still NIC-owned */
        }

        /* Consume this completed descriptor (advance base to it first). */
        g_u32RxIdx = u32Idx;

        /*
         * Length bits 13:0 on 8168G DESCV2 (GFRAGLEN). FCS may be included —
         * do NOT strip for demux: IP TotalLength bounds the payload; trailing
         * CRC is ignored. ROR → drop. RES alone with sane length: still accept.
         */
        u32Len = u32Opts & RTL_RX_LEN_MASK;
        if ((u32Opts & RTL_RX_ROR) != 0) {
            if (g_u32RxDrop < 0xffffffffu) {
                g_u32RxDrop++;
            }
            goto repost;
        }
        if (u32Len < 14u || u32Len > cbMax || u32Len > RTL_BUF_SZ) {
            if (g_u32RxDrop < 0xffffffffu) {
                g_u32RxDrop++;
            }
            if (g_u32RxDrop <= 4u) {
                kprintf("rtl8168: rx drop opts=0x%x len=%u idx=%u\n",
                        (unsigned)u32Opts, (unsigned)u32Len,
                        (unsigned)u32Idx);
            }
            goto repost;
        }

        for (u32Flush = 0; u32Flush < u32Len; u32Flush += 64u) {
            rtl_clflush((u8 *)g_aVaRxBuf[u32Idx] + u32Flush);
        }
        memcpy(pOut, g_aVaRxBuf[u32Idx], u32Len);
        if (g_u32RxCount < 0xffffffffu) {
            g_u32RxCount++;
        }
        fGood = 1;

repost:
        /* Re-post: CMD_BUFLEN (0x1FFF), not GFRAGLEN. */
        u32Opts = RTL_DESC_OWN | (RTL_BUF_SZ & RTL_RX_BUFLEN_MASK);
        if (u32Idx + 1u == RTL_RING_N) {
            u32Opts |= RTL_DESC_EOR;
        }
        pD->u32Opts2 = 0;
        pD->u32AddrLo = (u32)(g_aPaRxBuf[u32Idx] & 0xffffffffu);
        pD->u32AddrHi = 0; /* force32 / VT-d identity */
        __asm__ volatile("mfence" ::: "memory");
        pD->u32Opts1 = u32Opts;
        rtl_clflush(pD);

        g_u32RxIdx = (u32Idx + 1u) % RTL_RING_N;
        if (fGood != 0) {
            return (i32)u32Len;
        }
        /* Bad frame re-posted — keep scanning for a good one. */
        fGood = 0;
        u32Len = 0;
    }
    return -1;
}

void
rtl8168_mac(u8 *pMac)
{
    u32 i;

    if (pMac == NULL) {
        return;
    }
    for (i = 0; i < 6u; i++) {
        pMac[i] = g_fReady != 0 ? g_aMac[i] : 0;
    }
}

u32
rtl8168_tx_count(void)
{
    return g_u32TxCount;
}

u32
rtl8168_rx_count(void)
{
    return g_u32RxCount;
}

u32
rtl8168_tx_fail(void)
{
    return g_u32TxFail;
}

u32
rtl8168_tx_busy(void)
{
    return g_u32TxBusy;
}

u32
rtl8168_rx_drop(void)
{
    return g_u32RxDrop;
}
