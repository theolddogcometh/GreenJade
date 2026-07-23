/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4108: uint64_t floor average without overflow
 * (wave rename).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_avg_floor_u2(uint64_t a, uint64_t b);
 *     - Floor of (a + b) / 2 without overflow of a + b.
 *       Renamed from gj_u64_avg_floor_u (collision with batch3011).
 *   uint64_t __gj_u64_avg_floor_u2  (alias)
 *   __libcgj_batch4108_marker = "libcgj-batch4108"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110). Unique
 * gj_u64_avg_floor_u2 surface only; no multi-def. Distinct from
 * gj_u64_avg (batch615), gj_u64_avg_floor_u (batch3011),
 * gj_u64_avg_floor_n (batch2424), and gj_u64_avg_trunc_u (batch4052).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4108_marker[] = "libcgj-batch4108";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((a + b) / 2) without overflow.
 * Identity: (a & b) + ((a ^ b) >> 1)
 */
static uint64_t
b4108_avg_floor(uint64_t u64A, uint64_t u64B)
{
	return (u64A & u64B) + ((u64A ^ u64B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_avg_floor_u2 - floor((a + b) / 2) without intermediate overflow.
 *
 * a, b: operands
 *
 * Equivalent to floor((a + b) / 2) for all a, b in [0, UINT64_MAX],
 * including cases where a + b would wrap a 64-bit sum.
 * Wave-unique rename: gj_u64_avg_floor_u is owned by batch3011.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_avg_floor_u2(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4108_avg_floor(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_avg_floor_u2(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_avg_floor_u2")));
