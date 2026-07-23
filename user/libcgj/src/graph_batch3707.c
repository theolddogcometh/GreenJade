/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3707: parse decimal digit run → uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_u32_from_dec_digits_u(const char *s, size_t n, uint32_t *out);
 *     - Parse exactly n characters of s as an unsigned base-10 integer.
 *       Digits: '0'..'9' only. Empty input (n==0), bad digit, overflow
 *       past UINT32_MAX, NULL s (when n > 0), or NULL out → -1.
 *       Success → 0 and *out set. All n octets must be valid digits
 *       (strict; no trailing junk allowed).
 *   int __gj_u32_from_dec_digits_u  (alias)
 *   __libcgj_batch3707_marker = "libcgj-batch3707"
 *
 * Exclusive continuum CREATE-ONLY (3701-3710). Distinct from
 * gj_parse_u32_dec_n (batch2142 prefix parse) and
 * gj_u64_from_base_digits (batch1642) — unique from_dec_digits_u
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3707_marker[] = "libcgj-batch3707";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Strict decimal digit run → u32. Every of the n octets must be '0'..'9'.
 * Overflow of uint32_t is rejected (no wrap).
 */
static int
b3707_from_dec(const char *pS, size_t cN, uint32_t *pOut)
{
	uint32_t uAcc;
	size_t i;

	if (pOut == NULL) {
		return -1;
	}
	if (cN == 0u) {
		return -1;
	}
	if (pS == NULL) {
		return -1;
	}

	uAcc = 0u;
	for (i = 0u; i < cN; i++) {
		unsigned char u8C;
		uint32_t uDig;

		u8C = (unsigned char)pS[i];
		if (u8C < (unsigned char)'0' || u8C > (unsigned char)'9') {
			return -1;
		}
		uDig = (uint32_t)(u8C - (unsigned char)'0');
		if (uAcc > (UINT32_MAX - uDig) / 10u) {
			return -1;
		}
		uAcc = uAcc * 10u + uDig;
	}

	*pOut = uAcc;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_from_dec_digits_u - parse strict decimal digit run into *out.
 *
 * s: digit buffer (may be non-NUL-terminated); ignored when n == 0
 * n: exact character count to consume
 * out: result pointer
 *
 * Returns 0 on success, -1 on error. No parent wires.
 */
int
gj_u32_from_dec_digits_u(const char *s, size_t n, uint32_t *out)
{
	(void)NULL;
	return b3707_from_dec(s, n, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_from_dec_digits_u(const char *s, size_t n, uint32_t *out)
    __attribute__((alias("gj_u32_from_dec_digits_u")));
