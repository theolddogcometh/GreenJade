/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6453: achievement unlock bit-clear stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ach_bit_clear_6453(uint32_t mask, uint32_t slot);
 *     - Clear bit `slot` (0..31) in mask; slots > 31 leave mask unchanged.
 *       Returns the updated unlock mask (soft revoke path).
 *   uint32_t __gj_ach_bit_clear_6453  (alias)
 *   __libcgj_batch6453_marker = "libcgj-batch6453"
 *
 * Exclusive continuum CREATE-ONLY (6451-6460: achievement unlock mask
 * stubs). Unique gj_ach_bit_clear_6453 surface only; no multi-def.
 * Distinct from gj_ach_bit_set_6452. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6453_marker[] = "libcgj-batch6453";

#define B6453_SLOT_MAX  31u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6453_bit_clear(uint32_t uMask, uint32_t uSlot)
{
	if (uSlot > B6453_SLOT_MAX) {
		return uMask;
	}
	return uMask & ~(1u << uSlot);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ach_bit_clear_6453 - soft-clear one achievement unlock bit.
 *
 * mask: current unlock mask
 * slot: bit index 0..31; out of range is a no-op
 *
 * Returns mask with bit cleared. Pure integer. No parent wires.
 */
uint32_t
gj_ach_bit_clear_6453(uint32_t mask, uint32_t slot)
{
	(void)NULL;
	return b6453_bit_clear(mask, slot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ach_bit_clear_6453(uint32_t mask, uint32_t slot)
    __attribute__((alias("gj_ach_bit_clear_6453")));
