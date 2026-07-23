/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4622: 64-bit entropy mix (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_entropy_mix64_u(uint64_t x);
 *     - Non-cryptographic single-word entropy mixer for x:
 *       xorshift / odd-multiply / xorshift / odd-multiply / xorshift.
 *       Spreads sparse input entropy across all 64 bits.
 *   uint64_t __gj_entropy_mix64_u  (alias)
 *   __libcgj_batch4622_marker = "libcgj-batch4622"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630). Unique gj_entropy_mix64_u
 * surface only; no multi-def. Distinct from gj_u64_mix_final_u
 * (batch4026), gj_u64_avalanche_u (batch4028), gj_u64_scramble_u
 * (batch4031), gj_hash_mix64 (batch967) — different constants.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4622_marker[] = "libcgj-batch4622";

/* Odd mix multipliers unique to this TU (not murmur/splitmix/moremur). */
#define B4622_M0  0x7f4a7c15f39cc061ULL
#define B4622_M1  0x9e3779b97f4a7c55ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4622_mix64(uint64_t u64X)
{
	u64X ^= u64X >> 32;
	u64X *= B4622_M0;
	u64X ^= u64X >> 29;
	u64X *= B4622_M1;
	u64X ^= u64X >> 32;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_entropy_mix64_u - mix / diffuse entropy in a single 64-bit word.
 *
 * x: arbitrary input (all values accepted, including 0)
 *
 * Returns a well-mixed 64-bit word. Pure integer; no side effects.
 * No parent wires.
 */
uint64_t
gj_entropy_mix64_u(uint64_t u64X)
{
	(void)NULL;
	return b4622_mix64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_entropy_mix64_u(uint64_t u64X)
    __attribute__((alias("gj_entropy_mix64_u")));
