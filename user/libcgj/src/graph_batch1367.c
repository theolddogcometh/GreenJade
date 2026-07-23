/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1367: EWMA alpha from half-life (milli).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ewma_alpha_from_halflife(uint32_t half_life);
 *     — Smoothing factor α in milli-units (0..1000) such that the
 *       EWMA weight halves after half_life samples:
 *         (1 - α)^half_life ≈ 1/2
 *       Integer approximation α ≈ 1 - exp(-ln2 / half_life) via a
 *       short series in Q16. half_life == 0 → 0.
 *   uint32_t __gj_ewma_alpha_from_halflife  (alias)
 *   __libcgj_batch1367_marker = "libcgj-batch1367"
 *
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1367_marker[] = "libcgj-batch1367";

/* ln(2) in Q16: 0.693147 * 65536 ≈ 45426 */
#define B1367_LN2_Q16 45426ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * exp(-x) in Q16 for small x (x in Q16, typically x <= ln2).
 * Series: 1 - x + x^2/2 - x^3/6 + x^4/24  (Q16 arithmetic).
 */
static uint64_t
b1367_exp_neg_q16(uint64_t u64XQ16)
{
	uint64_t u64X2;
	uint64_t u64X3;
	uint64_t u64X4;
	uint64_t u64R;

	if (u64XQ16 >= 65536ull) {
		/* x >= 1 → exp(-x) < 1/e; clamp conservatively */
		return 24109ull; /* ≈ 0.367879 * 65536 */
	}

	u64X2 = (u64XQ16 * u64XQ16) >> 16;
	u64X3 = (u64X2 * u64XQ16) >> 16;
	u64X4 = (u64X3 * u64XQ16) >> 16;

	/* 1 - x + x2/2 - x3/6 + x4/24 */
	u64R = 65536ull;
	if (u64R > u64XQ16) {
		u64R -= u64XQ16;
	} else {
		return 0ull;
	}
	u64R += u64X2 >> 1;
	u64R -= u64X3 / 6ull;
	u64R += u64X4 / 24ull;
	if (u64R > 65536ull) {
		u64R = 65536ull;
	}
	return u64R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ewma_alpha_from_halflife — α (milli) from half-life in samples.
 *
 * u32HalfLife: samples until weight halves. 0 → 0.
 *
 * α = 1 - exp(-ln2 / hl)
 * x_q16 = ln2_q16 / hl
 * alpha_milli = round((65536 - exp(-x)_q16) * 1000 / 65536)
 * Clamped to [1, 1000] when hl >= 1.
 */
uint32_t
gj_ewma_alpha_from_halflife(uint32_t u32HalfLife)
{
	uint64_t u64XQ16;
	uint64_t u64Exp;
	uint64_t u64AlphaQ16;
	uint64_t u64Milli;

	if (u32HalfLife == 0u) {
		return 0u;
	}

	/* x = ln2 / hl in Q16 */
	u64XQ16 = (B1367_LN2_Q16 + ((uint64_t)u32HalfLife / 2ull)) /
	    (uint64_t)u32HalfLife;
	if (u64XQ16 == 0ull) {
		/* Very large half-life → tiny alpha */
		return 1u;
	}

	u64Exp = b1367_exp_neg_q16(u64XQ16);
	if (u64Exp >= 65536ull) {
		return 1u;
	}
	u64AlphaQ16 = 65536ull - u64Exp;

	/* milli = alpha_q16 * 1000 / 65536, rounded */
	u64Milli = (u64AlphaQ16 * 1000ull + 32768ull) >> 16;
	if (u64Milli < 1ull) {
		return 1u;
	}
	if (u64Milli > 1000ull) {
		return 1000u;
	}
	return (uint32_t)u64Milli;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ewma_alpha_from_halflife(uint32_t u32HalfLife)
    __attribute__((alias("gj_ewma_alpha_from_halflife")));
