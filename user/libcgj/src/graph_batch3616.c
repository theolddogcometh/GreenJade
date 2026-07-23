/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3616: unsigned 64-bit div with remainder (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_div_rem_u(uint64_t a, uint64_t b, uint64_t *rem);
 *     - Return floor(a / b). If rem is non-NULL, store a % b.
 *       b == 0 -> quotient 0; if rem non-NULL, store 0.
 *   uint64_t __gj_u64_div_rem_u  (alias)
 *   __libcgj_batch3616_marker = "libcgj-batch3616"
 *
 * Exclusive continuum CREATE-ONLY wave (3611-3620). Unique
 * gj_u64_div_rem_u surface only; no multi-def. Distinct from
 * gj_u64_div_floor (batch2034), gj_u64_rem_u (batch3006),
 * gj_u32_div_rem_u (batch3615), and gj_u64_div_mod_10 (batch1736).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3616_marker[] = "libcgj-batch3616";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(a/b) and a%b; b == 0 yields q=0, r=0. */
static uint64_t
b3616_div_rem(uint64_t u64A, uint64_t u64B, uint64_t *pu64Rem)
{
	uint64_t u64Q;
	uint64_t u64R;

	if (u64B == 0ull) {
		u64Q = 0ull;
		u64R = 0ull;
	} else {
		u64Q = u64A / u64B;
		u64R = u64A % u64B;
	}
	if (pu64Rem != NULL) {
		*pu64Rem = u64R;
	}
	return u64Q;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_div_rem_u - floor divide a by b; optional remainder.
 *
 * a:   dividend
 * b:   divisor (0 -> quotient 0, rem 0)
 * rem: optional remainder out (a % b, or 0 when b == 0)
 *
 * Returns the quotient. Remainder is written only when rem != NULL.
 * No parent wires.
 */
uint64_t
gj_u64_div_rem_u(uint64_t u64A, uint64_t u64B, uint64_t *pu64Rem)
{
	(void)NULL;
	return b3616_div_rem(u64A, u64B, pu64Rem);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_div_rem_u(uint64_t u64A, uint64_t u64B, uint64_t *pu64Rem)
    __attribute__((alias("gj_u64_div_rem_u")));
