/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4101: saturating uint32_t addition.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_saturate_add_u(uint32_t a, uint32_t b);
 *     - a + b with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_u32_saturate_add_u  (alias)
 *   __libcgj_batch4101_marker = "libcgj-batch4101"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110: u32_saturate_add_u,
 * u32_saturate_sub_u, u64_saturate_add_u, u64_saturate_sub_u,
 * u32_absdiff_u2, u64_absdiff_u3, u32_avg_floor_u2, u64_avg_floor_u2,
 * u32_avg_ceil_u2, batch_id_4110). Distinct from gj_u32_sat_add
 * (batch923) — unique gj_u32_saturate_add_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4101_marker[] = "libcgj-batch4101";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4101_saturate_add(uint32_t u32A, uint32_t u32B)
{
	if (u32B > (UINT32_MAX - u32A)) {
		return UINT32_MAX;
	}
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_saturate_add_u - saturating unsigned 32-bit add.
 *
 * a, b: addends
 *
 * Returns a + b, or UINT32_MAX if the true sum would overflow.
 * Detects overflow without wrapping: b > UINT32_MAX - a.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_saturate_add_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4101_saturate_add(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_saturate_add_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_saturate_add_u")));
