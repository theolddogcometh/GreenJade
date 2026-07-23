/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4151: unsigned Q16.16 multiply (_q16x).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16x_mul_u(uint32_t a, uint32_t b);
 *     - Q16.16 product: ((uint64_t)a * (uint64_t)b) >> 16, returned as
 *       uint32_t (low 32 bits of the shifted product).
 *   uint32_t __gj_q16x_mul_u  (alias)
 *   __libcgj_batch4151_marker = "libcgj-batch4151"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160: q16x_mul_u, q16x_div_u,
 * q16x_add_sat_u, q16x_sub_sat_u, q16x_from_u32_u, q16x_to_u32_floor_u,
 * q16x_frac_u, q16x_lerp_u, q16x_clamp01_u, batch_id_4160).
 * Distinct from gj_q16_mul (batch172) and gj_mul_u32 (batch426) —
 * unique gj_q16x_mul_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4151_marker[] = "libcgj-batch4151";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4151_mul — Q16.16 multiply via 64-bit product then >> 16.
 */
static uint32_t
b4151_mul(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64Prod;

	u64Prod = (uint64_t)u32A * (uint64_t)u32B;
	return (uint32_t)(u64Prod >> 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16x_mul_u - unsigned Q16.16 multiply.
 *
 * a, b: Q16.16 factors (1.0 == 0x10000)
 *
 * Returns ((uint64_t)a * b) >> 16 as uint32_t. No parent wires.
 */
uint32_t
gj_q16x_mul_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4151_mul(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16x_mul_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_q16x_mul_u")));
