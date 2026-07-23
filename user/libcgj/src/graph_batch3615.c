/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3615: unsigned 32-bit div with remainder (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_div_rem_u(uint32_t a, uint32_t b, uint32_t *rem);
 *     - Return floor(a / b). If rem is non-NULL, store a % b.
 *       b == 0 -> quotient 0; if rem non-NULL, store 0.
 *   uint32_t __gj_u32_div_rem_u  (alias)
 *   __libcgj_batch3615_marker = "libcgj-batch3615"
 *
 * Exclusive continuum CREATE-ONLY wave (3611-3620). Unique
 * gj_u32_div_rem_u surface only; no multi-def. Distinct from
 * gj_u32_div_floor_u (batch3003), gj_u32_rem_u (batch3005), and
 * gj_u64_div_mod_10 (batch1736). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3615_marker[] = "libcgj-batch3615";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(a/b) and a%b; b == 0 yields q=0, r=0. */
static uint32_t
b3615_div_rem(uint32_t u32A, uint32_t u32B, uint32_t *pu32Rem)
{
	uint32_t u32Q;
	uint32_t u32R;

	if (u32B == 0u) {
		u32Q = 0u;
		u32R = 0u;
	} else {
		u32Q = u32A / u32B;
		u32R = u32A % u32B;
	}
	if (pu32Rem != NULL) {
		*pu32Rem = u32R;
	}
	return u32Q;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_div_rem_u - floor divide a by b; optional remainder.
 *
 * a:   dividend
 * b:   divisor (0 -> quotient 0, rem 0)
 * rem: optional remainder out (a % b, or 0 when b == 0)
 *
 * Returns the quotient. Remainder is written only when rem != NULL.
 * No parent wires.
 */
uint32_t
gj_u32_div_rem_u(uint32_t u32A, uint32_t u32B, uint32_t *pu32Rem)
{
	(void)NULL;
	return b3615_div_rem(u32A, u32B, pu32Rem);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_div_rem_u(uint32_t u32A, uint32_t u32B, uint32_t *pu32Rem)
    __attribute__((alias("gj_u32_div_rem_u")));
