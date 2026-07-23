/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5190: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5190(void);
 *     - Returns the compile-time graph batch number for this TU (5190).
 *   uint32_t __gj_batch_id_5190  (alias)
 *   __libcgj_batch5190_marker = "libcgj-batch5190"
 *
 * Exclusive continuum CREATE-ONLY (5181-5190: PCI unique —
 * pci_cfg_addr_u, pci_bus_u, pci_dev_u, pci_fn_u, pci_reg_u,
 * pci_vendor_invalid_u, pci_is_multifn_u, pci_bar_is_io_u,
 * pci_bar_is_64_u, batch_id_5190). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5190_marker[] = "libcgj-batch5190";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5190_id(void)
{
	return 5190u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5190 - report this TU's graph batch number.
 *
 * Always returns 5190. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5190(void)
{
	(void)NULL;
	return b5190_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5190(void)
    __attribute__((alias("gj_batch_id_5190")));
