/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4624: combine two 64-bit entropy words (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_entropy_combine64_u(uint64_t a, uint64_t b);
 *     - Fold b into a with an entropy-oriented combine:
 *       t = a ^ (b + GOLD + (a << 6) + (a >> 2)); then a short mix of t.
 *       Pure; no mutable state.
 *   uint64_t __gj_entropy_combine64_u  (alias)
 *   __libcgj_batch4624_marker = "libcgj-batch4624"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630). Unique
 * gj_entropy_combine64_u surface only; no multi-def. Distinct from
 * gj_hash_combine_u64 (batch713) — extra post-fold mix and unique
 * multiplier. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4624_marker[] = "libcgj-batch4624";

/* 64-bit golden-ratio constant and post-fold mix multiplier. */
#define B4624_GOLD  0x9e3779b97f4a7c15ULL
#define B4624_M0    0xe7037ed1a0b428dbULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4624_combine64(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64T;

	u64T = u64A ^ (u64B + B4624_GOLD + (u64A << 6) + (u64A >> 2));
	u64T ^= u64T >> 30;
	u64T *= B4624_M0;
	u64T ^= u64T >> 27;
	return u64T;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_entropy_combine64_u - fold two 64-bit words into one mixed result.
 *
 * a: running combined entropy / seed
 * b: value (or pre-mixed field) to fold in
 *
 * Returns the combined 64-bit entropy word. Stateless pure function.
 * No parent wires.
 */
uint64_t
gj_entropy_combine64_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4624_combine64(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_entropy_combine64_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_entropy_combine64_u")));
