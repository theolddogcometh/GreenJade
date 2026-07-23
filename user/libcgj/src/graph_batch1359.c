/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1359: HyperLogLog register-count estimate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hll_estimate_m(uint32_t err_ppm);
 *     — Choose power-of-two register count m for a target relative
 *       standard error. HLL rule of thumb: σ ≈ 1.04 / sqrt(m), so
 *       m ≈ (1.04 / err)^2. err_ppm is the desired σ in parts per
 *       million (e.g. 16250 ≈ 1.625% → m≈4096). Returns m in
 *       [16, 65536] (next power of two), or 0 on bad args.
 *   uint32_t __gj_hll_estimate_m  (alias)
 *   __libcgj_batch1359_marker = "libcgj-batch1359"
 *
 * Distinct from gj_hll_init/add/card (batch494) and gj_hll_estimate_raw
 * (batch893). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1359_marker[] = "libcgj-batch1359";

/* ---- freestanding helpers ---------------------------------------------- */

/* Next power of two ≥ x, for x in 1..65536. 0 → 0. */
static uint32_t
b1359_next_pow2_u32(uint32_t u32X)
{
	if (u32X == 0u) {
		return 0u;
	}
	if (u32X >= 65536u) {
		return 65536u;
	}
	u32X--;
	u32X |= u32X >> 1;
	u32X |= u32X >> 2;
	u32X |= u32X >> 4;
	u32X |= u32X >> 8;
	u32X |= u32X >> 16;
	u32X++;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hll_estimate_m — HLL m (= 2^p) for a target relative std-error.
 *
 * u32ErrPpm: desired σ in ppm (1..1000000). 0 → 0.
 *
 * m ≈ (1.04 / err)^2 = (1.04e6 / err_ppm)^2
 * Integer path: t = (1040000 + err/2) / err; m0 = t*t; m = next_pow2(m0).
 */
uint32_t
gj_hll_estimate_m(uint32_t u32ErrPpm)
{
	uint64_t u64T;
	uint64_t u64M0;
	uint32_t u32M;

	if (u32ErrPpm == 0u || u32ErrPpm > 1000000u) {
		return 0u;
	}

	/* t = round(1.04e6 / err_ppm) */
	u64T = (1040000ull + ((uint64_t)u32ErrPpm / 2ull)) /
	    (uint64_t)u32ErrPpm;
	if (u64T == 0ull) {
		u64T = 1ull;
	}
	if (u64T > 65536ull) {
		/* err very small → cap before squaring */
		return 65536u;
	}
	u64M0 = u64T * u64T;
	if (u64M0 > 65536ull) {
		return 65536u;
	}
	u32M = b1359_next_pow2_u32((uint32_t)u64M0);
	if (u32M < 16u) {
		u32M = 16u;
	}
	return u32M;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hll_estimate_m(uint32_t u32ErrPpm)
    __attribute__((alias("gj_hll_estimate_m")));
