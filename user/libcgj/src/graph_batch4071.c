/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4071: uint32_t triangle wave (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_tri_u(uint32_t phase, uint32_t period);
 *     - Triangle wave of the given period. Maps phase into a rising
 *       then falling ramp over one period. Returns a value in
 *       [0, period/2]. period == 0 → 0 (soft guard).
 *   uint32_t __gj_u32_tri_u  (alias)
 *   __libcgj_batch4071_marker = "libcgj-batch4071"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080): u32_tri_u,
 * u64_tri_u, u32_square_wave_u, u64_square_wave_u, u32_mod_wrap_u,
 * u64_mod_wrap_u, u32_mod_mirror_u, u64_mod_mirror_u, u32_fract_q16_u,
 * batch_id_4080. Unique gj_u32_tri_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4071_marker[] = "libcgj-batch4071";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Triangle wave: rise 0..half over first half-period, fall half..0 over
 * the second. Soft-guard period == 0 → 0.
 */
static uint32_t
b4071_tri(uint32_t u32Phase, uint32_t u32Period)
{
	uint32_t u32T;
	uint32_t u32Half;

	if (u32Period == 0u) {
		return 0u;
	}
	u32T = u32Phase % u32Period;
	u32Half = u32Period / 2u;
	if (u32Half == 0u) {
		return 0u;
	}
	if (u32T <= u32Half) {
		return u32T;
	}
	return u32Period - u32T;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_tri_u - unsigned 32-bit triangle wave.
 *
 * phase:  free-running phase sample
 * period: wave period in samples (0 → 0 soft guard)
 *
 * Returns a value in [0, period/2] forming a triangle over one period.
 * No parent wires.
 */
uint32_t
gj_u32_tri_u(uint32_t u32Phase, uint32_t u32Period)
{
	(void)NULL;
	return b4071_tri(u32Phase, u32Period);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_tri_u(uint32_t u32Phase, uint32_t u32Period)
    __attribute__((alias("gj_u32_tri_u")));
