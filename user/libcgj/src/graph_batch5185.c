/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5185: pure-value PCI register field extract (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_reg_u(uint32_t addr);
 *     - Extract the dword-aligned register offset from a PCI
 *       CONFIG_ADDRESS word (bits 7:2, low 2 bits forced clear).
 *   uint32_t __gj_pci_reg_u  (alias)
 *   __libcgj_batch5185_marker = "libcgj-batch5185"
 *
 * Exclusive continuum CREATE-ONLY (5181-5190: PCI unique —
 * pci_cfg_addr_u, pci_bus_u, pci_dev_u, pci_fn_u, pci_reg_u,
 * pci_vendor_invalid_u, pci_is_multifn_u, pci_bar_is_io_u,
 * pci_bar_is_64_u, batch_id_5190). Unique gj_pci_reg_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5185_marker[] = "libcgj-batch5185";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5185_reg(uint32_t u32Addr)
{
	return u32Addr & 0xfcu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_reg_u - extract config register offset from a PCI address word.
 *
 * addr: packed CONFIG_ADDRESS-style word
 *
 * Returns bits 7:0 with low 2 bits cleared (dword-aligned offset).
 * Self-contained; no parent wires.
 */
uint32_t
gj_pci_reg_u(uint32_t u32Addr)
{
	(void)NULL;
	return b5185_reg(u32Addr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_reg_u(uint32_t u32Addr)
    __attribute__((alias("gj_pci_reg_u")));
