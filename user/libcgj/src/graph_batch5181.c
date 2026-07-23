/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5181: pure-value PCI config address pack (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_cfg_addr_u(uint32_t bus, uint32_t dev, uint32_t fn,
 *                              uint32_t reg);
 *     - Pack bus/dev/fn/reg into a classic Type-1 CONFIG_ADDRESS word
 *       with enable bit 31 set. Masks: bus 8, dev 5, fn 3, reg 8
 *       dword-aligned (low 2 bits cleared).
 *   uint32_t __gj_pci_cfg_addr_u  (alias)
 *   __libcgj_batch5181_marker = "libcgj-batch5181"
 *
 * Exclusive continuum CREATE-ONLY (5181-5190: PCI unique —
 * pci_cfg_addr_u, pci_bus_u, pci_dev_u, pci_fn_u, pci_reg_u,
 * pci_vendor_invalid_u, pci_is_multifn_u, pci_bar_is_io_u,
 * pci_bar_is_64_u, batch_id_5190). Distinct from gj_pci_parse_bdf
 * (batch158) — unique gj_pci_cfg_addr_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5181_marker[] = "libcgj-batch5181";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5181_cfg_addr(uint32_t u32Bus, uint32_t u32Dev, uint32_t u32Fn,
    uint32_t u32Reg)
{
	uint32_t u32Out;

	u32Out = 0x80000000u;
	u32Out |= (u32Bus & 0xffu) << 16;
	u32Out |= (u32Dev & 0x1fu) << 11;
	u32Out |= (u32Fn & 0x7u) << 8;
	u32Out |= (u32Reg & 0xfcu);
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_cfg_addr_u - pack bus/dev/fn/reg into a PCI CONFIG_ADDRESS word.
 *
 * bus: PCI bus number (masked to 8 bits)
 * dev: device number (masked to 5 bits)
 * fn:  function number (masked to 3 bits)
 * reg: config space offset (masked to dword-aligned 8 bits)
 *
 * Returns enable|bus|dev|fn|reg packed word. Self-contained; no parent
 * wires.
 */
uint32_t
gj_pci_cfg_addr_u(uint32_t u32Bus, uint32_t u32Dev, uint32_t u32Fn,
    uint32_t u32Reg)
{
	(void)NULL;
	return b5181_cfg_addr(u32Bus, u32Dev, u32Fn, u32Reg);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_cfg_addr_u(uint32_t u32Bus, uint32_t u32Dev, uint32_t u32Fn,
    uint32_t u32Reg)
    __attribute__((alias("gj_pci_cfg_addr_u")));
