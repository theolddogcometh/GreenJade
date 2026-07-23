/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2036: unsigned 64-bit remainder (mod).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mod_u(uint64_t a, uint64_t b);
 *     — a % b as unsigned remainder.
 *       b == 0 → 0. a == 0 → 0 for any b (including 0).
 *   uint64_t __gj_u64_mod_u  (alias)
 *   __libcgj_batch2036_marker = "libcgj-batch2036"
 *
 * Post-2030 u64 arith exclusive wave (2031–2040). Distinct from
 * gj_mod_u64 (batch430) and gj_u64_mod_pow2 (batch1068) — different
 * public names; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2036_marker[] = "libcgj-batch2036";

/* ---- freestanding helpers ---------------------------------------------- */

/* a % b; b == 0 yields 0. */
static uint64_t
b2036_mod(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return u64A % u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mod_u — unsigned remainder a % b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 → 0)
 */
uint64_t
gj_u64_mod_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2036_mod(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mod_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_mod_u")));
