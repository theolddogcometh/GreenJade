/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch207: IEEE CRC-32 combine (GF(2)).
 *
 * Given crc1 = CRC-32 of stream A and crc2 = CRC-32 of stream B of
 * length len2, return CRC-32 of the concatenation A||B without re-reading
 * either stream. Uses the standard GF(2) linear-operator / matrix-power
 * construction for "append len2 zero bytes" then XOR with crc2.
 *
 * Polynomial: IEEE / zlib / PNG reflected CRC-32 (0xEDB88320), matching
 * batch39 crc32 / crc32_z and batch111 gj_png_crc32 chaining. Input CRCs
 * are the external (final-XOR'd) values those APIs return.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_combine(uint32_t crc1, uint32_t crc2, size_t len2);
 *     — CRC(A||B) from CRC(A), CRC(B), |B|. len2==0 → crc1.
 *   __gj_crc32_combine  (alias)
 *   __libcgj_batch207_marker = "libcgj-batch207"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No third-party source copied. Does not redefine
 * crc32 / crc32_z / crc32c / crc32_pclmul / gj_png_crc32.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch207_marker[] = "libcgj-batch207";

/* IEEE CRC-32 reflected polynomial (same as batch39 / zlib). */
#define B207_POLY ((uint32_t)0xedb88320u)

/* ---- GF(2) 32×32 matrix helpers (column vectors as uint32_t) ---------- */

/*
 * Multiply matrix by vector over GF(2). mat[i] is the image of basis bit i
 * (bit 0 = LSB). Returns mat * vec.
 */
static uint32_t
b207_gf2_times(const uint32_t *mat, uint32_t vec)
{
	uint32_t sum = 0u;

	while (vec != 0u) {
		if ((vec & 1u) != 0u) {
			sum ^= *mat;
		}
		vec >>= 1;
		mat++;
	}
	return sum;
}

/* square = mat * mat over GF(2). */
static void
b207_gf2_square(uint32_t *square, const uint32_t *mat)
{
	unsigned n;

	for (n = 0u; n < 32u; n++) {
		square[n] = b207_gf2_times(mat, mat[n]);
	}
}

/* ---- gj_crc32_combine -------------------------------------------------- */

/*
 * Combine two IEEE CRC-32 values:
 *   crc1 = CRC-32(A), crc2 = CRC-32(B), len2 = |B|
 * returns CRC-32(A || B).
 *
 * Algorithm: build the GF(2) operator that advances a CRC by one zero bit
 * under the reflected poly, square it up to byte granularity, then apply
 * successive even/odd powers of two zero-bytes according to the bits of
 * len2. Finally XOR the advanced crc1 with crc2.
 *
 * Degenerate: len2 == 0 returns crc1 (B empty).
 */
uint32_t
gj_crc32_combine(uint32_t crc1, uint32_t crc2, size_t len2)
{
	uint32_t even[32];
	uint32_t odd[32];
	uint32_t row;
	unsigned n;

	if (len2 == 0u) {
		return crc1;
	}

	/*
	 * odd[] = operator for one zero bit: x → (x >> 1) XOR (poly if LSB).
	 * Column 0 is the poly (image of bit 0 after shift+reduce); column
	 * k for k>0 is bit (k-1) (image of bit k after pure shift).
	 */
	odd[0] = B207_POLY;
	row = 1u;
	for (n = 1u; n < 32u; n++) {
		odd[n] = row;
		row <<= 1;
	}

	/* even[] = operator for two zero bits. */
	b207_gf2_square(even, odd);

	/* odd[] = operator for four zero bits. */
	b207_gf2_square(odd, even);

	/*
	 * Walk bits of len2. First square puts the one-zero-byte (8-bit)
	 * operator into even; subsequent squares double the zero-byte count
	 * each step, alternating even/odd workspaces.
	 */
	do {
		b207_gf2_square(even, odd);
		if ((len2 & 1u) != 0u) {
			crc1 = b207_gf2_times(even, crc1);
		}
		len2 >>= 1;
		if (len2 == 0u) {
			break;
		}

		b207_gf2_square(odd, even);
		if ((len2 & 1u) != 0u) {
			crc1 = b207_gf2_times(odd, crc1);
		}
		len2 >>= 1;
	} while (len2 != 0u);

	return crc1 ^ crc2;
}

uint32_t __gj_crc32_combine(uint32_t crc1, uint32_t crc2, size_t len2)
    __attribute__((alias("gj_crc32_combine")));
