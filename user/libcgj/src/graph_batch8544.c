/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8544: IEEE-754 binary32 zero bit-pattern test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_iszero_u_8544(uint32_t bits);
 *     - Return 1 if bits encodes ±0 (all non-sign bits clear), else 0.
 *       Pure integer; no float ops.
 *   uint32_t __gj_fp_iszero_u_8544  (alias)
 *   __libcgj_batch8544_marker = "libcgj-batch8544"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32). Distinct from
 * gj_f32_bits_is_zero (batch1833) — unique gj_fp_iszero_u_8544 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8544_marker[] = "libcgj-batch8544";

/* Clear sign: remaining bits zero iff ±0. */
#define B8544_F32_ABS  0x7fffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8544_iszero(uint32_t uBits)
{
	return ((uBits & B8544_F32_ABS) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_iszero_u_8544 - 1 if binary32 bit pattern is ±0.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns 1 for +0 / -0, else 0.
 */
uint32_t
gj_fp_iszero_u_8544(uint32_t bits)
{
	(void)NULL;
	return b8544_iszero(bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_iszero_u_8544(uint32_t bits)
    __attribute__((alias("gj_fp_iszero_u_8544")));
