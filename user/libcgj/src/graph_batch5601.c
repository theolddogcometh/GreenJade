/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5601: Deck APU PCI IDs (vendor / device).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_pci_vendor_5601(void);
 *     - Returns 0x1002 (soft AMD PCI vendor ID for Deck APU).
 *   uint32_t gj_deck_apu_pci_device_vangogh_5601(void);
 *     - Returns 0x163F (soft Van Gogh / Deck LCD APU device ID).
 *   uint32_t gj_deck_apu_pci_device_sephiroth_5601(void);
 *     - Returns 0x1435 (soft Sephiroth / Deck OLED APU device ID).
 *   uint32_t gj_batch_id_5601(void);
 *     - Returns 5601 (this TU's graph batch number).
 *   uint32_t __gj_deck_apu_pci_vendor_5601  (alias)
 *   uint32_t __gj_deck_apu_pci_device_vangogh_5601  (alias)
 *   uint32_t __gj_deck_apu_pci_device_sephiroth_5601  (alias)
 *   uint32_t __gj_batch_id_5601  (alias)
 *   __libcgj_batch5601_marker = "libcgj-batch5601"
 *
 * Exclusive continuum CREATE-ONLY (5601-5610: Deck APU PCI IDs, VRAM
 * budget stubs, GPU clock min/max, mesa version pack). Toward bar #3
 * Steam Deck install / real-hw. Unique batch-suffixed symbols only;
 * no multi-def of bare names. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5601_marker[] = "libcgj-batch5601";

#define B5601_VENDOR_AMD   0x1002u
#define B5601_DEV_VANGOGH  0x163Fu
#define B5601_DEV_SEPH     0x1435u
#define B5601_BATCH        5601u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5601_vendor(void)
{
	return B5601_VENDOR_AMD;
}

static uint32_t
b5601_id(void)
{
	return B5601_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_pci_vendor_5601 - soft AMD PCI vendor ID for Deck APU.
 *
 * Always returns 0x1002. Compile-time product-path constant; does not
 * enumerate PCI. No parent wires.
 */
uint32_t
gj_deck_apu_pci_vendor_5601(void)
{
	(void)NULL;
	return b5601_vendor();
}

/*
 * gj_deck_apu_pci_device_vangogh_5601 - soft Van Gogh APU device ID.
 *
 * Always returns 0x163F (Deck LCD generation soft tag).
 */
uint32_t
gj_deck_apu_pci_device_vangogh_5601(void)
{
	return B5601_DEV_VANGOGH;
}

/*
 * gj_deck_apu_pci_device_sephiroth_5601 - soft Sephiroth APU device ID.
 *
 * Always returns 0x1435 (Deck OLED generation soft tag).
 */
uint32_t
gj_deck_apu_pci_device_sephiroth_5601(void)
{
	return B5601_DEV_SEPH;
}

/*
 * gj_batch_id_5601 - report this TU's graph batch number.
 *
 * Always returns 5601.
 */
uint32_t
gj_batch_id_5601(void)
{
	return b5601_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_deck_apu_pci_vendor_5601(void)
    __attribute__((alias("gj_deck_apu_pci_vendor_5601")));

uint32_t __gj_deck_apu_pci_device_vangogh_5601(void)
    __attribute__((alias("gj_deck_apu_pci_device_vangogh_5601")));

uint32_t __gj_deck_apu_pci_device_sephiroth_5601(void)
    __attribute__((alias("gj_deck_apu_pci_device_sephiroth_5601")));

uint32_t __gj_batch_id_5601(void)
    __attribute__((alias("gj_batch_id_5601")));
