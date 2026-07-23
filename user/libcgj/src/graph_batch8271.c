/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8271: safe uint32_t division (b==0 → 0).
 *
 * Surface (unique symbols):
 *   uint32_t gj_div_safe_u32_8271(uint32_t a, uint32_t b);
 *     - a / b as unsigned division. b == 0 → 0 (no trap).
 *   uint32_t __gj_div_safe_u32_8271  (alias)
 *   __libcgj_batch8271_marker = "libcgj-batch8271"
 *
 * Exclusive continuum CREATE-ONLY (8271-8280: div/mod safe integer
 * stubs — div_safe, mod_safe, div_ceil, div_floor, is_pow2, align_up,
 * align_down, round_up_pow2, gcd, batch_id_8280). Unique
 * gj_div_safe_u32_8271 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8271_marker[] = "libcgj-batch8271";

/* ---- freestanding helpers ---------------------------------------------- */

/* a / b; b == 0 yields 0. */
static uint32_t
b8271_div_safe(uint32_t u32A, uint32_t u32B)
{
	if (u32B == 0u) {
		return 0u;
	}
	return u32A / u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_div_safe_u32_8271 - safe unsigned division; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 → 0)
 * No parent wires.
 */
uint32_t
gj_div_safe_u32_8271(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8271_div_safe(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_div_safe_u32_8271(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_div_safe_u32_8271")));
