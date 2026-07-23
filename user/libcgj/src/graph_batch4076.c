/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4076: uint64_t modular wrap (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mod_wrap_u(uint64_t x, uint64_t m);
 *     - Wrap x into the half-open interval [0, m) via unsigned remainder.
 *       m == 0 → 0 (soft guard). Distinct public name from rem/mod
 *       siblings so the continuum can coexist at link time.
 *   uint64_t __gj_u64_mod_wrap_u  (alias)
 *   __libcgj_batch4076_marker = "libcgj-batch4076"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080). Unique
 * gj_u64_mod_wrap_u surface only; no multi-def. Distinct from
 * gj_u64_mod_u (batch2036), gj_u32_mod_wrap_u (batch4075), and
 * gj_u64_mod_pow2_u (batch2911). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4076_marker[] = "libcgj-batch4076";

/* ---- freestanding helpers ---------------------------------------------- */

/* x mod m into [0, m); m == 0 yields 0. */
static uint64_t
b4076_mod_wrap(uint64_t u64X, uint64_t u64M)
{
	if (u64M == 0ull) {
		return 0ull;
	}
	return u64X % u64M;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mod_wrap_u - wrap x into [0, m) by modular remainder.
 *
 * x: value to wrap
 * m: modulus / period (0 → 0 soft guard)
 *
 * Returns x % m, or 0 when m is zero. No parent wires.
 */
uint64_t
gj_u64_mod_wrap_u(uint64_t u64X, uint64_t u64M)
{
	(void)NULL;
	return b4076_mod_wrap(u64X, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mod_wrap_u(uint64_t u64X, uint64_t u64M)
    __attribute__((alias("gj_u64_mod_wrap_u")));
