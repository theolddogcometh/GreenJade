/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Example skeleton — clean-room pattern for Linux porters (PCI soft path).
 *
 * WHAT THIS FILE IS
 *   A minimal PCI driver host that exercises the full UDX path:
 *     register → inject (host lab) / DDI grant (GJ freestanding) →
 *     probe → MMIO/DMA/IRQ → work → quiesce → remove.
 *   Docs: docs/UDX_LINUX_PORTER.md
 *   Product: GREENJADE_UDX / UDX_PRODUCT (user/udx/README.md markers)
 *
 * PRODUCT PATH HONESTY (Soft!=product; G-AC-1; dual MIT OR Apache-2.0)
 *   freestanding class drivers (kernel rtl8168 / xhci_msc) = SKIP
 *   product = UDX + ABI: Linux-shaped userspace hosts
 *     (rtl8168_udx / xhci_udx) over Cap MMIO_FRAME + IRQ Notification +
 *     DMA window (mint OPEN today — soft inventory != product grant)
 *   Host inject / fire_irq / identity DMA = lab soft only (Soft!=product)
 *
 * HOST BUILD (default — `make udx-example` or `make -C user/udx example`)
 *   Compile with -DUDX_HOST_LIBC=1, link build/libudx.a.
 *   Soft lab path (probe → irq → work) — Soft!=product:
 *     1. udx_init / register_driver
 *     2. udx_host_bind_by_id → expected soft SKIP (no GJ_SYS_DDI on host)
 *     3. udx_host_inject_pci → match + probe
 *     4. udx_host_bar_writel(status) + udx_host_fire_irq
 *     5. ISR: readl status → ack → schedule_work
 *     6. udx_run flushes work → udx_request_stop
 *     7. unregister (quiesce+remove) / udx_exit
 *     8. greppable: "udx: skeleton PASS"
 *
 * FREESTANDING DRIVER-HOST SHAPE (on GreenJade; product attach)
 *   Same probe / remove / quiesce / mmio / dma / request_irq path.
 *   Product bind residual (host.c; not inject) walks GJ_SYS_DDI (103):
 *     SCAN  → device count
 *     GET   → gj_ddi_dev_info (vend/dev/BARs; no CF8/CFC in UDX)
 *     OPEN  → soft handle id
 *     MAP_BAR preferred BARs (product residual; Soft!=product mint OPEN)
 *       rtl8168_udx (10ec:8168): BAR0 + BAR2
 *       xhci_udx    (8086:a12f): BAR0
 *       other IDs              : BAR0
 *     Side residual (deepen honesty; mint OPEN; handle retained — no CLOSE):
 *       CFG_READ (ident+cmdst) → MAP_REMAP → DMA_NOTE → IRQ_BIND
 *     → udx_host_install_granted_pci (BAR0 VA wired for ioremap)
 *     → window-register each preferred BAR VA (BAR2 etc. for rtl)
 *     → pci match / probe (ioremap granted PA)
 *     Core greppable chain: SCAN,GET,OPEN,MAP_BAR
 *     Side greppable: CFG_READ,MAP_REMAP,DMA_NOTE,IRQ_BIND (ddi.h LIFE_*)
 *     greppable: udx: soft ddi bind residual / residual path / PASS|SKIP
 *     APIs: udx_host_bind_by_id / udx_host_bind_scan (host.h / ddi.h)
 *   Host soft inventory (lab observation; never gates skeleton PASS):
 *     udx_host_soft_init / note_ddi / lifecycle_note
 *     soft_bdf_from_pdev / soft_bar_snapshot[_all]
 *     soft_dma_window_request → always not-granted (product_mint=0)
 *   Differences from host-linux:
 *     - No udx_host_inject_pci; bind PASS uses granted PA/VA.
 *     - Soft SKIP until kernel DDI is live (Soft!=product).
 *     - IRQ delivery is pumped from udx_run via NOTIFY_WAIT badge bits,
 *       not from udx_request_irq itself (request_irq only binds the table).
 *     - DMA cookies use freestanding static slab + IOMMU grant helpers.
 *   Optional: udx_skeleton_freestanding_register() /
 *             udx_skeleton_freestanding_bind_g752() without UDX_HOST_LIBC.
 *   G752 first targets: UDX_DDI_G752_RTL8168_* / UDX_DDI_G752_XHCI_*.
 *   Dual DoD residual (Soft!=product; G-AC-1; dual MIT OR Apache-2.0):
 *     Dual DoD A OPEN — xhci_udx (8086:a12f) preferred BAR0
 *     Dual DoD B OPEN — rtl8168_udx (10ec:8168) preferred BAR0+BAR2
 *     host.h: UDX_HOST_SOFT_DOD_A_OPEN / UDX_HOST_SOFT_DOD_B_OPEN = 1
 *     cap_mint=0 multi_server=0 confine=0; lamps never close DoD
 *
 * CLEAN-ROOM RULES
 *   Replace MY_VEND/MY_DEV and the register program from public hardware
 *   docs / PCI IDs / vendor manuals only. Do NOT paste Linux kernel source.
 *   Caps, mint/revoke, and IOMMU programming stay inside UDX — never in
 *   driver .c. No Linux .ko product (G-AC-1). Dual MIT OR Apache-2.0.
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
 * Soft inventory residual (C0 deepen; Soft!=product; never product mint):
 *   u32FSoftInv host soft BDF/BAR/DMA-window catalog walked once
 *   u8SoftStage 1=open 2=map 3=dma 4=irq 5=work (porter progress only)
 */
struct my_soft {
    struct udx_pci_dev *pPdev;   /* owning PCI function */
    struct udx_iomem   *pRegs;   /* BAR0 mapping (ioremap) */
    struct udx_work     workIsr; /* bottom half from ISR */
    struct udx_spinlock lockSoft;
    u32                 u32IrqHits;
    u32                 u32WorkHits;
    u32                 u32FQuiesced;
    u32                 u32FSoftInv; /* soft inventory residual once */
    u8                  u8SoftStage; /* 1..5 soft progress (lab only) */
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
    if (pSoft->u8SoftStage < 5u) {
        pSoft->u8SoftStage = 5u; /* soft stage: work ran */
    }
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
    if (pSoft->u8SoftStage < 4u) {
        pSoft->u8SoftStage = 4u; /* soft stage: irq claimed */
    }
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
    pSoft->u32FSoftInv = 0;
    pSoft->u8SoftStage = 1u; /* soft stage: open (enable+regions) */
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
    pSoft->u8SoftStage = 2u; /* soft stage: map */

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
    pSoft->u8SoftStage = 3u; /* soft stage: dma cookie (identity lab) */

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
    udx_printk("my_drv: probe ok bar0=%llx dma=%llx dev=%s stage=%u\n",
               (unsigned long long)u64Bar,
               (unsigned long long)dmaHandle,
               udx_dev_name(pPdev->pDev),
               (unsigned)pSoft->u8SoftStage);
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
 * Soft lab path (Soft!=product; greppable markers in README):
 *   udx_init                     → GREENJADE_UDX / UDX_PRODUCT
 *                                  + soft residual lean / freestanding
 *                                    class SKIP / product=UDX+ABI
 *   udx_host_soft_init/note_ddi/lifecycle_note  → soft DDI surface lamps
 *   udx_pci_register_driver
 *   udx_host_bind_by_id (G752 ids) → expected SKIP on host-linux
 *   udx_host_inject_pci          → match + probe (lab only)
 *   soft BDF/BAR/DMA inventory   → observation; dma window not-granted
 *   udx_host_bar_writel + fire   → my_isr → schedule_work
 *   udx_run                      → flush work → stop
 *   unregister + udx_exit
 *   "udx: skeleton PASS" (never gated on soft inventory / dual DoD)
 *   Residual lamps: soft inventory + soft honesty + skeleton residual
 *     dual_dod_a/b OPEN via UDX_HOST_SOFT_DOD_*; cap_mint=0
 *
 * Product freestanding attach (not this host main):
 *   SCAN→GET→OPEN→MAP preferred BARs (+ side CFG/REMAP/DMA_NOTE/IRQ_BIND)
 *   → install_granted→window_register via udx_host_bind_*
 *   Dual DoD A/B OPEN (Soft!=product); freestanding class SKIP.
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
    u32 u32SoftInv = 0;
    u8 u8SoftStage = 0;
    u8 u8DmaWinOk = 0xffu;

    (void)argc;
    (void)argv;
    fPass = 0;

    if (udx_init() != UDX_OK) {
        return 1;
    }

    /*
     * Soft DDI host surface (observation only; Soft!=product).
     * Emits: soft ddi host note PASS / soft ddi-ready / lifecycle note.
     * Never gates skeleton PASS or Dual DoD A/B close.
     * greppable: udx: soft ddi host note PASS
     * greppable: udx: soft ddi-ready
     * greppable: udx: soft ddi lifecycle note
     */
    udx_host_soft_init();
    udx_host_soft_note_ddi();
    udx_host_soft_lifecycle_note();

    if (udx_pci_register_driver(&my_driver) != UDX_OK) {
        udx_exit();
        return 1;
    }

    /*
     * Soft DDI bind probe (host-linux): expected SKIP — freestanding
     * product residual is SCAN→GET→OPEN→MAP preferred BARs (+ side
     * CFG_READ→MAP_REMAP→DMA_NOTE→IRQ_BIND) via GJ_SYS_DDI (103)
     * then install_granted + window-register. Soft fallback keeps inject
     * below for lab tests (Soft!=product).
     * greppable: udx: soft ddi bind SKIP
     * greppable: udx: soft ddi bind residual (tally once-lamp on bind enter)
     * G752 product-host first targets: rtl8168_udx / xhci_udx (ddi.h).
     */
    (void)udx_host_bind_by_id(UDX_DDI_G752_RTL8168_VEND,
                              UDX_DDI_G752_RTL8168_DEV, NULL);
    (void)udx_host_bind_by_id(UDX_DDI_G752_XHCI_VEND,
                              UDX_DDI_G752_XHCI_DEV, NULL);

    /* BAR0 memory window of MY_BAR0_LEN; other BARs unused. */
    for (iBar = 0; iBar < 6; iBar++) {
        aBarLen[iBar] = 0;
        aBarMem[iBar] = 1; /* memory BAR (not I/O port) */
    }
    aBarLen[0] = MY_BAR0_LEN;

    /* Lab inject only — Soft!=product; not freestanding DDI grant. */
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
     * Soft inventory residual (C0 deepen; observation only).
     * Porter pattern: BDF + BAR snap + DMA window request fail-closed.
     * greppable: udx: skeleton soft inventory
     * greppable: udx: soft ddi dma not-granted
     * product_mint=0; Soft!=product; never gates PASS.
     */
    {
        struct my_soft *pSoftInv =
            (struct my_soft *)udx_get_drvdata(pPdev->pDev);
        struct udx_host_bdf bdfSnap;
        struct udx_host_bar barSnap;
        struct udx_host_bar aBars[UDX_HOST_SOFT_BAR_MAX];
        struct udx_host_window_grant winGrant;
        u32 u32BarCount;
        u32 u32InvBits;
        udx_dma_addr_t dmaCookie;

        u32InvBits = 0;
        u32BarCount = 0;
        dmaCookie = 0;
        winGrant.u8Ok = 0;
        winGrant.u32WinId = 0xffffffffu;

        if (udx_host_soft_bdf_from_pdev(pPdev, &bdfSnap) == UDX_OK) {
            u32InvBits |= 1u; /* BDF soft ok */
        }
        if (udx_host_soft_bar_snapshot(pPdev, 0, &barSnap) == UDX_OK &&
            barSnap.u64Cb != 0) {
            u32InvBits |= 2u; /* BAR0 soft ok */
        }
        if (udx_host_soft_bar_snapshot_all(pPdev, aBars,
                                           &u32BarCount) == UDX_OK) {
            u32InvBits |= 4u; /* inventory ok */
        }
        if (pSoftInv != NULL) {
            dmaCookie = pSoftInv->dmaRing;
        }
        if ((u32InvBits & 1u) != 0u) {
            (void)udx_host_soft_dma_window_request(
                &bdfSnap, (u64)dmaCookie, (u64)MY_RING_BYTES, 0x3u,
                &winGrant);
            u32InvBits |= 8u; /* request walked (expect not-granted) */
            u8DmaWinOk = winGrant.u8Ok;
        }
        if (pSoftInv != NULL) {
            pSoftInv->u32FSoftInv = u32InvBits;
            u8SoftStage = pSoftInv->u8SoftStage;
        }
        u32SoftInv = u32InvBits;
        /*
         * Lean once-lamp (no stamp storm). ready= soft DDI host ready.
         * dma_win_ok / cap_mint must stay 0 until product Cap mint.
         * Dual DoD A/B remain OPEN (host.h UDX_HOST_SOFT_DOD_*); never close.
         * Soft tallies are observation only — Soft!=product; G-AC-1.
         */
        udx_printk("udx: skeleton soft inventory "
                   "bits=0x%x bdf=%u bar0=%u bars=%u dma_req=%u "
                   "dma_win_ok=%u win_id=0x%x stage=%u ready=%d "
                   "init_calls=%u dma_reqs=%u dma_rej=%u "
                   "cap_mint=%u multi_server=%u confine=%u "
                   "dual_dod_a=%u dual_dod_b=%u "
                   "product_mint=0 Soft!=product G-AC-1\n",
                   (unsigned)u32InvBits,
                   (unsigned)((u32InvBits & 1u) != 0u),
                   (unsigned)((u32InvBits & 2u) != 0u),
                   (unsigned)u32BarCount,
                   (unsigned)((u32InvBits & 8u) != 0u),
                   (unsigned)winGrant.u8Ok,
                   (unsigned)winGrant.u32WinId,
                   (unsigned)u8SoftStage,
                   udx_host_soft_ready(),
                   (unsigned)udx_host_soft_init_calls(),
                   (unsigned)udx_host_soft_dma_requests(),
                   (unsigned)udx_host_soft_dma_rejects(),
                   (unsigned)UDX_HOST_SOFT_CAP_MINT,
                   (unsigned)UDX_HOST_SOFT_MULTI_SERVER,
                   (unsigned)UDX_HOST_SOFT_CONFINE,
                   (unsigned)UDX_HOST_SOFT_DOD_A_OPEN,
                   (unsigned)UDX_HOST_SOFT_DOD_B_OPEN);
    }

    /*
     * Soft path step 4: raise fake device IRQ without soft pointer.
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
            u8SoftStage = pSoft->u8SoftStage;
            if (pSoft->u32FSoftInv != 0u) {
                u32SoftInv = pSoft->u32FSoftInv;
            }
        }
    }

    udx_pci_unregister_driver(&my_driver);
    udx_exit();

    if (fPass) {
        udx_printk("my_drv: host demo done\n");
        /*
         * Residual honesty lamps (observation only; Soft!=product).
         * Never claim freestanding class drivers product or Cap mint.
         * greppable: freestanding class SKIP / product=UDX+ABI
         * multi_server=0 confine=0; skeleton PASS != product DoD close.
         * Dual DoD A (xhci_udx) / B (rtl8168_udx) remain OPEN (host.h).
         * Side residual is honesty deepen only. Stamp-free; no GPL.
         */
        udx_printk("udx: skeleton soft honesty "
                   "freestanding_class=SKIP product=UDX+ABI "
                   "bind=SCAN_GET_OPEN_MAP_BAR "
                   "side=CFG_MAP_REMAP_DMA_NOTE_IRQ_BIND "
                   "pref_bars=product_host "
                   "lab_inject=1 soft=1 product_mint=%u "
                   "soft_inv=0x%x dma_win_ok=%u stage=%u "
                   "cap_mint=%u multi_server=%u confine=%u "
                   "exclusive=1 "
                   "dual_dod_a=%u path_a=xhci_udx id_a=8086:a12f "
                   "dual_dod_b=%u path_b=rtl8168_udx id_b=10ec:8168 "
                   "dual_dod=OPEN dual_dod_ab=OPEN "
                   "soft_ne_product=1 G-AC-1 Soft!=product\n",
                   (unsigned)UDX_HOST_SOFT_CAP_MINT,
                   (unsigned)u32SoftInv,
                   (unsigned)(u8DmaWinOk == 0xffu ? 0u : u8DmaWinOk),
                   (unsigned)u8SoftStage,
                   (unsigned)UDX_HOST_SOFT_CAP_MINT,
                   (unsigned)UDX_HOST_SOFT_MULTI_SERVER,
                   (unsigned)UDX_HOST_SOFT_CONFINE,
                   (unsigned)UDX_HOST_SOFT_DOD_A_OPEN,
                   (unsigned)UDX_HOST_SOFT_DOD_B_OPEN);
        udx_printk("udx: skeleton residual "
                   "freestanding class SKIP "
                   "product=UDX+ABI Linux-shaped userspace "
                   "chain=SCAN,GET,OPEN,MAP_BAR "
                   "side_chain=CFG_READ,MAP_REMAP,DMA_NOTE,IRQ_BIND "
                   "hosts=rtl8168_udx,xhci_udx "
                   "soft_inv=0x%x "
                   "dual_dod_a=%u dual_dod_b=%u dual_dod_ab=OPEN "
                   "cap_mint=%u multi_server=%u confine=%u "
                   "handle_retain=%u close_on_bind=%u "
                   "soft=1 product=0 (Soft!=product) G-AC-1\n",
                   (unsigned)u32SoftInv,
                   (unsigned)UDX_HOST_SOFT_DOD_A_OPEN,
                   (unsigned)UDX_HOST_SOFT_DOD_B_OPEN,
                   (unsigned)UDX_HOST_SOFT_CAP_MINT,
                   (unsigned)UDX_HOST_SOFT_MULTI_SERVER,
                   (unsigned)UDX_HOST_SOFT_CONFINE,
                   (unsigned)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (unsigned)UDX_HOST_SOFT_CLOSE_ON_BIND);
        udx_printk("udx: skeleton PASS\n");
        return 0;
    }
    (void)u32SoftInv;
    (void)u8SoftStage;
    (void)u8DmaWinOk;
    udx_printk("udx: skeleton FAIL\n");
    return 1;
}

#else /* !UDX_HOST_LIBC — freestanding shape stub */

/*
 * Freestanding entry is provided by the driver-host process on GJ
 * (init / spawn). This TU can still be type-checked without host inject:
 * keep probe/remove/isr symbols and a tiny init that only registers.
 * Link against freestanding libudx + platform when wiring a real host.
 *
 * Product attach honesty (soft until kernel GJ_SYS_DDI is live):
 *   udx_init → udx_pci_register_driver → udx_host_bind_by_id(vend,dev)
 *     or udx_host_bind_scan()
 *   Bind residual (host.c host_ddi_open_map_install*):
 *     SCAN → GET → OPEN → MAP preferred BARs
 *       → side residual: CFG_READ → MAP_REMAP → DMA_NOTE → IRQ_BIND
 *       → udx_host_install_granted_pci (BAR0 VA)
 *       → window-register each preferred BAR VA (rtl BAR2 etc.)
 *       → pci match / probe (ioremap granted PA)
 *   Preferred BAR residual (Soft!=product; ddi.h / product hosts):
 *     rtl8168_udx 10ec:8168 → BAR0 + BAR2
 *     xhci_udx    8086:a12f → BAR0
 *     other                 → BAR0
 *   On PASS: preferred BAR PAs window-registered; probe may ioremap.
 *   On SKIP: syscall soft-stub / no device (inject is host-linux only).
 * Soft!=product: freestanding class SKIP; product=UDX+ABI (userspace hosts).
 * Dual DoD A/B OPEN. G752: UDX_DDI_G752_RTL8168_* / UDX_DDI_G752_XHCI_*.
 */
int
udx_skeleton_freestanding_register(void)
{
    if (udx_init() != UDX_OK) {
        return UDX_ERR_IO;
    }
    return udx_pci_register_driver(&my_driver);
}

/**
 * Optional freestanding DDI bind helper for GJ driver-host mains.
 * Registers my_drv then soft-binds G752 product-host ids (or any id
 * table match after scan). Residual walk (Soft!=product until live):
 *   SCAN → GET → OPEN → MAP preferred BARs
 *     → side: CFG_READ → MAP_REMAP → DMA_NOTE → IRQ_BIND
 *     → install_granted → window_register → probe.
 * greppable: soft ddi bind PASS|SKIP|residual[ path]
 * product=UDX+ABI; freestanding class SKIP; dual MIT OR Apache-2.0; G-AC-1.
 * Dual DoD A/B remain OPEN (soft inventory != product grant).
 */
int
udx_skeleton_freestanding_bind_g752(void)
{
    udx_status_t st;
    struct udx_pci_dev *pPdev;

    st = udx_skeleton_freestanding_register();
    if (st != UDX_OK) {
        return st;
    }

    /* Prefer rtl8168_udx then xhci_udx — product UDX host first targets. */
    pPdev = NULL;
    st = udx_host_bind_by_id(UDX_DDI_G752_RTL8168_VEND,
                             UDX_DDI_G752_RTL8168_DEV, &pPdev);
    if (st == UDX_OK) {
        return UDX_OK;
    }
    pPdev = NULL;
    st = udx_host_bind_by_id(UDX_DDI_G752_XHCI_VEND,
                             UDX_DDI_G752_XHCI_DEV, &pPdev);
    if (st == UDX_OK) {
        return UDX_OK;
    }
    /* Full SCAN→GET→OPEN→MAP preferred BARs residual (may still SKIP). */
    return udx_host_bind_scan();
}

#endif /* UDX_HOST_LIBC */

/*
 * Residual honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   greppable: freestanding class SKIP
 *   greppable: product=UDX+ABI
 *   greppable: bind=SCAN_GET_OPEN_MAP_BAR
 *   greppable: side=CFG_MAP_REMAP_DMA_NOTE_IRQ_BIND
 *   greppable: pref_bars=product_host
 *   greppable: chain=SCAN,GET,OPEN,MAP_BAR
 *   greppable: side_chain=CFG_READ,MAP_REMAP,DMA_NOTE,IRQ_BIND
 *   greppable: udx: skeleton soft inventory
 *   greppable: udx: skeleton soft honesty
 *   greppable: udx: skeleton residual
 *   greppable: dual_dod=OPEN / dual_dod_ab=OPEN
 *   greppable: dual_dod_a= (UDX_HOST_SOFT_DOD_A_OPEN) path_a=xhci_udx
 *   greppable: dual_dod_b= (UDX_HOST_SOFT_DOD_B_OPEN) path_b=rtl8168_udx
 *   greppable: cap_mint=0 multi_server=0 confine=0 (host.h soft constants)
 * Skeleton host demo = lab soft path only; product lamps 0 until Cap mint.
 * Soft inventory / DMA window not-granted never closes Dual DoD A/B.
 * Product freestanding attach = preferred-BAR residual (not BAR0-only).
 * C0 residual deepen: soft DDI surface + inventory + Dual DoD A/B OPEN.
 * freestanding class SKIP; no GPL; no Linux .ko product (G-AC-1).
 * Stamp-free; never bump GJ_IMAGE_VERSION from this TU.
 */
