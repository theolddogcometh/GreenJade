/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6833: pidfd_open flags bit set.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pidfd_open_flags_bit_set_6833(uint32_t flags, uint32_t bit);
 *     - Return flags with bit set when bit < 32; else return flags
 *       unchanged (out-of-range is a no-op).
 *   uint32_t __gj_pidfd_open_flags_bit_set_6833  (alias)
 *   __libcgj_batch6833_marker = "libcgj-batch6833"
 *
 * Exclusive continuum CREATE-ONLY (6831-6840: pidfd open flags stubs).
 * Unique gj_pidfd_open_flags_bit_set_6833 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6833_marker[] = "libcgj-batch6833";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6833_set(uint32_t u32Flags, uint32_t u32Bit)
{
	if (u32Bit >= 32u) {
		return u32Flags;
	}
	return u32Flags | (1u << u32Bit);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pidfd_open_flags_bit_set_6833 - set one bit in pidfd_open flags.
 *
 * flags: soft pidfd_open flags bitmask
 * bit:   bit index (0 = LSB); bit >= 32 leaves flags unchanged
 *
 * Returns updated flags. Soft pure-data op; no pidfd_open.
 */
uint32_t
gj_pidfd_open_flags_bit_set_6833(uint32_t u32Flags, uint32_t u32Bit)
{
	(void)NULL;
	return b6833_set(u32Flags, u32Bit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pidfd_open_flags_bit_set_6833(uint32_t u32Flags, uint32_t u32Bit)
    __attribute__((alias("gj_pidfd_open_flags_bit_set_6833")));
