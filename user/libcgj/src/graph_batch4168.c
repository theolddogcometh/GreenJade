/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4168: fold a uint64_t into a uint32_t hash.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u64_hash_fold32_u(uint64_t x);
 *     - Fold x to 32 bits via hi^lo, then apply a light avalanche mix
 *       (xor-shift / multiply stages) so nearby inputs diverge.
 *       All inputs accepted including 0.
 *   uint32_t __gj_u64_hash_fold32_u  (alias)
 *   __libcgj_batch4168_marker = "libcgj-batch4168"
 *
 * CREATE-ONLY exclusive continuum wave (4161-4170). Unique
 * gj_u64_hash_fold32_u surface only; no multi-def. Distinct from
 * gj_xor_fold32_u (batch2808 buffer fold), gj_hash_mix32_u (batch2051),
 * and gj_u32_hash_mix64_u (batch4167). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4168_marker[] = "libcgj-batch4168";

/* Light 32-bit avalanche constants (public mixer lineage). */
#define B4168_M1 0x85ebca6bu
#define B4168_M2 0xc2b2ae35u

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold hi^lo then fmix-style avalanche to 32 bits. */
static uint32_t
b4168_fold32(uint64_t u64X)
{
	uint32_t u32H;

	u32H = (uint32_t)(u64X ^ (u64X >> 32));
	u32H ^= u32H >> 16;
	u32H *= B4168_M1;
	u32H ^= u32H >> 13;
	u32H *= B4168_M2;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_hash_fold32_u - fold a uint64_t into a mixed uint32_t.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns the folded and mixed 32-bit digest. Stateless pure function.
 * No parent wires.
 */
uint32_t
gj_u64_hash_fold32_u(uint64_t u64X)
{
	(void)NULL;
	return b4168_fold32(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u64_hash_fold32_u(uint64_t u64X)
    __attribute__((alias("gj_u64_hash_fold32_u")));
