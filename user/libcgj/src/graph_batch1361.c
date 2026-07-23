/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1361: Count-Min Sketch width estimate.
 *
 * Surface (unique symbols):
 *   uint64_t gj_cms_estimate_w(uint32_t eps_ppm);
 *     — CMS table width w ≈ ceil(e / ε) for additive error bound ε.
 *       eps_ppm is ε in parts-per-million (1..1000000). Returns 0 on
 *       bad args. Integer path; no floating point.
 *   uint64_t __gj_cms_estimate_w  (alias)
 *   __libcgj_batch1361_marker = "libcgj-batch1361"
 *
 * Distinct from gj_cms_add/est (batch495) and gj_cms_clear (batch894).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1361_marker[] = "libcgj-batch1361";

/* e * 1e6 rounded: 2.718281828... * 1e6 ≈ 2718282 */
#define B1361_E_PPM 2718282ull

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cms_estimate_w — Count-Min Sketch width from epsilon (ppm).
 *
 * u32EpsPpm: additive error ε as parts-per-million (1 = 1e-6).
 *            0 or > 1000000 → 0.
 *
 * Classic CMS: w = ceil(e / ε) = ceil(e * 1e6 / eps_ppm).
 * Integer ceil: (B1361_E_PPM + eps_ppm - 1) / eps_ppm.
 */
uint64_t
gj_cms_estimate_w(uint32_t u32EpsPpm)
{
	uint64_t u64W;

	if (u32EpsPpm == 0u || u32EpsPpm > 1000000u) {
		return 0ull;
	}

	u64W = (B1361_E_PPM + (uint64_t)u32EpsPpm - 1ull) /
	    (uint64_t)u32EpsPpm;
	if (u64W < 1ull) {
		u64W = 1ull;
	}
	return u64W;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_cms_estimate_w(uint32_t u32EpsPpm)
    __attribute__((alias("gj_cms_estimate_w")));
