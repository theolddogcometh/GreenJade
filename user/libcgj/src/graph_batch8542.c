/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8542: IEEE-754 binary32 exponent field extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_exp_u_8542(uint32_t bits);
 *     - Return the 8-bit biased exponent field (bits 30..23) of an
 *       IEEE-754 binary32 encoding, as an integer in [0, 255]. Pure
 *       integer; no float ops.
 *   uint32_t __gj_fp_exp_u_8542  (alias)
 *   __libcgj_batch8542_marker = "libcgj-batch8542"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32). Unique gj_fp_exp_u_8542
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8542_marker[] = "libcgj-batch8542";

/* binary32: exp bits 30..23. */
#define B8542_F32_EXP_MASK  0x7f800000u
#define B8542_F32_EXP_SHIFT 23u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8542_exp(uint32_t uBits)
{
	return (uBits & B8542_F32_EXP_MASK) >> B8542_F32_EXP_SHIFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_exp_u_8542 - extract binary32 biased exponent field.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns (bits >> 23) & 0xff — 0 subnormal/zero, 1..254 normal,
 * 255 Inf/NaN.
 */
uint32_t
gj_fp_exp_u_8542(uint32_t bits)
{
	(void)NULL;
	return b8542_exp(bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_exp_u_8542(uint32_t bits)
    __attribute__((alias("gj_fp_exp_u_8542")));
