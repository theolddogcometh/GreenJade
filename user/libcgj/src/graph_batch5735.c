/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5735: Deck APU GPU class / vendor match.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_deck_gpu_vendor_id_5735(void);
 *     - Returns public AMD/ATI GPU vendor id 0x1002.
 *   uint32_t gj_pci_deck_gpu_class_ok_5735(uint32_t class_code);
 *     - Return 1 if base class is display controller (0x03), else 0.
 *   uint32_t gj_pci_deck_gpu_match_5735(uint32_t vendor_id,
 *                                       uint32_t class_code);
 *     - Return 1 when vendor is 0x1002 and class base is 0x03.
 *   uint32_t __gj_pci_deck_gpu_vendor_id_5735  (alias)
 *   uint32_t __gj_pci_deck_gpu_class_ok_5735  (alias)
 *   uint32_t __gj_pci_deck_gpu_match_5735  (alias)
 *   __libcgj_batch5735_marker = "libcgj-batch5735"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe). Unique *_5735 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5735_marker[] = "libcgj-batch5735";

/* Public AMD GPU vendor (PCI-SIG / ATI legacy). */
#define B5735_AMD_GPU_VENDOR  0x1002u
/* PCI base class: display controller. */
#define B5735_CLASS_DISPLAY   0x03u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5735_vendor_id(void)
{
	return B5735_AMD_GPU_VENDOR;
}

static uint32_t
b5735_class_ok(uint32_t u32Class)
{
	/* class_code packed as 0xBBSSPP — base class is bits 23:16. */
	if (((u32Class >> 16) & 0xFFu) == B5735_CLASS_DISPLAY) {
		return 1u;
	}
	return 0u;
}

static uint32_t
b5735_match(uint32_t u32Vendor, uint32_t u32Class)
{
	if (u32Vendor != B5735_AMD_GPU_VENDOR) {
		return 0u;
	}
	return b5735_class_ok(u32Class);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_deck_gpu_vendor_id_5735 - Deck iGPU soft vendor constant.
 *
 * Always returns 0x1002. No config I/O.
 */
uint32_t
gj_pci_deck_gpu_vendor_id_5735(void)
{
	(void)NULL;
	return b5735_vendor_id();
}

/*
 * gj_pci_deck_gpu_class_ok_5735 - display base-class gate.
 *
 * class_code: 24-bit packed class
 * Returns 1 when base class == 0x03.
 */
uint32_t
gj_pci_deck_gpu_class_ok_5735(uint32_t u32Class)
{
	return b5735_class_ok(u32Class);
}

/*
 * gj_pci_deck_gpu_match_5735 - vendor + display-class combo match.
 *
 * vendor_id / class_code: soft fields from a config header snapshot.
 * Returns 1 only when both gates pass.
 */
uint32_t
gj_pci_deck_gpu_match_5735(uint32_t u32Vendor, uint32_t u32Class)
{
	return b5735_match(u32Vendor, u32Class);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pci_deck_gpu_vendor_id_5735(void)
    __attribute__((alias("gj_pci_deck_gpu_vendor_id_5735")));

uint32_t __gj_pci_deck_gpu_class_ok_5735(uint32_t u32Class)
    __attribute__((alias("gj_pci_deck_gpu_class_ok_5735")));

uint32_t __gj_pci_deck_gpu_match_5735(uint32_t u32Vendor, uint32_t u32Class)
    __attribute__((alias("gj_pci_deck_gpu_match_5735")));
