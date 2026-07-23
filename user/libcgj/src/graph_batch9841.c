/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9841: soft PCI bus-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_bus_ok_u_9841(void);
 *     - Returns 1 (PCI bus soft-id continuum ok). Pure-data product
 *       tag; does not probe buses or walk config space.
 *   uint32_t __gj_pci_bus_ok_u_9841  (alias)
 *   __libcgj_batch9841_marker = "libcgj-batch9841"
 *
 * Exclusive continuum CREATE-ONLY (9841-9850: pci soft id stubs —
 * pci_bus_ok_u_9841, pci_dev_ok_u_9842, pci_func_ok_u_9843,
 * pci_vendor_ok_u_9844, pci_class_ok_u_9845, pci_bar_ok_u_9846,
 * pci_msi_ok_u_9847, pci_enum_ok_u_9848, pci_ready_u_9849,
 * batch_id_9850). Unique surface only; no multi-def. Distinct from
 * gj_pci_bus_u (batch5182). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9841_marker[] = "libcgj-batch9841";

#define B9841_BUS_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9841_bus_ok(void)
{
	return B9841_BUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_bus_ok_u_9841 - PCI bus soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not scan buses or
 * call libc. No parent wires.
 */
uint32_t
gj_pci_bus_ok_u_9841(void)
{
	(void)NULL;
	return b9841_bus_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_bus_ok_u_9841(void)
    __attribute__((alias("gj_pci_bus_ok_u_9841")));
