/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3561: uint64_t safe arithmetic right shift (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sar_safe_u(uint64_t x, unsigned n);
 *     - Arithmetic right shift of x as a two's-complement signed
 *       64-bit value, result re-interpreted as uint64_t. For n in
 *       0..63, shift in the sign bit. If n >= 64, return all zeros
 *       (non-negative view) or all ones (negative view). Fully
 *       defined for every n (no shift UB; saturating count).
 *   uint64_t __gj_u64_sar_safe_u  (alias)
 *   __libcgj_batch3561_marker = "libcgj-batch3561"
 *
 * Exclusive continuum CREATE-ONLY (3561-3570): u64_sar_safe_u,
 * u32_bit_count_range_u, u64_bit_count_range_u, u32_find_first_set_u,
 * u64_find_first_set_u, u32_find_last_set_u, u64_find_last_set_u,
 * u32_mask_from_to_u, u64_mask_from_to_u, batch_id_3570. Distinct
 * from gj_u64_sar_u (batch3226 mod-64) and gj_u64_ashr_sat
 * (batch2409) — unique sar_safe_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3561_marker[] = "libcgj-batch3561";

/* Sign bit of a 64-bit two's-complement view. */
#define B3561_SIGN  ((uint64_t)1 << 63)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Safe (saturating) arithmetic right shift of u64X as a signed view.
 * n >= 64 saturates to 0 or all-ones by sign. Unsigned ops only.
 */
static uint64_t
b3561_sar_safe(uint64_t u64X, unsigned uN)
{
	if (uN >= 64u) {
		if ((u64X & B3561_SIGN) != 0u) {
			return ~(uint64_t)0;
		}
		return 0u;
	}
	if (uN == 0u) {
		return u64X;
	}
	if ((u64X & B3561_SIGN) == 0u) {
		return u64X >> uN;
	}
	/* Negative view: logical shift then fill high bits with ones. */
	return (u64X >> uN) | (~(uint64_t)0 << (64u - uN));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sar_safe_u - saturating arithmetic right shift (signed view).
 *
 * x: bit pattern to shift (MSB is the sign bit)
 * n: shift count; n >= 64 -> 0 if sign clear, all ones if sign set
 *
 * Returns the arithmetic-right-shifted bit pattern as uint64_t.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_sar_safe_u(uint64_t u64X, unsigned uN)
{
	(void)NULL;
	return b3561_sar_safe(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sar_safe_u(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_sar_safe_u")));
