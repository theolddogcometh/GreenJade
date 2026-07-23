/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8545: IEEE-754 binary32 finite bit-pattern test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_isfinite_u_8545(uint32_t bits);
 *     - Return 1 if bits encodes a finite value (including ±0 and
 *       subnormals): exponent field is not all-ones. Else 0 (Inf/NaN).
 *       Pure integer; no float ops.
 *   uint32_t __gj_fp_isfinite_u_8545  (alias)
 *   __libcgj_batch8545_marker = "libcgj-batch8545"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32). Unique gj_fp_isfinite_u_8545
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8545_marker[] = "libcgj-batch8545";

/* binary32: exp all-ones means Inf or NaN. */
#define B8545_F32_EXP  0x7f800000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8545_isfinite(uint32_t uBits)
{
	return ((uBits & B8545_F32_EXP) != B8545_F32_EXP) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_isfinite_u_8545 - 1 if binary32 bit pattern is finite.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t
 * Returns 1 for normal / subnormal / ±0, else 0 (Inf or NaN).
 */
uint32_t
gj_fp_isfinite_u_8545(uint32_t bits)
{
	(void)NULL;
	return b8545_isfinite(bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_isfinite_u_8545(uint32_t bits)
    __attribute__((alias("gj_fp_isfinite_u_8545")));
