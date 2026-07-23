/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9850: pci soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9850(void);
 *     - Returns the compile-time graph batch number for this TU (9850).
 *   uint32_t __gj_batch_id_9850  (alias)
 *   __libcgj_batch9850_marker = "libcgj-batch9850"
 *
 * Exclusive continuum CREATE-ONLY (9841-9850: pci soft id stubs —
 * pci_bus_ok_u_9841, pci_dev_ok_u_9842, pci_func_ok_u_9843,
 * pci_vendor_ok_u_9844, pci_class_ok_u_9845, pci_bar_ok_u_9846,
 * pci_msi_ok_u_9847, pci_enum_ok_u_9848, pci_ready_u_9849,
 * batch_id_9850). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9850_marker[] = "libcgj-batch9850";

#define B9850_BATCH_ID  9850u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9850_id(void)
{
	return B9850_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9850 - report this TU's graph batch number.
 *
 * Always returns 9850.
 */
uint32_t
gj_batch_id_9850(void)
{
	(void)NULL;
	return b9850_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9850(void)
    __attribute__((alias("gj_batch_id_9850")));
