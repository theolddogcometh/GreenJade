/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8548: IEEE-754 binary32 normal bit-pattern test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_isnormal_u_8548(uint32_t bits);
 *     - Return 1 if bits encodes a normal number: biased exponent in
 *       [1, 254]. Else 0 (zero/subnormal/Inf/NaN). Pure integer; no
 *       float ops.
 *   uint32_t __gj_fp_isnormal_u_8548  (alias)
 *   __libcgj_batch8548_marker = "libcgj-batch8548"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32). Unique gj_fp_isnormal_u_8548
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8548_marker[] = "libcgj-batch8548";

/* binary32: exp bits 30..23. Normal when exp in 1..254. */
#define B8548_F32_EXP_MASK  0x7f800000u
#define B8548_F32_EXP_SHIFT 23u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8548_isnormal(uint32_t uBits)
{
	uint32_t uExp;

	uExp = (uBits & B8548_F32_EXP_MASK) >> B8548_F32_EXP_SHIFT;
	return ((uExp != 0u) && (uExp != 255u)) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_isnormal_u_8548 - 1 if binary32 bit pattern is a normal number.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns 1 when biased exponent is in [1, 254], else 0.
 */
uint32_t
gj_fp_isnormal_u_8548(uint32_t bits)
{
	(void)NULL;
	return b8548_isnormal(bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_isnormal_u_8548(uint32_t bits)
    __attribute__((alias("gj_fp_isnormal_u_8548")));
