/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4723: normalize/clamp degree angle mod 360.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clamp_angle360_u(uint32_t deg);
 *     - Fold deg into the half-open degree circle [0, 360) via
 *       unsigned modulo 360 (angle normalize / clamp-to-circle).
 *   uint32_t __gj_clamp_angle360_u  (alias)
 *   __libcgj_batch4723_marker = "libcgj-batch4723"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730). Unique
 * gj_clamp_angle360_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4723_marker[] = "libcgj-batch4723";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold deg into [0, 360). */
static uint32_t
b4723_clamp360(uint32_t u32Deg)
{
	return u32Deg % 360u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clamp_angle360_u - normalize degree angle into [0, 360).
 *
 * deg: angle in whole degrees (any uint32_t)
 *
 * Returns deg mod 360 in [0, 359]. Self-contained; no parent wires.
 */
uint32_t
gj_clamp_angle360_u(uint32_t u32Deg)
{
	(void)NULL;
	return b4723_clamp360(u32Deg);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clamp_angle360_u(uint32_t u32Deg)
    __attribute__((alias("gj_clamp_angle360_u")));
