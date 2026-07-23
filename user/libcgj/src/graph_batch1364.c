/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1364: reservoir capacity from error (ppm).
 *
 * Surface (unique symbols):
 *   uint64_t gj_reservoir_cap_for_error(uint32_t err_ppm);
 *     — Suggested reservoir sample size for additive relative-style
 *       error ε: n ≈ (1/ε)^2. err_ppm is ε in parts-per-million
 *       (1..1000000). Returns 0 on bad args. Capped at 2^32.
 *   uint64_t __gj_reservoir_cap_for_error  (alias)
 *   __libcgj_batch1364_marker = "libcgj-batch1364"
 *
 * Distinct from gj_reservoir_u32 (batch304) and gj_reservoir_offer
 * (batch719). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1364_marker[] = "libcgj-batch1364";

/* Soft upper bound to keep sizing practical. */
#define B1364_CAP_MAX 0x100000000ull /* 2^32 */

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reservoir_cap_for_error — reservoir size from error ppm.
 *
 * u32ErrPpm: desired error ε as parts-per-million (1 = 1e-6).
 *            0 or > 1000000 → 0.
 *
 * Rule of thumb: n ≈ (1/ε)^2 = (1e6 / err_ppm)^2.
 * Always at least 1 when args are valid.
 */
uint64_t
gj_reservoir_cap_for_error(uint32_t u32ErrPpm)
{
	uint64_t u64T;
	uint64_t u64N;

	if (u32ErrPpm == 0u || u32ErrPpm > 1000000u) {
		return 0ull;
	}

	/* t = ceil(1e6 / err_ppm) */
	u64T = (1000000ull + (uint64_t)u32ErrPpm - 1ull) /
	    (uint64_t)u32ErrPpm;
	if (u64T == 0ull) {
		u64T = 1ull;
	}

	/* Cap before squaring to avoid overflow / absurd sizes. */
	if (u64T > 65536ull) {
		/* (1e6/err)^2 would exceed 2^32 for t > 65536 */
		return B1364_CAP_MAX;
	}

	u64N = u64T * u64T;
	if (u64N > B1364_CAP_MAX) {
		return B1364_CAP_MAX;
	}
	if (u64N < 1ull) {
		return 1ull;
	}
	return u64N;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_reservoir_cap_for_error(uint32_t u32ErrPpm)
    __attribute__((alias("gj_reservoir_cap_for_error")));
