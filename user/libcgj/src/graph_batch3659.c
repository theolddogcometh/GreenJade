/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3659: gj_seahash_mix_u - SeaHash diffuse mix.
 *
 * Surface (unique symbols):
 *   uint64_t gj_seahash_mix_u(uint64_t x);
 *     - SeaHash-style diffuse of a single u64: multiply by public
 *       0x6eed0e9da4d94a4f, xor with (high >> (x>>60)), multiply again.
 *       Pure; no buffer walk.
 *   uint64_t __gj_seahash_mix_u  (alias)
 *   __libcgj_batch3659_marker = "libcgj-batch3659"
 *
 * Hash exclusive wave (3651-3660). Unique SeaHash diffuse surface only;
 * no multi-def with prior city/wyhash/murmur mixers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3659_marker[] = "libcgj-batch3659";

/* SeaHash public diffuse multiply constant. */
#define B3659_K 0x6eed0e9da4d94a4fULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3659_diffuse(uint64_t u64X)
{
	uint64_t u64A;
	unsigned uShift;

	u64X *= B3659_K;
	u64A = u64X >> 32;
	uShift = (unsigned)(u64X >> 60);
	u64X ^= u64A >> uShift;
	u64X *= B3659_K;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seahash_mix_u - SeaHash-style diffuse mix of a single 64-bit word.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns the diffused 64-bit output. Stateless pure function.
 */
uint64_t
gj_seahash_mix_u(uint64_t u64X)
{
	(void)NULL;
	return b3659_diffuse(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_seahash_mix_u(uint64_t u64X)
    __attribute__((alias("gj_seahash_mix_u")));
