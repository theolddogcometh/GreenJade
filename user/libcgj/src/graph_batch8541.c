/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8541: IEEE-754 binary32 sign bit (uint32 bits).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_signbit_u_8541(uint32_t bits);
 *     - Return 1 if the IEEE-754 binary32 sign bit (bit 31) is set,
 *       else 0. Pure integer bit check; no float ops.
 *   uint32_t __gj_fp_signbit_u_8541  (alias)
 *   __libcgj_batch8541_marker = "libcgj-batch8541"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32 — signbit, exp, mant, iszero,
 * isfinite, isinf, isnan, isnormal, classify_id, batch_id). Distinct
 * from gj_f32_bits_sign (batch1834) / gj_u32_signbit_u (batch4003) —
 * unique gj_fp_signbit_u_8541 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8541_marker[] = "libcgj-batch8541";

/* binary32 sign is bit 31. */
#define B8541_F32_SIGN  0x80000000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8541_signbit(uint32_t uBits)
{
	return ((uBits & B8541_F32_SIGN) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_signbit_u_8541 - 1 if binary32 bit pattern has sign bit set.
 *
 * bits: IEEE-754 binary32 encoding as uint32_t (no float load/store)
 * Returns 1 when bit 31 is set (incl. -0, -Inf, negative NaN), else 0.
 */
uint32_t
gj_fp_signbit_u_8541(uint32_t bits)
{
	(void)NULL;
	return b8541_signbit(bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_signbit_u_8541(uint32_t bits)
    __attribute__((alias("gj_fp_signbit_u_8541")));
