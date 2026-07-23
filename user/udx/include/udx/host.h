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

/** Fire threaded IRQ path as if kernel Notification arrived. */
void udx_host_fire_irq(int nIrq);

/** Match all unbound injected devices against registered drivers (probe). */
void udx_host_rescan_pci(void);

/**
 * Host-only BAR dword poke without driver soft pointer.
 * Used by skeleton main to raise a fake status bit before fire_irq.
 * Returns UDX_ERR_NOSYS when not built with UDX_HOST_LIBC.
 */
udx_status_t udx_host_bar_writel(struct udx_pci_dev *pPdev, int nBar,
                                 u64 u64Off, u32 u32Val);

/** Host-only BAR dword read (symmetric to writel). */
udx_status_t udx_host_bar_readl(struct udx_pci_dev *pPdev, int nBar,
                                u64 u64Off, u32 *pu32Val);
