/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4903: unsigned 2-element dot product (widened).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u32_dot2_u(uint32_t a0, uint32_t a1,
 *                          uint32_t b0, uint32_t b1);
 *     - a0*b0 + a1*b1 with each product formed in uint64_t.
 *   uint64_t __gj_u32_dot2_u  (alias)
 *   __libcgj_batch4903_marker = "libcgj-batch4903"
 *
 * Exclusive continuum CREATE-ONLY (4901-4910). Unique gj_u32_dot2_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4903_marker[] = "libcgj-batch4903";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4903_dot2(uint32_t u32A0, uint32_t u32A1, uint32_t u32B0, uint32_t u32B1)
{
	uint64_t u64P0;
	uint64_t u64P1;

	u64P0 = (uint64_t)u32A0 * (uint64_t)u32B0;
	u64P1 = (uint64_t)u32A1 * (uint64_t)u32B1;
	return u64P0 + u64P1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_dot2_u - two-lane unsigned dot product a·b.
 *
 * a0, a1: left vector lanes
 * b0, b1: right vector lanes
 *
 * Returns a0*b0 + a1*b1 with widened multiplies. Self-contained;
 * no parent wires.
 */
uint64_t
gj_u32_dot2_u(uint32_t u32A0, uint32_t u32A1, uint32_t u32B0, uint32_t u32B1)
{
	(void)NULL;
	return b4903_dot2(u32A0, u32A1, u32B0, u32B1);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u32_dot2_u(uint32_t u32A0, uint32_t u32A1,
    uint32_t u32B0, uint32_t u32B1)
    __attribute__((alias("gj_u32_dot2_u")));
