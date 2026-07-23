/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3009: floor average of two uint32_t values (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_avg_floor_u(uint32_t a, uint32_t b);
 *     - Floor of (a + b) / 2 computed without overflow of a + b.
 *       Equivalent to floor((a + b) / 2) for all a, b in [0, UINT32_MAX],
 *       including cases where a + b would wrap a 32-bit sum.
 *   uint32_t __gj_u32_avg_floor_u  (alias)
 *   __libcgj_batch3009_marker = "libcgj-batch3009"
 *
 * Exclusive continuum CREATE-ONLY (3001-3010). Distinct from
 * gj_avg_u64 (batch381) / gj_u32_avg_floor_n (batch2423) — unique
 * gj_u32_avg_floor_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3009_marker[] = "libcgj-batch3009";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((a + b) / 2) without overflow.
 * Identity: (a & b) + ((a ^ b) >> 1)
 */
static uint32_t
b3009_avg_floor(uint32_t u32A, uint32_t u32B)
{
	return (u32A & u32B) + ((u32A ^ u32B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_avg_floor_u - floor((a + b) / 2) without intermediate overflow.
 *
 * a, b: operands
 * No parent wires.
 */
uint32_t
gj_u32_avg_floor_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3009_avg_floor(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_avg_floor_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_avg_floor_u")));
