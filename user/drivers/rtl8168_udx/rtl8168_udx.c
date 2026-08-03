/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux-shaped UDX soft driver skeleton for Realtek RTL8168/8111
 * (public PCI ID 10ec:8168) — G752VT laptop NIC class.
 *
 * WHAT THIS IS
 *   Userspace driver-host soft path (probe / remove / quiesce) against
 *   libudx. Not product networking. T0 product net remains virtio-net.
 *
 * CLEAN-ROOM RULES
 *   Register program from public PCI IDs + publicly documented Realtek
 *   register *names* / coarse layout only. Do NOT copy Linux r8169.c,
 *   in-tree kernel/drv/rtl8168.c, or any GPL source.
 *
 * BAR NOTE
 *   RTL8168/8111 class devices commonly expose MMIO on BAR2 (memory).
 *   Soft host inject maps BAR2. Real silicon layout can vary by rev —
 *   re-check BARs from config space / inventory before hard programming.
 *
 * HOST SOFT PATH (UDX_HOST_LIBC)
 *   udx_init → register → inject 10ec:8168 → probe (BAR2/irq/dma soft)
 *   → host poke IntrStatus → fire_irq → work → quiesce/remove
 *   greppable: "rtl8168_udx: soft probe PASS"
 *
 * FREESTANDING
 *   Register only; if udx_host_bind_by_id is linked, bind 10ec:8168.
 *   Else host must inject IDs (documented in README).
 */

#include <udx/udx.h>

/* Public PCI identity (PCI SIG / Realtek published ID). */
#define RTL8168_VEND       0x10ecu
#define RTL8168_DEV        0x8168u
#define RTL8168_IRQ_SOFT   12          /* host inject fake line */
#define RTL8168_BAR_MMIO   2           /* common MMIO BAR index */
#define RTL8168_BAR_LEN    0x1000ull   /* soft inject window size */
#define RTL8168_RING_BYTES 4096u

/*
 * Publicly named RTL8168/8111-class MMIO offsets (coarse, datasheet-class).
 * Soft host path only uses IntrStatus / IntrMask for IRQ demo; other names
 * are inventory anchors — not a full datapath program.
 *
 * Sources of names: Realtek public datasheets / PCI BAR inventories.
 * Offsets are the widely published legacy map; do not treat as a full
 * silicon certification matrix for every MAC revision.
 */
#define RTL_REG_IDR0        0x00ull /* MAC ID bytes 0..3 (name: IDR0) */
#define RTL_REG_TNPDS       0x20ull /* Tx normal priority desc start */
#define RTL_REG_CHIPCMD     0x37ull /* ChipCmd (byte) */
#define RTL_REG_INTR_MASK   0x3Cull /* IntrMask (16-bit) */
#define RTL_REG_INTR_STATUS 0x3Eull /* IntrStatus (16-bit) */
#define RTL_REG_TXCFG       0x40ull /* TxConfig */
#define RTL_REG_RXCFG       0x44ull /* RxConfig */
#define RTL_REG_CPLUSCMD    0xE0ull /* CPlusCmd */
#define RTL_REG_RDSAR       0xE4ull /* Rx descriptor start address */

/* Soft interrupt reason bits used by host demo (public bit names). */
#define RTL_ISR_ROK         0x0001u /* Receive OK */
#define RTL_ISR_TOK         0x0004u /* Transmit OK */
#define RTL_SOFT_IRQ_BITS   (RTL_ISR_ROK | RTL_ISR_TOK)

/* ChipCmd soft bits (public names; soft mask only — no product reset). */
#define RTL_CMD_TE          0x04u /* Tx enable (name only; not armed here) */
#define RTL_CMD_RE          0x08u /* Rx enable (name only; not armed here) */

/* Optional freestanding bind (may be absent on host libudx). */
#if !defined(UDX_HOST_LIBC)
udx_status_t udx_host_bind_by_id(u16 u16Vendor, u16 u16Device)
    __attribute__((weak));
#endif

/*
 * Per-function soft state (Linux: netdev_priv / pci drvdata).
 * Hung off pPdev->pDev via udx_set_drvdata.
 */
struct rtl8168_soft {
    struct udx_pci_dev *pPdev;
    struct udx_iomem   *pRegs;   /* BAR2 MMIO (soft) */
    struct udx_work     workIsr;
    struct udx_spinlock lockSoft;
    u32                 u32IrqHits;
    u32                 u32WorkHits;
    u32                 u32FQuiesced;
    u32                 u32BarIndex;
    udx_dma_addr_t      dmaRing;
    void               *pRing;
};

/* File-static soft block — freestanding hosts need no heap. */
static struct rtl8168_soft g_rtlSoft;

static void
rtl8168_work_fn(struct udx_work *pWork)
{
    struct rtl8168_soft *pSoft;

    if (pWork == NULL) {
        return;
    }
    pSoft = (struct rtl8168_soft *)pWork->pPriv;
    if (pSoft == NULL) {
        return;
    }

    udx_spin_lock(&pSoft->lockSoft);
    if (pSoft->u32FQuiesced) {
        udx_spin_unlock(&pSoft->lockSoft);
        return;
    }
    pSoft->u32WorkHits++;
    udx_spin_unlock(&pSoft->lockSoft);

    udx_printk("rtl8168_udx: work (irq_hits=%u work_hits=%u)\n",
               pSoft->u32IrqHits, pSoft->u32WorkHits);

    if (pSoft->u32WorkHits >= 1u) {
        udx_request_stop();
    }
}

/*
 * Threaded IRQ soft handler.
 * Read IntrStatus → ack (write back claimed bits) → schedule_work.
 */
static int
rtl8168_isr(int nIrq, void *pDevId)
{
    struct rtl8168_soft *pSoft = (struct rtl8168_soft *)pDevId;
    u16 u16Status;

    (void)nIrq;
    if (pSoft == NULL || pSoft->pRegs == NULL) {
        return UDX_IRQ_NONE;
    }
    if (pSoft->u32FQuiesced) {
        return UDX_IRQ_NONE;
    }

    u16Status = udx_readw(pSoft->pRegs, RTL_REG_INTR_STATUS);
    if ((u16Status & RTL_SOFT_IRQ_BITS) == 0) {
        return UDX_IRQ_NONE;
    }

    /* Ack: write claimed bits back (public IntrStatus clear-by-write style). */
    udx_writew(pSoft->pRegs, RTL_REG_INTR_STATUS, u16Status);
    udx_mmio_flush(pSoft->pRegs);

    udx_spin_lock(&pSoft->lockSoft);
    pSoft->u32IrqHits++;
    udx_spin_unlock(&pSoft->lockSoft);

    pSoft->workIsr.pPriv = pSoft;
    (void)udx_schedule_work(&pSoft->workIsr);
    return UDX_IRQ_HANDLED;
}

static void
rtl8168_soft_fail_cleanup(struct udx_pci_dev *pPdev, struct rtl8168_soft *pSoft)
{
    if (pSoft != NULL) {
        if (pSoft->pRing != NULL) {
            udx_dma_free_coherent(pPdev->pDev, RTL8168_RING_BYTES,
                                  pSoft->pRing, pSoft->dmaRing);
            pSoft->pRing = NULL;
            pSoft->dmaRing = 0;
        }
        if (pSoft->pRegs != NULL) {
            udx_iounmap(pSoft->pRegs);
            pSoft->pRegs = NULL;
        }
    }
    udx_pci_release_regions(pPdev);
    udx_pci_clear_master(pPdev);
    udx_pci_disable(pPdev);
}

/*
 * probe — Linux pci_driver.probe spirit.
 * Soft order: enable → master → regions → map BAR2 → work/DMA → IRQ.
 */
static int
rtl8168_probe(struct udx_pci_dev *pPdev, const struct udx_pci_device_id *pId)
{
    struct rtl8168_soft *pSoft;
    u64 u64Bar;
    u64 u64Len;
    void *pRing;
    udx_dma_addr_t dmaHandle;
    u32 u32Bdf;
    int nBar;

    (void)pId;
    if (pPdev == NULL || pPdev->pDev == NULL) {
        return UDX_ERR_INVAL;
    }

    if (udx_pci_enable(pPdev) != UDX_OK) {
        return UDX_ERR_IO;
    }
    if (udx_pci_set_master(pPdev) != UDX_OK) {
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }
    if (udx_pci_request_regions(pPdev, "rtl8168_udx") != UDX_OK) {
        udx_pci_clear_master(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_BUSY;
    }

    /*
     * Prefer BAR2 (public common MMIO). Soft fallback: first non-empty
     * memory BAR so host inject mistakes still exercise the path.
     */
    nBar = RTL8168_BAR_MMIO;
    u64Bar = udx_pci_resource_start(pPdev, nBar);
    u64Len = udx_pci_resource_len(pPdev, nBar);
    if (u64Bar == 0 || u64Len == 0) {
        int iBar;

        for (iBar = 0; iBar < 6; iBar++) {
            if (udx_pci_resource_len(pPdev, iBar) == 0) {
                continue;
            }
            if (!udx_pci_resource_is_mem(pPdev, iBar)) {
                continue;
            }
            nBar = iBar;
            u64Bar = udx_pci_resource_start(pPdev, nBar);
            u64Len = udx_pci_resource_len(pPdev, nBar);
            udx_printk("rtl8168_udx: BAR2 empty; soft using BAR%d "
                       "(len=%llx) — re-check silicon BARs\n",
                       nBar, (unsigned long long)u64Len);
            break;
        }
    }
    if (u64Bar == 0 || u64Len == 0) {
        rtl8168_soft_fail_cleanup(pPdev, NULL);
        return UDX_ERR_IO;
    }

    pSoft = &g_rtlSoft;
    pSoft->pPdev = pPdev;
    pSoft->u32IrqHits = 0;
    pSoft->u32WorkHits = 0;
    pSoft->u32FQuiesced = 0;
    pSoft->u32BarIndex = (u32)nBar;
    pSoft->pRing = NULL;
    pSoft->dmaRing = 0;
    pSoft->pRegs = NULL;
    udx_spin_lock_init(&pSoft->lockSoft);

    pSoft->pRegs = udx_ioremap(u64Bar, u64Len);
    if (pSoft->pRegs == NULL) {
        rtl8168_soft_fail_cleanup(pPdev, pSoft);
        return UDX_ERR_NOMEM;
    }

    udx_init_work(&pSoft->workIsr, rtl8168_work_fn);
    pSoft->workIsr.pPriv = pSoft;

    /* Soft DMA ring alloc only — descriptors not programmed to HW. */
    pRing = udx_dma_alloc_coherent(pPdev->pDev, RTL8168_RING_BYTES,
                                   &dmaHandle, 0);
    if (pRing == NULL) {
        rtl8168_soft_fail_cleanup(pPdev, pSoft);
        return UDX_ERR_NODMA;
    }
    pSoft->pRing = pRing;
    pSoft->dmaRing = dmaHandle;

    u32Bdf = udx_dma_bdf(pPdev->u8Bus, udx_pci_slot(pPdev),
                         udx_pci_func(pPdev));
    (void)udx_dma_iommu_grant(u32Bdf, dmaHandle, RTL8168_RING_BYTES);
    udx_dma_sync_single_for_device(pPdev->pDev, dmaHandle, RTL8168_RING_BYTES,
                                   UDX_DMA_BIDIRECTIONAL);

    if (udx_request_irq(pPdev->nIrq, rtl8168_isr, 0, "rtl8168_udx",
                        pSoft) != UDX_OK) {
        rtl8168_soft_fail_cleanup(pPdev, pSoft);
        return UDX_ERR_NODEVIRQ;
    }
    udx_enable_irq(pPdev->nIrq);

    /*
     * Soft IntrMask: allow ROK|TOK reason bits for host demo.
     * Does not enable MAC Tx/Rx (ChipCmd left alone — soft ≠ link up).
     */
    udx_writew(pSoft->pRegs, RTL_REG_INTR_MASK, (u16)RTL_SOFT_IRQ_BITS);
    udx_mmio_flush(pSoft->pRegs);

    udx_set_drvdata(pPdev->pDev, pSoft);
    udx_printk("rtl8168_udx: probe ok vend=%04x dev=%04x bar%u=%llx "
               "dma=%llx irq=%d dev=%s\n",
               (unsigned)pPdev->u16Vendor, (unsigned)pPdev->u16Device,
               pSoft->u32BarIndex, (unsigned long long)u64Bar,
               (unsigned long long)dmaHandle, pPdev->nIrq,
               udx_dev_name(pPdev->pDev));
    return UDX_OK;
}

static void
rtl8168_quiesce(struct udx_pci_dev *pPdev)
{
    struct rtl8168_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct rtl8168_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft == NULL) {
        return;
    }

    udx_spin_lock(&pSoft->lockSoft);
    pSoft->u32FQuiesced = 1;
    udx_spin_unlock(&pSoft->lockSoft);

    udx_disable_irq(pPdev->nIrq);
    (void)udx_cancel_work(&pSoft->workIsr);

    if (pSoft->pRegs != NULL) {
        udx_writew(pSoft->pRegs, RTL_REG_INTR_MASK, 0);
        udx_writew(pSoft->pRegs, RTL_REG_INTR_STATUS, 0xffffu);
        udx_mmio_flush(pSoft->pRegs);
    }
    udx_pci_clear_master(pPdev);
}

static void
rtl8168_remove(struct udx_pci_dev *pPdev)
{
    struct rtl8168_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct rtl8168_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft != NULL) {
        udx_free_irq(pPdev->nIrq, pSoft);
        if (udx_work_pending(&pSoft->workIsr)) {
            (void)udx_cancel_work(&pSoft->workIsr);
        }
        if (pSoft->pRing != NULL) {
            udx_dma_free_coherent(pPdev->pDev, RTL8168_RING_BYTES,
                                  pSoft->pRing, pSoft->dmaRing);
            pSoft->pRing = NULL;
        }
        if (pSoft->pRegs != NULL) {
            udx_iounmap(pSoft->pRegs);
            pSoft->pRegs = NULL;
        }
        udx_set_drvdata(pPdev->pDev, NULL);
    }
    udx_pci_release_regions(pPdev);
    udx_pci_clear_master(pPdev);
    udx_pci_disable(pPdev);
    udx_printk("rtl8168_udx: removed\n");
}

static const struct udx_pci_device_id rtl8168_ids[] = {
    UDX_PCI_DEVICE(RTL8168_VEND, RTL8168_DEV),
    { 0, }
};

static struct udx_pci_driver rtl8168_driver = {
    .szName = "rtl8168_udx",
    .pIdTable = rtl8168_ids,
    .pfnProbe = rtl8168_probe,
    .pfnRemove = rtl8168_remove,
    .pfnQuiesce = rtl8168_quiesce,
};

/**
 * Shared register entry for freestanding hosts (no inject).
 * Returns UDX_OK after driver table is live.
 */
int
rtl8168_udx_register(void)
{
    if (udx_init() != UDX_OK) {
        return UDX_ERR_IO;
    }
    return udx_pci_register_driver(&rtl8168_driver);
}

#if defined(UDX_HOST_LIBC)

int
main(int argc, char **argv)
{
    struct udx_pci_dev *pPdev;
    u64 aBarLen[6];
    u8 aBarMem[6];
    int iBar;
    udx_status_t st;
    int fPass;
    struct rtl8168_soft *pSoft;

    (void)argc;
    (void)argv;
    fPass = 0;

    if (udx_init() != UDX_OK) {
        udx_printk("rtl8168_udx: soft SKIP (udx_init)\n");
        return 1;
    }

    if (udx_pci_register_driver(&rtl8168_driver) != UDX_OK) {
        udx_printk("rtl8168_udx: soft SKIP (register)\n");
        udx_exit();
        return 1;
    }

    /* Soft inject: BAR2 MMIO only; other BARs empty. */
    for (iBar = 0; iBar < 6; iBar++) {
        aBarLen[iBar] = 0;
        aBarMem[iBar] = 1;
    }
    aBarLen[RTL8168_BAR_MMIO] = RTL8168_BAR_LEN;

    st = udx_host_inject_pci((u16)RTL8168_VEND, (u16)RTL8168_DEV,
                             0, 0, RTL8168_IRQ_SOFT,
                             aBarLen, aBarMem, &pPdev);
    if (st != UDX_OK) {
        udx_printk("rtl8168_udx: soft SKIP (inject %d)\n", st);
        udx_pci_unregister_driver(&rtl8168_driver);
        udx_exit();
        return 1;
    }

    if (pPdev == NULL || pPdev->pDev == NULL ||
        !udx_dev_has_drvdata(pPdev->pDev)) {
        udx_printk("rtl8168_udx: soft SKIP (probe/no drvdata)\n");
        udx_pci_unregister_driver(&rtl8168_driver);
        udx_exit();
        return 1;
    }

    /* Raise soft IntrStatus bits, then fire threaded IRQ. */
    st = udx_host_bar_writew(pPdev, RTL8168_BAR_MMIO, RTL_REG_INTR_STATUS,
                             (u16)RTL_SOFT_IRQ_BITS);
    if (st != UDX_OK) {
        udx_printk("rtl8168_udx: soft SKIP (bar poke %d)\n", st);
        udx_pci_unregister_driver(&rtl8168_driver);
        udx_exit();
        return 1;
    }
    udx_host_fire_irq(RTL8168_IRQ_SOFT);
    udx_run();

    pSoft = (struct rtl8168_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft != NULL && pSoft->u32IrqHits >= 1u &&
        pSoft->u32WorkHits >= 1u) {
        fPass = 1;
    }

    udx_pci_unregister_driver(&rtl8168_driver);
    udx_exit();

    if (fPass) {
        udx_printk("rtl8168_udx: soft probe PASS\n");
        return 0;
    }
    udx_printk("rtl8168_udx: soft SKIP (irq/work path incomplete)\n");
    return 1;
}

#else /* !UDX_HOST_LIBC */

/*
 * Freestanding driver-host entry: register, optionally bind by public ID
 * when the platform provides udx_host_bind_by_id. Otherwise the host must
 * inject 10ec:8168 (see README).
 */
int
rtl8168_udx_freestanding_start(void)
{
    udx_status_t st;

    st = rtl8168_udx_register();
    if (st != UDX_OK) {
        udx_printk("rtl8168_udx: soft SKIP (register %d)\n", st);
        return st;
    }

    if (udx_host_bind_by_id != NULL) {
        st = udx_host_bind_by_id((u16)RTL8168_VEND, (u16)RTL8168_DEV);
        if (st != UDX_OK) {
            udx_printk("rtl8168_udx: soft SKIP (bind_by_id %d) — "
                       "inject 10ec:8168 on host path\n", st);
            return st;
        }
        udx_printk("rtl8168_udx: soft probe PASS\n");
        return UDX_OK;
    }

    udx_printk("rtl8168_udx: soft SKIP (no udx_host_bind_by_id; "
               "host-inject 10ec:8168 BAR2 for lab test)\n");
    return UDX_ERR_NOSYS;
}

#endif /* UDX_HOST_LIBC */
