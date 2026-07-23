/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4236: Stafford Mix01 64-bit avalanche.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mix_const_u(uint64_t x);
 *     - Pure 64-bit mix using David Stafford Mix01 constants:
 *         x ^= x >> 31;  x *= 0x7fb5d329728ea185;
 *         x ^= x >> 27;  x *= 0x81dadef4bc2dd44d;
 *         x ^= x >> 33;
 *       Stateless avalanche of all input bits into the output.
 *   uint64_t __gj_u64_mix_const_u  (alias)
 *   __libcgj_batch4236_marker = "libcgj-batch4236"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Unique gj_u64_mix_const_u surface
 * only; no multi-def. Distinct from gj_u64_mix_final_u (batch4026
 * Murmur fmix64) — Stafford Mix01 constants. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants from Stafford's public
 * "Better Bit Mixing" Mix01.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4236_marker[] = "libcgj-batch4236";

/* David Stafford Mix01 multiplicative constants. */
#define B4236_M1  0x7fb5d329728ea185ULL
#define B4236_M2  0x81dadef4bc2dd44dULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4236_mix01(uint64_t u64X)
{
	u64X = (u64X ^ (u64X >> 31)) * B4236_M1;
	u64X = (u64X ^ (u64X >> 27)) * B4236_M2;
	return u64X ^ (u64X >> 33);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mix_const_u - Stafford Mix01 64-bit avalanche mix.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns a well-mixed 64-bit value. Pure; no side effects.
 * No parent wires.
 */
uint64_t
gj_u64_mix_const_u(uint64_t x)
{
	(void)NULL;
	return b4236_mix01(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mix_const_u(uint64_t x)
    __attribute__((alias("gj_u64_mix_const_u")));
