/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4065: GLSL-style step on uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_step_u(uint64_t edge, uint64_t x);
 *     - Generate-step: returns 0 if x < edge, else 1.
 *   uint64_t __gj_u64_step_u  (alias)
 *   __libcgj_batch4065_marker = "libcgj-batch4065"
 *
 * Exclusive continuum CREATE-ONLY (4061-4070). Unique gj_u64_step_u
 * surface only; no multi-def. Distinct from gj_u32_step_u (batch4064)
 * and RNG / sequence step helpers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4065_marker[] = "libcgj-batch4065";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4065_step(uint64_t u64Edge, uint64_t u64X)
{
	return (u64X < u64Edge) ? 0ull : 1ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_step_u - GLSL-style step on uint64_t.
 *
 * edge: threshold
 * x:    sample value
 *
 * Returns 0 when x < edge, otherwise 1. Does not call libc.
 * No parent wires.
 */
uint64_t
gj_u64_step_u(uint64_t u64Edge, uint64_t u64X)
{
	(void)NULL;
	return b4065_step(u64Edge, u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_step_u(uint64_t u64Edge, uint64_t u64X)
    __attribute__((alias("gj_u64_step_u")));
