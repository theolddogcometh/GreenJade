/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch537: bit-level Hamming distance over u8 buffers.
 *
 * Surface (unique symbols):
 *   size_t gj_hamming_u8(const unsigned char *a, const unsigned char *b,
 *                        size_t n);
 *     — sum of popcount(a[i] XOR b[i]) for i in [0, n). Returns 0 if
 *       a or b is NULL or n == 0. Distinct from gj_hamming_distance
 *       (batch184, int return) and gj_hamming_str (batch213).
 *   size_t __gj_hamming_u8  (alias)
 *   __libcgj_batch537_marker = "libcgj-batch537"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls, no __builtin_popcount (SWAR scalar). No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch537_marker[] = "libcgj-batch537";

/* ---- SWAR scalar popcount ---------------------------------------------- */

/*
 * Hamming weight of one byte via parallel bit sums. No multiplies;
 * safe for freestanding -fno-builtin.
 */
static unsigned
b537_popcount8(unsigned char x)
{
	unsigned u = (unsigned)x;

	u = u - ((u >> 1) & 0x55u);
	u = (u & 0x33u) + ((u >> 2) & 0x33u);
	u = (u + (u >> 4)) & 0x0fu;
	return u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hamming_u8 — bit-level Hamming distance between two length-n u8 buffers.
 *
 * a, b: source buffers of at least n bytes (may be NULL).
 * n:    number of bytes to compare.
 *
 * Returns the total number of differing bits across n bytes
 * (sum of popcount(a[i] ^ b[i])). Returns 0 if a is NULL, b is NULL,
 * or n is 0.
 */
size_t
gj_hamming_u8(const unsigned char *a, const unsigned char *b, size_t n)
{
	size_t i;
	size_t total;

	if (a == NULL || b == NULL || n == 0u) {
		return 0u;
	}

	total = 0u;
	for (i = 0u; i < n; i++) {
		total += (size_t)b537_popcount8((unsigned char)(a[i] ^ b[i]));
	}
	return total;
}

size_t __gj_hamming_u8(const unsigned char *a, const unsigned char *b, size_t n)
    __attribute__((alias("gj_hamming_u8")));
