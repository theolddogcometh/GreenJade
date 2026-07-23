/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5731: PCI BDF pack for real-hw enum stubs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_bdf_pack_5731(uint32_t bus, uint32_t dev,
 *                                 uint32_t func);
 *     - Pack bus:dev.func into a 16-bit-shaped BDF word:
 *       (bus << 8) | (dev << 3) | func. Out-of-range fields → 0.
 *   uint32_t __gj_pci_bdf_pack_5731  (alias)
 *   __libcgj_batch5731_marker = "libcgj-batch5731"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe). Unique gj_pci_bdf_pack_5731 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5731_marker[] = "libcgj-batch5731";

/* PCI bus 0..255, device 0..31, function 0..7 (type-0/1 config shape). */
#define B5731_BUS_MAX   255u
#define B5731_DEV_MAX   31u
#define B5731_FUNC_MAX  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5731_pack(uint32_t u32Bus, uint32_t u32Dev, uint32_t u32Func)
{
	if (u32Bus > B5731_BUS_MAX) {
		return 0u;
	}
	if (u32Dev > B5731_DEV_MAX) {
		return 0u;
	}
	if (u32Func > B5731_FUNC_MAX) {
		return 0u;
	}
	return (u32Bus << 8) | (u32Dev << 3) | u32Func;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_bdf_pack_5731 - pack bus/dev/func into a soft BDF word.
 *
 * bus:  0..255
 * dev:  0..31
 * func: 0..7
 *
 * Returns packed BDF, or 0 if any field is out of range. Pure integer;
 * does not touch config space. No parent wires.
 */
uint32_t
gj_pci_bdf_pack_5731(uint32_t u32Bus, uint32_t u32Dev, uint32_t u32Func)
{
	(void)NULL;
	return b5731_pack(u32Bus, u32Dev, u32Func);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_bdf_pack_5731(uint32_t u32Bus, uint32_t u32Dev,
    uint32_t u32Func)
    __attribute__((alias("gj_pci_bdf_pack_5731")));
