/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5602: Deck APU PCI IDs (match helpers).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_apu_pci_is_amd_5602(uint32_t vendor);
 *     - Return 1 when vendor == 0x1002, else 0.
 *   uint32_t gj_deck_apu_pci_is_vangogh_5602(uint32_t device);
 *     - Return 1 when device == 0x163F, else 0.
 *   uint32_t gj_deck_apu_pci_is_sephiroth_5602(uint32_t device);
 *     - Return 1 when device == 0x1435, else 0.
 *   uint32_t gj_deck_apu_pci_match_score_5602(void);
 *     - Returns 5602 (wave-tagged PCI match score).
 *   uint32_t __gj_deck_apu_pci_is_amd_5602  (alias)
 *   uint32_t __gj_deck_apu_pci_is_vangogh_5602  (alias)
 *   uint32_t __gj_deck_apu_pci_is_sephiroth_5602  (alias)
 *   uint32_t __gj_deck_apu_pci_match_score_5602  (alias)
 *   __libcgj_batch5602_marker = "libcgj-batch5602"
 *
 * Exclusive continuum CREATE-ONLY (5601-5610). Deck APU PCI ID match
 * helpers; distinct from gj_deck_apu_pci_*_5601. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5602_marker[] = "libcgj-batch5602";

#define B5602_VENDOR_AMD   0x1002u
#define B5602_DEV_VANGOGH  0x163Fu
#define B5602_DEV_SEPH     0x1435u
#define B5602_SCORE        5602u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5602_eq(uint32_t u32A, uint32_t u32B)
{
	return (u32A == u32B) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_apu_pci_is_amd_5602 - soft AMD vendor match.
 *
 * vendor: candidate PCI vendor ID
 *
 * Returns 1 if vendor is 0x1002, else 0.
 */
uint32_t
gj_deck_apu_pci_is_amd_5602(uint32_t u32Vendor)
{
	(void)NULL;
	return b5602_eq(u32Vendor, B5602_VENDOR_AMD);
}

/*
 * gj_deck_apu_pci_is_vangogh_5602 - soft Van Gogh device match.
 *
 * device: candidate PCI device ID
 *
 * Returns 1 if device is 0x163F, else 0.
 */
uint32_t
gj_deck_apu_pci_is_vangogh_5602(uint32_t u32Device)
{
	return b5602_eq(u32Device, B5602_DEV_VANGOGH);
}

/*
 * gj_deck_apu_pci_is_sephiroth_5602 - soft Sephiroth device match.
 *
 * device: candidate PCI device ID
 *
 * Returns 1 if device is 0x1435, else 0.
 */
uint32_t
gj_deck_apu_pci_is_sephiroth_5602(uint32_t u32Device)
{
	return b5602_eq(u32Device, B5602_DEV_SEPH);
}

/*
 * gj_deck_apu_pci_match_score_5602 - wave-tagged PCI match score.
 *
 * Always returns 5602.
 */
uint32_t
gj_deck_apu_pci_match_score_5602(void)
{
	return B5602_SCORE;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_deck_apu_pci_is_amd_5602(uint32_t u32Vendor)
    __attribute__((alias("gj_deck_apu_pci_is_amd_5602")));

uint32_t __gj_deck_apu_pci_is_vangogh_5602(uint32_t u32Device)
    __attribute__((alias("gj_deck_apu_pci_is_vangogh_5602")));

uint32_t __gj_deck_apu_pci_is_sephiroth_5602(uint32_t u32Device)
    __attribute__((alias("gj_deck_apu_pci_is_sephiroth_5602")));

uint32_t __gj_deck_apu_pci_match_score_5602(void)
    __attribute__((alias("gj_deck_apu_pci_match_score_5602")));
