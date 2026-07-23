/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4068: rising sawtooth phase on uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_saw_u(uint32_t x, uint32_t period);
 *     - Rising sawtooth: returns x mod period in [0, period). period==0
 *       yields 0 (soft no-op; freestanding-safe, no errno).
 *   uint32_t __gj_u32_saw_u  (alias)
 *   __libcgj_batch4068_marker = "libcgj-batch4068"
 *
 * Exclusive continuum CREATE-ONLY (4061-4070). Unique gj_u32_saw_u
 * surface only; no multi-def. Distinct from gj_u32_wrap_range_u
 * (batch3266 closed modular fold) and gj_u32_pingpong_u (batch3268
 * triangle). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4068_marker[] = "libcgj-batch4068";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4068_saw(uint32_t u32X, uint32_t u32Period)
{
	if (u32Period == 0u) {
		return 0u;
	}
	return u32X % u32Period;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_saw_u - rising sawtooth phase on uint32_t.
 *
 * x:      free-running phase
 * period: sawtooth period (0 → 0)
 *
 * Returns a value in [0, period) that ramps and resets. No parent wires.
 */
uint32_t
gj_u32_saw_u(uint32_t u32X, uint32_t u32Period)
{
	(void)NULL;
	return b4068_saw(u32X, u32Period);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_saw_u(uint32_t u32X, uint32_t u32Period)
    __attribute__((alias("gj_u32_saw_u")));
