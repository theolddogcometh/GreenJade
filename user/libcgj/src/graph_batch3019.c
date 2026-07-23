/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3019: uint64 add-with-carry (exclusive
 * carry_add_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_carry_add_u64_u(uint64_t a, uint64_t b, uint64_t c_in,
 *                              uint64_t *c_out);
 *     - Compute r = a + b + c_in (mod 2^64). If c_out is non-NULL, store
 *       carry-out 1 when the true sum exceeds UINT64_MAX, else 0.
 *       Intended c_in is 0 or 1 (limb carry chain). c_out may be NULL.
 *   uint64_t __gj_carry_add_u64_u  (alias)
 *   __libcgj_batch3019_marker = "libcgj-batch3019"
 *
 * Exclusive continuum CREATE-ONLY wave 3011-3020. Distinct from
 * gj_addc_u64 (batch569), gj_carry_add_u32_u (batch3018). Unique
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3019_marker[] = "libcgj-batch3019";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3019_addc(uint64_t u64A, uint64_t u64B, uint64_t u64Cin, uint64_t *pCout)
{
	uint64_t t;
	uint64_t r;
	uint64_t cout;

	t = u64A + u64B;
	cout = (t < u64A) ? 1ull : 0ull;
	r = t + u64Cin;
	if (r < t) {
		cout = 1ull;
	}
	if (pCout != NULL) {
		*pCout = cout;
	}
	return r;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_carry_add_u64_u - add with carry (64-bit limbs).
 *
 * a:     first addend limb
 * b:     second addend limb
 * c_in:  carry in (0 or 1 for multiprecision chains)
 * c_out: optional; receives 0 or 1 carry out
 *
 * Returns (a + b + c_in) mod 2^64. Carry-out is set when
 * a + b + c_in >= 2^64, computed without a wider type / __int128:
 *   t = a + b;  cout = (t < a);
 *   r = t + c_in; cout |= (r < t);
 */
uint64_t
gj_carry_add_u64_u(uint64_t u64A, uint64_t u64B, uint64_t u64Cin,
    uint64_t *pCout)
{
	(void)NULL;
	return b3019_addc(u64A, u64B, u64Cin, pCout);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_carry_add_u64_u(uint64_t u64A, uint64_t u64B, uint64_t u64Cin,
    uint64_t *pCout)
    __attribute__((alias("gj_carry_add_u64_u")));
