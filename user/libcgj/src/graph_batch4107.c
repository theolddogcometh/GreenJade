/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4107: uint32_t floor average without overflow
 * (wave rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_avg_floor_u2(uint32_t a, uint32_t b);
 *     - Floor of (a + b) / 2 without overflow of a + b.
 *       Renamed from gj_u32_avg_floor_u (collision with batch3009).
 *   uint32_t __gj_u32_avg_floor_u2  (alias)
 *   __libcgj_batch4107_marker = "libcgj-batch4107"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110). Unique
 * gj_u32_avg_floor_u2 surface only; no multi-def. Distinct from
 * gj_u32_avg_floor_u (batch3009), gj_u32_avg_floor_n (batch2423),
 * and gj_u32_avg_trunc_u (batch4051). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4107_marker[] = "libcgj-batch4107";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((a + b) / 2) without overflow.
 * Identity: (a & b) + ((a ^ b) >> 1)
 */
static uint32_t
b4107_avg_floor(uint32_t u32A, uint32_t u32B)
{
	return (u32A & u32B) + ((u32A ^ u32B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_avg_floor_u2 - floor((a + b) / 2) without intermediate overflow.
 *
 * a, b: operands
 *
 * Equivalent to floor((a + b) / 2) for all a, b in [0, UINT32_MAX],
 * including cases where a + b would wrap a 32-bit sum.
 * Wave-unique rename: gj_u32_avg_floor_u is owned by batch3009.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_avg_floor_u2(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4107_avg_floor(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_avg_floor_u2(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_avg_floor_u2")));
