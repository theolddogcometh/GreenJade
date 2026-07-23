/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8273: ceiling unsigned 32-bit division.
 *
 * Surface (unique symbols):
 *   uint32_t gj_div_ceil_u32_8273(uint32_t a, uint32_t b);
 *     - ceil(a / b) as unsigned integer division.
 *       b == 0 → 0. a == 0 → 0 for any b.
 *   uint32_t __gj_div_ceil_u32_8273  (alias)
 *   __libcgj_batch8273_marker = "libcgj-batch8273"
 *
 * Exclusive continuum CREATE-ONLY (8271-8280: div/mod safe integer
 * stubs — div_safe, mod_safe, div_ceil, div_floor, is_pow2, align_up,
 * align_down, round_up_pow2, gcd, batch_id_8280). Unique
 * gj_div_ceil_u32_8273 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8273_marker[] = "libcgj-batch8273";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(a / b) via quotient + (remainder != 0) so intermediate
 * a + b - 1 never overflows UINT32_MAX. b == 0 yields 0.
 */
static uint32_t
b8273_div_ceil(uint32_t u32A, uint32_t u32B)
{
	if (u32B == 0u) {
		return 0u;
	}
	return (u32A / u32B) + ((u32A % u32B) != 0u ? 1u : 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_div_ceil_u32_8273 - ceiling divide a by b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 → 0)
 * No parent wires.
 */
uint32_t
gj_div_ceil_u32_8273(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8273_div_ceil(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_div_ceil_u32_8273(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_div_ceil_u32_8273")));
