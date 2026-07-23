/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2315: wrapping uint64_t subtraction
 * (post-2310 arith exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sub_wrap(uint64_t a, uint64_t b);
 *     - Return a - b with modular wrap (mod 2^64). Plain unsigned
 *       wrap; not saturating.
 *   uint64_t __gj_u64_sub_wrap  (alias)
 *   __libcgj_batch2315_marker = "libcgj-batch2315"
 *
 * Post-2310 wrap/ovf exclusive wave (2311-2320). Distinct from
 * gj_u64_wrap_sub (batch739) and gj_u64_add_wrap (batch2314) -
 * unique gj_u64_sub_wrap surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2315_marker[] = "libcgj-batch2315";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular wrap sub mod 2^64 (C unsigned arithmetic). */
static uint64_t
b2315_sub(uint64_t u64A, uint64_t u64B)
{
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sub_wrap - modular wrap subtraction of two uint64_t values.
 *
 * a: minuend
 * b: subtrahend
 *
 * Returns (a - b) mod 2^64. C unsigned arithmetic already wraps; this
 * surface makes the wrap intent explicit for graph callers.
 */
uint64_t
gj_u64_sub_wrap(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2315_sub(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sub_wrap(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_sub_wrap")));
