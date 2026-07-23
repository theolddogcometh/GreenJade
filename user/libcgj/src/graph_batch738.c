/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch738: wrapping uint64_t addition.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_wrap_add(uint64_t a, uint64_t b);
 *     — Return a + b with modular wrap (mod 2^64). Plain unsigned
 *       wrap; not saturating.
 *   uint64_t __gj_u64_wrap_add  (alias)
 *   __libcgj_batch738_marker = "libcgj-batch738"
 *
 * Distinct from saturating add surfaces (e.g. batch377). Unique
 * gj_u64_wrap_add surface — no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch738_marker[] = "libcgj-batch738";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_wrap_add — modular wrap addition of two uint64_t values.
 *
 * a, b: summands
 *
 * Returns (a + b) mod 2^64. C unsigned arithmetic already wraps; this
 * surface makes the wrap intent explicit for graph callers.
 */
uint64_t
gj_u64_wrap_add(uint64_t a, uint64_t b)
{
	return a + b;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_wrap_add(uint64_t a, uint64_t b)
    __attribute__((alias("gj_u64_wrap_add")));
