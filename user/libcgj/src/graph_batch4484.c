/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4484: uint32 percentage of whole
 * (part * 100 / whole).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_percent_u(uint32_t part, uint32_t whole);
 *     - Return floor((part * 100) / whole). whole==0 → 0.
 *   uint32_t __gj_u32_percent_u  (alias)
 *   __libcgj_batch4484_marker = "libcgj-batch4484"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Float-free
 * rational unique wave. Distinct from gj_u32_percent_of_u (batch3255)
 * and gj_u32_pct_u (batch3053) — unique gj_u32_percent_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4484_marker[] = "libcgj-batch4484";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor((part * 100) / whole). whole==0 → 0. */
static uint32_t
b4484_percent(uint32_t u32Part, uint32_t u32Whole)
{
	uint64_t u64Prod;

	if (u32Whole == 0u) {
		return 0u;
	}
	u64Prod = (uint64_t)u32Part * 100ull;
	return (uint32_t)(u64Prod / (uint64_t)u32Whole);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_percent_u - percentage of whole occupied by part.
 *
 * part:  portion count
 * whole: total count (whole==0 yields 0)
 *
 * Returns floor((part * 100) / whole). Product in uint64_t.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_percent_u(uint32_t u32Part, uint32_t u32Whole)
{
	(void)NULL;
	return b4484_percent(u32Part, u32Whole);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_percent_u(uint32_t u32Part, uint32_t u32Whole)
    __attribute__((alias("gj_u32_percent_u")));
