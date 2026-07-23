/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3666: HyperLogLog raw estimate (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hll_estimate_u(const uint8_t *regs, size_t m);
 *     - Integer raw HLL harmonic-mean cardinality from m power-of-two
 *       registers (m in [1, 256]). No linear-counting small-range
 *       correction. Empty sketch / bad args → 0.
 *   uint64_t __gj_hll_estimate_u  (alias)
 *   __libcgj_batch3666_marker = "libcgj-batch3666"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670). Distinct from
 * gj_hll_card (batch494) and gj_hll_estimate_raw (batch893) — unique
 * _u estimate surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3666_marker[] = "libcgj-batch3666";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3666_m_ok(size_t m)
{
	if (m == 0u || m > 256u) {
		return 0;
	}
	return (m & (m - 1u)) == 0u;
}

/* Alpha bias in Q16.16 for small fixed m; asymptotic form for m >= 128. */
static uint32_t
b3666_alpha_q16(size_t m)
{
	if (m <= 16u) {
		return 44100u; /* ≈ 0.673 */
	}
	if (m == 32u) {
		return 45678u; /* ≈ 0.697 */
	}
	if (m == 64u) {
		return 46465u; /* ≈ 0.709 */
	}
	{
		uint64_t num;
		uint64_t den;

		/* alpha ≈ 0.7213 / (1 + 1.079/m); 47255 ≈ 0.7213*65536 */
		num = 47255ull * (uint64_t)m * 1000ull;
		den = 1000ull * (uint64_t)m + 1079ull;
		return (uint32_t)(num / den);
	}
}

/*
 * Raw HLL: E = alpha * m^2 / Σ 2^{-R[j]}
 * Σ of 2^{-R} in Q32 units; alpha in Q16 → (alpha_q16 * m * m << 16) / sum.
 */
static uint64_t
b3666_est(const uint8_t *pRegs, size_t cM)
{
	size_t i;
	size_t zeros;
	uint64_t sum;
	uint64_t num;
	uint32_t alpha_q16;

	sum = 0ull;
	zeros = 0u;
	for (i = 0u; i < cM; i++) {
		unsigned r = (unsigned)pRegs[i];

		if (r == 0u) {
			zeros++;
		}
		if (r >= 32u) {
			sum += 1ull;
		} else {
			sum += 1ull << (32u - r);
		}
	}

	if (zeros == cM || sum == 0ull) {
		return 0ull;
	}

	alpha_q16 = b3666_alpha_q16(cM);
	num = (uint64_t)alpha_q16 * (uint64_t)cM * (uint64_t)cM;
	return (num << 16) / sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hll_estimate_u - raw HLL harmonic-mean cardinality (integer).
 *
 * regs: register array of m bytes (may be NULL → 0)
 * m:    power of two in [1, 256]; invalid → 0
 *
 * No linear-counting correction. Does not call libc. No parent wires.
 */
uint64_t
gj_hll_estimate_u(const uint8_t *pRegs, size_t cM)
{
	(void)NULL;
	if (pRegs == NULL || !b3666_m_ok(cM)) {
		return 0ull;
	}
	return b3666_est(pRegs, cM);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hll_estimate_u(const uint8_t *pRegs, size_t cM)
    __attribute__((alias("gj_hll_estimate_u")));
