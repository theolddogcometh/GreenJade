/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch595: 64-bit population parity (unsigned).
 *
 * Surface (unique symbols; distinct from gj_parity64 / __gj_parity64
 * in graph_batch419.c — no multi-def):
 *   unsigned gj_parity_u64(uint64_t x);
 *     — Return 0 if x has even popcount, 1 if odd (including x == 0 → 0).
 *       XOR-fold pure integer path; no builtins.
 *   unsigned __gj_parity_u64  (alias)
 *   __libcgj_batch595_marker = "libcgj-batch595"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch595_marker[] = "libcgj-batch595";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parity_u64 — parity of set bits in a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns 0 for even popcount (including x == 0), 1 for odd.
 * Successive XOR-folds collapse to the LSB.
 */
unsigned
gj_parity_u64(uint64_t x)
{
	uint64_t v;

	v = x;
	v ^= v >> 32;
	v ^= v >> 16;
	v ^= v >> 8;
	v ^= v >> 4;
	v ^= v >> 2;
	v ^= v >> 1;
	return (unsigned)(v & 1ull);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_parity_u64(uint64_t x)
    __attribute__((alias("gj_parity_u64")));
