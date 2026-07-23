/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6832: pidfd_open flags bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pidfd_open_flags_bit_test_6832(uint32_t flags, uint32_t bit);
 *     - Return 1 if bit < 32 and (flags & (1u << bit)) != 0; else 0.
 *   uint32_t __gj_pidfd_open_flags_bit_test_6832  (alias)
 *   __libcgj_batch6832_marker = "libcgj-batch6832"
 *
 * Exclusive continuum CREATE-ONLY (6831-6840: pidfd open flags stubs).
 * Unique gj_pidfd_open_flags_bit_test_6832 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6832_marker[] = "libcgj-batch6832";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6832_test(uint32_t u32Flags, uint32_t u32Bit)
{
	if (u32Bit >= 32u) {
		return 0u;
	}
	return ((u32Flags >> u32Bit) & 1u) != 0u ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pidfd_open_flags_bit_test_6832 - test one bit in pidfd_open flags.
 *
 * flags: soft pidfd_open flags bitmask
 * bit:   bit index (0 = LSB); bit >= 32 yields 0
 *
 * Returns 1 if set, else 0. Soft pure-data bit test; no pidfd_open.
 */
uint32_t
gj_pidfd_open_flags_bit_test_6832(uint32_t u32Flags, uint32_t u32Bit)
{
	(void)NULL;
	return b6832_test(u32Flags, u32Bit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pidfd_open_flags_bit_test_6832(uint32_t u32Flags, uint32_t u32Bit)
    __attribute__((alias("gj_pidfd_open_flags_bit_test_6832")));
