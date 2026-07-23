/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4069: rising sawtooth phase on uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_saw_u(uint64_t x, uint64_t period);
 *     - Rising sawtooth: returns x mod period in [0, period). period==0
 *       yields 0 (soft no-op; freestanding-safe, no errno).
 *   uint64_t __gj_u64_saw_u  (alias)
 *   __libcgj_batch4069_marker = "libcgj-batch4069"
 *
 * Exclusive continuum CREATE-ONLY (4061-4070). Unique gj_u64_saw_u
 * surface only; no multi-def. Distinct from gj_u32_saw_u (batch4068),
 * gj_u64_wrap_range_u (batch3267), and gj_u64_pingpong_u (batch3269).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4069_marker[] = "libcgj-batch4069";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4069_saw(uint64_t u64X, uint64_t u64Period)
{
	if (u64Period == 0ull) {
		return 0ull;
	}
	return u64X % u64Period;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_saw_u - rising sawtooth phase on uint64_t.
 *
 * x:      free-running phase
 * period: sawtooth period (0 → 0)
 *
 * Returns a value in [0, period) that ramps and resets. No parent wires.
 */
uint64_t
gj_u64_saw_u(uint64_t u64X, uint64_t u64Period)
{
	(void)NULL;
	return b4069_saw(u64X, u64Period);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_saw_u(uint64_t u64X, uint64_t u64Period)
    __attribute__((alias("gj_u64_saw_u")));
