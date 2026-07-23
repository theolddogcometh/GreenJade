/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6836: pidfd_open flags popcount.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pidfd_open_flags_pop_6836(uint32_t flags);
 *     - Count set bits in flags (0..32).
 *   uint32_t __gj_pidfd_open_flags_pop_6836  (alias)
 *   __libcgj_batch6836_marker = "libcgj-batch6836"
 *
 * Exclusive continuum CREATE-ONLY (6831-6840: pidfd open flags stubs).
 * Unique gj_pidfd_open_flags_pop_6836 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6836_marker[] = "libcgj-batch6836";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6836_pop(uint32_t u32Flags)
{
	uint32_t u32N = 0u;
	uint32_t u32Bits = u32Flags;

	while (u32Bits != 0u) {
		u32N += (u32Bits & 1u);
		u32Bits >>= 1;
	}
	return u32N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pidfd_open_flags_pop_6836 - count set bits in pidfd_open flags.
 *
 * flags: soft pidfd_open flags bitmask
 *
 * Returns 0..32. Soft pure-data popcount; no pidfd_open.
 */
uint32_t
gj_pidfd_open_flags_pop_6836(uint32_t u32Flags)
{
	(void)NULL;
	return b6836_pop(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pidfd_open_flags_pop_6836(uint32_t u32Flags)
    __attribute__((alias("gj_pidfd_open_flags_pop_6836")));
