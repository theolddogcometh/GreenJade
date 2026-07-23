/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch569: uint64 add-with-carry.
 *
 * Surface (unique symbols):
 *   uint64_t gj_addc_u64(uint64_t a, uint64_t b, uint64_t c_in,
 *                        uint64_t *c_out);
 *     — Compute r = a + b + c_in (mod 2^64). If c_out is non-NULL, store
 *       carry-out 1 when the true sum exceeds UINT64_MAX, else 0.
 *       Intended c_in is 0 or 1 (limb carry chain), matching hardware
 *       ADC / __builtin_addcll-style semantics. c_out may be NULL.
 *   __gj_addc_u64  (alias)
 *   __libcgj_batch569_marker = "libcgj-batch569"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch569_marker[] = "libcgj-batch569";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_addc_u64 — add with carry.
 *
 * a:    first addend limb
 * b:    second addend limb
 * c_in: carry in (0 or 1 for multiprecision chains)
 * c_out: optional; receives 0 or 1 carry out
 *
 * Returns (a + b + c_in) mod 2^64. Carry-out is set when
 * a + b + c_in >= 2^64, computed without a wider type:
 *   t = a + b;  cout = (t < a);
 *   r = t + c_in; cout |= (r < t);
 * With c_in in {0,1} the two partial carries never both fire, so cout
 * is always 0 or 1.
 */
uint64_t
gj_addc_u64(uint64_t a, uint64_t b, uint64_t c_in, uint64_t *c_out)
{
	uint64_t t;
	uint64_t r;
	uint64_t cout;

	t = a + b;
	cout = (t < a) ? (uint64_t)1 : (uint64_t)0;
	r = t + c_in;
	if (r < t) {
		cout = (uint64_t)1;
	}

	if (c_out != NULL) {
		*c_out = cout;
	}
	return r;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_addc_u64(uint64_t a, uint64_t b, uint64_t c_in, uint64_t *c_out)
    __attribute__((alias("gj_addc_u64")));
