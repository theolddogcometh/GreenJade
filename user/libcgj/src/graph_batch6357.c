/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6357: Xbox right-trigger scale stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_rt_scale_6357(uint32_t raw8);
 *     - Map 8-bit right analog trigger (0..255) to a 0..1000 milli-unit
 *       range: out = (raw8 * 1000) / 255, with raw8 clamped to 255.
 *       Parallel surface to gj_xpad_lt_scale_6356 for RT.
 *   uint32_t __gj_xpad_rt_scale_6357  (alias)
 *   __libcgj_batch6357_marker = "libcgj-batch6357"
 *
 * Exclusive continuum CREATE-ONLY (6351-6360: Xbox controller feature
 * stubs). Unique gj_xpad_rt_scale_6357 surface only; no multi-def.
 * Distinct from gj_xpad_lt_scale_6356. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6357_marker[] = "libcgj-batch6357";

#define B6357_RAW_MAX   255u
#define B6357_OUT_MAX  1000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6357_scale(uint32_t uRaw8)
{
	uint32_t uClamped;

	uClamped = uRaw8;
	if (uClamped > B6357_RAW_MAX) {
		uClamped = B6357_RAW_MAX;
	}
	return (uClamped * B6357_OUT_MAX) / B6357_RAW_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_rt_scale_6357 - right trigger 8-bit → milli-unit [0, 1000].
 *
 * Soft policy scale for RT analog paths. Pure integer; no float.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_xpad_rt_scale_6357(uint32_t uRaw8)
{
	(void)NULL;
	return b6357_scale(uRaw8);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xpad_rt_scale_6357(uint32_t uRaw8)
    __attribute__((alias("gj_xpad_rt_scale_6357")));
