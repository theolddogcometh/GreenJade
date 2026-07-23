/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9842: soft PCI device-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_dev_ok_u_9842(void);
 *     - Returns 1 (PCI device soft-id continuum ok). Pure-data product
 *       tag; does not probe devices or walk config space.
 *   uint32_t __gj_pci_dev_ok_u_9842  (alias)
 *   __libcgj_batch9842_marker = "libcgj-batch9842"
 *
 * Exclusive continuum CREATE-ONLY (9841-9850: pci soft id stubs —
 * pci_bus_ok_u_9841, pci_dev_ok_u_9842, pci_func_ok_u_9843,
 * pci_vendor_ok_u_9844, pci_class_ok_u_9845, pci_bar_ok_u_9846,
 * pci_msi_ok_u_9847, pci_enum_ok_u_9848, pci_ready_u_9849,
 * batch_id_9850). Unique surface only; no multi-def. Distinct from
 * gj_pci_dev_u (batch5183). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9842_marker[] = "libcgj-batch9842";

#define B9842_DEV_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9842_dev_ok(void)
{
	return B9842_DEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_dev_ok_u_9842 - PCI device soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not probe devices
 * or call libc. No parent wires.
 */
uint32_t
gj_pci_dev_ok_u_9842(void)
{
	(void)NULL;
	return b9842_dev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_dev_ok_u_9842(void)
    __attribute__((alias("gj_pci_dev_ok_u_9842")));
