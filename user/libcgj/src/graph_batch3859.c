/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3859: unsigned 32-bit multiply-subtract (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mul_sub_u(uint32_t a, uint32_t b, uint32_t c);
 *     - Return (a * b) - c with modular wrap (mod 2^32).
 *       Plain unsigned wrap; not saturating.
 *   uint32_t __gj_u32_mul_sub_u  (alias)
 *   __libcgj_batch3859_marker = "libcgj-batch3859"
 *
 * Exclusive continuum CREATE-ONLY (3851-3860). Distinct from
 * gj_u32_mul_add_u (batch3857) and gj_u32_mul_wrap (batch2313) —
 * unique gj_u32_mul_sub_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3859_marker[] = "libcgj-batch3859";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Modular wrap fused mul-sub mod 2^32.
 * C unsigned arithmetic is defined to wrap; no wide type needed.
 */
static uint32_t
b3859_mul_sub(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	return (u32A * u32B) - u32C;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mul_sub_u - modular wrap multiply-subtract of three uint32_t.
 *
 * a, b: factors
 * c: subtrahend
 *
 * Returns ((a * b) - c) mod 2^32. No parent wires.
 */
uint32_t
gj_u32_mul_sub_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	(void)NULL;
	return b3859_mul_sub(u32A, u32B, u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mul_sub_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
    __attribute__((alias("gj_u32_mul_sub_u")));
