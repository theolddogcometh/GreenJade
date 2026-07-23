/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2486: min-width zero-padded u64 decimal format.
 *
 * Surface (unique symbols):
 *   int gj_format_u64_dec_min(uint64_t n, char *out, size_t cap,
 *                             size_t min_width);
 *     — Write the base-10 representation of n into out as a
 *       NUL-terminated string, left-padded with '0' to at least
 *       min_width characters. No leading zeros beyond the pad (except
 *       n == 0, which is "0" when min_width <= 1). Returns characters
 *       written excluding NUL on success, or -1 on error.
 *   int __gj_format_u64_dec_min  (alias)
 *   __libcgj_batch2486_marker = "libcgj-batch2486"
 *
 * Distinct from gj_format_u64_dec (batch391), gj_format_u64_dec_w
 * (batch1536, space pad), and gj_str_from_u64_fixed (batch1071).
 * Post-2480 parse exclusive wave (2481-2490).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2486_marker[] = "libcgj-batch2486";

/* UINT64_MAX needs 20 decimal digits; reverse buffer holds that. */
#define B2486_MAX_DIGITS 20

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_u64_dec_min — decimal with minimum zero-padded field width.
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   min_width 0 → natural digits only
 *   natural digits longer than min_width → no pad, full digits
 *   result (max(digits, min_width) + NUL) does not fit → -1
 * Success: length excluding NUL.
 *
 * Examples:
 *   42, min_width=5 → "00042"
 *   42, min_width=1 → "42"
 *   0,  min_width=3 → "000"
 */
int
gj_format_u64_dec_min(uint64_t u64N, char *pOut, size_t cbCap, size_t cMin)
{
	char aDig[B2486_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uVal;
	size_t cbField;
	size_t cbNeed;
	size_t iOut;
	size_t iPad;

	(void)NULL;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	uVal = u64N;
	nDig = 0;
	if (uVal == 0ull) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0ull && nDig < B2486_MAX_DIGITS) {
			aDig[nDig++] = (char)('0' + (int)(uVal % 10ull));
			uVal /= 10ull;
		}
	}

	cbField = (size_t)nDig;
	if (cMin > cbField) {
		cbField = cMin;
	}
	cbNeed = cbField + 1u;
	if (cbCap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	if (cMin > (size_t)nDig) {
		iPad = cMin - (size_t)nDig;
		while (iPad > 0u) {
			pOut[iOut++] = '0';
			iPad--;
		}
	}
	for (iDig = nDig; iDig > 0; iDig--) {
		pOut[iOut++] = aDig[iDig - 1];
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_format_u64_dec_min(uint64_t u64N, char *pOut, size_t cbCap,
    size_t cMin)
    __attribute__((alias("gj_format_u64_dec_min")));
