/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5184: pure-value PCI function field extract (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_fn_u(uint32_t addr);
 *     - Extract the 3-bit function field from a PCI CONFIG_ADDRESS word
 *       (bits 10:8).
 *   uint32_t __gj_pci_fn_u  (alias)
 *   __libcgj_batch5184_marker = "libcgj-batch5184"
 *
 * Exclusive continuum CREATE-ONLY (5181-5190: PCI unique —
 * pci_cfg_addr_u, pci_bus_u, pci_dev_u, pci_fn_u, pci_reg_u,
 * pci_vendor_invalid_u, pci_is_multifn_u, pci_bar_is_io_u,
 * pci_bar_is_64_u, batch_id_5190). Unique gj_pci_fn_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5184_marker[] = "libcgj-batch5184";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5184_fn(uint32_t u32Addr)
{
	return (u32Addr >> 8) & 0x7u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_fn_u - extract function number from a PCI config address word.
 *
 * addr: packed CONFIG_ADDRESS-style word
 *
 * Returns bits 10:8 as a 3-bit function number. Self-contained; no parent
 * wires.
 */
uint32_t
gj_pci_fn_u(uint32_t u32Addr)
{
	(void)NULL;
	return b5184_fn(u32Addr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_fn_u(uint32_t u32Addr)
    __attribute__((alias("gj_pci_fn_u")));
