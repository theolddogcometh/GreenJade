/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4239: seed u32 from pointer via mix.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seed_from_ptr_u(const void *p);
 *     - Cast pointer p to uintptr_t / uint64_t, apply Stafford Mix01,
 *       return high 32 bits as a seed word. p == NULL -> 0.
 *   uint32_t __gj_seed_from_ptr_u  (alias)
 *   __libcgj_batch4239_marker = "libcgj-batch4239"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Unique gj_seed_from_ptr_u surface
 * only; no multi-def. Distinct from gj_hash_seed_from_ptr (batch2329
 * returns u64). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4239_marker[] = "libcgj-batch4239";

/* David Stafford Mix01 multiplicative constants. */
#define B4239_M1  0x7fb5d329728ea185ULL
#define B4239_M2  0x81dadef4bc2dd44dULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4239_mix01(uint64_t u64X)
{
	u64X = (u64X ^ (u64X >> 31)) * B4239_M1;
	u64X = (u64X ^ (u64X >> 27)) * B4239_M2;
	return u64X ^ (u64X >> 33);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seed_from_ptr_u - derive a 32-bit seed from pointer p.
 *
 * p: arbitrary pointer, or NULL
 *
 * Casts p to uint64_t via uintptr_t, mixes with Stafford Mix01, returns
 * the high 32 bits. NULL yields 0. No parent wires.
 */
uint32_t
gj_seed_from_ptr_u(const void *p)
{
	uint64_t u64X;

	(void)NULL;

	if (p == NULL) {
		return 0u;
	}

	u64X = (uint64_t)(uintptr_t)p;
	return (uint32_t)(b4239_mix01(u64X) >> 32);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seed_from_ptr_u(const void *p)
    __attribute__((alias("gj_seed_from_ptr_u")));
