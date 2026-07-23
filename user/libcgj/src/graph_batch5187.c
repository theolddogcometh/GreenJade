/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5187: pure-value PCI multifunction probe (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_is_multifn_u(uint32_t header_type);
 *     - Return 1 if header_type bit 7 is set (multi-function device),
 *       else 0.
 *   uint32_t __gj_pci_is_multifn_u  (alias)
 *   __libcgj_batch5187_marker = "libcgj-batch5187"
 *
 * Exclusive continuum CREATE-ONLY (5181-5190: PCI unique —
 * pci_cfg_addr_u, pci_bus_u, pci_dev_u, pci_fn_u, pci_reg_u,
 * pci_vendor_invalid_u, pci_is_multifn_u, pci_bar_is_io_u,
 * pci_bar_is_64_u, batch_id_5190). Unique gj_pci_is_multifn_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5187_marker[] = "libcgj-batch5187";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5187_is_multifn(uint32_t u32Hdr)
{
	if ((u32Hdr & 0x80u) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_is_multifn_u - 1 if Header Type multi-function bit is set.
 *
 * header_type: PCI Header Type register value (bit 7 = multi-function)
 *
 * Returns 1 when bit 7 is set, else 0. Self-contained; no parent wires.
 */
uint32_t
gj_pci_is_multifn_u(uint32_t u32HeaderType)
{
	(void)NULL;
	return b5187_is_multifn(u32HeaderType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_is_multifn_u(uint32_t u32HeaderType)
    __attribute__((alias("gj_pci_is_multifn_u")));
