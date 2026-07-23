/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch244: fill binary-reflected Gray code sequence.
 *
 * Surface (unique symbols):
 *   int gj_gray_seq(unsigned nbits, uint32_t *out, size_t out_cap);
 *     — Fill out[0 .. 2^nbits - 1] with the nbits-bit binary-reflected
 *       Gray codes: out[i] = i ^ (i >> 1). nbits in 0..16; requires
 *       out non-NULL and out_cap >= (1u << nbits). Returns 0 on success,
 *       -1 on bad arguments.
 *   __gj_gray_seq  (alias)
 *   __libcgj_batch244_marker = "libcgj-batch244"
 *
 * Does not redefine gj_gray_encode / gj_gray_decode (batch182). Uses a
 * private b244_gray helper only. Clean-room freestanding pure C (integer
 * only). Compiles with -ffreestanding -msse2. No malloc, no errno. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch244_marker[] = "libcgj-batch244";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b244_gray — binary-reflected Gray code of index i.
 * Same map as gj_gray_encode (batch182); kept private so this TU does
 * not redefine that public symbol.
 */
static uint32_t
b244_gray(uint32_t i)
{
	return i ^ (i >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gray_seq — fill the full nbits-bit binary-reflected Gray sequence.
 *
 * out[i] = Gray(i) for i in [0, 2^nbits). Adjacent entries differ by
 * exactly one bit. nbits==0 writes a single zero at out[0].
 *
 * Returns 0 on success. Returns -1 if nbits > 16, out is NULL, or
 * out_cap is smaller than 1<<nbits.
 */
int
gj_gray_seq(unsigned nbits, uint32_t *out, size_t out_cap)
{
	size_t n;
	size_t i;

	if (nbits > 16u || out == NULL) {
		return -1;
	}

	n = (size_t)1u << nbits;
	if (out_cap < n) {
		return -1;
	}

	for (i = 0u; i < n; i++) {
		out[i] = b244_gray((uint32_t)i);
	}
	return 0;
}

int __gj_gray_seq(unsigned nbits, uint32_t *out, size_t out_cap)
    __attribute__((alias("gj_gray_seq")));
