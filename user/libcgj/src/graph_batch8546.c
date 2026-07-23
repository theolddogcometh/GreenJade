/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8546: IEEE-754 binary32 infinity bit-pattern test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_isinf_u_8546(uint32_t bits);
 *     - Return 1 if bits encodes ±Inf (exp all-ones, zero fraction),
 *       else 0. Pure integer; no float ops.
 *   uint32_t __gj_fp_isinf_u_8546  (alias)
 *   __libcgj_batch8546_marker = "libcgj-batch8546"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32). Distinct from
 * gj_f32_bits_is_inf (batch1832) — unique gj_fp_isinf_u_8546 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8546_marker[] = "libcgj-batch8546";

/* binary32 abs / exp masks. */
#define B8546_F32_ABS  0x7fffffffu
#define B8546_F32_EXP  0x7f800000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8546_isinf(uint32_t uBits)
{
	uint32_t uAbs;

	uAbs = uBits & B8546_F32_ABS;
	return (uAbs == B8546_F32_EXP) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_isinf_u_8546 - 1 if binary32 bit pattern is ±infinity.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns 1 for +Inf / -Inf, else 0 (NaN/finite/±0).
 */
uint32_t
gj_fp_isinf_u_8546(uint32_t bits)
{
	(void)NULL;
	return b8546_isinf(bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_isinf_u_8546(uint32_t bits)
    __attribute__((alias("gj_fp_isinf_u_8546")));
