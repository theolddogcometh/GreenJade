/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3559: uint32_t UB-safe arithmetic right shift.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sar_safe_u(uint32_t x, unsigned n);
 *     - Arithmetic right shift of x treated as a two's-complement
 *       signed 32-bit value, result re-interpreted as uint32_t.
 *       For n in 0..31, shift in the sign bit. If n >= 32, return all
 *       zeros (non-negative view) or all ones (negative view). No
 *       shift UB; pure integer; no builtins; no signed-shift
 *       implementation-defined reliance.
 *   uint32_t __gj_u32_sar_safe_u  (alias)
 *   __libcgj_batch3559_marker = "libcgj-batch3559"
 *
 * Exclusive continuum CREATE-ONLY (3551-3560). Distinct from
 * gj_u64_ashr_sat (batch2409) — unique u32 sar safe surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3559_marker[] = "libcgj-batch3559";

/* Sign bit of a 32-bit two's-complement view. */
#define B3559_SIGN  ((uint32_t)1u << 31)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Arithmetic right shift of u32X as a signed view. n >= 32 saturates
 * to 0 or all-ones by sign. Implemented with unsigned ops only so the
 * result is fully defined (no signed >> reliance).
 */
static uint32_t
b3559_sar_safe(uint32_t u32X, unsigned uN)
{
	if (uN >= 32u) {
		if ((u32X & B3559_SIGN) != 0u) {
			return ~(uint32_t)0;
		}
		return 0u;
	}
	if (uN == 0u) {
		return u32X;
	}
	if ((u32X & B3559_SIGN) == 0u) {
		return u32X >> uN;
	}
	/* Negative view: logical shift then fill high bits with ones. */
	return (u32X >> uN) | (~(uint32_t)0 << (32u - uN));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sar_safe_u - UB-safe arithmetic right shift (signed view).
 *
 * x: bit pattern to shift (MSB is the sign bit)
 * n: shift count; n >= 32 -> 0 if sign clear, all ones if sign set
 *
 * Returns the arithmetic-right-shifted bit pattern as uint32_t.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_sar_safe_u(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b3559_sar_safe(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sar_safe_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_sar_safe_u")));
