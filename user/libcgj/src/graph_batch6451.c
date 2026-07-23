/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6451: achievement slot index soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ach_slot_ok_6451(uint32_t slot);
 *     - Soft-ok when slot is in [0, 31] (single uint32_t unlock mask
 *       bit index). Returns 1 if ok, else 0.
 *   uint32_t __gj_ach_slot_ok_6451  (alias)
 *   __libcgj_batch6451_marker = "libcgj-batch6451"
 *
 * Exclusive continuum CREATE-ONLY (6451-6460: achievement unlock mask
 * stubs — slot_ok, bit_set, bit_clear, bit_test, mask_merge, mask_filter,
 * popcount, progress_ok, unlock_gate, continuum + batch_id). Unique
 * gj_ach_slot_ok_6451 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6451_marker[] = "libcgj-batch6451";

/* Soft max bit index for a 32-bit achievement unlock mask. */
#define B6451_SLOT_MAX  31u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6451_slot_ok(uint32_t uSlot)
{
	if (uSlot > B6451_SLOT_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ach_slot_ok_6451 - soft-validate achievement bit slot index.
 *
 * slot: achievement bit index; must be <= 31 for a uint32 mask.
 * Returns 1 when valid, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_ach_slot_ok_6451(uint32_t slot)
{
	(void)NULL;
	return b6451_slot_ok(slot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ach_slot_ok_6451(uint32_t slot)
    __attribute__((alias("gj_ach_slot_ok_6451")));
