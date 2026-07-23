/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1443: rrmxmx 64-bit mixer (Pelle Evensen).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rrmxmx_u64(uint64_t x);
 *     — rrmxmx avalanche of a single u64: rotate-xor / multiply /
 *       xor-shift / multiply / xor-shift. Pure; no buffer walk.
 *   uint64_t __gj_rrmxmx_u64  (alias)
 *   __libcgj_batch1443_marker = "libcgj-batch1443"
 *
 * Distinct from file-static XXH3 rrmxmx helpers (batch41/batch98, not
 * exported), gj_hash_mix64, and gj_hash_finalizer64. Unique surface
 * only; no multi-def. Avoids splitmix/xorshift/pcg names.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public rrmxmx
 * mixer shape.
 */

#include <stdint.h>

const char __libcgj_batch1443_marker[] = "libcgj-batch1443";

/* rrmxmx public odd multiply constant. */
#define B1443_M 0x9fb21c651e98df25ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1443_rotl64(uint64_t u64X, unsigned uBits)
{
	return (u64X << uBits) | (u64X >> (64u - uBits));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rrmxmx_u64 — rrmxmx mixer of a single 64-bit word.
 *
 * u64X: arbitrary input (all values accepted, including 0).
 *
 * Public shape:
 *   x ^= rotl(x, 49) ^ rotl(x, 24);
 *   x *= M; x ^= x >> 28;
 *   x *= M; x ^= x >> 28;
 */
uint64_t
gj_rrmxmx_u64(uint64_t u64X)
{
	uint64_t u64V = u64X;

	u64V ^= b1443_rotl64(u64V, 49u) ^ b1443_rotl64(u64V, 24u);
	u64V *= B1443_M;
	u64V ^= u64V >> 28;
	u64V *= B1443_M;
	u64V ^= u64V >> 28;
	return u64V;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rrmxmx_u64(uint64_t u64X)
    __attribute__((alias("gj_rrmxmx_u64")));
