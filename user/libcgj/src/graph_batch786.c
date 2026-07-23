/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch786: population count of a uint16_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_popcnt_u16(uint16_t x);
 *     — Number of 1-bits in x (0 .. 16). x == 0 yields 0.
 *   unsigned __gj_popcnt_u16  (alias)
 *   __libcgj_batch786_marker = "libcgj-batch786"
 *
 * Distinct from gj_popcnt64 (batch368) — unique u16 surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch786_marker[] = "libcgj-batch786";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_popcnt_u16 — count set bits in a 16-bit word (SWAR parallel sum).
 */
unsigned
gj_popcnt_u16(uint16_t u16X)
{
	uint32_t u32V;

	u32V = (uint32_t)u16X;
	u32V = u32V - ((u32V >> 1) & 0x5555u);
	u32V = (u32V & 0x3333u) + ((u32V >> 2) & 0x3333u);
	u32V = (u32V + (u32V >> 4)) & 0x0f0fu;
	u32V = (u32V + (u32V >> 8)) & 0x001fu;
	return (unsigned)u32V;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_popcnt_u16(uint16_t u16X)
    __attribute__((alias("gj_popcnt_u16")));
