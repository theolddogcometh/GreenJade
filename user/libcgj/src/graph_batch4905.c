/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4905: unsigned 4-element dot product (widened).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u32_dot4_u(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,
 *                          uint32_t b0, uint32_t b1, uint32_t b2, uint32_t b3);
 *     - a0*b0 + a1*b1 + a2*b2 + a3*b3 with each product in uint64_t.
 *   uint64_t __gj_u32_dot4_u  (alias)
 *   __libcgj_batch4905_marker = "libcgj-batch4905"
 *
 * Exclusive continuum CREATE-ONLY (4901-4910). Distinct from
 * gj_u32_dot2_u (batch4903) and gj_u32_dot3_u (batch4904) — unique
 * gj_u32_dot4_u surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4905_marker[] = "libcgj-batch4905";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4905_dot4(uint32_t u32A0, uint32_t u32A1, uint32_t u32A2, uint32_t u32A3,
    uint32_t u32B0, uint32_t u32B1, uint32_t u32B2, uint32_t u32B3)
{
	uint64_t u64Acc;

	u64Acc = (uint64_t)u32A0 * (uint64_t)u32B0;
	u64Acc += (uint64_t)u32A1 * (uint64_t)u32B1;
	u64Acc += (uint64_t)u32A2 * (uint64_t)u32B2;
	u64Acc += (uint64_t)u32A3 * (uint64_t)u32B3;
	return u64Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_dot4_u - four-lane unsigned dot product a·b.
 *
 * a0..a3, b0..b3: vector lanes
 *
 * Returns sum of widened lane products. Self-contained; no parent wires.
 */
uint64_t
gj_u32_dot4_u(uint32_t u32A0, uint32_t u32A1, uint32_t u32A2, uint32_t u32A3,
    uint32_t u32B0, uint32_t u32B1, uint32_t u32B2, uint32_t u32B3)
{
	(void)NULL;
	return b4905_dot4(u32A0, u32A1, u32A2, u32A3,
	    u32B0, u32B1, u32B2, u32B3);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u32_dot4_u(uint32_t u32A0, uint32_t u32A1,
    uint32_t u32A2, uint32_t u32A3,
    uint32_t u32B0, uint32_t u32B1, uint32_t u32B2, uint32_t u32B3)
    __attribute__((alias("gj_u32_dot4_u")));
