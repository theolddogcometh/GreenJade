/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch688: format uint64_t as binary ASCII.
 *
 * Surface (unique symbols):
 *   int gj_format_bin_u64(uint64_t v, char *out, size_t cap);
 *     — Write the base-2 representation of v into out as a
 *       NUL-terminated string (no "0b" prefix; no leading zeros
 *       except for v == 0). Minimal binary digits only.
 *       Examples:
 *         0          → "0"
 *         5          → "101"
 *         255        → "11111111"
 *         UINT64_MAX → 64 ones
 *       Returns 0 on success, or -1 on error (out == NULL, cap == 0,
 *       or result including NUL does not fit in cap). On error out is
 *       undefined.
 *   int __gj_format_bin_u64  (alias)
 *   __libcgj_batch688_marker = "libcgj-batch688"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch688_marker[] = "libcgj-batch688";

/* UINT64_MAX needs 64 binary digits; reverse buffer holds that. */
#define B688_MAX_DIGITS 64

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_bin_u64 — write binary digits of v into out.
 *
 * Digits are emitted LSD-first into a stack temp, then reversed into out
 * so the result is MSD-first with no leading zeros (except zero itself).
 * Alphabet is "01" only (never "0b" prefix).
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   cap too small for digits + NUL → -1
 * Success: 0 (OK). Failure: -1 (FAIL).
 */
int
gj_format_bin_u64(uint64_t v, char *out, size_t cap)
{
	char aDig[B688_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uVal;
	size_t iOut;
	size_t cbNeed;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	uVal = v;
	nDig = 0;

	if (uVal == 0ull) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0ull && nDig < B688_MAX_DIGITS) {
			aDig[nDig] = (char)('0' + (int)(uVal & 1ull));
			nDig++;
			uVal >>= 1;
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
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_format_bin_u64(uint64_t v, char *out, size_t cap)
    __attribute__((alias("gj_format_bin_u64")));
