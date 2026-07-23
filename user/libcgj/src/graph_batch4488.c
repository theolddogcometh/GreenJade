/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4488: classic Hermite smoothstep on a
 * rational u32 domain [0, tmax].
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_smoothstep_tmax_u(uint32_t t, uint32_t tmax);
 *     - Hermite smoothstep of t over [0, tmax]. Returns a value in
 *       [0, tmax]: 0 at/before 0, tmax at/after tmax, and
 *       floor(s^2*(3-2*s)*tmax) between with s = t/tmax.
 *       tmax==0 → 0.
 *   uint32_t __gj_u32_smoothstep_tmax_u  (alias)
 *   __libcgj_batch4488_marker = "libcgj-batch4488"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Float-free
 * rational unique wave. Distinct from gj_u32_smoothstep_u (batch4062
 * edge0/edge1 form) and gj_smoothstep_q16_u (batch3057) — unique
 * tmax-domain surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4488_marker[] = "libcgj-batch4488";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((hi:lo) / d) for d > 0, quotient fitting in 64 bits.
 * hi is small in our use (product of u32 * ~3*u32 reduced); when
 * hi >= d the mathematical quotient is >= 2^64 — saturate.
 */
static uint64_t
b4488_div128_64(uint64_t u64Hi, uint64_t u64Lo, uint64_t u64D)
{
	uint64_t u64Q;
	uint64_t u64R;
	uint32_t uBit;

	if (u64D == 0ull) {
		return 0ull;
	}
	if (u64Hi >= u64D) {
		return UINT64_MAX;
	}

	u64Q = 0ull;
	u64R = u64Hi;
	for (uBit = 0u; uBit < 64u; uBit++) {
		uint64_t u64Msb = u64R >> 63;
		uint64_t u64NewR = (u64R << 1) | (u64Lo >> 63);

		u64Lo <<= 1;
		u64Q <<= 1;
		u64R = u64NewR;
		if (u64Msb != 0ull || u64R >= u64D) {
			u64R -= u64D;
			u64Q |= 1ull;
		}
	}
	return u64Q;
}

/*
 * Classic smoothstep on [0, tmax]:
 *   result = t^2 * (3*tmax - 2*t) / tmax^2
 * Form 96-ish bit numerator from (t*t)*s without __int128, then
 * divide by tmax*tmax.
 */
static uint32_t
b4488_smoothstep(uint32_t u32T, uint32_t u32Tmax)
{
	uint64_t u64Tt;
	uint64_t u64S;
	uint64_t u64Den;
	uint64_t u64TtLo;
	uint64_t u64TtHi;
	uint64_t u64SLo;
	uint64_t u64SHi;
	uint64_t u64P0;
	uint64_t u64P1;
	uint64_t u64P2;
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Q;

	if (u32Tmax == 0u) {
		return 0u;
	}
	if (u32T == 0u) {
		return 0u;
	}
	if (u32T >= u32Tmax) {
		return u32Tmax;
	}

	/* s = 3*tmax - 2*t  (fits uint64; max ~ 3*2^32). */
	u64S = (3ull * (uint64_t)u32Tmax) - (2ull * (uint64_t)u32T);
	u64Tt = (uint64_t)u32T * (uint64_t)u32T;

	/* Fast path: tt * s fits in 64 bits. */
	if (u64S == 0ull) {
		return 0u;
	}
	if (u64Tt <= UINT64_MAX / u64S) {
		u64Den = (uint64_t)u32Tmax * (uint64_t)u32Tmax;
		return (uint32_t)((u64Tt * u64S) / u64Den);
	}

	/*
	 * Wide product tt * s via 32-bit limbs.
	 * tt = ttHi:ttLo, s = sHi:sLo (sHi is 0 or small).
	 */
	u64TtLo = u64Tt & 0xffffffffull;
	u64TtHi = u64Tt >> 32;
	u64SLo = u64S & 0xffffffffull;
	u64SHi = u64S >> 32;

	u64P0 = u64TtLo * u64SLo;
	u64P1 = u64TtLo * u64SHi + u64TtHi * u64SLo;
	u64P2 = u64TtHi * u64SHi;

	{
		uint64_t u64Mid = (u64P0 >> 32) + (u64P1 & 0xffffffffull);

		u64Lo = (u64P0 & 0xffffffffull) | (u64Mid << 32);
		u64Hi = u64P2 + (u64P1 >> 32) + (u64Mid >> 32);
	}

	u64Den = (uint64_t)u32Tmax * (uint64_t)u32Tmax;
	u64Q = b4488_div128_64(u64Hi, u64Lo, u64Den);
	if (u64Q > (uint64_t)u32Tmax) {
		return u32Tmax;
	}
	return (uint32_t)u64Q;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_smoothstep_tmax_u - Hermite smoothstep on domain [0, tmax].
 *
 * t:    sample along the domain
 * tmax: domain end (tmax==0 yields 0)
 *
 * Returns value in [0, tmax]. Endpoints map to 0 / tmax; interior
 * uses s*s*(3-2*s) with s = t/tmax. Self-contained; no parent wires.
 */
uint32_t
gj_u32_smoothstep_tmax_u(uint32_t u32T, uint32_t u32Tmax)
{
	(void)NULL;
	return b4488_smoothstep(u32T, u32Tmax);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_smoothstep_tmax_u(uint32_t u32T, uint32_t u32Tmax)
    __attribute__((alias("gj_u32_smoothstep_tmax_u")));
