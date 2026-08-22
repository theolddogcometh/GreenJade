/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host-oracle offsets for RHEL-class Linux struct pci_dev / device / resource.
 * Used only for soft module-path experiments (r8169.ko probe-shaped blob).
 * Soft != ABI-stable product. Soft != product AC (G-AC-1).
 * Do not claim layout matches any other kver / CONFIG / RH_KABI rebuild.
 *
 * Regenerated on a lab host with kernel-devel via:
 *   ./scripts/gen-linux-pci-hostish-off.sh
 *   GJ_LINUX_KVER=... GJ_LINUX_KBUILD=... ./scripts/gen-linux-pci-hostish-off.sh
 *
 * Measured: kver=5.14.0-687.15.1.el9_8.x86_64
 * Date (UTC): 2026-08-03
 * kbuild: /lib/modules/5.14.0-687.15.1.el9_8.x86_64/build
 *
 * No Linux GPL source is in this header - only dual-license offset #defines
 * extracted via offsetof/sizeof from system headers on the build host.
 * See docs/PCI_DEV_SOFT_LAYOUT.md (Strategy A hostish blob).
 */
#pragma once

/* ---- sizeof ----------------------------------------------------------- */
#define LINUX_PCI_HOSTISH_SIZE_PCI_DEV     0x0b40u /* 2880 */
#define LINUX_PCI_HOSTISH_SIZE_DEVICE      0x0300u /* 768 */
#define LINUX_PCI_HOSTISH_SIZE_RESOURCE    0x0040u /* 64 */

/* ---- struct pci_dev field offsets ------------------------------------- */
#define LINUX_PCI_HOSTISH_OFF_BUS_LIST     0x0000u
#define LINUX_PCI_HOSTISH_OFF_BUS          0x0010u
#define LINUX_PCI_HOSTISH_OFF_DEVFN        0x0038u
#define LINUX_PCI_HOSTISH_OFF_VENDOR       0x003cu
#define LINUX_PCI_HOSTISH_OFF_DEVICE       0x003eu
#define LINUX_PCI_HOSTISH_OFF_SUBSYS_VEND  0x0040u
#define LINUX_PCI_HOSTISH_OFF_SUBSYS_DEV   0x0042u
#define LINUX_PCI_HOSTISH_OFF_CLASS        0x0044u
#define LINUX_PCI_HOSTISH_OFF_REVISION     0x0048u
#define LINUX_PCI_HOSTISH_OFF_DRIVER       0x0080u
#define LINUX_PCI_HOSTISH_OFF_DMA_MASK     0x0088u
#define LINUX_PCI_HOSTISH_OFF_DEV          0x00c8u /* embedded struct device */
#define LINUX_PCI_HOSTISH_OFF_IRQ          0x03ccu
#define LINUX_PCI_HOSTISH_OFF_RESOURCE     0x03d0u /* resource[0] */
#define LINUX_PCI_HOSTISH_OFF_ENABLE_CNT   0x0858u

/* Absolute offsets of resource[0..2] (BAR0..BAR2) */
#define LINUX_PCI_HOSTISH_OFF_RESOURCE_0   0x03d0u
#define LINUX_PCI_HOSTISH_OFF_RESOURCE_1   0x0410u
#define LINUX_PCI_HOSTISH_OFF_RESOURCE_2   0x0450u

/* resource[n] = RESOURCE + n * SIZE_RESOURCE */
#define LINUX_PCI_HOSTISH_OFF_RESOURCE_N(n) \
    (LINUX_PCI_HOSTISH_OFF_RESOURCE + (unsigned)(n) * LINUX_PCI_HOSTISH_SIZE_RESOURCE)

/* ---- struct device relative to pdev->dev base (add OFF_DEV) ----------- */
#define LINUX_PCI_HOSTISH_DEV_OFF_PARENT            0x0040u
#define LINUX_PCI_HOSTISH_DEV_OFF_DRIVER_DATA       0x0078u
#define LINUX_PCI_HOSTISH_DEV_OFF_DMA_MASK          0x0250u /* u64 * */
#define LINUX_PCI_HOSTISH_DEV_OFF_COHERENT_DMA_MASK 0x0258u /* u64 value */

/* Absolute pci_dev offsets for common device fields */
#define LINUX_PCI_HOSTISH_OFF_DEV_PARENT \
    (LINUX_PCI_HOSTISH_OFF_DEV + LINUX_PCI_HOSTISH_DEV_OFF_PARENT)
#define LINUX_PCI_HOSTISH_OFF_DEV_DRIVER_DATA \
    (LINUX_PCI_HOSTISH_OFF_DEV + LINUX_PCI_HOSTISH_DEV_OFF_DRIVER_DATA)
#define LINUX_PCI_HOSTISH_OFF_DEV_DMA_MASK \
    (LINUX_PCI_HOSTISH_OFF_DEV + LINUX_PCI_HOSTISH_DEV_OFF_DMA_MASK)
#define LINUX_PCI_HOSTISH_OFF_DEV_COHERENT_DMA_MASK \
    (LINUX_PCI_HOSTISH_OFF_DEV + LINUX_PCI_HOSTISH_DEV_OFF_COHERENT_DMA_MASK)

/* ---- struct resource relative to resource[n] base --------------------- */
#define LINUX_PCI_HOSTISH_RES_OFF_START  0x0000u
#define LINUX_PCI_HOSTISH_RES_OFF_END    0x0008u
#define LINUX_PCI_HOSTISH_RES_OFF_NAME   0x0010u
#define LINUX_PCI_HOSTISH_RES_OFF_FLAGS  0x0018u

/* Soft pool / blob sizing for freestanding fill (parent implements fill). */
#define LINUX_PCI_HOSTISH_POOL           2u
#define LINUX_PCI_HOSTISH_BLOB_BYTES     0x0c00u /* 3072 ≥ sizeof(pci_dev) */

/* IORESOURCE_* low bits (host-compatible; soft fill may set these). */
#define LINUX_PCI_HOSTISH_IORESOURCE_IO   0x00000100ul
#define LINUX_PCI_HOSTISH_IORESOURCE_MEM  0x00000200ul

/*
 * Usage (freestanding hostish fill - not implemented here):
 *
 *   u8 *p = hostish_blob;
 *   memset(p, 0, LINUX_PCI_HOSTISH_BLOB_BYTES);
 *   *(u16 *)(p + LINUX_PCI_HOSTISH_OFF_VENDOR) = 0x10ec;
 *   *(u16 *)(p + LINUX_PCI_HOSTISH_OFF_DEVICE) = 0x8168;
 *   *(u32 *)(p + LINUX_PCI_HOSTISH_OFF_DEVFN)  = devfn;
 *   *(u64 *)(p + LINUX_PCI_HOSTISH_OFF_RESOURCE_0
 *              + LINUX_PCI_HOSTISH_RES_OFF_START) = bar0_phys;
 *   *(u64 *)(p + LINUX_PCI_HOSTISH_OFF_RESOURCE_0
 *              + LINUX_PCI_HOSTISH_RES_OFF_END)   = bar0_phys + bar0_len - 1;
 *   *(unsigned long *)(p + LINUX_PCI_HOSTISH_OFF_RESOURCE_0
 *              + LINUX_PCI_HOSTISH_RES_OFF_FLAGS) = LINUX_PCI_HOSTISH_IORESOURCE_MEM;
 *   *(void **)(p + LINUX_PCI_HOSTISH_OFF_DEV_DRIVER_DATA) = soft_priv;
 *   *(u64 **)(p + LINUX_PCI_HOSTISH_OFF_DEV_DMA_MASK) =
 *        (u64 *)(p + LINUX_PCI_HOSTISH_OFF_DMA_MASK);
 *
 * Gate real .ko probe behind an explicit flag; default remains soft EMU.
 */
