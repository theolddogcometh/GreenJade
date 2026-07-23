/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch561: population count (Hamming weight) of a
 * uint32_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_popcnt_u32(uint32_t x);
 *     — Number of 1-bits in x (0 .. 32). x == 0 yields 0.
 *   unsigned __gj_popcnt_u32  (alias)
 *   __libcgj_batch561_marker = "libcgj-batch561"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 *
 * Note: gj_popcnt64 / __gj_popcnt64 live in graph_batch368.c — not
 * redefined here (no multi-def).
 */

#include <stdint.h>

const char __libcgj_batch561_marker[] = "libcgj-batch561";

/*
 * gj_popcnt_u32 — count set bits in a 32-bit word (SWAR parallel sum).
 *
 * x: value to inspect
 *
 * Classic mask/add tree: 2-bit → 4-bit → 8-bit groups, then multiply-
 * fold by 0x01010101 and take the high byte.
 */
unsigned
gj_popcnt_u32(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X;
	u32V = u32V - ((u32V >> 1) & 0x55555555u);
	u32V = (u32V & 0x33333333u) + ((u32V >> 2) & 0x33333333u);
	u32V = (u32V + (u32V >> 4)) & 0x0f0f0f0fu;
	u32V = (u32V * 0x01010101u) >> 24;

	return (unsigned)u32V;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_popcnt_u32(uint32_t u32X)
    __attribute__((alias("gj_popcnt_u32")));
