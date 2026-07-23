/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4409: safe uint64_t modulo (b==0 → 0).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mod_safe_u(uint64_t a, uint64_t b);
 *     - a % b as unsigned remainder. b == 0 → 0 (no trap).
 *   uint64_t __gj_u64_mod_safe_u  (alias)
 *   __libcgj_batch4409_marker = "libcgj-batch4409"
 *
 * Exclusive continuum CREATE-ONLY (4401-4410). Distinct from
 * gj_u32_mod_safe_u (batch4408) and wrap rem surfaces — unique
 * gj_u64_mod_safe_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4409_marker[] = "libcgj-batch4409";

/* ---- freestanding helpers ---------------------------------------------- */

/* a % b; b == 0 yields 0. */
static uint64_t
b4409_mod_safe(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return u64A % u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mod_safe_u - safe unsigned remainder; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 → 0)
 * No parent wires.
 */
uint64_t
gj_u64_mod_safe_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4409_mod_safe(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mod_safe_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_mod_safe_u")));
