/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3206: wrapping uint64_t multiply (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_wrap_u(uint64_t a, uint64_t b);
 *     - Return a * b with modular wrap (mod 2^64). Plain unsigned
 *       wrap; not saturating. Unsigned-form exclusive surface.
 *       Portable C wrap only - no __int128.
 *   uint64_t __gj_u64_mul_wrap_u  (alias)
 *   __libcgj_batch3206_marker = "libcgj-batch3206"
 *
 * Exclusive continuum CREATE-ONLY wave (3201-3210). Unique
 * gj_u64_mul_wrap_u surface only; no multi-def. Distinct from
 * gj_u64_mul_wrap_soft (batch2316) and gj_u32_mul_wrap_u (batch3205).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3206_marker[] = "libcgj-batch3206";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular wrap mul mod 2^64 (C unsigned arithmetic; no __int128). */
static uint64_t
b3206_mul(uint64_t u64A, uint64_t u64B)
{
return u64A * u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_wrap_u - modular wrap multiplication of two uint64_t values.
 *
 * a, b: factors
 *
 * Returns (a * b) mod 2^64. C unsigned arithmetic already wraps; this
 * surface makes the wrap intent explicit for graph callers.
 * No parent wires. No __int128.
 */
uint64_t
gj_u64_mul_wrap_u(uint64_t u64A, uint64_t u64B)
{
(void)NULL;
return b3206_mul(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_wrap_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_mul_wrap_u")));
