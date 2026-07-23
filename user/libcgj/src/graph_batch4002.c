/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4002: clamp uint64_t to the unit interval [0, 1].
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clamp01_u(uint64_t x);
 *     - Confine x to the closed interval [0, 1]. Values above 1 clamp
 *       to 1; the unsigned domain already saturates the lower bound at 0.
 *   uint64_t __gj_u64_clamp01_u  (alias)
 *   __libcgj_batch4002_marker = "libcgj-batch4002"
 *
 * Exclusive continuum CREATE-ONLY (4001-4010). Distinct from
 * gj_u32_clamp01_u (batch4001), gj_clamp01_q16_u (batch3058), and
 * gj_u64_clamp_range (batch1371) — unique gj_u64_clamp01_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4002_marker[] = "libcgj-batch4002";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4002_clamp01(uint64_t u64X)
{
	if (u64X > 1ULL) {
		return 1ULL;
	}
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clamp01_u - clamp uint64_t into the closed unit interval [0, 1].
 *
 * x: value to clamp
 *
 * Returns 0 when x is 0, 1 when x >= 1, otherwise x (only 0/1 possible).
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_clamp01_u(uint64_t u64X)
{
	(void)NULL;
	return b4002_clamp01(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clamp01_u(uint64_t u64X)
    __attribute__((alias("gj_u64_clamp01_u")));
