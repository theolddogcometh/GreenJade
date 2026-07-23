/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5732: PCI config header offset ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_cfg_hdr_off_ok_5732(uint32_t off, uint32_t width);
 *     - Soft gate: off in [0, 255], width in {1,2,4}, and off aligned
 *       to width. Used by pure-data PCI enum walk stubs.
 *   uint32_t __gj_pci_cfg_hdr_off_ok_5732  (alias)
 *   __libcgj_batch5732_marker = "libcgj-batch5732"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe). Unique gj_pci_cfg_hdr_off_ok_5732
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5732_marker[] = "libcgj-batch5732";

/* Standard type-0/1 config space is 256 bytes (extended is 4K later). */
#define B5732_CFG_MAX  256u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5732_off_ok(uint32_t u32Off, uint32_t u32Width)
{
	if (u32Width != 1u && u32Width != 2u && u32Width != 4u) {
		return 0u;
	}
	if (u32Off >= B5732_CFG_MAX) {
		return 0u;
	}
	if ((u32Off + u32Width) > B5732_CFG_MAX) {
		return 0u;
	}
	if ((u32Off & (u32Width - 1u)) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_cfg_hdr_off_ok_5732 - validate config-space offset + width.
 *
 * off:   byte offset into 256-byte config header
 * width: access size 1, 2, or 4
 *
 * Returns 1 when in-range and naturally aligned, else 0. No MMIO.
 * No parent wires.
 */
uint32_t
gj_pci_cfg_hdr_off_ok_5732(uint32_t u32Off, uint32_t u32Width)
{
	(void)NULL;
	return b5732_off_ok(u32Off, u32Width);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pci_cfg_hdr_off_ok_5732(uint32_t u32Off, uint32_t u32Width)
    __attribute__((alias("gj_pci_cfg_hdr_off_ok_5732")));
