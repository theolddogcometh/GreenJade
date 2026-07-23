/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4714: 32-bit hash finalizer mix cascade (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_mix_final32_u(uint32_t x);
 *     - Avalanche finalizer for a single 32-bit hash accumulator:
 *       xorshift / odd-multiply cascade unique to this TU. Pure; no
 *       buffer walk.
 *   uint32_t __gj_hash_mix_final32_u  (alias)
 *   __libcgj_batch4714_marker = "libcgj-batch4714"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720). Unique
 * gj_hash_mix_final32_u surface only; no multi-def. Distinct from
 * gj_hash_mix_final32 (batch2327 XXH), gj_u32_mix_final_u (batch4025
 * Murmur fmix32), and gj_entropy_mix32_u (batch4621). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4714_marker[] = "libcgj-batch4714";

/* Cascade-unique 32-bit finalizer multipliers (not murmur/xxh). */
#define B4714_C1  0x7feb352du
#define B4714_C2  0x846ca68bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4714_mix_final32(uint32_t u32X)
{
	u32X ^= u32X >> 15;
	u32X *= B4714_C1;
	u32X ^= u32X >> 13;
	u32X *= B4714_C2;
	u32X ^= u32X >> 16;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_mix_final32_u - 32-bit cascade finalizer mix of x.
 *
 * x: arbitrary 32-bit input (all values accepted, including 0)
 *
 * Returns a fully diffused 32-bit value. Stateless pure function.
 * No parent wires.
 */
uint32_t
gj_hash_mix_final32_u(uint32_t u32X)
{
	(void)NULL;
	return b4714_mix_final32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_mix_final32_u(uint32_t u32X)
    __attribute__((alias("gj_hash_mix_final32_u")));
