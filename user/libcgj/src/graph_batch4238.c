/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4238: mix u64 to u32 (high 32 of Stafford Mix01).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_from_u64_mix_u(uint64_t x);
 *     - Apply Stafford Mix01 avalanche to x, return high 32 bits of the
 *       mixed word. Pure; all input values accepted (including 0).
 *   uint32_t __gj_u32_from_u64_mix_u  (alias)
 *   __libcgj_batch4238_marker = "libcgj-batch4238"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Unique gj_u32_from_u64_mix_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Stafford Mix01 constants.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4238_marker[] = "libcgj-batch4238";

/* David Stafford Mix01 multiplicative constants. */
#define B4238_M1  0x7fb5d329728ea185ULL
#define B4238_M2  0x81dadef4bc2dd44dULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4238_mix01(uint64_t u64X)
{
	u64X = (u64X ^ (u64X >> 31)) * B4238_M1;
	u64X = (u64X ^ (u64X >> 27)) * B4238_M2;
	return u64X ^ (u64X >> 33);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_from_u64_mix_u - mix a 64-bit value down to 32 bits.
 *
 * x: arbitrary 64-bit input
 *
 * Returns (uint32_t)(Mix01(x) >> 32). Pure; no side effects.
 * No parent wires.
 */
uint32_t
gj_u32_from_u64_mix_u(uint64_t x)
{
	(void)NULL;
	return (uint32_t)(b4238_mix01(x) >> 32);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_from_u64_mix_u(uint64_t x)
    __attribute__((alias("gj_u32_from_u64_mix_u")));
