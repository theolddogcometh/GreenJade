/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4031: freestanding uint64_t bit scramble (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_scramble_u(uint64_t x);
 *     - Non-cryptographic single-word scramble / avalanche of x:
 *       xorshift / odd-multiply / xorshift / odd-multiply / xorshift.
 *       Stateless pure function; all inputs accepted (including 0).
 *   uint64_t __gj_u64_scramble_u  (alias)
 *   __libcgj_batch4031_marker = "libcgj-batch4031"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040). Unique gj_u64_scramble_u
 * surface only; no multi-def. Distinct from gj_murmur_fmix64 (batch1442),
 * gj_hash_mix64_u (batch2052), gj_wyhash64_mix (batch1441), gj_splitmix64
 * (batch424), and gj_hash_avalanche64 (batch2056). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4031_marker[] = "libcgj-batch4031";

/* Odd mix multipliers unique to this TU (not murmur/splitmix/moremur). */
#define B4031_M0  0xd6e8feb86659fd93ULL
#define B4031_M1  0xa5cb529ac4b5d1b3ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* Xorshift + multiply avalanche of a single u64. */
static uint64_t
b4031_scramble(uint64_t u64X)
{
	u64X ^= u64X >> 32;
	u64X *= B4031_M0;
	u64X ^= u64X >> 29;
	u64X *= B4031_M1;
	u64X ^= u64X >> 32;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_scramble_u - scramble / avalanche a single 64-bit word.
 *
 * x: arbitrary input (all values accepted, including 0)
 *
 * Returns a well-mixed 64-bit word. Pure integer; no libc. No parent wires.
 */
uint64_t
gj_u64_scramble_u(uint64_t u64X)
{
	(void)NULL;
	return b4031_scramble(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_scramble_u(uint64_t u64X)
    __attribute__((alias("gj_u64_scramble_u")));
