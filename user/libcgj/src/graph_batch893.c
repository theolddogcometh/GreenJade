/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch893: raw HyperLogLog cardinality estimate.
 *
 * Distinct from gj_hll_card (batch494) — no multi-def. This is the raw
 * harmonic-mean estimate only (no linear-counting small-range correction).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hll_estimate_raw(const uint8_t *regs, size_t m);
 *     — Integer raw HLL estimate from m power-of-two registers (m <= 256).
 *       Empty / bad args → 0.
 *   uint64_t __gj_hll_estimate_raw  (alias)
 *   __libcgj_batch893_marker = "libcgj-batch893"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch893_marker[] = "libcgj-batch893";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b893_m_ok(size_t m)
{
	if (m == 0u || m > 256u) {
		return 0;
	}
	return (m & (m - 1u)) == 0u;
}

/* Alpha bias in Q16.16 for small fixed m; asymptotic form for m >= 128. */
static uint32_t
b893_alpha_q16(size_t m)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hll_estimate_raw — raw HLL harmonic-mean cardinality (integer).
 *
 * E = alpha * m^2 / Σ 2^{-R[j]} with Σ in Q32 and alpha in Q16:
 *   est = (alpha_q16 * m * m << 16) / sum
 *
 * No linear-counting correction (contrast gj_hll_card). Returns 0 on
 * NULL regs, invalid m, or empty sketch (all registers zero).
 */
uint64_t
gj_hll_estimate_raw(const uint8_t *regs, size_t m)
{
	size_t i;
	size_t zeros;
	uint64_t sum;
	uint64_t num;
	uint32_t alpha_q16;

	if (regs == NULL || !b893_m_ok(m)) {
		return 0ull;
	}

	sum = 0ull;
	zeros = 0u;
	for (i = 0u; i < m; i++) {
		unsigned r = (unsigned)regs[i];

		if (r == 0u) {
			zeros++;
		}
		if (r >= 32u) {
			sum += 1ull;
		} else {
			sum += 1ull << (32u - r);
		}
	}

	if (zeros == m || sum == 0ull) {
		return 0ull;
	}

	alpha_q16 = b893_alpha_q16(m);
	num = (uint64_t)alpha_q16 * (uint64_t)m * (uint64_t)m;
	return (num << 16) / sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hll_estimate_raw(const uint8_t *regs, size_t m)
    __attribute__((alias("gj_hll_estimate_raw")));
