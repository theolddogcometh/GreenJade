/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5737: USB host-controller enum slot bounds.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pci_usb_hc_slot_max_5737(void);
 *     - Soft max USB HC slots for a single enum pass (8).
 *   uint32_t gj_pci_usb_hc_slot_ok_5737(uint32_t slot);
 *     - Return 1 if slot < max, else 0.
 *   uint32_t gj_pci_usb_hc_count_clamp_5737(uint32_t found);
 *     - Clamp found HC count to [0, max].
 *   uint32_t __gj_pci_usb_hc_slot_max_5737  (alias)
 *   uint32_t __gj_pci_usb_hc_slot_ok_5737  (alias)
 *   uint32_t __gj_pci_usb_hc_count_clamp_5737  (alias)
 *   __libcgj_batch5737_marker = "libcgj-batch5737"
 *
 * Exclusive continuum CREATE-ONLY (5731-5740: real-hw PCI enum stubs
 * for Deck APU / USB / NVMe). Unique *_5737 surfaces only; no
 * multi-def. Distinct from gj_pci_usb_xhci_class_ok_5736. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5737_marker[] = "libcgj-batch5737";

/* Soft table depth for USB HC entries during PCI walk stubs. */
#define B5737_HC_SLOT_MAX  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5737_slot_max(void)
{
	return B5737_HC_SLOT_MAX;
}

static uint32_t
b5737_slot_ok(uint32_t u32Slot)
{
	if (u32Slot < B5737_HC_SLOT_MAX) {
		return 1u;
	}
	return 0u;
}

static uint32_t
b5737_count_clamp(uint32_t u32Found)
{
	if (u32Found > B5737_HC_SLOT_MAX) {
		return B5737_HC_SLOT_MAX;
	}
	return u32Found;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pci_usb_hc_slot_max_5737 - max USB HC slots for soft enum table.
 *
 * Always returns 8. Pure-data bound; does not scan the bus.
 */
uint32_t
gj_pci_usb_hc_slot_max_5737(void)
{
	(void)NULL;
	return b5737_slot_max();
}

/*
 * gj_pci_usb_hc_slot_ok_5737 - slot index in-range gate.
 *
 * slot: table index for a discovered HC
 * Returns 1 when slot < 8, else 0.
 */
uint32_t
gj_pci_usb_hc_slot_ok_5737(uint32_t u32Slot)
{
	return b5737_slot_ok(u32Slot);
}

/*
 * gj_pci_usb_hc_count_clamp_5737 - clamp discovered HC count.
 *
 * found: raw count from a soft enum pass
 * Returns min(found, 8).
 */
uint32_t
gj_pci_usb_hc_count_clamp_5737(uint32_t u32Found)
{
	return b5737_count_clamp(u32Found);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pci_usb_hc_slot_max_5737(void)
    __attribute__((alias("gj_pci_usb_hc_slot_max_5737")));

uint32_t __gj_pci_usb_hc_slot_ok_5737(uint32_t u32Slot)
    __attribute__((alias("gj_pci_usb_hc_slot_ok_5737")));

uint32_t __gj_pci_usb_hc_count_clamp_5737(uint32_t u32Found)
    __attribute__((alias("gj_pci_usb_hc_count_clamp_5737")));
