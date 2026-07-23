/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4625: rotate-then-mix entropy step (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_entropy_rot_mix_u(uint32_t x, uint32_t r);
 *     - Left-rotate x by (r & 31), then apply a short odd-multiply
 *       avalanche. Useful for position-dependent entropy folds.
 *   uint32_t __gj_entropy_rot_mix_u  (alias)
 *   __libcgj_batch4625_marker = "libcgj-batch4625"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630). Unique
 * gj_entropy_rot_mix_u surface only; no multi-def. Distinct from
 * gj_entropy_mix32_u (batch4621) — includes rotate by r. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4625_marker[] = "libcgj-batch4625";

/* Odd mix multipliers unique to this TU. */
#define B4625_M0  0xac4c1b51u
#define B4625_M1  0x27d4eb2fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4625_rotl32(uint32_t u32X, unsigned uN)
{
	uN &= 31u;
	if (uN == 0u) {
		return u32X;
	}
	return (u32X << uN) | (u32X >> (32u - uN));
}

static uint32_t
b4625_rot_mix(uint32_t u32X, uint32_t u32R)
{
	u32X = b4625_rotl32(u32X, (unsigned)u32R);
	u32X ^= u32X >> 15;
	u32X *= B4625_M0;
	u32X ^= u32X >> 13;
	u32X *= B4625_M1;
	u32X ^= u32X >> 16;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_entropy_rot_mix_u - rotate x by r then mix for entropy diffusion.
 *
 * x: value to rotate and mix
 * r: rotation amount (taken mod 32)
 *
 * Returns a well-mixed 32-bit word. Pure integer; no side effects.
 * No parent wires.
 */
uint32_t
gj_entropy_rot_mix_u(uint32_t u32X, uint32_t u32R)
{
	(void)NULL;
	return b4625_rot_mix(u32X, u32R);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_entropy_rot_mix_u(uint32_t u32X, uint32_t u32R)
    __attribute__((alias("gj_entropy_rot_mix_u")));
