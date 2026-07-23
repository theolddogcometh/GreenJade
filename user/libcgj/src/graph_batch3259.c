/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3259: uint32 scale by unsigned Q16.16 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_scale_q16_u(uint32_t x, uint32_t s_q16);
 *     - Return floor((x * s_q16) / 65536) via a uint64 intermediate.
 *       s_q16 is an unsigned Q16.16 scale factor (1.0 == 0x00010000).
 *       High bits above bit 31 of the shifted product are discarded
 *       (modular wrap to uint32).
 *   uint32_t __gj_u32_scale_q16_u  (alias)
 *   __libcgj_batch3259_marker = "libcgj-batch3259"
 *
 * CREATE-ONLY exclusive continuum wave (3251-3260). Unique
 * gj_u32_scale_q16_u surface only; no multi-def. Distinct from
 * gj_u32_scale (batch1208 array form) and gj_q16_mul_u (batch3059).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3259_marker[] = "libcgj-batch3259";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Scale integer x by unsigned Q16.16 factor: (x * s) >> 16.
 * Product in uint64_t; result is low 32 bits of the shifted product.
 */
static uint32_t
b3259_scale_q16(uint32_t u32X, uint32_t u32S)
{
	uint64_t u64Prod;

	u64Prod = (uint64_t)u32X * (uint64_t)u32S;
	return (uint32_t)(u64Prod >> 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_scale_q16_u - scale a uint32_t by an unsigned Q16.16 factor.
 *
 * x:     integer value to scale
 * s_q16: unsigned Q16.16 scale (1.0 == 0x00010000)
 *
 * Product is computed in uint64_t then shifted right by 16. Self-
 * contained; no parent wires to q16 / scale helpers.
 */
uint32_t
gj_u32_scale_q16_u(uint32_t u32X, uint32_t u32S)
{
	(void)NULL;
	return b3259_scale_q16(u32X, u32S);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_scale_q16_u(uint32_t u32X, uint32_t u32S)
    __attribute__((alias("gj_u32_scale_q16_u")));
