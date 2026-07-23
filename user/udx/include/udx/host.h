/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host / test inject surface for Linux porters.
 * Use on Linux/macOS hosts to exercise drivers before GreenJade kernel
 * wires real PCI/IOMMU. Not available inside freestanding GJ userland
 * until devmgr replaces inject with cap grants.
 *
 * Soft path for PCI skeleton demos:
 *   inject → probe → host_bar_writel(status) → fire_irq → run → work
 */
#pragma once

#include <udx/types.h>
#include <udx/pci.h>

/**
 * Inject a synthetic PCI function for probe matching.
 * BAR lengths: non-zero bars get host-backed MMIO (ioremap works).
 * aBarLen[6] / aBarIsMem[6] may be NULL (all zero / all memory).
 *
 * Returns UDX_OK and fills *ppOut on success (owned by UDX until remove).
 */
udx_status_t udx_host_inject_pci(u16 u16Vendor, u16 u16Device,
                                 u8 u8Bus, u8 u8Devfn, int nIrq,
                                 const u64 *aBarLen, const u8 *aBarIsMem,
                                 struct udx_pci_dev **ppOut);

/**
 * Extended inject: subsystem IDs + 24-bit class code for id-table match.
 * u32Class is class_code << 8 | subclass << 8 | prog-if style 24-bit
 * (same packing as Linux pci_device_id.class with class_mask).
 */
udx_status_t udx_host_inject_pci_ex(u16 u16Vendor, u16 u16Device,
                                    u16 u16SubVendor, u16 u16SubDevice,
                                    u32 u32Class,
                                    u8 u8Bus, u8 u8Devfn, int nIrq,
                                    const u64 *aBarLen, const u8 *aBarIsMem,
                                    struct udx_pci_dev **ppOut);

/**
 * Unbind + free a host-injected function (BAR windows + soft cfg).
 * Safe no-op when pPdev is NULL or not host-injected.
 * Returns UDX_ERR_NOSYS when not built with UDX_HOST_LIBC.
 */
udx_status_t udx_host_remove_pci(struct udx_pci_dev *pPdev);

/** Fire threaded IRQ path as if kernel Notification arrived. */
void udx_host_fire_irq(int nIrq);

/** Match all unbound injected devices against registered drivers (probe). */
void udx_host_rescan_pci(void);

/**
 * Host-only BAR poke without driver soft pointer.
 * Used by skeleton main to raise a fake status bit before fire_irq.
 * Returns UDX_ERR_NOSYS when not built with UDX_HOST_LIBC.
 */
udx_status_t udx_host_bar_writel(struct udx_pci_dev *pPdev, int nBar,
                                 u64 u64Off, u32 u32Val);

/** Host-only BAR dword read (symmetric to writel). */
udx_status_t udx_host_bar_readl(struct udx_pci_dev *pPdev, int nBar,
                                u64 u64Off, u32 *pu32Val);

udx_status_t udx_host_bar_writew(struct udx_pci_dev *pPdev, int nBar,
                                 u64 u64Off, u16 u16Val);
udx_status_t udx_host_bar_readw(struct udx_pci_dev *pPdev, int nBar,
                                u64 u64Off, u16 *pu16Val);

udx_status_t udx_host_bar_writeb(struct udx_pci_dev *pPdev, int nBar,
                                 u64 u64Off, u8 u8Val);
udx_status_t udx_host_bar_readb(struct udx_pci_dev *pPdev, int nBar,
                                u64 u64Off, u8 *pu8Val);

udx_status_t udx_host_bar_writeq(struct udx_pci_dev *pPdev, int nBar,
                                 u64 u64Off, u64 u64Val);
udx_status_t udx_host_bar_readq(struct udx_pci_dev *pPdev, int nBar,
                                u64 u64Off, u64 *pu64Val);

/**
 * Host-only bulk BAR copy (for firmware blobs / ring seed in demos).
 * cb bytes at BAR offset; bounds-checked against BAR length.
 */
udx_status_t udx_host_bar_memcpy_to(struct udx_pci_dev *pPdev, int nBar,
                                    u64 u64Off, const void *pSrc, size_t cb);
udx_status_t udx_host_bar_memcpy_from(struct udx_pci_dev *pPdev, int nBar,
                                      u64 u64Off, void *pDst, size_t cb);
