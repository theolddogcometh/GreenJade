/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch461: freestanding Bitcoin Base58 encode.
 *
 * Surface (unique symbols):
 *   int gj_b58_encode(const uint8_t *in, size_t nin, char *out,
 *                     size_t out_cap);
 *     — Encode in[0..nin) as Bitcoin Base58 (alphabet without 0/O/I/l)
 *       into out as a NUL-terminated string. Leading zero octets become
 *       leading '1' characters. Returns the number of characters written
 *       excluding NUL on success, or -1 on error (NULL args when nin > 0,
 *       out == NULL / out_cap == 0, input too large for the stack work
 *       buffer, or result including NUL does not fit in out_cap). On
 *       error out is undefined. No errno (freestanding).
 *   int __gj_b58_encode  (alias)
 *   __libcgj_batch461_marker = "libcgj-batch461"
 *
 * Distinct from batch42 base58_encode (ssize_t / errno-era libc graph).
 * Unique gj_ name only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch461_marker[] = "libcgj-batch461";

/* Bitcoin Base58 alphabet (no 0, O, I, l). */
static const char s_b461_alpha[] =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

/*
 * Work buffer for the base-256 → base-58 digit expansion.
 * log(256)/log(58) ≈ 1.365 → size = nin * 138 / 100 + 1.
 * 512 bytes admits nin up to ~370.
 */
#define B461_WORK_MAX 512u
#define B461_MAX_IN   370u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b58_encode — Bitcoin Base58 encode of a binary buffer.
 *
 * in:      input octets (NULL allowed only when nin == 0)
 * nin:     input length in octets
 * out:     destination for NUL-terminated Base58 text
 * out_cap: capacity of out in bytes (must hold result + NUL)
 *
 * Edge cases:
 *   nin == 0 with out non-NULL and out_cap >= 1 → write "" and return 0
 *   all-zero input of length n → n times '1'
 *   leading zero octets preserved as leading '1'
 *   out_cap too small / work size overflow → -1
 *
 * Examples (Bitcoin alphabet):
 *   {0x00}           → "1"
 *   {0x00,0x00}      → "11"
 *   "Hello"          → "9Ajdvzr"
 */
int
gj_b58_encode(const uint8_t *in, size_t nin, char *out, size_t out_cap)
{
	uint8_t aWork[B461_WORK_MAX];
	size_t cZeros;
	size_t cbSize;
	size_t cbHigh;
	size_t cbLen;
	size_t i;
	size_t iOut;
	char *pDst;

	if (out == NULL || out_cap == 0u) {
		return -1;
	}
	if (in == NULL && nin != 0u) {
		return -1;
	}
	if (nin > B461_MAX_IN) {
		return -1;
	}

	/* Count leading zero octets (map to leading '1'). */
	cZeros = 0u;
	while (cZeros < nin && in[cZeros] == 0u) {
		cZeros++;
	}

	/* log(256)/log(58) ≈ 1.37 → size = nin*138/100 + 1 */
	cbSize = nin * 138u / 100u + 1u;
	if (cbSize > B461_WORK_MAX) {
		return -1;
	}

	for (i = 0u; i < cbSize; i++) {
		aWork[i] = 0u;
	}

	/* Big-endian base-256 → base-58 digits in aWork (MSD at low index). */
	for (i = cZeros; i < nin; i++) {
		unsigned uCarry = (unsigned)in[i];
		size_t j;

		for (j = cbSize; j > 0u; j--) {
			uCarry += 256u * (unsigned)aWork[j - 1u];
			aWork[j - 1u] = (uint8_t)(uCarry % 58u);
			uCarry /= 58u;
		}
	}

	cbHigh = 0u;
	while (cbHigh < cbSize && aWork[cbHigh] == 0u) {
		cbHigh++;
	}

	cbLen = cZeros + (cbSize - cbHigh);
	if (cbLen + 1u > out_cap) {
		return -1;
	}
	/* Result length must fit in int (work max keeps this true). */
	if (cbLen > (size_t)0x7fffffff) {
		return -1;
	}

	pDst = out;
	iOut = 0u;
	for (i = 0u; i < cZeros; i++) {
		pDst[iOut++] = '1';
	}
	for (i = cbHigh; i < cbSize; i++) {
		pDst[iOut++] = s_b461_alpha[aWork[i]];
	}
	pDst[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_b58_encode(const uint8_t *in, size_t nin, char *out, size_t out_cap)
    __attribute__((alias("gj_b58_encode")));
