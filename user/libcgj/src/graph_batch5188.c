/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5188: pure-value PCI BAR I/O space probe (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_bar_is_io_u(uint32_t bar);
 *     - Return 1 if bar bit 0 is set (I/O space BAR), else 0
 *       (memory space).
 *   uint32_t __gj_pci_bar_is_io_u  (alias)
 *   __libcgj_batch5188_marker = "libcgj-batch5188"
 *
 * Exclusive continuum CREATE-ONLY (5181-5190: PCI unique —
 * pci_cfg_addr_u, pci_bus_u, pci_dev_u, pci_fn_u, pci_reg_u,
 * pci_vendor_invalid_u, pci_is_multifn_u, pci_bar_is_io_u,
 * pci_bar_is_64_u, batch_id_5190). Unique gj_pci_bar_is_io_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5188_marker[] = "libcgj-batch5188";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5188_bar_is_io(uint32_t u32Bar)
{
	if ((u32Bar & 1u) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_bar_is_io_u - 1 if BAR low bit marks I/O space.
 *
 * bar: raw Base Address Register value
 *
 * Returns 1 when (bar & 1) != 0, else 0. Self-contained; no parent wires.
 */
uint32_t
gj_pci_bar_is_io_u(uint32_t u32Bar)
{
	(void)NULL;
	return b5188_bar_is_io(u32Bar);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_bar_is_io_u(uint32_t u32Bar)
    __attribute__((alias("gj_pci_bar_is_io_u")));
