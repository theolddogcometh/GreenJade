/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4909: saturating uint32_t scale multiply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mat2_scale_u(uint32_t s, uint32_t v);
 *     - s * v with saturation at UINT32_MAX on overflow (no wrap).
 *       Scale a matrix/vector lane by an unsigned factor.
 *   uint32_t __gj_mat2_scale_u  (alias)
 *   __libcgj_batch4909_marker = "libcgj-batch4909"
 *
 * Exclusive continuum CREATE-ONLY (4901-4910). Distinct from
 * gj_sat_mul_u64 (batch567) and gj_u32_saturate_add_u (batch4101) —
 * unique gj_mat2_scale_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4909_marker[] = "libcgj-batch4909";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4909_scale_sat(uint32_t u32S, uint32_t u32V)
{
	if (u32S == 0u || u32V == 0u) {
		return 0u;
	}
	if (u32S > (UINT32_MAX / u32V)) {
		return UINT32_MAX;
	}
	return u32S * u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat2_scale_u - saturating unsigned scale multiply s * v.
 *
 * s: scale factor
 * v: value / matrix lane
 *
 * Returns s * v, or UINT32_MAX if the true product would overflow.
 * Detects overflow without wrapping: s > UINT32_MAX / v (when v != 0).
 * Self-contained; no parent wires.
 */
uint32_t
gj_mat2_scale_u(uint32_t u32S, uint32_t u32V)
{
	(void)NULL;
	return b4909_scale_sat(u32S, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mat2_scale_u(uint32_t u32S, uint32_t u32V)
    __attribute__((alias("gj_mat2_scale_u")));
