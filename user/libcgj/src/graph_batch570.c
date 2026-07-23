/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch570: uint64 subtract-with-borrow.
 *
 * Surface (unique symbols):
 *   uint64_t gj_subb_u64(uint64_t a, uint64_t b, uint64_t b_in,
 *                        uint64_t *b_out);
 *     — Compute r = a - b - b_in (mod 2^64). If b_out is non-NULL, store
 *       borrow-out 1 when the true difference is negative (underflow),
 *       else 0. Intended b_in is 0 or 1 (limb borrow chain), matching
 *       hardware SBB / __builtin_subcll-style semantics.
 *   __gj_subb_u64  (alias)
 *   __libcgj_batch570_marker = "libcgj-batch570"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch570_marker[] = "libcgj-batch570";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_subb_u64 — subtract with borrow.
 *
 * a:    minuend limb
 * b:    subtrahend limb
 * b_in: borrow in (0 or 1 for multiprecision chains)
 * b_out: optional; receives 0 or 1 borrow out
 *
 * Returns (a - b - b_in) mod 2^64. Borrow-out is set when
 * a < b + b_in in the wide sense, computed without a wider type:
 *   t = a - b;  bout = (a < b);
 *   r = t - b_in; bout |= (t < b_in);
 */
uint64_t
gj_subb_u64(uint64_t a, uint64_t b, uint64_t b_in, uint64_t *b_out)
{
	uint64_t t;
	uint64_t r;
	uint64_t bout;

	t = a - b;
	bout = (a < b) ? (uint64_t)1 : (uint64_t)0;
	r = t - b_in;
	if (t < b_in) {
		bout = (uint64_t)1;
	}

	if (b_out != NULL) {
		*b_out = bout;
	}
	return r;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_subb_u64(uint64_t a, uint64_t b, uint64_t b_in, uint64_t *b_out)
    __attribute__((alias("gj_subb_u64")));
