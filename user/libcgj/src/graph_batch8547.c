/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8547: IEEE-754 binary32 NaN bit-pattern test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_isnan_u_8547(uint32_t bits);
 *     - Return 1 if bits encodes a NaN (exp all-ones and nonzero
 *       payload), else 0. Sign bit ignored. Pure integer; no float ops.
 *   uint32_t __gj_fp_isnan_u_8547  (alias)
 *   __libcgj_batch8547_marker = "libcgj-batch8547"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32). Distinct from
 * gj_f32_bits_is_nan (batch1831) — unique gj_fp_isnan_u_8547 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8547_marker[] = "libcgj-batch8547";

/* binary32: exp bits 30..23, frac bits 22..0; abs mask clears sign. */
#define B8547_F32_ABS  0x7fffffffu
#define B8547_F32_EXP  0x7f800000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8547_isnan(uint32_t uBits)
{
	uint32_t uAbs;

	uAbs = uBits & B8547_F32_ABS;
	return (uAbs > B8547_F32_EXP) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_isnan_u_8547 - 1 if binary32 bit pattern is a NaN.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns 1 for any quiet/signaling NaN, else 0 (incl. Inf/finite/±0).
 */
uint32_t
gj_fp_isnan_u_8547(uint32_t bits)
{
	(void)NULL;
	return b8547_isnan(bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_isnan_u_8547(uint32_t bits)
    __attribute__((alias("gj_fp_isnan_u_8547")));
