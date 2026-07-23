/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4621: 32-bit entropy mix (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_entropy_mix32_u(uint32_t x);
 *     - Non-cryptographic single-word entropy mixer for x:
 *       xorshift / odd-multiply / xorshift / odd-multiply / xorshift.
 *       Spreads sparse input entropy across all 32 bits.
 *   uint32_t __gj_entropy_mix32_u  (alias)
 *   __libcgj_batch4621_marker = "libcgj-batch4621"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630: entropy_mix32_u,
 * entropy_mix64_u, entropy_combine32_u, entropy_combine64_u,
 * entropy_rot_mix_u, entropy_avalanche32_u, entropy_avalanche64_u,
 * entropy_from_bytes_u, entropy_seed_seq_u, batch_id_4630).
 * Distinct from gj_u32_mix_final_u (batch4025), gj_u32_avalanche_u
 * (batch4027) — unique constants and gj_entropy_mix32_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4621_marker[] = "libcgj-batch4621";

/* Odd mix multipliers unique to this TU (not murmur/splitmix fmix). */
#define B4621_M0  0x7f4a7c15u
#define B4621_M1  0x165667b1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4621_mix32(uint32_t u32X)
{
	u32X ^= u32X >> 16;
	u32X *= B4621_M0;
	u32X ^= u32X >> 13;
	u32X *= B4621_M1;
	u32X ^= u32X >> 16;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_entropy_mix32_u - mix / diffuse entropy in a single 32-bit word.
 *
 * x: arbitrary input (all values accepted, including 0)
 *
 * Returns a well-mixed 32-bit word. Pure integer; no side effects.
 * No parent wires.
 */
uint32_t
gj_entropy_mix32_u(uint32_t u32X)
{
	(void)NULL;
	return b4621_mix32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_entropy_mix32_u(uint32_t u32X)
    __attribute__((alias("gj_entropy_mix32_u")));
