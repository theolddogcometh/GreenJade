/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3869: saturating uint32_t to int32_t (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_u32_to_i32_sat_u(uint32_t x);
 *     - Map unsigned x into the int32_t domain with saturation:
 *       values above INT32_MAX clamp to INT32_MAX; otherwise
 *       return (int32_t)x.
 *   int32_t __gj_u32_to_i32_sat_u  (alias)
 *   __libcgj_batch3869_marker = "libcgj-batch3869"
 *
 * Exclusive continuum CREATE-ONLY (3861-3870). Unique
 * gj_u32_to_i32_sat_u surface only; no multi-def. Distinct from
 * gj_u32_abs_to_i32_u (batch3866 bit-abs) and
 * gj_u64_clamped_to_i64 (batch1738). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3869_marker[] = "libcgj-batch3869";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating cast u32 -> i32.
 * Clamp values above INT32_MAX to INT32_MAX.
 */
static int32_t
b3869_to_i32_sat(uint32_t u32X)
{
	if (u32X > (uint32_t)INT32_MAX) {
		return INT32_MAX;
	}
	return (int32_t)u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_to_i32_sat_u - map u32 into [0, INT32_MAX] as int32_t.
 *
 * Values larger than INT32_MAX saturate to INT32_MAX.
 * No parent wires.
 */
int32_t
gj_u32_to_i32_sat_u(uint32_t u32X)
{
	(void)NULL;
	return b3869_to_i32_sat(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_u32_to_i32_sat_u(uint32_t u32X)
    __attribute__((alias("gj_u32_to_i32_sat_u")));
