/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4109: uint32_t ceiling average without overflow
 * (wave rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_avg_ceil_u2(uint32_t a, uint32_t b);
 *     - Ceiling of (a + b) / 2 without overflow of a + b.
 *       Renamed from gj_u32_avg_ceil_u (collision with batch3012).
 *   uint32_t __gj_u32_avg_ceil_u2  (alias)
 *   __libcgj_batch4109_marker = "libcgj-batch4109"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110). Unique
 * gj_u32_avg_ceil_u2 surface only; no multi-def. Distinct from
 * gj_u32_avg_ceil_u (batch3012) and gj_u32_avg_round_u (batch3253).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4109_marker[] = "libcgj-batch4109";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil((a + b) / 2) without overflow.
 * Identity: (a | b) - ((a ^ b) >> 1)
 */
static uint32_t
b4109_avg_ceil(uint32_t u32A, uint32_t u32B)
{
	return (u32A | u32B) - ((u32A ^ u32B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_avg_ceil_u2 - ceil((a + b) / 2) without intermediate overflow.
 *
 * a, b: operands
 *
 * Identity: (a | b) - ((a ^ b) >> 1)
 * Equivalent to floor((a + b + 1) / 2) for all a, b in [0, UINT32_MAX].
 * Wave-unique rename: gj_u32_avg_ceil_u is owned by batch3012.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_avg_ceil_u2(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4109_avg_ceil(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_avg_ceil_u2(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_avg_ceil_u2")));
