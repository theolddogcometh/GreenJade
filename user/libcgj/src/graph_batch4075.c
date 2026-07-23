/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4075: uint32_t modular wrap (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mod_wrap_u(uint32_t x, uint32_t m);
 *     - Wrap x into the half-open interval [0, m) via unsigned remainder.
 *       m == 0 → 0 (soft guard). Distinct public name from rem/mod
 *       siblings so the continuum can coexist at link time.
 *   uint32_t __gj_u32_mod_wrap_u  (alias)
 *   __libcgj_batch4075_marker = "libcgj-batch4075"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080). Unique
 * gj_u32_mod_wrap_u surface only; no multi-def. Distinct from
 * gj_u32_rem_u (batch3005), gj_u32_wrap_range_u (batch3266), and
 * gj_u32_mod_pow2_u (batch2909). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4075_marker[] = "libcgj-batch4075";

/* ---- freestanding helpers ---------------------------------------------- */

/* x mod m into [0, m); m == 0 yields 0. */
static uint32_t
b4075_mod_wrap(uint32_t u32X, uint32_t u32M)
{
	if (u32M == 0u) {
		return 0u;
	}
	return u32X % u32M;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mod_wrap_u - wrap x into [0, m) by modular remainder.
 *
 * x: value to wrap
 * m: modulus / period (0 → 0 soft guard)
 *
 * Returns x % m, or 0 when m is zero. No parent wires.
 */
uint32_t
gj_u32_mod_wrap_u(uint32_t u32X, uint32_t u32M)
{
	(void)NULL;
	return b4075_mod_wrap(u32X, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mod_wrap_u(uint32_t u32X, uint32_t u32M)
    __attribute__((alias("gj_u32_mod_wrap_u")));
