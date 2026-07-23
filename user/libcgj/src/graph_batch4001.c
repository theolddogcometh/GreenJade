/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4001: clamp uint32_t to the unit interval [0, 1].
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clamp01_u(uint32_t x);
 *     - Confine x to the closed interval [0, 1]. Values above 1 clamp
 *       to 1; the unsigned domain already saturates the lower bound at 0.
 *   uint32_t __gj_u32_clamp01_u  (alias)
 *   __libcgj_batch4001_marker = "libcgj-batch4001"
 *
 * Exclusive continuum CREATE-ONLY (4001-4010: u32_clamp01_u, u64_clamp01_u,
 * u32_signbit_u, u64_signbit_u, u32_clear_msb_u, u64_clear_msb_u,
 * u32_set_msb_u, u64_set_msb_u, u32_toggle_msb_u, batch_id_4010).
 * Distinct from gj_clamp01_q16_u (batch3058) and gj_u32_clamp_u
 * (batch2703) — unique gj_u32_clamp01_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4001_marker[] = "libcgj-batch4001";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4001_clamp01(uint32_t u32X)
{
	if (u32X > 1u) {
		return 1u;
	}
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clamp01_u - clamp uint32_t into the closed unit interval [0, 1].
 *
 * x: value to clamp
 *
 * Returns 0 when x is 0, 1 when x >= 1, otherwise x (only 0/1 possible).
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_clamp01_u(uint32_t u32X)
{
	(void)NULL;
	return b4001_clamp01(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clamp01_u(uint32_t u32X)
    __attribute__((alias("gj_u32_clamp01_u")));
