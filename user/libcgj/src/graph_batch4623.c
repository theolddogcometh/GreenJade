/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4623: combine two 32-bit entropy words (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_entropy_combine32_u(uint32_t a, uint32_t b);
 *     - Fold b into a with an entropy-oriented combine:
 *       t = a + b + GOLD + (a << 6) + (a >> 2); then a short mix of t.
 *       Pure; no mutable state.
 *   uint32_t __gj_entropy_combine32_u  (alias)
 *   __libcgj_batch4623_marker = "libcgj-batch4623"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630). Unique
 * gj_entropy_combine32_u surface only; no multi-def. Distinct from
 * gj_hash_combine_u64 (batch713) — 32-bit width and extra mix step.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4623_marker[] = "libcgj-batch4623";

/* 32-bit golden-ratio constant and post-fold mix multiplier. */
#define B4623_GOLD  0x9e3779b9u
#define B4623_M0    0xed5ad4bbu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4623_combine32(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32T;

	u32T = u32A ^ (u32B + B4623_GOLD + (u32A << 6) + (u32A >> 2));
	u32T ^= u32T >> 16;
	u32T *= B4623_M0;
	u32T ^= u32T >> 15;
	return u32T;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_entropy_combine32_u - fold two 32-bit words into one mixed result.
 *
 * a: running combined entropy / seed
 * b: value (or pre-mixed field) to fold in
 *
 * Returns the combined 32-bit entropy word. Stateless pure function.
 * No parent wires.
 */
uint32_t
gj_entropy_combine32_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4623_combine32(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_entropy_combine32_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_entropy_combine32_u")));
