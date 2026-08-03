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
 */
#include <gj/config.h>
#include <gj/dma_buf.h>
#include <gj/error.h>
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

/* Desc flags (public; own + eor + fs/ls) */
#define RTL_DESC_OWN (1u << 31)
#define RTL_DESC_EOR (1u << 30)
#define RTL_DESC_FS  (1u << 29)
#define RTL_DESC_LS  (1u << 28)

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
static u8 g_aMac[6];
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

static int
rtl_alloc_rings(void)
{
    u32 i;
    void *pVa = NULL;

    g_paTxDesc = dma_buf_alloc_page(&pVa);
    g_pTxDesc = (struct rtl_desc *)pVa;
    g_paRxDesc = dma_buf_alloc_page(&pVa);
    g_pRxDesc = (struct rtl_desc *)pVa;
    if (g_paTxDesc == 0 || g_paRxDesc == 0 || g_pTxDesc == NULL ||
        g_pRxDesc == NULL) {
        kprintf("rtl8168: desc dma FAIL\n");
        return -1;
    }
    memset(g_pTxDesc, 0, GJ_PAGE_SIZE);
    memset(g_pRxDesc, 0, GJ_PAGE_SIZE);

    for (i = 0; i < RTL_RING_N; i++) {
        g_aPaTxBuf[i] = dma_buf_alloc_page(&g_aVaTxBuf[i]);
        g_aPaRxBuf[i] = dma_buf_alloc_page(&g_aVaRxBuf[i]);
        if (g_aPaTxBuf[i] == 0 || g_aPaRxBuf[i] == 0) {
            kprintf("rtl8168: buf dma FAIL i=%u\n", (unsigned)i);
            return -1;
        }
        memset(g_aVaTxBuf[i], 0, GJ_PAGE_SIZE);
        memset(g_aVaRxBuf[i], 0, GJ_PAGE_SIZE);

        g_pTxDesc[i].u32AddrLo = (u32)(g_aPaTxBuf[i] & 0xffffffffu);
        g_pTxDesc[i].u32AddrHi = (u32)((g_aPaTxBuf[i] < 0x100000000ull)
                                           ? 0u
                                           : (g_aPaTxBuf[i] >> 32));
        g_pTxDesc[i].u32Opts1 = 0;
        g_pTxDesc[i].u32Opts2 = 0;

        g_pRxDesc[i].u32AddrLo = (u32)(g_aPaRxBuf[i] & 0xffffffffu);
        /* Match TX: force addr_hi=0 when PA < 4GiB (non-DAC / IOMMU identity). */
        g_pRxDesc[i].u32AddrHi = (u32)((g_aPaRxBuf[i] < 0x100000000ull)
                                           ? 0u
                                           : (g_aPaRxBuf[i] >> 32));
        g_pRxDesc[i].u32Opts1 = RTL_DESC_OWN | (RTL_BUF_SZ & 0x1fffu);
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
    return 0;
}

static void
rtl_program_hw(void)
{
    u32 i;
    u32 u32RxCfg;
    u16 u16Cp;
    u32 u32Spin;

    /* Stop engines before programming rings (required on many 8168G). */
    mmio_w8(RTL_CHIPCMD, 0);
    for (u32Spin = 0; u32Spin < 10000u; u32Spin++) {
        __asm__ volatile("pause");
    }

    /* Unlock config space (required before MAC/Rx/Tx cfg on many 8168). */
    mmio_w8(RTL_CFG9346, RTL_CFG9346_UNLOCK);

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

    /* Desc rings — 64-bit DMA addresses (PCIe). TE/RE must be off here. */
    mmio_w32(RTL_TNPDS_LO, (u32)(g_paTxDesc & 0xffffffffu));
    mmio_w32(RTL_TNPDS_HI, (u32)(g_paTxDesc >> 32));
    mmio_w32(RTL_RDSAR_LO, (u32)(g_paRxDesc & 0xffffffffu));
    mmio_w32(RTL_RDSAR_HI, (u32)(g_paRxDesc >> 32));
    /* High-priority TX ring = same (some revs poll HPQ). */
    mmio_w32(0x28u, (u32)(g_paTxDesc & 0xffffffffu)); /* THPDS lo */
    mmio_w32(0x2Cu, (u32)(g_paTxDesc >> 32));

    /*
     * RxConfig: promisc + bcast/mcast + my + runt + unlimited DMA.
     * FIFO thresh = none (bits 15:13 = 7) so small unicast ICMP is not held.
     * 0x0000E70F is a common bring-up mask on 8168G/8111G class.
     */
    u32RxCfg = 0x0000E70Fu | RTL_RX_ACCEPT_ERR | RTL_RX_ACCEPT_RUNT |
               RTL_RX_ACCEPT_BCAST | RTL_RX_ACCEPT_MCAST | RTL_RX_ACCEPT_MY |
               RTL_RX_ACCEPT_ALL;
    mmio_w32(RTL_RXCFG, u32RxCfg);
    /*
     * TxConfig: IFG normal + DMA unlimited. Also set bit 0 (empty?) clear.
     * 0x03000700 is the classic IFG=3 / MXDMA=7 bring-up value.
     */
    mmio_w32(RTL_TXCFG, 0x03000700u);
    /* Max RX packet size — must be ≥ MTU+headers; 0x1FFF = 8KiB-1 class. */
    mmio_w16(RTL_RXMAXSIZE, 0x1fffu);
    /* Early TX threshold: 0x3F = store-and-forward (no cut-through). */
    mmio_w8(0xECu, 0x3Fu);
    /* Max TX packet size (8168G): 0x3F ≈ no limit class. */
    mmio_w8(0xEEu, 0x3Fu);

    u16Cp = mmio_r16(RTL_CPLUSCMD);
    u16Cp |= (1u << 3);        /* PCIMulRW */
    u16Cp &= (u16) ~(1u << 5); /* RxChkSum off — avoid bogus csum flags */
    u16Cp &= (u16) ~(1u << 6); /* RxVlan off */
    /* Bit 0 = RxVlanDisable on some maps; keep RxVlan off via bit 6 above. */
    mmio_w16(RTL_CPLUSCMD, u16Cp);

    mmio_w8(RTL_CONFIG1, (u8)(mmio_r8(RTL_CONFIG1) & (u8)~0x01u));
    /* Clear missed-packet counter (some revs sticky until read). */
    (void)*(volatile u32 *)(void *)(g_pMmio + RTL_RXMISS);
    mmio_w16(RTL_INTRSTATUS, 0xffffu);
    mmio_w8(RTL_CFG9346, RTL_CFG9346_LOCK);

    /* Enable RX+TX engines together. */
    mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    for (u32Spin = 0; u32Spin < 10000u; u32Spin++) {
        __asm__ volatile("pause");
    }
    /* Re-assert RE after a beat — 8168G can drop RE if rings race enable. */
    mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);

    kprintf("rtl8168: hw cmd=0x%x rxcfg=0x%x txcfg=0x%x cplus=0x%x phy=0x%x "
            "tnpds=0x%x:%x rdsar=0x%x:%x\n",
            (unsigned)mmio_r8(RTL_CHIPCMD),
            (unsigned)(*(volatile u32 *)(void *)(g_pMmio + RTL_RXCFG)),
            (unsigned)(*(volatile u32 *)(void *)(g_pMmio + RTL_TXCFG)),
            (unsigned)u16Cp, (unsigned)mmio_r8(RTL_PHYSTATUS),
            (unsigned)(g_paTxDesc >> 32), (unsigned)(u32)g_paTxDesc,
            (unsigned)(g_paRxDesc >> 32), (unsigned)(u32)g_paRxDesc);
}

/* Keep RX engine alive; clear sticky ISR. Call from net_eth_poll. */
void
rtl8168_poll_hw(void)
{
    u16 u16Isr;
    u8 u8Cmd;
    static u32 s_u32EmptyKick;

    if (g_fReady == 0 || g_pMmio == NULL) {
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
    /* Soft: if RX starved with link up, re-assert RE + flush first RX desc. */
    s_u32EmptyKick++;
    if ((s_u32EmptyKick & 255u) == 0u &&
        (mmio_r8(RTL_PHYSTATUS) & RTL_PHYSTATUS_LINK) != 0) {
        mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
        if (g_pRxDesc != NULL) {
            rtl_clflush(&g_pRxDesc[g_u32RxIdx % RTL_RING_N]);
        }
    }
}

/* Kick both TX poll registers used across 8168 revisions. */
static void
rtl_tx_kick(void)
{
    mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    /* TPPOLL 0xD9: bit0=HPQ, bit6=NPQ (public 8168 map). */
    mmio_w8(RTL_TPPOLL, (u8)(0x01u | RTL_TPPOLL_NPQ));
    mmio_w8(0x38u, 0x40u); /* older TxPoll */
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

    /* Soft VT-d identity for this BDF (G752 Translated domain). */
    {
        extern int iommu_vtd_xhci_identity(u8 bus, u8 slot, u8 func);
        int nId;

        nId = iommu_vtd_xhci_identity(u8Bus, u8Slot, u8Func);
        kprintf("rtl8168: iommu identity bdf=%02x:%02x.%u ret=%d\n",
                (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func, nId);
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
         */
        {
            int fTxId =
                dma_buf_in_vtd_identity(g_paTxDesc, (u64)GJ_PAGE_SIZE);
            int fRxId =
                dma_buf_in_vtd_identity(g_paRxDesc, (u64)GJ_PAGE_SIZE);
            int fBufId =
                dma_buf_in_vtd_identity(g_aPaRxBuf[0], (u64)GJ_PAGE_SIZE);

            kprintf("rtl8168: dma desc_tx=0x%lx in_id=%d desc_rx=0x%lx "
                    "in_id=%d buf0=0x%lx in_id=%d\n",
                    (unsigned long)g_paTxDesc, fTxId,
                    (unsigned long)g_paRxDesc, fRxId,
                    (unsigned long)g_aPaRxBuf[0], fBufId);
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
    u32 u32Spin;
    u32 u32Flush;
    u8 u8Cmd;

    if (g_fReady == 0 || g_pMmio == NULL || pFrame == NULL || cbLen < 14u ||
        cbLen > 1514u) {
        if (g_u32TxFail < 0xffffffffu) {
            g_u32TxFail++;
        }
        return -1;
    }

    /* Ensure engines are on without thrashing. */
    u8Cmd = mmio_r8(RTL_CHIPCMD);
    if ((u8Cmd & (RTL_CHIPCMD_RE | RTL_CHIPCMD_TE)) !=
        (RTL_CHIPCMD_RE | RTL_CHIPCMD_TE)) {
        mmio_w8(RTL_CHIPCMD, RTL_CHIPCMD_RE | RTL_CHIPCMD_TE);
    }

    /*
     * Reclaim: walk from next slots that may have completed (OWN clear) so
     * producer does not stampede a full ring (G752 f450 storm).
     */
    {
        u32 u32Reclaim;
        u32 u32Scan;

        for (u32Scan = 0; u32Scan < RTL_RING_N; u32Scan++) {
            u32Reclaim = (g_u32TxIdx + u32Scan) % RTL_RING_N;
            pD = &g_pTxDesc[u32Reclaim];
            rtl_clflush(pD);
            if ((pD->u32Opts1 & RTL_DESC_OWN) != 0) {
                break;
            }
        }
    }

    u32Idx = g_u32TxIdx;
    pD = &g_pTxDesc[u32Idx];
    rtl_clflush(pD);
    if ((pD->u32Opts1 & RTL_DESC_OWN) != 0) {
        /*
         * Ring full (OWN still set): brief kick+wait. On timeout count
         * tx_busy, not tx_fail — OpenBSD/if_re busy ≠ hard error.
         */
        rtl_tx_kick();
        for (u32Spin = 0; u32Spin < 20000u; u32Spin++) {
            rtl_clflush(pD);
            if ((pD->u32Opts1 & RTL_DESC_OWN) == 0) {
                break;
            }
            __asm__ volatile("pause");
        }
        if ((pD->u32Opts1 & RTL_DESC_OWN) != 0) {
            if (g_u32TxBusy < 0xffffffffu) {
                g_u32TxBusy++;
            }
            return -1;
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
    pD->u32AddrHi = (u32)((g_aPaTxBuf[u32Idx] < 0x100000000ull)
                              ? 0u
                              : (g_aPaTxBuf[u32Idx] >> 32));
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
     * descs (G752 b90). Bounded wait — not the old 500k-spin tax every packet.
     */
    for (u32Spin = 0; u32Spin < 150000u; u32Spin++) {
        rtl_clflush(pD);
        if ((pD->u32Opts1 & RTL_DESC_OWN) == 0) {
            break;
        }
        if ((u32Spin & 0x1fffu) == 0u) {
            rtl_tx_kick();
        }
        __asm__ volatile("pause");
    }
    if ((pD->u32Opts1 & RTL_DESC_OWN) != 0) {
        if (g_u32TxBusy < 0xffffffffu) {
            g_u32TxBusy++;
        }
        /* Leave OWN for NIC; do not advance producer (retry same slot). */
        return -1;
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
 * Length is bits 12:0 (0x1FFF) on 8168G; often includes FCS.
 */
#define RTL_RX_RES (1u << 21)
#define RTL_RX_ROR (1u << 22)
#define RTL_RX_LEN_MASK 0x1fffu

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

    if (g_fReady == 0 || pOut == NULL || cbMax < 14u) {
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
                 * Soft: first few empty polls with link up — dump opts1 so
                 * OWN-stuck is greppable without flooding serial.
                 * Every 1024 empty hits, max 8 lines.
                 */
                /* One-shot only — multi-line spam hid STATUS on panel. */
                if (g_u32RxAuditN < 1u &&
                    (mmio_r8(RTL_PHYSTATUS) & RTL_PHYSTATUS_LINK) != 0) {
                    g_u32RxAuditN++;
                    kprintf("rtl8168: rx audit opts1=0x%x idx=%u link=1\n",
                            (unsigned)u32Opts, (unsigned)u32Idx);
                }
            }
            continue; /* this slot still NIC-owned */
        }

        /* Consume this completed descriptor (advance base to it first). */
        g_u32RxIdx = u32Idx;

        /*
         * Length bits 12:0. FCS may be included — do NOT strip for demux:
         * IP TotalLength bounds the payload; trailing CRC is ignored.
         * ROR (FIFO overflow) → drop. RES alone with a sane length: still
         * accept (some 8168 revs set RES spuriously on good unicast).
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
        u32Opts = RTL_DESC_OWN | (RTL_BUF_SZ & RTL_RX_LEN_MASK);
        if (u32Idx + 1u == RTL_RING_N) {
            u32Opts |= RTL_DESC_EOR;
        }
        pD->u32Opts2 = 0;
        pD->u32AddrLo = (u32)(g_aPaRxBuf[u32Idx] & 0xffffffffu);
        pD->u32AddrHi = (u32)((g_aPaRxBuf[u32Idx] < 0x100000000ull)
                                  ? 0u
                                  : (g_aPaRxBuf[u32Idx] >> 32));
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
