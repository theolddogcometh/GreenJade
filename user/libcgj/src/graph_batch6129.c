/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6129: Mesa RADV Deck soft feature baseline.
 *
 * Surface (unique symbols):
 *   uint32_t gj_radv_feat_deck_baseline_6129(void);
 *     - Returns soft Deck APU RADV baseline feature mask (0x0000000F):
 *       bits 0..3 reserved as continuum baseline lamps.
 *   uint32_t gj_radv_feat_deck_ok_6129(uint32_t mask);
 *     - Return 1 if mask covers the Deck baseline (has-all baseline).
 *   uint32_t __gj_radv_feat_deck_baseline_6129  (alias)
 *   uint32_t __gj_radv_feat_deck_ok_6129  (alias)
 *   __libcgj_batch6129_marker = "libcgj-batch6129"
 *
 * Exclusive CREATE-ONLY (6121-6130: mesa radv feature bit stubs).
 * Unique gj_radv_feat_*_6129 surfaces only; no multi-def. Distinct
 * from gj_mesa_radv_deck_ready_5518. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6129_marker[] = "libcgj-batch6129";

/* Soft Deck baseline: low four feature lamps (compile-time tag only). */
#define B6129_DECK_BASELINE  0x0000000Fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6129_baseline(void)
{
	return B6129_DECK_BASELINE;
}

static uint32_t
b6129_deck_ok(uint32_t u32Mask)
{
	uint32_t u32Need = B6129_DECK_BASELINE;

	return ((u32Mask & u32Need) == u32Need) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radv_feat_deck_baseline_6129 - soft Deck RADV baseline feature mask.
 *
 * Always returns 0xF. Soft pure-data product tag; does not query RADV,
 * Vulkan, or amdgpu. No parent wires.
 */
uint32_t
gj_radv_feat_deck_baseline_6129(void)
{
	(void)NULL;
	return b6129_baseline();
}

/*
 * gj_radv_feat_deck_ok_6129 - mask covers Deck RADV baseline bits.
 *
 * mask: soft RADV feature bitmask
 *
 * Returns 1 if all baseline bits present; else 0.
 */
uint32_t
gj_radv_feat_deck_ok_6129(uint32_t u32Mask)
{
	return b6129_deck_ok(u32Mask);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_radv_feat_deck_baseline_6129(void)
    __attribute__((alias("gj_radv_feat_deck_baseline_6129")));

uint32_t __gj_radv_feat_deck_ok_6129(uint32_t u32Mask)
    __attribute__((alias("gj_radv_feat_deck_ok_6129")));
