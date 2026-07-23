/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1351: Szudzik pairing function.
 *
 * Surface (unique symbols):
 *   uint64_t gj_szudzik_pair(uint32_t x, uint32_t y);
 *     — if x < y then y*y + x else x*x + x + y  (bijection N×N → N).
 *   uint64_t __gj_szudzik_pair  (alias)
 *   __libcgj_batch1351_marker = "libcgj-batch1351"
 *
 * Distinct from Cantor/elegant pairing. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1351_marker[] = "libcgj-batch1351";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_szudzik_pair — Szudzik elegant lattice pairing.
 *
 * More compact than Cantor for axis-aligned ranges. Pure; all inputs
 * accepted. Result fits in uint64 for full uint32 domain.
 */
uint64_t
gj_szudzik_pair(uint32_t u32X, uint32_t u32Y)
{
	uint64_t u64X;
	uint64_t u64Y;

	u64X = (uint64_t)u32X;
	u64Y = (uint64_t)u32Y;
	if (u64X < u64Y) {
		return u64Y * u64Y + u64X;
	}
	return u64X * u64X + u64X + u64Y;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_szudzik_pair(uint32_t u32X, uint32_t u32Y)
    __attribute__((alias("gj_szudzik_pair")));
