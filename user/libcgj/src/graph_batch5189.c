/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5189: pure-value PCI BAR 64-bit type probe (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_bar_is_64_u(uint32_t bar);
 *     - Return 1 if memory BAR type bits (2:1) equal 10b ((bar & 6) == 4),
 *       else 0.
 *   uint32_t __gj_pci_bar_is_64_u  (alias)
 *   __libcgj_batch5189_marker = "libcgj-batch5189"
 *
 * Exclusive continuum CREATE-ONLY (5181-5190: PCI unique —
 * pci_cfg_addr_u, pci_bus_u, pci_dev_u, pci_fn_u, pci_reg_u,
 * pci_vendor_invalid_u, pci_is_multifn_u, pci_bar_is_io_u,
 * pci_bar_is_64_u, batch_id_5190). Unique gj_pci_bar_is_64_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5189_marker[] = "libcgj-batch5189";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5189_bar_is_64(uint32_t u32Bar)
{
	if ((u32Bar & 6u) == 4u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_bar_is_64_u - 1 if memory BAR type is 64-bit ((bar & 6) == 4).
 *
 * bar: raw Base Address Register value
 *
 * Returns 1 when type bits 2:1 encode 64-bit memory, else 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_pci_bar_is_64_u(uint32_t u32Bar)
{
	(void)NULL;
	return b5189_bar_is_64(u32Bar);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_bar_is_64_u(uint32_t u32Bar)
    __attribute__((alias("gj_pci_bar_is_64_u")));
