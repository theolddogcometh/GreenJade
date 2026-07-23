/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4052: uint64_t overflow-safe truncated average
 * (exclusive avg_trunc_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_avg_trunc_u(uint64_t a, uint64_t b);
 *     - Truncating (floor) average of (a + b) / 2 without overflow of
 *       a + b. Equivalent to floor((a + b) / 2) for all a, b in
 *       [0, UINT64_MAX].
 *   uint64_t __gj_u64_avg_trunc_u  (alias)
 *   __libcgj_batch4052_marker = "libcgj-batch4052"
 *
 * CREATE-ONLY exclusive continuum wave (4051-4060). Unique
 * gj_u64_avg_trunc_u surface only; no multi-def. Distinct from
 * gj_u64_avg (batch615), gj_u64_avg_u (batch2644), gj_u64_avg_floor_u
 * (batch3011), gj_u64_avg_ceil_u (batch3013), gj_u64_avg_round_u
 * (batch3254). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4052_marker[] = "libcgj-batch4052";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((a + b) / 2) without overflow.
 * Identity: (a & b) + ((a ^ b) >> 1)
 */
static uint64_t
b4052_avg_trunc(uint64_t u64A, uint64_t u64B)
{
	return (u64A & u64B) + ((u64A ^ u64B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_avg_trunc_u - truncating floor((a + b) / 2) without overflow.
 *
 * a, b: operands
 * Truncates toward zero (unsigned floor). Self-contained; no parent wires.
 */
uint64_t
gj_u64_avg_trunc_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4052_avg_trunc(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_avg_trunc_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_avg_trunc_u")));
