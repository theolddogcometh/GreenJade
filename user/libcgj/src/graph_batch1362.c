/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1362: Count-Min Sketch depth estimate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cms_estimate_d(uint32_t delta_ppm);
 *     — CMS table depth d ≈ ceil(ln(1/δ)) for failure probability δ.
 *       delta_ppm is δ in parts-per-million (1..1000000). Returns 0 on
 *       bad args. Clamped to [1, 64]. Integer path; no floating point.
 *   uint32_t __gj_cms_estimate_d  (alias)
 *   __libcgj_batch1362_marker = "libcgj-batch1362"
 *
 * Distinct from gj_cms_add/est (batch495), gj_cms_clear (batch894), and
 * gj_cms_estimate_w (batch1361). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1362_marker[] = "libcgj-batch1362";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(log2(x)) for x > 0; 0 if x == 0. Binary peel. */
static unsigned
b1362_flog2(uint64_t u64X)
{
	unsigned uN;

	if (u64X == 0ull) {
		return 0u;
	}
	uN = 0u;
	if (u64X >= (1ull << 32)) {
		uN += 32u;
		u64X >>= 32;
	}
	if (u64X >= (1ull << 16)) {
		uN += 16u;
		u64X >>= 16;
	}
	if (u64X >= (1ull << 8)) {
		uN += 8u;
		u64X >>= 8;
	}
	if (u64X >= (1ull << 4)) {
		uN += 4u;
		u64X >>= 4;
	}
	if (u64X >= (1ull << 2)) {
		uN += 2u;
		u64X >>= 2;
	}
	if (u64X >= (1ull << 1)) {
		uN += 1u;
	}
	return uN;
}

/*
 * Approximate ceil(ln(num/den)) for num > den > 0.
 * ln(x) = log2(x) * ln(2); ln2 ≈ 0.693147 ≈ 45426/65536 (Q16).
 * log2 uses integer part + linear fractional bit in Q16.
 */
static uint32_t
b1362_ceil_ln_ratio(uint64_t u64Num, uint64_t u64Den)
{
	uint64_t u64R;
	uint64_t u64Pow;
	uint64_t u64FracQ16;
	uint64_t u64Log2Q16;
	uint64_t u64LnQ16;
	uint64_t u64D;
	unsigned uLg;

	if (u64Num == 0ull || u64Den == 0ull || u64Num <= u64Den) {
		return 1u;
	}

	u64R = u64Num / u64Den;
	if (u64R <= 1ull) {
		return 1u;
	}

	uLg = b1362_flog2(u64R);
	u64Pow = 1ull << uLg;
	/* frac ≈ (R - 2^lg) / 2^lg in Q16 */
	u64FracQ16 = ((u64R - u64Pow) << 16) / u64Pow;
	u64Log2Q16 = ((uint64_t)uLg << 16) + u64FracQ16;

	/* ln_q16 = log2_q16 * ln2_q16 / 2^16 */
	u64LnQ16 = (u64Log2Q16 * 45426ull) >> 16;
	/* ceil to integer depth */
	u64D = (u64LnQ16 + 65535ull) >> 16;
	if (u64D < 1ull) {
		return 1u;
	}
	if (u64D > 64ull) {
		return 64u;
	}
	return (uint32_t)u64D;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cms_estimate_d — Count-Min Sketch depth from delta (ppm).
 *
 * u32DeltaPpm: failure probability δ as parts-per-million (1 = 1e-6).
 *              0 or > 1000000 → 0.
 *
 * Classic CMS: d = ceil(ln(1/δ)) = ceil(ln(1e6 / delta_ppm)).
 */
uint32_t
gj_cms_estimate_d(uint32_t u32DeltaPpm)
{
	if (u32DeltaPpm == 0u || u32DeltaPpm > 1000000u) {
		return 0u;
	}
	return b1362_ceil_ln_ratio(1000000ull, (uint64_t)u32DeltaPpm);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cms_estimate_d(uint32_t u32DeltaPpm)
    __attribute__((alias("gj_cms_estimate_d")));
