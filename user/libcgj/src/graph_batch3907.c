/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3907: nonzero-divisor u32 quotient (quot_nz_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_quot_nz_u(uint32_t a, uint32_t b);
 *     - a / b as unsigned floor quotient (intended nonzero divisor).
 *       b == 0 -> 0 (guard; avoids UB). a == 0 -> 0 for any b.
 *   uint32_t __gj_u32_quot_nz_u  (alias)
 *   __libcgj_batch3907_marker = "libcgj-batch3907"
 *
 * Exclusive continuum CREATE-ONLY (3901-3910). Distinct from
 * gj_u32_fdiv_u (batch3903) / gj_u32_div_floor_u (batch3003) — unique
 * gj_u32_quot_nz_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3907_marker[] = "libcgj-batch3907";

/* ---- freestanding helpers ---------------------------------------------- */

/* a / b; b == 0 yields 0. */
static uint32_t
b3907_quot(uint32_t u32A, uint32_t u32B)
{
	if (u32B == 0u) {
		return 0u;
	}
	return u32A / u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_quot_nz_u - unsigned quotient a / b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 -> 0; nonzero path is the normal case)
 * No parent wires.
 */
uint32_t
gj_u32_quot_nz_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3907_quot(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_quot_nz_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_quot_nz_u")));
