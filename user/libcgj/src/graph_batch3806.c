/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3806: wrapping uint64_t subtraction (wrap_sub_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_wrap_sub_u(uint64_t a, uint64_t b);
 *     - Return a - b with modular wrap (mod 2^64). Plain unsigned
 *       wrap; not saturating. Unsigned-form exclusive surface.
 *   uint64_t __gj_u64_wrap_sub_u  (alias)
 *   __libcgj_batch3806_marker = "libcgj-batch3806"
 *
 * Exclusive continuum CREATE-ONLY wave (3801-3810). Unique
 * gj_u64_wrap_sub_u surface only; no multi-def. Distinct from
 * gj_u64_sub_wrap_u (batch3204), gj_u64_wrap_sub (batch739), and
 * gj_u32_wrap_sub_u (batch3805). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3806_marker[] = "libcgj-batch3806";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular wrap sub mod 2^64 (C unsigned arithmetic). */
static uint64_t
b3806_sub(uint64_t u64A, uint64_t u64B)
{
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_wrap_sub_u - modular wrap subtraction of two uint64_t values.
 *
 * a: minuend
 * b: subtrahend
 *
 * Returns (a - b) mod 2^64. C unsigned arithmetic already wraps; this
 * surface makes the wrap intent explicit for graph callers.
 * No parent wires.
 */
uint64_t
gj_u64_wrap_sub_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3806_sub(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_wrap_sub_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_wrap_sub_u")));
