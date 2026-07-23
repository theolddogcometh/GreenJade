/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch586: format uint64_t as lowercase hex ASCII.
 *
 * Surface (unique symbols):
 *   int gj_format_hex_u64(uint64_t v, char *out, size_t cap);
 *     — Write the base-16 representation of v into out as a
 *       NUL-terminated lowercase string (no "0x" prefix; no leading
 *       zeros except for v == 0).
 *       Examples:
 *         0          → "0"
 *         255        → "ff"
 *         UINT64_MAX → "ffffffffffffffff"  (16 digits)
 *       Returns 0 on success, or -1 on error (out == NULL, cap == 0,
 *       or result including NUL does not fit in cap). On error out is
 *       undefined.
 *   int __gj_format_hex_u64  (alias)
 *   __libcgj_batch586_marker = "libcgj-batch586"
 *
 * Distinct from gj_format_u64_hex (graph_batch393), which takes an
 * upper/lower selector and returns written length. This TU is the
 * lowercase-only OK/FAIL surface paired with gj_parse_hex_u64
 * (graph_batch585).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch586_marker[] = "libcgj-batch586";

/* UINT64_MAX needs 16 hex digits; reverse buffer holds that. */
#define B586_MAX_DIGITS 16

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_hex_u64 — write lowercase hex digits of v into out.
 *
 * Digits are emitted LSD-first into a stack temp, then reversed into out
 * so the result is MSD-first with no leading zeros (except zero itself).
 * Alphabet is "0123456789abcdef" only (never uppercase, never "0x").
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   cap too small for digits + NUL → -1
 * Success: 0 (OK). Failure: -1 (FAIL).
 */
int
gj_format_hex_u64(uint64_t v, char *out, size_t cap)
{
	char aDig[B586_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uVal;
	size_t iOut;
	size_t cbNeed;
	unsigned uNibble;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	uVal = v;
	nDig = 0;

	if (uVal == 0ull) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0ull && nDig < B586_MAX_DIGITS) {
			uNibble = (unsigned)(uVal & 0xfull);
			if (uNibble < 10u) {
				aDig[nDig] = (char)('0' + (int)uNibble);
			} else {
				aDig[nDig] = (char)('a' + (int)(uNibble - 10u));
			}
			nDig++;
			uVal >>= 4;
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

int __gj_format_hex_u64(uint64_t v, char *out, size_t cap)
    __attribute__((alias("gj_format_hex_u64")));
