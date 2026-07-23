/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4725: rough Q15 sine of Q32 turns (parabola).
 *
 * Surface (unique symbols):
 *   int16_t gj_sin_turns_q15_u(uint32_t turns_q32);
 *     - Approximate sin(2π * turns) in Q1.15 via a tableless quarter-wave
 *       parabola. Result in [-32767, 32767].
 *   int16_t __gj_sin_turns_q15_u  (alias)
 *   __libcgj_batch4725_marker = "libcgj-batch4725"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730). Planned gj_sin_q15_u is
 * owned by batch3068 (Q15-radian Taylor); rename on collision to
 * gj_sin_turns_q15_u (turns_q32 domain). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4725_marker[] = "libcgj-batch4725";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturate to signed Q15 peak ±32767. */
static int16_t
b4725_sat_q15(int32_t i32V)
{
	if (i32V > 32767) {
		return (int16_t)32767;
	}
	if (i32V < -32767) {
		return (int16_t)(-32767);
	}
	return (int16_t)i32V;
}

/*
 * Rough sin from Q32 turns via quarter-wave parabola:
 *   p in [0, 1] over a quarter → s ≈ 2p - p^2  (0 at 0°, 1 at 90°)
 * Quadrants: 0:+, 1:+ (mirrored), 2:-, 3:- (mirrored).
 */
static int16_t
b4725_sin_turns(uint32_t u32Turns)
{
	uint32_t u32Quad;
	uint32_t u32Local;
	uint32_t u32P;
	uint64_t u64S;
	int32_t i32S;
	int iNeg;

	u32Quad = u32Turns >> 30;
	u32Local = u32Turns & 0x3fffffffu;
	if ((u32Quad & 1u) != 0u) {
		/* Mirror within quarter: 90°→0° becomes 0°→90°. */
		u32Local = 0x40000000u - u32Local;
	}

	/* p in Q15: local is in [0, 2^30], >>15 → [0, 2^15]. */
	u32P = u32Local >> 15;
	if (u32P > 32768u) {
		u32P = 32768u;
	}

	/* s = 2p - p^2 in Q15: ((p << 16) - p*p) >> 15 */
	u64S = ((uint64_t)u32P << 16) - (uint64_t)u32P * (uint64_t)u32P;
	i32S = (int32_t)(u64S >> 15);

	iNeg = (u32Quad >= 2u) ? 1 : 0;
	if (iNeg != 0) {
		i32S = -i32S;
	}
	return b4725_sat_q15(i32S);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sin_turns_q15_u - rough sine of a Q32 turn angle, Q15 result.
 *
 * turns_q32: angle as fraction of a circle (2^32 = 360°)
 *
 * Returns approximate sin in Q1.15 (tableless parabola). Distinct from
 * gj_sin_q15_u (batch3068, radian Q15). No parent wires. No __int128.
 */
int16_t
gj_sin_turns_q15_u(uint32_t u32TurnsQ32)
{
	(void)NULL;
	return b4725_sin_turns(u32TurnsQ32);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_sin_turns_q15_u(uint32_t u32TurnsQ32)
    __attribute__((alias("gj_sin_turns_q15_u")));
