/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch391: format uint64_t as a decimal ASCII string.
 *
 * Surface (unique symbols):
 *   int gj_format_u64_dec(uint64_t n, char *out, size_t cap);
 *     — Write the base-10 representation of n into out as a
 *       NUL-terminated string (no leading zeros except for n == 0).
 *       Examples:
 *         0          → "0"
 *         42         → "42"
 *         UINT64_MAX → "18446744073709551615"  (20 digits)
 *       Returns the number of characters written excluding NUL on
 *       success, or -1 on error (out == NULL, cap == 0, or result
 *       including NUL does not fit in cap). On error out is undefined.
 *   int __gj_format_u64_dec  (alias)
 *   __libcgj_batch391_marker = "libcgj-batch391"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch391_marker[] = "libcgj-batch391";

/* UINT64_MAX needs 20 decimal digits; reverse buffer holds that. */
#define B391_MAX_DIGITS 20

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_u64_dec — write decimal digits of n into out.
 *
 * Digits are emitted LSD-first into a stack temp, then reversed into out
 * so the result is MSD-first with no leading zeros (except zero itself).
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   cap too small for digits + NUL → -1
 * Success: length excluding NUL.
 */
int
gj_format_u64_dec(uint64_t n, char *out, size_t cap)
{
	char aDig[B391_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uVal;
	size_t iOut;
	size_t cbNeed;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	uVal = n;
	nDig = 0;

	if (uVal == 0ull) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0ull && nDig < B391_MAX_DIGITS) {
			aDig[nDig++] = (char)('0' + (int)(uVal % 10ull));
			uVal /= 10ull;
		}
	}

	/* digits + NUL */
	cbNeed = (size_t)nDig + 1u;
	if (cap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	for (iDig = nDig; iDig > 0; iDig--) {
		out[iOut++] = aDig[iDig - 1];
	}
	out[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_format_u64_dec(uint64_t n, char *out, size_t cap)
    __attribute__((alias("gj_format_u64_dec")));
