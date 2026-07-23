/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch739: wrapping uint64_t subtraction.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_wrap_sub(uint64_t a, uint64_t b);
 *     — Return a - b with modular wrap (mod 2^64). Plain unsigned
 *       wrap; not saturating.
 *   uint64_t __gj_u64_wrap_sub  (alias)
 *   __libcgj_batch739_marker = "libcgj-batch739"
 *
 * Distinct from saturating sub surfaces (e.g. batch377) and from
 * gj_u64_wrap_add (batch738). Unique gj_u64_wrap_sub surface — no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch739_marker[] = "libcgj-batch739";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_wrap_sub — modular wrap subtraction of two uint64_t values.
 *
 * a: minuend
 * b: subtrahend
 *
 * Returns (a - b) mod 2^64. C unsigned arithmetic already wraps; this
 * surface makes the wrap intent explicit for graph callers.
 */
uint64_t
gj_u64_wrap_sub(uint64_t a, uint64_t b)
{
	return a - b;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_wrap_sub(uint64_t a, uint64_t b)
    __attribute__((alias("gj_u64_wrap_sub")));
