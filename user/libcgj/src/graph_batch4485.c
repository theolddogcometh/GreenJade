/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4485: uint32 basis points of whole
 * (part * 10000 / whole).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_basis_points_u(uint32_t part, uint32_t whole);
 *     - Return floor((part * 10000) / whole). whole==0 → 0.
 *   uint32_t __gj_u32_basis_points_u  (alias)
 *   __libcgj_batch4485_marker = "libcgj-batch4485"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Float-free
 * rational unique wave. Distinct from gj_basis_points (batch997) and
 * gj_basis_points_of_u64 (batch1747) — unique gj_u32_basis_points_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4485_marker[] = "libcgj-batch4485";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor((part * 10000) / whole). whole==0 → 0. */
static uint32_t
b4485_bp(uint32_t u32Part, uint32_t u32Whole)
{
	uint64_t u64Prod;

	if (u32Whole == 0u) {
		return 0u;
	}
	u64Prod = (uint64_t)u32Part * 10000ull;
	return (uint32_t)(u64Prod / (uint64_t)u32Whole);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_basis_points_u - basis points of whole occupied by part.
 *
 * part:  portion count
 * whole: total count (whole==0 yields 0)
 *
 * Returns floor((part * 10000) / whole). 10000 bp = 100%. Product in
 * uint64_t. Self-contained; no parent wires.
 */
uint32_t
gj_u32_basis_points_u(uint32_t u32Part, uint32_t u32Whole)
{
	(void)NULL;
	return b4485_bp(u32Part, u32Whole);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_basis_points_u(uint32_t u32Part, uint32_t u32Whole)
    __attribute__((alias("gj_u32_basis_points_u")));
