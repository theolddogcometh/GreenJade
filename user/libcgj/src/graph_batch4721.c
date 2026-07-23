/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4721: degrees to Q32 turns (fp-free).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deg_to_turns_u(uint32_t deg);
 *     - Map integer degrees to turns in Q32 fixed-point, where one full
 *       circle is 2^32 (wraps naturally in uint32_t). Equivalent to
 *       floor((deg mod 360) * 2^32 / 360).
 *   uint32_t __gj_deg_to_turns_u  (alias)
 *   __libcgj_batch4721_marker = "libcgj-batch4721"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730: deg_to_turns_u,
 * turns_to_deg_u, clamp_angle360_u, angle_diff_u, sin_turns_q15_u,
 * cos_turns_q15_u, isqrt_u32_floor_u, isqrt_u64_floor_u, ihypot_u32_u,
 * batch_id_4730). fp-free angle/trig integer stubs. Unique
 * gj_deg_to_turns_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4721_marker[] = "libcgj-batch4721";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * deg → turns_q32 without 128-bit intermediates:
 *   turns = ((deg % 360) * 2^32) / 360
 * (deg % 360) < 360 so (r << 32) fits in uint64_t.
 */
static uint32_t
b4721_deg_to_turns(uint32_t u32Deg)
{
	uint32_t u32R;
	uint64_t u64Num;

	u32R = u32Deg % 360u;
	u64Num = (uint64_t)u32R << 32;
	return (uint32_t)(u64Num / 360ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deg_to_turns_u - convert integer degrees to Q32 turns.
 *
 * deg: angle in whole degrees (any uint32_t; reduced mod 360)
 *
 * Returns turns in [0, 2^32) with 2^32 representing one full circle
 * (so 360° → 0, 90° → 0x40000000, 180° → 0x80000000). Self-contained;
 * no parent wires. No __int128.
 */
uint32_t
gj_deg_to_turns_u(uint32_t u32Deg)
{
	(void)NULL;
	return b4721_deg_to_turns(u32Deg);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deg_to_turns_u(uint32_t u32Deg)
    __attribute__((alias("gj_deg_to_turns_u")));
