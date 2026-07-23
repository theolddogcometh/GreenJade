/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8543: IEEE-754 binary32 mantissa field extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_mant_u_8543(uint32_t bits);
 *     - Return the 23-bit fraction / mantissa field (bits 22..0) of an
 *       IEEE-754 binary32 encoding. Pure integer; no float ops. Does
 *       not re-insert the implicit leading 1.
 *   uint32_t __gj_fp_mant_u_8543  (alias)
 *   __libcgj_batch8543_marker = "libcgj-batch8543"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32). Unique gj_fp_mant_u_8543
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8543_marker[] = "libcgj-batch8543";

/* binary32: fraction bits 22..0. */
#define B8543_F32_MANT_MASK  0x007fffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8543_mant(uint32_t uBits)
{
	return uBits & B8543_F32_MANT_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_mant_u_8543 - extract binary32 fraction/mantissa field.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns bits & 0x007fffff (raw trailing significand only).
 */
uint32_t
gj_fp_mant_u_8543(uint32_t bits)
{
	(void)NULL;
	return b8543_mant(bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_mant_u_8543(uint32_t bits)
    __attribute__((alias("gj_fp_mant_u_8543")));
