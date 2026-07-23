/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6452: achievement unlock bit-set stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ach_bit_set_6452(uint32_t mask, uint32_t slot);
 *     - Set bit `slot` (0..31) in mask; slots > 31 leave mask unchanged.
 *       Returns the updated unlock mask.
 *   uint32_t __gj_ach_bit_set_6452  (alias)
 *   __libcgj_batch6452_marker = "libcgj-batch6452"
 *
 * Exclusive continuum CREATE-ONLY (6451-6460: achievement unlock mask
 * stubs). Unique gj_ach_bit_set_6452 surface only; no multi-def.
 * Distinct from gj_ach_slot_ok_6451. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6452_marker[] = "libcgj-batch6452";

#define B6452_SLOT_MAX  31u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6452_bit_set(uint32_t uMask, uint32_t uSlot)
{
	if (uSlot > B6452_SLOT_MAX) {
		return uMask;
	}
	return uMask | (1u << uSlot);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ach_bit_set_6452 - soft-set one achievement unlock bit.
 *
 * mask: current unlock mask
 * slot: bit index 0..31; out of range is a no-op
 *
 * Returns mask with bit set. Pure integer. No parent wires.
 */
uint32_t
gj_ach_bit_set_6452(uint32_t mask, uint32_t slot)
{
	(void)NULL;
	return b6452_bit_set(mask, slot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ach_bit_set_6452(uint32_t mask, uint32_t slot)
    __attribute__((alias("gj_ach_bit_set_6452")));
