/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3006: unsigned 64-bit remainder (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_rem_u(uint64_t a, uint64_t b);
 *     - a % b as unsigned remainder.
 *       b == 0 -> 0. a == 0 -> 0 for any b (including 0).
 *   uint64_t __gj_u64_rem_u  (alias)
 *   __libcgj_batch3006_marker = "libcgj-batch3006"
 *
 * Exclusive continuum CREATE-ONLY (3001-3010). Unique gj_u64_rem_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3006_marker[] = "libcgj-batch3006";

/* ---- freestanding helpers ---------------------------------------------- */

/* a % b; b == 0 yields 0. */
static uint64_t
b3006_rem(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return u64A % u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_rem_u - unsigned remainder a mod b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 -> 0)
 * No parent wires.
 */
uint64_t
gj_u64_rem_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3006_rem(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_rem_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_rem_u")));
