/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3018: uint32 add-with-carry (exclusive
 * carry_add_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_carry_add_u32_u(uint32_t a, uint32_t b, uint32_t c_in,
 *                              uint32_t *c_out);
 *     - Compute r = a + b + c_in (mod 2^32). If c_out is non-NULL, store
 *       carry-out 1 when the true sum exceeds UINT32_MAX, else 0.
 *       Intended c_in is 0 or 1 (limb carry chain). c_out may be NULL.
 *   uint32_t __gj_carry_add_u32_u  (alias)
 *   __libcgj_batch3018_marker = "libcgj-batch3018"
 *
 * Exclusive continuum CREATE-ONLY wave 3011-3020. Distinct from
 * gj_addc_u64 (batch569), gj_u64_add_check (batch647). Unique surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3018_marker[] = "libcgj-batch3018";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3018_addc(uint32_t u32A, uint32_t u32B, uint32_t u32Cin, uint32_t *pCout)
{
	uint32_t t;
	uint32_t r;
	uint32_t cout;

	t = u32A + u32B;
	cout = (t < u32A) ? 1u : 0u;
	r = t + u32Cin;
	if (r < t) {
		cout = 1u;
	}
	if (pCout != NULL) {
		*pCout = cout;
	}
	return r;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_carry_add_u32_u - add with carry (32-bit limbs).
 *
 * a:     first addend limb
 * b:     second addend limb
 * c_in:  carry in (0 or 1 for multiprecision chains)
 * c_out: optional; receives 0 or 1 carry out
 *
 * Returns (a + b + c_in) mod 2^32. Carry-out is set when
 * a + b + c_in >= 2^32, computed without a wider type:
 *   t = a + b;  cout = (t < a);
 *   r = t + c_in; cout |= (r < t);
 */
uint32_t
gj_carry_add_u32_u(uint32_t u32A, uint32_t u32B, uint32_t u32Cin,
    uint32_t *pCout)
{
	(void)NULL;
	return b3018_addc(u32A, u32B, u32Cin, pCout);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_carry_add_u32_u(uint32_t u32A, uint32_t u32B, uint32_t u32Cin,
    uint32_t *pCout)
    __attribute__((alias("gj_carry_add_u32_u")));
