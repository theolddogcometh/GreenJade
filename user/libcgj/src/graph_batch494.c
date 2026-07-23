/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch494: freestanding HyperLogLog sketch (m <= 256).
 *
 * Surface (unique symbols):
 *   void     gj_hll_init(uint8_t *regs, size_t m);
 *     — Zero m registers. m must be a power of two in [1, 256].
 *       No-op on NULL regs or invalid m.
 *   void     gj_hll_add(uint8_t *regs, size_t m, uint64_t hash);
 *     — Observe one 64-bit hash: update the addressed register with the
 *       max leading-one rank (rho) of the remaining bits.
 *   uint64_t gj_hll_card(const uint8_t *regs, size_t m);
 *     — Rough integer cardinality estimate from the register array.
 *       Empty sketch (all zeros) → 0. Sparse → linear counting; else
 *       fixed-point HLL harmonic mean (no floating point).
 *   void     __gj_hll_init  (alias)
 *   void     __gj_hll_add   (alias)
 *   uint64_t __gj_hll_card  (alias)
 *   __libcgj_batch494_marker = "libcgj-batch494"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float, no compiler clz builtins. No third-party source copied.
 * Self-contained (no cross-batch call to gj_clz64).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch494_marker[] = "libcgj-batch494";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b494_clz64 — count leading zeros in a 64-bit word (0 → 64).
 * Binary-search peel; no builtins (matches batch367 style, static here).
 */
static unsigned
b494_clz64(uint64_t x)
{
	unsigned n;
	uint64_t v;

	if (x == 0ull) {
		return 64u;
	}
	v = x;
	n = 0u;
	if ((v & 0xffffffff00000000ull) == 0ull) {
		n += 32u;
		v <<= 32;
	}
	if ((v & 0xffff000000000000ull) == 0ull) {
		n += 16u;
		v <<= 16;
	}
	if ((v & 0xff00000000000000ull) == 0ull) {
		n += 8u;
		v <<= 8;
	}
	if ((v & 0xf000000000000000ull) == 0ull) {
		n += 4u;
		v <<= 4;
	}
	if ((v & 0xc000000000000000ull) == 0ull) {
		n += 2u;
		v <<= 2;
	}
	if ((v & 0x8000000000000000ull) == 0ull) {
		n += 1u;
	}
	return n;
}

/* True iff m is a power of two in [1, 256]. */
static int
b494_m_ok(size_t m)
{
	if (m == 0u || m > 256u) {
		return 0;
	}
	return (m & (m - 1u)) == 0u;
}

/* p = log2(m) for power-of-two m in [1, 256]. */
static unsigned
b494_log2_m(size_t m)
{
	/* m is power of two; ctz via clz of (m) after placing in low bits. */
	return 63u - b494_clz64((uint64_t)m);
}

/*
 * Alpha bias constant in Q16.16 (alpha * 65536), per classic HLL table
 * for small m; asymptotic form for m >= 128.
 */
static uint32_t
b494_alpha_q16(size_t m)
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
	/*
	 * alpha ≈ 0.7213 / (1 + 1.079/m) = 0.7213 * m / (m + 1.079)
	 * Q16: (47255 * m * 1000) / (1000*m + 1079), 47255 ≈ 0.7213*65536.
	 */
	{
		uint64_t num;
		uint64_t den;

		num = 47255ull * (uint64_t)m * 1000ull;
		den = 1000ull * (uint64_t)m + 1079ull;
		return (uint32_t)(num / den);
	}
}

/*
 * log2(x) in Q16.16 for x > 0 (floor exponent + 16-bit fraction from
 * the mantissa after normalizing the leading 1 to bit 63).
 */
static uint32_t
b494_log2_q16(uint64_t x)
{
	unsigned clz;
	unsigned e;
	uint64_t n;
	uint32_t frac;

	if (x == 0ull) {
		return 0u;
	}
	clz = b494_clz64(x);
	e = 63u - clz;
	n = x << clz; /* leading 1 at bit 63 */
	/* 16 fraction bits immediately below the leading one (exclude it). */
	frac = (uint32_t)((n & 0x7fffffffffffffffull) >> 47);
	return ((uint32_t)e << 16) | frac;
}

/*
 * b494_linear_count — m * ln(m/V) via fixed-point log2 * ln(2).
 * ln(2) ≈ 45426/65536. V must be in (0, m].
 */
static uint64_t
b494_linear_count(size_t m, size_t v)
{
	uint32_t log_m;
	uint32_t log_v;
	int64_t dlog;
	unsigned p;

	p = b494_log2_m(m);
	log_m = (uint32_t)p << 16; /* m is an exact power of two */
	log_v = b494_log2_q16((uint64_t)v);
	dlog = (int64_t)log_m - (int64_t)log_v;
	if (dlog < 0) {
		dlog = 0;
	}
	/* m * dlog_q16 * ln2_q16 >> 32 */
	return ((uint64_t)m * (uint64_t)dlog * 45426ull) >> 32;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hll_init — zero m HyperLogLog registers.
 *
 * regs: register array (m bytes), may be NULL → no-op
 * m:    power of two in [1, 256]; invalid → no-op
 */
void
gj_hll_init(uint8_t *regs, size_t m)
{
	size_t i;

	if (regs == NULL || !b494_m_ok(m)) {
		return;
	}
	for (i = 0u; i < m; i++) {
		regs[i] = 0u;
	}
}

/*
 * gj_hll_add — observe one hash into the sketch.
 *
 * High p bits select the register; remaining bits yield rho = rank of
 * the first 1-bit (leading-zero count of the left-aligned remainder + 1).
 * Register stores the max rho seen. No-op on NULL regs or invalid m.
 */
void
gj_hll_add(uint8_t *regs, size_t m, uint64_t hash)
{
	unsigned p;
	unsigned idx;
	unsigned rho;
	uint64_t w;
	uint8_t cur;

	if (regs == NULL || !b494_m_ok(m)) {
		return;
	}

	p = b494_log2_m(m);
	/* High p bits → register index (p==0 → single register). */
	if (p == 0u) {
		idx = 0u;
		w = hash;
	} else {
		idx = (unsigned)(hash >> (64u - p));
		w = hash << p; /* remaining bits left-aligned; low p bits 0 */
	}

	/*
	 * rho: position of first 1 in the (64-p)-bit remainder, 1-based.
	 * All-zero remainder → (64-p)+1. Result fits in uint8_t (≤ 65).
	 */
	if (w == 0ull) {
		rho = (64u - p) + 1u;
	} else {
		rho = b494_clz64(w) + 1u;
	}

	cur = regs[idx];
	if ((uint8_t)rho > cur) {
		regs[idx] = (uint8_t)rho;
	}
}

/*
 * gj_hll_card — rough integer cardinality estimate.
 *
 * Uses linear counting when many registers are still zero (or the raw
 * estimate is below 2.5·m); otherwise the classic HLL harmonic mean
 * with fixed-point alpha and 2^{-R} terms scaled at 2^32.
 *
 * Returns 0 on NULL / invalid m / empty sketch.
 */
uint64_t
gj_hll_card(const uint8_t *regs, size_t m)
{
	size_t i;
	size_t v;
	uint64_t sum;
	uint64_t num;
	uint64_t est;
	uint32_t alpha_q16;

	if (regs == NULL || !b494_m_ok(m)) {
		return 0ull;
	}

	/* Harmonic sum of 2^{-R[j]} in Q32 units, and zero-register count. */
	sum = 0ull;
	v = 0u;
	for (i = 0u; i < m; i++) {
		unsigned r = (unsigned)regs[i];

		if (r == 0u) {
			v++;
		}
		if (r >= 32u) {
			/* 2^(32-r) < 1 → contribute 1 (tiny; rough floor). */
			sum += 1ull;
		} else {
			sum += 1ull << (32u - r);
		}
	}

	if (v == m) {
		return 0ull; /* empty sketch */
	}
	if (sum == 0ull) {
		return 0ull;
	}

	/*
	 * Raw HLL: E = alpha * m^2 / Σ 2^{-R[j]}
	 * With Σ in Q32: E = alpha * m^2 * 2^32 / sum
	 * alpha in Q16:  E = (alpha_q16 * m * m << 16) / sum
	 */
	alpha_q16 = b494_alpha_q16(m);
	num = (uint64_t)alpha_q16 * (uint64_t)m * (uint64_t)m;
	/* num max ≈ 0.72*65536 * 65536 ≈ 3.1e9; << 16 fits uint64. */
	est = (num << 16) / sum;

	/* Small-range correction: linear counting when E ≤ 2.5·m and V > 0. */
	if (v > 0u && est <= ((uint64_t)m * 5ull) / 2ull) {
		return b494_linear_count(m, v);
	}

	return est;
}

/* ---- underscored aliases ----------------------------------------------- */

void __gj_hll_init(uint8_t *regs, size_t m)
    __attribute__((alias("gj_hll_init")));
void __gj_hll_add(uint8_t *regs, size_t m, uint64_t hash)
    __attribute__((alias("gj_hll_add")));
uint64_t __gj_hll_card(const uint8_t *regs, size_t m)
    __attribute__((alias("gj_hll_card")));
