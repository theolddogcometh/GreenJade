/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6454: achievement unlock bit-test stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ach_bit_test_6454(uint32_t mask, uint32_t slot);
 *     - Return 1 if bit `slot` (0..31) is set in mask, else 0.
 *       Slots > 31 always return 0.
 *   uint32_t __gj_ach_bit_test_6454  (alias)
 *   __libcgj_batch6454_marker = "libcgj-batch6454"
 *
 * Exclusive continuum CREATE-ONLY (6451-6460: achievement unlock mask
 * stubs). Unique gj_ach_bit_test_6454 surface only; no multi-def.
 * Distinct from gj_ach_bit_set_6452 / bit_clear_6453. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6454_marker[] = "libcgj-batch6454";

#define B6454_SLOT_MAX  31u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6454_bit_test(uint32_t uMask, uint32_t uSlot)
{
	if (uSlot > B6454_SLOT_MAX) {
		return 0u;
	}
	if ((uMask & (1u << uSlot)) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ach_bit_test_6454 - soft-test one achievement unlock bit.
 *
 * mask: current unlock mask
 * slot: bit index 0..31
 *
 * Returns 1 when unlocked, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_ach_bit_test_6454(uint32_t mask, uint32_t slot)
{
	(void)NULL;
	return b6454_bit_test(mask, slot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ach_bit_test_6454(uint32_t mask, uint32_t slot)
    __attribute__((alias("gj_ach_bit_test_6454")));
