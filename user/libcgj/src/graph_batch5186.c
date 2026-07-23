/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5186: pure-value PCI vendor-ID invalid probe (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_vendor_invalid_u(uint32_t vid);
 *     - Return 1 if vid == 0xffff (absent / invalid vendor), else 0.
 *   uint32_t __gj_pci_vendor_invalid_u  (alias)
 *   __libcgj_batch5186_marker = "libcgj-batch5186"
 *
 * Exclusive continuum CREATE-ONLY (5181-5190: PCI unique —
 * pci_cfg_addr_u, pci_bus_u, pci_dev_u, pci_fn_u, pci_reg_u,
 * pci_vendor_invalid_u, pci_is_multifn_u, pci_bar_is_io_u,
 * pci_bar_is_64_u, batch_id_5190). Unique gj_pci_vendor_invalid_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5186_marker[] = "libcgj-batch5186";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5186_vendor_invalid(uint32_t u32Vid)
{
	if (u32Vid == 0xffffu) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_vendor_invalid_u - 1 if vendor ID is the absent sentinel 0xffff.
 *
 * vid: 16-bit vendor ID (upper bits ignored by equality to 0xffff)
 *
 * Returns 1 when vid == 0xffff, else 0. Self-contained; no parent wires.
 */
uint32_t
gj_pci_vendor_invalid_u(uint32_t u32Vid)
{
	(void)NULL;
	return b5186_vendor_invalid(u32Vid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_vendor_invalid_u(uint32_t u32Vid)
    __attribute__((alias("gj_pci_vendor_invalid_u")));
