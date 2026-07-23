/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4726: rough Q15 cosine of Q32 turns (parabola).
 *
 * Surface (unique symbols):
 *   int16_t gj_cos_turns_q15_u(uint32_t turns_q32);
 *     - Approximate cos(2π * turns) in Q1.15 via a tableless quarter-wave
 *       parabola (phase-shifted sine). Result in [-32767, 32767].
 *   int16_t __gj_cos_turns_q15_u  (alias)
 *   __libcgj_batch4726_marker = "libcgj-batch4726"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730). Planned gj_cos_q15_u is
 * owned by batch3069 (Q15-radian); rename on collision to
 * gj_cos_turns_q15_u (turns_q32 domain). Does not call
 * gj_sin_turns_q15_u (no parent wires). Unique surface only; no
 * multi-def. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4726_marker[] = "libcgj-batch4726";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturate to signed Q15 peak ±32767. */
static int16_t
b4726_sat_q15(int32_t i32V)
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
 * cos(turns) = sin(turns + 1/4 turn). Implement independently:
 * advance phase by 0x40000000 then apply the same quarter-wave parabola.
 */
static int16_t
b4726_cos_turns(uint32_t u32Turns)
{
	uint32_t u32Phase;
	uint32_t u32Quad;
	uint32_t u32Local;
	uint32_t u32P;
	uint64_t u64S;
	int32_t i32S;
	int iNeg;

	u32Phase = u32Turns + 0x40000000u;
	u32Quad = u32Phase >> 30;
	u32Local = u32Phase & 0x3fffffffu;
	if ((u32Quad & 1u) != 0u) {
		u32Local = 0x40000000u - u32Local;
	}

	u32P = u32Local >> 15;
	if (u32P > 32768u) {
		u32P = 32768u;
	}

	u64S = ((uint64_t)u32P << 16) - (uint64_t)u32P * (uint64_t)u32P;
	i32S = (int32_t)(u64S >> 15);

	iNeg = (u32Quad >= 2u) ? 1 : 0;
	if (iNeg != 0) {
		i32S = -i32S;
	}
	return b4726_sat_q15(i32S);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cos_turns_q15_u - rough cosine of a Q32 turn angle, Q15 result.
 *
 * turns_q32: angle as fraction of a circle (2^32 = 360°)
 *
 * Returns approximate cos in Q1.15 (tableless parabola). Distinct from
 * gj_cos_q15_u (batch3069, radian Q15). No parent wires. No __int128.
 */
int16_t
gj_cos_turns_q15_u(uint32_t u32TurnsQ32)
{
	(void)NULL;
	return b4726_cos_turns(u32TurnsQ32);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_cos_turns_q15_u(uint32_t u32TurnsQ32)
    __attribute__((alias("gj_cos_turns_q15_u")));
