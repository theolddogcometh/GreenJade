/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5734: Deck APU PCI vendor match (AMD).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_deck_apu_vendor_id_5734(void);
 *     - Returns public AMD system vendor id 0x1022 (Van Gogh / Deck
 *       APU family soft tag from public PCI-SIG vendor list).
 *   uint32_t gj_pci_deck_apu_vendor_ok_5734(uint32_t vendor_id);
 *     - Return 1 if vendor_id == 0x1022, else 0.
 *   uint32_t __gj_pci_deck_apu_vendor_id_5734  (alias)
 *   uint32_t __gj_pci_deck_apu_vendor_ok_5734  (alias)
 *   __libcgj_batch5734_marker = "libcgj-batch5734"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe). Unique *_5734 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5734_marker[] = "libcgj-batch5734";

/* Public AMD vendor id (PCI-SIG). Deck APU root / FCH soft match. */
#define B5734_AMD_VENDOR  0x1022u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5734_vendor_id(void)
{
	return B5734_AMD_VENDOR;
}

static uint32_t
b5734_vendor_ok(uint32_t u32Vendor)
{
	if (u32Vendor == B5734_AMD_VENDOR) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_deck_apu_vendor_id_5734 - Deck APU soft vendor constant.
 *
 * Always returns 0x1022. Pure-data product tag; no config space I/O.
 */
uint32_t
gj_pci_deck_apu_vendor_id_5734(void)
{
	(void)NULL;
	return b5734_vendor_id();
}

/*
 * gj_pci_deck_apu_vendor_ok_5734 - match Deck APU vendor id.
 *
 * vendor_id: 16-bit PCI vendor field
 * Returns 1 on AMD (0x1022), else 0.
 */
uint32_t
gj_pci_deck_apu_vendor_ok_5734(uint32_t u32Vendor)
{
	return b5734_vendor_ok(u32Vendor);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pci_deck_apu_vendor_id_5734(void)
    __attribute__((alias("gj_pci_deck_apu_vendor_id_5734")));

uint32_t __gj_pci_deck_apu_vendor_ok_5734(uint32_t u32Vendor)
    __attribute__((alias("gj_pci_deck_apu_vendor_ok_5734")));
