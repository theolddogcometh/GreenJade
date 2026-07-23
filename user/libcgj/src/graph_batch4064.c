/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4064: GLSL-style step on uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_step_u(uint32_t edge, uint32_t x);
 *     - Generate-step: returns 0 if x < edge, else 1.
 *   uint32_t __gj_u32_step_u  (alias)
 *   __libcgj_batch4064_marker = "libcgj-batch4064"
 *
 * Exclusive continuum CREATE-ONLY (4061-4070). Unique gj_u32_step_u
 * surface only; no multi-def. Distinct from gj_kmp_step_u (batch3772)
 * and RNG / sequence step helpers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4064_marker[] = "libcgj-batch4064";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4064_step(uint32_t u32Edge, uint32_t u32X)
{
	return (u32X < u32Edge) ? 0u : 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_step_u - GLSL-style step on uint32_t.
 *
 * edge: threshold
 * x:    sample value
 *
 * Returns 0 when x < edge, otherwise 1. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_u32_step_u(uint32_t u32Edge, uint32_t u32X)
{
	(void)NULL;
	return b4064_step(u32Edge, u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_step_u(uint32_t u32Edge, uint32_t u32X)
    __attribute__((alias("gj_u32_step_u")));
