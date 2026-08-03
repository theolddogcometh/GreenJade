/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room UDX skeleton — Intel PCH xHCI soft host path (G752VT 8086:a12f).
 *
 * WHAT THIS IS
 *   Userspace Linux-shaped driver host against libudx:
 *     match → probe (ioremap BAR0 + soft CAPLENGTH/HCIVERSION) →
 *     quiesce → remove.
 *   Public register offsets only (xHCI Specification capability space).
 *   No GPL. No Linux xHCI source paste. No BOT / MSC product claim.
 *
 * WHAT THIS IS NOT
 *   Not freestanding kernel/drv/xhci_msc.c (lab soft scaffold stays there).
 *   Not a shipping USB host stack. Soft host inject ≠ real MMIO/IRQ/DMA grants.
 *
 * HOST BUILD
 *   make -C user/drivers/xhci_udx
 *   ./user/drivers/xhci_udx/build/xhci_udx
 *   Grep: "xhci_udx: soft probe PASS" / "xhci_udx: soft cap ver="
 *
 * License choice: MIT OR Apache-2.0 (dual). See repo LICENSE.
 */

#include <udx/udx.h>

/* ------------------------------------------------------------------ */
/* PCI identity — G752VT Intel 100 Series / C230 USB 3.0 xHCI.         */
/* Vendor/device from public PCI ID inventory (HCL / Linux lspci).    */
/* Class 0x0c0330 = Serial Bus / USB / xHCI (PCI class code, 24-bit).  */
/* ------------------------------------------------------------------ */
#define XHCI_UDX_VEND       0x8086u
#define XHCI_UDX_DEV        0xa12fu
#define XHCI_UDX_CLASS      0x0c0330u
#define XHCI_UDX_CLASS_MASK 0xffffffu

/* Soft host BDF for G752VT inventory: 0000:00:14.0 */
#define XHCI_UDX_BUS   0u
#define XHCI_UDX_SLOT  0x14u
#define XHCI_UDX_FUNC  0u
#define XHCI_UDX_IRQ   16

/* Intel PCH xHCI BAR0 is typically a 64 KiB memory window. */
#define XHCI_UDX_BAR0_LEN  0x10000ull

/*
 * Capability registers — xHCI Specification (public), base of BAR0 MMIO.
 *
 *   Offset 00h  CAPLENGTH  (RO)  bits 7:0  — length of capability regs
 *   Offset 00h  Rsvd       (RO)  bits 15:8
 *   Offset 02h  HCIVERSION (RO)  16-bit Interface Version Number
 *   Offset 04h  HCSPARAMS1 (RO)  — not read by this soft skeleton
 *   ...
 *
 * Soft host seeds only CAPLENGTH + HCIVERSION so probe can print ver=.
 * Do not invent operational/runtime register programs here.
 */
#define XHCI_CAP_CAPLENGTH   0x00ull /* 1-byte CAPLENGTH within dword 0 */
#define XHCI_CAP_HCIVERSION  0x02ull /* 16-bit HCIVERSION */

/* Soft seed values (illustrative; not a claim of live silicon readout). */
#define XHCI_SOFT_CAPLENGTH  0x20u   /* common 32-byte capability region */
#define XHCI_SOFT_HCIVERSION 0x0100u /* xHCI Interface Version 1.0 */

/* Packed dword0: CAPLENGTH | (0 << 8) | (HCIVERSION << 16) */
#define XHCI_SOFT_CAP_DWORD0 \
    ((u32)XHCI_SOFT_CAPLENGTH | ((u32)XHCI_SOFT_HCIVERSION << 16))

/*
 * Per-device soft state (Linux: private struct / dev_get_drvdata).
 * Static pool so freestanding hosts need no heap for this skeleton.
 */
struct xhci_udx_soft {
    struct udx_pci_dev *pPdev;
    struct udx_iomem   *pCap;     /* BAR0 capability / MMIO window */
    u8                  u8CapLen; /* CAPLENGTH soft snapshot */
    u16                 u16HciVer;/* HCIVERSION soft snapshot */
    u32                 u32FQuiesced;
    u32                 u32FProbeOk;
};

static struct xhci_udx_soft s_soft;

/* ------------------------------------------------------------------ */
/* probe — enable, map BAR0, soft-read public cap regs only.           */
/* ------------------------------------------------------------------ */
static int
xhci_udx_probe(struct udx_pci_dev *pPdev, const struct udx_pci_device_id *pId)
{
    struct xhci_udx_soft *pSoft;
    u64 u64Bar;
    u64 u64Len;
    u8  u8Cap;
    u16 u16Ver;
    u32 u32Dword0;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return UDX_ERR_INVAL;
    }

    (void)pId;

    if (udx_pci_enable(pPdev) != UDX_OK) {
        return UDX_ERR_IO;
    }
    if (udx_pci_request_regions(pPdev, "xhci_udx") != UDX_OK) {
        udx_pci_disable(pPdev);
        return UDX_ERR_BUSY;
    }

    u64Bar = udx_pci_resource_start(pPdev, 0);
    u64Len = udx_pci_resource_len(pPdev, 0);
    if (u64Bar == 0 || u64Len < 4ull) {
        udx_pci_release_regions(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }
    if (!udx_pci_resource_is_mem(pPdev, 0)) {
        udx_printk("xhci_udx: BAR0 not memory\n");
        udx_pci_release_regions(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }

    pSoft = &s_soft;
    pSoft->pPdev = pPdev;
    pSoft->pCap = NULL;
    pSoft->u8CapLen = 0;
    pSoft->u16HciVer = 0;
    pSoft->u32FQuiesced = 0;
    pSoft->u32FProbeOk = 0;

    pSoft->pCap = udx_ioremap(u64Bar, u64Len);
    if (pSoft->pCap == NULL) {
        udx_pci_release_regions(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_NOMEM;
    }

    /*
     * Soft capability snapshot — public offsets only.
     * Prefer byte/word accessors matching the spec field widths;
     * also read dword0 once for a consistent soft view.
     */
    u32Dword0 = udx_readl(pSoft->pCap, XHCI_CAP_CAPLENGTH);
    u8Cap     = udx_readb(pSoft->pCap, XHCI_CAP_CAPLENGTH);
    u16Ver    = udx_readw(pSoft->pCap, XHCI_CAP_HCIVERSION);

    /* Cross-check: CAPLENGTH is low byte of dword0; version is high half. */
    if (u8Cap != (u8)(u32Dword0 & 0xffu)) {
        u8Cap = (u8)(u32Dword0 & 0xffu);
    }
    if (u16Ver != (u16)((u32Dword0 >> 16) & 0xffffu)) {
        u16Ver = (u16)((u32Dword0 >> 16) & 0xffffu);
    }

    pSoft->u8CapLen  = u8Cap;
    pSoft->u16HciVer = u16Ver;

    udx_printk("xhci_udx: soft cap ver=0x%04x caplen=0x%02x bar0=%llx\n",
               (unsigned)u16Ver, (unsigned)u8Cap,
               (unsigned long long)u64Bar);

    /*
     * Soft success: non-zero CAPLENGTH and a plausible Interface Version.
     * Host demo seeds 0x20 / 0x0100; live silicon would also be non-zero
     * for a present controller. Zero/zero → soft FAIL (unseeded BAR).
     */
    if (u8Cap == 0u || u16Ver == 0u) {
        udx_printk("xhci_udx: soft cap empty (no seed / no HC)\n");
        udx_iounmap(pSoft->pCap);
        pSoft->pCap = NULL;
        udx_pci_release_regions(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }

    pSoft->u32FProbeOk = 1;
    udx_set_drvdata(pPdev->pDev, pSoft);
    udx_printk("xhci_udx: soft probe PASS dev=%s id=%04x:%04x class=%06x\n",
               udx_dev_name(pPdev->pDev),
               (unsigned)pPdev->u16Vendor,
               (unsigned)pPdev->u16Device,
               (unsigned)(pPdev->u32Class & 0x00ffffffu));
    return UDX_OK;
}

/* ------------------------------------------------------------------ */
/* quiesce — stop further soft activity before remove / crash path.    */
/* ------------------------------------------------------------------ */
static void
xhci_udx_quiesce(struct udx_pci_dev *pPdev)
{
    struct xhci_udx_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct xhci_udx_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft == NULL) {
        return;
    }
    pSoft->u32FQuiesced = 1;
    /* Soft skeleton: no bus-master, no IRQ, no rings — flag only. */
    udx_printk("xhci_udx: soft quiesce\n");
}

/* ------------------------------------------------------------------ */
/* remove — reverse probe: iounmap → release regions → disable.        */
/* ------------------------------------------------------------------ */
static void
xhci_udx_remove(struct udx_pci_dev *pPdev)
{
    struct xhci_udx_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct xhci_udx_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft != NULL) {
        if (pSoft->pCap != NULL) {
            udx_iounmap(pSoft->pCap);
            pSoft->pCap = NULL;
        }
        pSoft->u32FProbeOk = 0;
        udx_set_drvdata(pPdev->pDev, NULL);
    }
    udx_pci_release_regions(pPdev);
    udx_pci_disable(pPdev);
    udx_printk("xhci_udx: soft remove\n");
}

/* ID table: exact G752VT function + optional any-xHCI class row. */
static const struct udx_pci_device_id xhci_udx_ids[] = {
    UDX_PCI_DEVICE(XHCI_UDX_VEND, XHCI_UDX_DEV),
    UDX_PCI_DEVICE_CLASS(XHCI_UDX_CLASS, XHCI_UDX_CLASS_MASK),
    { 0, } /* sentinel */
};

static struct udx_pci_driver xhci_udx_driver = {
    .szName     = "xhci_udx",
    .pIdTable   = xhci_udx_ids,
    .pfnProbe   = xhci_udx_probe,
    .pfnRemove  = xhci_udx_remove,
    .pfnQuiesce = xhci_udx_quiesce,
};

#if defined(UDX_HOST_LIBC)

/*
 * Host soft demo main.
 *
 * Order matters: inject + seed CAP dword *before* register so probe sees
 * non-zero CAPLENGTH/HCIVERSION (register_driver rescans unbound devices).
 */
int
main(int argc, char **argv)
{
    struct udx_pci_dev *pPdev;
    u64 aBarLen[6];
    u8  aBarMem[6];
    int iBar;
    udx_status_t st;
    int fPass;
    struct xhci_udx_soft *pSoft;

    (void)argc;
    (void)argv;
    fPass = 0;
    pPdev = NULL;

    if (udx_init() != UDX_OK) {
        return 1;
    }

    for (iBar = 0; iBar < 6; iBar++) {
        aBarLen[iBar] = 0;
        aBarMem[iBar] = 1;
    }
    aBarLen[0] = XHCI_UDX_BAR0_LEN;

    /*
     * Inject Intel PCH xHCI soft function (class 0x0c0330) without a
     * driver bound yet — BAR host memory is zero-filled by UDX.
     */
    st = udx_host_inject_pci_ex(XHCI_UDX_VEND, XHCI_UDX_DEV,
                                0xffffu, 0xffffu,
                                XHCI_UDX_CLASS,
                                (u8)XHCI_UDX_BUS,
                                udx_pci_devfn((u8)XHCI_UDX_SLOT,
                                              (u8)XHCI_UDX_FUNC),
                                XHCI_UDX_IRQ,
                                aBarLen, aBarMem, &pPdev);
    if (st != UDX_OK || pPdev == NULL) {
        udx_printk("xhci_udx: inject FAIL %d\n", (int)st);
        udx_exit();
        return 1;
    }

    /* Seed public capability fields in soft BAR0 (host-only poke). */
    st = udx_host_bar_writel(pPdev, 0, XHCI_CAP_CAPLENGTH, XHCI_SOFT_CAP_DWORD0);
    if (st != UDX_OK) {
        udx_printk("xhci_udx: soft cap seed FAIL %d\n", (int)st);
        (void)udx_host_remove_pci(pPdev);
        udx_exit();
        return 1;
    }

    if (udx_pci_register_driver(&xhci_udx_driver) != UDX_OK) {
        udx_printk("xhci_udx: register FAIL\n");
        (void)udx_host_remove_pci(pPdev);
        udx_exit();
        return 1;
    }

    if (pPdev->pDev == NULL || !udx_dev_has_drvdata(pPdev->pDev)) {
        udx_printk("xhci_udx: soft probe FAIL (no drvdata)\n");
        udx_pci_unregister_driver(&xhci_udx_driver);
        (void)udx_host_remove_pci(pPdev);
        udx_exit();
        return 1;
    }

    pSoft = (struct xhci_udx_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft != NULL &&
        pSoft->u32FProbeOk != 0u &&
        pSoft->u8CapLen == XHCI_SOFT_CAPLENGTH &&
        pSoft->u16HciVer == XHCI_SOFT_HCIVERSION) {
        fPass = 1;
    }

    /* Teardown: unregister runs quiesce then remove. */
    udx_pci_unregister_driver(&xhci_udx_driver);
    (void)udx_host_remove_pci(pPdev);
    udx_exit();

    if (fPass) {
        udx_printk("xhci_udx: host soft demo done\n");
        return 0;
    }
    udx_printk("xhci_udx: soft probe FAIL\n");
    return 1;
}

#else /* !UDX_HOST_LIBC */

/*
 * Freestanding registration stub — GJ driver-host process calls this
 * after udx_init when devmgr grants the real function. No host inject.
 */
int
xhci_udx_freestanding_register(void)
{
    if (udx_init() != UDX_OK) {
        return UDX_ERR_IO;
    }
    return udx_pci_register_driver(&xhci_udx_driver);
}

#endif /* UDX_HOST_LIBC */
