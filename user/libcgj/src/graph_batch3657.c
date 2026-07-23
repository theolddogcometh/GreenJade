/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3657: gj_city_mix_u - CityHash Hash128to64 mix.
 *
 * Surface (unique symbols):
 *   uint64_t gj_city_mix_u(uint64_t x);
 *     - CityHash / FarmHash Hash128to64-style single-word mix: shiftmix,
 *       multiply by public KM 0x9ddfea08eb382d69, shiftmix again. Pure;
 *       no buffer walk.
 *   uint64_t __gj_city_mix_u  (alias)
 *   __libcgj_batch3657_marker = "libcgj-batch3657"
 *
 * Hash exclusive wave (3651-3660). Distinct from gj_cityhash64 (batch267
 * buffer hash) and cityhash64 (batch61) — unique single-word city mix
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3657_marker[] = "libcgj-batch3657";

/* CityHash Hash128to64 multiply constant (public algorithm). */
#define B3657_KM 0x9ddfea08eb382d69ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3657_shiftmix(uint64_t u64V)
{
	return u64V ^ (u64V >> 47);
}

static uint64_t
b3657_city_mix(uint64_t u64X)
{
	uint64_t u64H = u64X;

	u64H = b3657_shiftmix(u64H) * B3657_KM;
	u64H = b3657_shiftmix(u64H);
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_city_mix_u - CityHash-style 64-bit finalizer mix of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns the mixed 64-bit output. Stateless pure function.
 */
uint64_t
gj_city_mix_u(uint64_t u64X)
{
	(void)NULL;
	return b3657_city_mix(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_city_mix_u(uint64_t u64X)
    __attribute__((alias("gj_city_mix_u")));
