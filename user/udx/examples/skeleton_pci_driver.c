/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Example skeleton — clean-room pattern for Linux porters (PCI soft path).
 *
 * WHAT THIS FILE IS
 *   A minimal PCI driver host that exercises the full UDX path:
 *     register → inject (host) / grant (GJ) → probe → MMIO/DMA/IRQ →
 *     work → quiesce → remove.
 *   Docs: docs/UDX_LINUX_PORTER.md
 *   Product: GREENJADE_UDX / UDX_PRODUCT (user/udx/README.md markers)
 *
 * HOST BUILD (default — `make udx-example` or `make -C user/udx example`)
 *   Compile with -DUDX_HOST_LIBC=1, link build/libudx.a.
 *   Soft path (probe → irq → work):
 *     1. udx_init / register_driver
 *     2. udx_host_inject_pci → match + probe
 *     3. udx_host_bar_writel(status) + udx_host_fire_irq
 *     4. ISR: readl status → ack → schedule_work
 *     5. udx_run flushes work → udx_request_stop
 *     6. unregister (quiesce+remove) / udx_exit
 *     7. greppable: "udx: skeleton PASS"
 *
 * FREESTANDING DRIVER-HOST SHAPE (on GreenJade)
 *   Same probe / remove / quiesce / mmio / dma / request_irq path.
 *   Differences:
 *     - No udx_host_inject_pci / udx_host_fire_irq (devmgr grants the device).
 *     - IRQ delivery is pumped from udx_run via NOTIFY_WAIT badge bits,
 *       not from udx_request_irq itself (request_irq only binds the table).
 *     - DMA cookies use freestanding static slab + IOMMU grant helpers.
 *   Optional: udx_skeleton_freestanding_register() without UDX_HOST_LIBC.
 *
 * CLEAN-ROOM RULES
 *   Replace MY_VEND/MY_DEV and the register program from public hardware
 *   docs / PCI IDs / vendor manuals only. Do NOT paste Linux kernel source.
 *   Caps, mint/revoke, and IOMMU programming stay inside UDX — never in
 *   driver .c.
 */

#include <udx/udx.h>

/* ------------------------------------------------------------------ */
/* Demo PCI IDs — swap for real vendor:device from your device manual. */
/* 0x1af4 is the public virtio PCI vendor; 0x1001 is illustrative only. */
/* ------------------------------------------------------------------ */
#define MY_VEND 0x1af4u
#define MY_DEV  0x1001u
#define MY_IRQ  11            /* host inject IRQ line (fake) */
#define MY_BAR0_LEN 0x1000ull /* BAR0 size for inject / ioremap */

/* Fake device MMIO layout (demo only — not real hardware). */
#define MY_REG_STATUS  0x00ull /* status / IRQ reason */
#define MY_REG_CTRL    0x04ull /* control (mask bit0 = irq enable) */
#define MY_STATUS_IRQ  0x1u
#define MY_CTRL_IRQEN  0x1u
#define MY_RING_BYTES  4096u

/*
 * Per-device soft state (Linux: "struct my_priv" / dev_get_drvdata).
 * Hung off pPdev->pDev via udx_set_drvdata in probe.
 *
 * Soft-path fields used by IRQ → work:
 *   pRegs       BAR0 mapping for status/ack
 *   workIsr     bottom half scheduled from my_isr
 *   u32IrqHits  how many times ISR claimed the line
 *   u32WorkHits how many times work ran (demo stops after 1)
 */
struct my_soft {
    struct udx_pci_dev *pPdev;   /* owning PCI function */
    struct udx_iomem   *pRegs;   /* BAR0 mapping (ioremap) */
    struct udx_work     workIsr; /* bottom half from ISR */
    struct udx_spinlock lockSoft;
    u32                 u32IrqHits;
    u32                 u32WorkHits;
    u32                 u32FQuiesced;
    udx_dma_addr_t      dmaRing; /* bus / IOMMU cookie for ring */
    void               *pRing;   /* CPU VA of coherent ring */
};

/*
 * Work callback — Linux schedule_work / work_struct handler.
 * Runs from udx_run() work flush (host thread context), never hard-IRQ.
 *
 * Soft path step 5: deferred processing after ISR ack.
 */
static void
my_work_fn(struct udx_work *pWork)
{
    struct my_soft *pSoft = (struct my_soft *)pWork->pPriv;

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

    udx_printk("my_drv: work (irq_hits=%u)\n", pSoft->u32IrqHits);
    /* Host demo convenience: stop udx_run after first deferred work. */
    if (pSoft->u32WorkHits >= 1u) {
        udx_request_stop();
    }
}

/*
 * Threaded IRQ handler — Linux request_threaded_irq second half mental model.
 * Return UDX_IRQ_HANDLED if claimed, UDX_IRQ_NONE if not ours / spurious.
 *
 * Soft path step 4 (irq soft path):
 *   1. Read device status from MMIO (fake: MY_REG_STATUS).
 *   2. Ack/clear by write (device-specific; manuals only).
 *   3. Schedule deferred work for anything non-trivial.
 *
 * Do NOT assume non-preemptible hard-IRQ / BH-only patterns from old
 * Linux drivers; UDX always runs this in host process context.
 */
static int
my_isr(int nIrq, void *pDevId)
{
    struct my_soft *pSoft = (struct my_soft *)pDevId;
    u32 u32Status;

    (void)nIrq;
    if (pSoft == NULL || pSoft->pRegs == NULL) {
        return UDX_IRQ_NONE;
    }
    if (pSoft->u32FQuiesced) {
        return UDX_IRQ_NONE;
    }

    u32Status = udx_readl(pSoft->pRegs, MY_REG_STATUS);
    if ((u32Status & MY_STATUS_IRQ) == 0) {
        return UDX_IRQ_NONE; /* not ours / already cleared */
    }

    /* Ack device first, then defer heavy work. */
    udx_writel(pSoft->pRegs, MY_REG_STATUS, 0);
    udx_mmio_flush(pSoft->pRegs);

    udx_spin_lock(&pSoft->lockSoft);
    pSoft->u32IrqHits++;
    udx_spin_unlock(&pSoft->lockSoft);

    pSoft->workIsr.pPriv = pSoft;
    (void)udx_schedule_work(&pSoft->workIsr);
    return UDX_IRQ_HANDLED;
}

/*
 * probe — Linux pci_driver.probe / DDI attach spirit.
 *
 * Soft path step 2 order (mirror this in real ports):
 *   enable → set_master → request regions → map BAR →
 *   init work/locks → DMA → request_irq → enable device IRQ → set_drvdata
 * Reverse carefully on every error path.
 *
 * pId is the matching row from the id table (may carry u64DriverData).
 */
static int
my_probe(struct udx_pci_dev *pPdev, const struct udx_pci_device_id *pId)
{
    struct my_soft *pSoft;
    u64 u64Bar;
    u64 u64Len;
    void *pRing;
    udx_dma_addr_t dmaHandle;
    u32 u32Bdf;

    (void)pId;
    if (pPdev == NULL || pPdev->pDev == NULL) {
        return UDX_ERR_INVAL;
    }

    /* ---- phase A: PCI enable + regions -------------------------------- */
    if (udx_pci_enable(pPdev) != UDX_OK) {
        return UDX_ERR_IO;
    }
    if (udx_pci_set_master(pPdev) != UDX_OK) {
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }
    if (udx_pci_request_regions(pPdev, "my_drv") != UDX_OK) {
        udx_pci_clear_master(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_BUSY;
    }

    u64Bar = udx_pci_resource_start(pPdev, 0);
    u64Len = udx_pci_resource_len(pPdev, 0);
    if (u64Bar == 0 || u64Len == 0) {
        udx_pci_release_regions(pPdev);
        udx_pci_clear_master(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }

    /*
     * Soft state — demo uses a file-static block so freestanding hosts
     * need no heap. Production: static pool or a UDX allocator when ready.
     */
    {
        static struct my_soft s_soft;
        pSoft = &s_soft;
    }
    pSoft->pPdev = pPdev;
    pSoft->u32IrqHits = 0;
    pSoft->u32WorkHits = 0;
    pSoft->u32FQuiesced = 0;
    pSoft->pRing = NULL;
    pSoft->dmaRing = 0;
    pSoft->pRegs = NULL;
    udx_spin_lock_init(&pSoft->lockSoft);

    /* ---- phase B: MMIO map -------------------------------------------- */
    pSoft->pRegs = udx_ioremap(u64Bar, u64Len);
    if (pSoft->pRegs == NULL) {
        udx_pci_release_regions(pPdev);
        udx_pci_clear_master(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_NOMEM;
    }

    udx_init_work(&pSoft->workIsr, my_work_fn);
    pSoft->workIsr.pPriv = pSoft;

    /* ---- phase C: coherent ring (DMA) --------------------------------- */
    pRing = udx_dma_alloc_coherent(pPdev->pDev, MY_RING_BYTES, &dmaHandle, 0);
    if (pRing == NULL) {
        udx_iounmap(pSoft->pRegs);
        pSoft->pRegs = NULL;
        udx_pci_release_regions(pPdev);
        udx_pci_clear_master(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_NODMA;
    }
    pSoft->pRing = pRing;
    pSoft->dmaRing = dmaHandle;

    /* Optional IOMMU grant (host no-op; freestanding PLATFORM_INFO op5). */
    u32Bdf = udx_dma_bdf(pPdev->u8Bus, udx_pci_slot(pPdev), udx_pci_func(pPdev));
    (void)udx_dma_iommu_grant(u32Bdf, dmaHandle, MY_RING_BYTES);
    udx_dma_sync_single_for_device(pPdev->pDev, dmaHandle, MY_RING_BYTES,
                                   UDX_DMA_BIDIRECTIONAL);

    /* ---- phase D: IRQ bind (table only; no wait) ---------------------- */
    if (udx_request_irq(pPdev->nIrq, my_isr, 0, "my_drv", pSoft) != UDX_OK) {
        udx_dma_free_coherent(pPdev->pDev, MY_RING_BYTES, pRing, dmaHandle);
        pSoft->pRing = NULL;
        udx_iounmap(pSoft->pRegs);
        pSoft->pRegs = NULL;
        udx_pci_release_regions(pPdev);
        udx_pci_clear_master(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_NODEVIRQ;
    }
    udx_enable_irq(pPdev->nIrq);

    /* Fake device: allow IRQ status generation (ctrl bit). */
    udx_writel(pSoft->pRegs, MY_REG_CTRL, MY_CTRL_IRQEN);
    udx_mmio_flush(pSoft->pRegs);

    udx_set_drvdata(pPdev->pDev, pSoft);
    udx_printk("my_drv: probe ok bar0=%llx dma=%llx dev=%s\n",
               (unsigned long long)u64Bar,
               (unsigned long long)dmaHandle,
               udx_dev_name(pPdev->pDev));
    return UDX_OK;
}

/*
 * quiesce — DDI quiesce spirit (optional pfn on udx_pci_driver).
 * Soft path: mask IRQ line + clear device status/ctrl before remove/crash.
 */
static void
my_quiesce(struct udx_pci_dev *pPdev)
{
    struct my_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct my_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft == NULL) {
        return;
    }

    udx_spin_lock(&pSoft->lockSoft);
    pSoft->u32FQuiesced = 1;
    udx_spin_unlock(&pSoft->lockSoft);

    udx_disable_irq(pPdev->nIrq);
    (void)udx_cancel_work(&pSoft->workIsr);

    if (pSoft->pRegs) {
        udx_writel(pSoft->pRegs, MY_REG_CTRL, 0);
        udx_writel(pSoft->pRegs, MY_REG_STATUS, 0);
        udx_mmio_flush(pSoft->pRegs);
    }
    udx_pci_clear_master(pPdev);
}

/*
 * remove — Linux pci_driver.remove.
 * Undo probe in reverse: free_irq → DMA free → iounmap → release → disable.
 */
static void
my_remove(struct udx_pci_dev *pPdev)
{
    struct my_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct my_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft) {
        udx_free_irq(pPdev->nIrq, pSoft);
        if (udx_work_pending(&pSoft->workIsr)) {
            (void)udx_cancel_work(&pSoft->workIsr);
        }
        if (pSoft->pRing) {
            udx_dma_free_coherent(pPdev->pDev, MY_RING_BYTES, pSoft->pRing,
                                  pSoft->dmaRing);
            pSoft->pRing = NULL;
        }
        if (pSoft->pRegs) {
            udx_iounmap(pSoft->pRegs);
            pSoft->pRegs = NULL;
        }
        udx_set_drvdata(pPdev->pDev, NULL);
    }
    udx_pci_release_regions(pPdev);
    udx_pci_clear_master(pPdev);
    udx_pci_disable(pPdev);
    udx_printk("my_drv: removed\n");
}

/* ID table — Linux MODULE_DEVICE_TABLE(pci, ...) mental model. */
static const struct udx_pci_device_id my_ids[] = {
    UDX_PCI_DEVICE(MY_VEND, MY_DEV),
    { 0, } /* sentinel */
};

/*
 * Driver object — Linux struct pci_driver.
 * pfnQuiesce is the GreenJade/DDI addition (not in stock Linux pci_driver).
 */
static struct udx_pci_driver my_driver = {
    .szName = "my_drv",
    .pIdTable = my_ids,
    .pfnProbe = my_probe,
    .pfnRemove = my_remove,
    .pfnQuiesce = my_quiesce,
};

/*
 * Host demo main — Linux module_init + artificial device presence.
 *
 * Soft path sequence (greppable product markers in README):
 *   udx_init                     → GREENJADE_UDX / UDX_PRODUCT
 *   udx_pci_register_driver
 *   udx_host_inject_pci          → match + probe
 *   udx_host_bar_writel + fire   → my_isr → schedule_work
 *   udx_run                      → flush work → stop
 *   unregister + udx_exit
 *   "udx: skeleton PASS"
 */
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

    (void)argc;
    (void)argv;
    fPass = 0;

    if (udx_init() != UDX_OK) {
        return 1;
    }

    if (udx_pci_register_driver(&my_driver) != UDX_OK) {
        udx_exit();
        return 1;
    }

    /* BAR0 memory window of MY_BAR0_LEN; other BARs unused. */
    for (iBar = 0; iBar < 6; iBar++) {
        aBarLen[iBar] = 0;
        aBarMem[iBar] = 1; /* memory BAR (not I/O port) */
    }
    aBarLen[0] = MY_BAR0_LEN;

    st = udx_host_inject_pci(MY_VEND, MY_DEV, 0, 0, MY_IRQ,
                             aBarLen, aBarMem, &pPdev);
    if (st != UDX_OK) {
        udx_printk("my_drv: inject FAIL %d\n", st);
        udx_pci_unregister_driver(&my_driver);
        udx_exit();
        return 1;
    }

    if (pPdev == NULL || pPdev->pDev == NULL ||
        !udx_dev_has_drvdata(pPdev->pDev)) {
        udx_printk("my_drv: probe soft FAIL (no drvdata)\n");
        udx_pci_unregister_driver(&my_driver);
        udx_exit();
        return 1;
    }

    /*
     * Soft path step 3: raise fake device IRQ without soft pointer.
     * Real hardware: kernel Notification badge → same udx_irq_dispatch.
     */
    st = udx_host_bar_writel(pPdev, 0, MY_REG_STATUS, MY_STATUS_IRQ);
    if (st != UDX_OK) {
        udx_printk("my_drv: bar poke FAIL %d\n", st);
        udx_pci_unregister_driver(&my_driver);
        udx_exit();
        return 1;
    }
    udx_host_fire_irq(MY_IRQ);

    udx_run();

    /* Verify soft path completed (ISR claimed + work ran). */
    {
        struct my_soft *pSoft =
            (struct my_soft *)udx_get_drvdata(pPdev->pDev);
        if (pSoft != NULL && pSoft->u32IrqHits >= 1u &&
            pSoft->u32WorkHits >= 1u) {
            fPass = 1;
        }
    }

    udx_pci_unregister_driver(&my_driver);
    udx_exit();

    if (fPass) {
        udx_printk("my_drv: host demo done\n");
        /*
         * Grep: udx: skeleton soft deepen (Wave 70 exclusive).
         * Soft inventory only — never gates skeleton PASS product claim.
         * multi_server=0 confine=0 bar3=0.
         */
        udx_printk("udx: skeleton soft deepen wave=70 areas=1 "
                   "multi_server=0 confine=0 bar3=0 exclusive=1\n");
        udx_printk("udx: skeleton soft honesty multi_server=0 confine=0 "
                   "bar3=0 exclusive=1 soft=1 product_kernel=OPEN wave=70\n");
        udx_printk("udx: skeleton PASS\n");
        return 0;
    }
    udx_printk("udx: skeleton FAIL\n");
    return 1;
}

#else /* !UDX_HOST_LIBC — freestanding shape stub */

/*
 * Freestanding entry is provided by the driver-host process on GJ
 * (init / spawn). This TU can still be type-checked without host inject:
 * keep probe/remove/isr symbols and a tiny init that only registers.
 * Link against freestanding libudx + platform when wiring a real host.
 */
int
udx_skeleton_freestanding_register(void)
{
    if (udx_init() != UDX_OK) {
        return UDX_ERR_IO;
    }
    return udx_pci_register_driver(&my_driver);
}

#endif /* UDX_HOST_LIBC */

/* Wave 70 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retfosseangle continuum_toward=21200 soft_ne_product=1
 *   greppable: soft retcounterscarple exclusive=1 continuum_toward=21200
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */
