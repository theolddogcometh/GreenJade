/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9846: soft PCI BAR-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_bar_ok_u_9846(void);
 *     - Returns 1 (PCI BAR soft-id continuum ok). Pure-data product
 *       tag; does not size BARs or map MMIO/I/O regions.
 *   uint32_t __gj_pci_bar_ok_u_9846  (alias)
 *   __libcgj_batch9846_marker = "libcgj-batch9846"
 *
 * Exclusive continuum CREATE-ONLY (9841-9850: pci soft id stubs —
 * pci_bus_ok_u_9841, pci_dev_ok_u_9842, pci_func_ok_u_9843,
 * pci_vendor_ok_u_9844, pci_class_ok_u_9845, pci_bar_ok_u_9846,
 * pci_msi_ok_u_9847, pci_enum_ok_u_9848, pci_ready_u_9849,
 * batch_id_9850). Unique surface only; no multi-def. Distinct from
 * gj_pci_bar_is_io_u / gj_pci_bar_is_64_u (batch5188/5189). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9846_marker[] = "libcgj-batch9846";

#define B9846_BAR_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9846_bar_ok(void)
{
	return B9846_BAR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_bar_ok_u_9846 - PCI BAR soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not size BARs or
 * call libc. No parent wires.
 */
uint32_t
gj_pci_bar_ok_u_9846(void)
{
	(void)NULL;
	return b9846_bar_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_bar_ok_u_9846(void)
    __attribute__((alias("gj_pci_bar_ok_u_9846")));
