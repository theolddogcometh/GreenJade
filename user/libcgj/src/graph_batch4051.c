/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4051: uint32_t overflow-safe truncated average
 * (exclusive avg_trunc_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_avg_trunc_u(uint32_t a, uint32_t b);
 *     - Truncating (floor) average of (a + b) / 2 without overflow of
 *       a + b. Equivalent to floor((a + b) / 2) for all a, b in
 *       [0, UINT32_MAX].
 *   uint32_t __gj_u32_avg_trunc_u  (alias)
 *   __libcgj_batch4051_marker = "libcgj-batch4051"
 *
 * CREATE-ONLY exclusive continuum wave (4051-4060). Unique
 * gj_u32_avg_trunc_u surface only; no multi-def. Distinct from
 * gj_u32_avg_u (batch2643), gj_u32_avg_floor_u (batch3009),
 * gj_u32_avg_ceil_u (batch3012), gj_u32_avg_round_u (batch3253).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4051_marker[] = "libcgj-batch4051";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((a + b) / 2) without overflow.
 * Identity: (a & b) + ((a ^ b) >> 1)
 */
static uint32_t
b4051_avg_trunc(uint32_t u32A, uint32_t u32B)
{
	return (u32A & u32B) + ((u32A ^ u32B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_avg_trunc_u - truncating floor((a + b) / 2) without overflow.
 *
 * a, b: operands
 * Truncates toward zero (unsigned floor). Self-contained; no parent wires.
 */
uint32_t
gj_u32_avg_trunc_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4051_avg_trunc(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_avg_trunc_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_avg_trunc_u")));
