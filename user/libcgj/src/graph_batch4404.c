/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4404: saturating uint64_t multiply.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_sat_u(uint64_t a, uint64_t b);
 *     - a * b with saturation at UINT64_MAX on overflow (no wrap).
 *       Software path only (no __int128 / no wide builtins).
 *   uint64_t __gj_u64_mul_sat_u  (alias)
 *   __libcgj_batch4404_marker = "libcgj-batch4404"
 *
 * Exclusive continuum CREATE-ONLY (4401-4410). Distinct from
 * gj_u64_mul_sat_soft (batch2033), gj_sat_mul_u64 (batch567), and
 * gj_u64_saturating_mul (batch1374) — unique gj_u64_mul_sat_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4404_marker[] = "libcgj-batch4404";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft saturating multiply: overflow iff a > floor(UINT64_MAX / b)
 * when both factors are non-zero.
 */
static uint64_t
b4404_mul_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		return UINT64_MAX;
	}
	return u64A * u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_sat_u - unsigned 64-bit multiply; clamp to UINT64_MAX.
 *
 * a, b: factors
 * Zero factor never overflows. No __int128. No parent wires.
 */
uint64_t
gj_u64_mul_sat_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4404_mul_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_sat_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_mul_sat_u")));
