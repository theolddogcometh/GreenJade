/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3205: wrapping uint32_t multiply (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mul_wrap_u(uint32_t a, uint32_t b);
 *     - Return a * b with modular wrap (mod 2^32). Plain unsigned
 *       wrap; not saturating. Unsigned-form exclusive surface.
 *   uint32_t __gj_u32_mul_wrap_u  (alias)
 *   __libcgj_batch3205_marker = "libcgj-batch3205"
 *
 * Exclusive continuum CREATE-ONLY wave (3201-3210). Unique
 * gj_u32_mul_wrap_u surface only; no multi-def. Distinct from
 * gj_u32_mul_wrap (batch2313). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3205_marker[] = "libcgj-batch3205";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular wrap mul mod 2^32 (C unsigned arithmetic). */
static uint32_t
b3205_mul(uint32_t u32A, uint32_t u32B)
{
return u32A * u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mul_wrap_u - modular wrap multiplication of two uint32_t values.
 *
 * a, b: factors
 *
 * Returns (a * b) mod 2^32. C unsigned arithmetic already wraps; this
 * surface makes the wrap intent explicit for graph callers.
 * No parent wires.
 */
uint32_t
gj_u32_mul_wrap_u(uint32_t u32A, uint32_t u32B)
{
(void)NULL;
return b3205_mul(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mul_wrap_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_mul_wrap_u")));
