/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1642: parse base-N digit string → u64 (2..36).
 *
 * Surface (unique symbols):
 *   int gj_u64_from_base_digits(const char *s, size_t n, unsigned base,
 *                               uint64_t *out);
 *     — Parse n characters of s as an unsigned integer in base `base`.
 *       Digits: '0'..'9', 'a'..'z', 'A'..'Z' (case-insensitive). base in
 *       2..36. Empty input, bad digit, digit >= base, overflow, NULL s
 *       (when n > 0), or NULL out → -1. Success → 0 and *out set.
 *   int __gj_u64_from_base_digits  (alias)
 *   __libcgj_batch1642_marker = "libcgj-batch1642"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1642_marker[] = "libcgj-batch1642";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1642_base_ok(unsigned uBase)
{
	return (uBase >= 2u && uBase <= 36u) ? 1 : 0;
}

/*
 * Map one character to a digit value 0..35, or -1 if not a base-36 digit.
 */
static int
b1642_digit_val(int nCh)
{
	if (nCh >= '0' && nCh <= '9') {
		return nCh - '0';
	}
	if (nCh >= 'a' && nCh <= 'z') {
		return nCh - 'a' + 10;
	}
	if (nCh >= 'A' && nCh <= 'Z') {
		return nCh - 'A' + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_from_base_digits — parse base-N digit run into *out.
 *
 * Strict: every of the n octets must be a valid digit strictly less than
 * base. Overflow of uint64_t is rejected (no wrap).
 */
int
gj_u64_from_base_digits(const char *pS, size_t cN, unsigned uBase,
    uint64_t *pOut)
{
	uint64_t u64Acc;
	size_t i;

	if (pOut == NULL || !b1642_base_ok(uBase)) {
		return -1;
	}
	if (cN == 0u) {
		return -1;
	}
	if (pS == NULL) {
		return -1;
	}

	u64Acc = 0ull;
	for (i = 0u; i < cN; i++) {
		int nDig;
		uint64_t u64Next;

		nDig = b1642_digit_val((unsigned char)pS[i]);
		if (nDig < 0 || (unsigned)nDig >= uBase) {
			return -1;
		}
		/* overflow: acc * base + dig */
		if (u64Acc > UINT64_MAX / (uint64_t)uBase) {
			return -1;
		}
		u64Next = u64Acc * (uint64_t)uBase;
		if (u64Next > UINT64_MAX - (uint64_t)(unsigned)nDig) {
			return -1;
		}
		u64Acc = u64Next + (uint64_t)(unsigned)nDig;
	}

	*pOut = u64Acc;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_from_base_digits(const char *pS, size_t cN, unsigned uBase,
    uint64_t *pOut)
    __attribute__((alias("gj_u64_from_base_digits")));
