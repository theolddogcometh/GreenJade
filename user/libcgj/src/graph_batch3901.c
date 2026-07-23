/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3901: ceiling unsigned 32-bit division (cdiv_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_cdiv_u(uint32_t a, uint32_t b);
 *     - ceil(a / b) as unsigned integer division.
 *       b == 0 -> 0. a == 0 -> 0 for any b (including 0).
 *       Exact multiples: returns a / b; else (a / b) + 1.
 *   uint32_t __gj_u32_cdiv_u  (alias)
 *   __libcgj_batch3901_marker = "libcgj-batch3901"
 *
 * Exclusive continuum CREATE-ONLY (3901-3910: u32_cdiv_u, u64_cdiv_u,
 * u32_fdiv_u, u64_fdiv_u, u32_rem_nz_u, u64_rem_nz_u, u32_quot_nz_u,
 * u64_quot_nz_u, u32_divisible_u, batch_id_3910). Distinct from
 * gj_u32_div_ceil_u (batch2714) — unique gj_u32_cdiv_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3901_marker[] = "libcgj-batch3901";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(a / b) via quotient + (remainder != 0) so intermediate
 * a + b - 1 never overflows UINT32_MAX. b == 0 yields 0.
 */
static uint32_t
b3901_cdiv(uint32_t u32A, uint32_t u32B)
{
	if (u32B == 0u) {
		return 0u;
	}
	return (u32A / u32B) + ((u32A % u32B) != 0u ? 1u : 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_cdiv_u - ceiling divide a by b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 -> 0)
 * No parent wires.
 */
uint32_t
gj_u32_cdiv_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3901_cdiv(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_cdiv_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_cdiv_u")));
