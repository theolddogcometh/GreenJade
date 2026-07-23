/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5603: Deck APU PCI IDs (pair / class tags).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_pci_pair_vangogh_5603(void);
 *     - Returns packed (vendor<<16)|device for Van Gogh (0x1002163F).
 *   uint32_t gj_deck_apu_pci_pair_sephiroth_5603(void);
 *     - Returns packed (vendor<<16)|device for Sephiroth (0x10021435).
 *   uint32_t gj_deck_apu_pci_class_display_5603(void);
 *     - Returns 0x030000 (soft PCI display class base tag).
 *   uint32_t gj_deck_apu_pci_pair_ok_5603(uint32_t pair);
 *     - Return 1 if pair is Van Gogh or Sephiroth pack, else 0.
 *   uint32_t __gj_deck_apu_pci_pair_vangogh_5603  (alias)
 *   uint32_t __gj_deck_apu_pci_pair_sephiroth_5603  (alias)
 *   uint32_t __gj_deck_apu_pci_class_display_5603  (alias)
 *   uint32_t __gj_deck_apu_pci_pair_ok_5603  (alias)
 *   __libcgj_batch5603_marker = "libcgj-batch5603"
 *
 * Exclusive continuum CREATE-ONLY (5601-5610). Deck APU PCI pair tags;
 * distinct from gj_deck_apu_pci_*_5601/5602. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5603_marker[] = "libcgj-batch5603";

#define B5603_PAIR_VANGOGH  0x1002163Fu
#define B5603_PAIR_SEPH     0x10021435u
#define B5603_CLASS_DISP    0x030000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5603_pair_ok(uint32_t u32Pair)
{
	if (u32Pair == B5603_PAIR_VANGOGH) {
		return 1u;
	}
	if (u32Pair == B5603_PAIR_SEPH) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_pci_pair_vangogh_5603 - soft packed Van Gogh vendor:device.
 *
 * Always returns 0x1002163F.
 */
uint32_t
gj_deck_apu_pci_pair_vangogh_5603(void)
{
	(void)NULL;
	return B5603_PAIR_VANGOGH;
}

/*
 * gj_deck_apu_pci_pair_sephiroth_5603 - soft packed Sephiroth vendor:device.
 *
 * Always returns 0x10021435.
 */
uint32_t
gj_deck_apu_pci_pair_sephiroth_5603(void)
{
	return B5603_PAIR_SEPH;
}

/*
 * gj_deck_apu_pci_class_display_5603 - soft PCI display class tag.
 *
 * Always returns 0x030000 (base class/subclass/prog-if style tag).
 */
uint32_t
gj_deck_apu_pci_class_display_5603(void)
{
	return B5603_CLASS_DISP;
}

/*
 * gj_deck_apu_pci_pair_ok_5603 - accept known Deck APU packed pairs.
 *
 * pair: packed (vendor<<16)|device
 *
 * Returns 1 for Van Gogh or Sephiroth packs, else 0.
 */
uint32_t
gj_deck_apu_pci_pair_ok_5603(uint32_t u32Pair)
{
	return b5603_pair_ok(u32Pair);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_deck_apu_pci_pair_vangogh_5603(void)
    __attribute__((alias("gj_deck_apu_pci_pair_vangogh_5603")));

uint32_t __gj_deck_apu_pci_pair_sephiroth_5603(void)
    __attribute__((alias("gj_deck_apu_pci_pair_sephiroth_5603")));

uint32_t __gj_deck_apu_pci_class_display_5603(void)
    __attribute__((alias("gj_deck_apu_pci_class_display_5603")));

uint32_t __gj_deck_apu_pci_pair_ok_5603(uint32_t u32Pair)
    __attribute__((alias("gj_deck_apu_pci_pair_ok_5603")));
