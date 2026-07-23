/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3708: parse decimal digit run → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_u64_from_dec_digits_u(const char *s, size_t n, uint64_t *out);
 *     - Parse exactly n characters of s as an unsigned base-10 integer.
 *       Digits: '0'..'9' only. Empty input (n==0), bad digit, overflow
 *       past UINT64_MAX, NULL s (when n > 0), or NULL out → -1.
 *       Success → 0 and *out set. All n octets must be valid digits
 *       (strict; no trailing junk allowed).
 *   int __gj_u64_from_dec_digits_u  (alias)
 *   __libcgj_batch3708_marker = "libcgj-batch3708"
 *
 * Exclusive continuum CREATE-ONLY (3701-3710). Distinct from
 * gj_u32_from_dec_digits_u (batch3707), gj_parse_u64_dec_n, and
 * gj_u64_from_base_digits (batch1642) — unique from_dec_digits_u
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3708_marker[] = "libcgj-batch3708";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Strict decimal digit run → u64. Every of the n octets must be '0'..'9'.
 * Overflow of uint64_t is rejected (no wrap).
 */
static int
b3708_from_dec(const char *pS, size_t cN, uint64_t *pOut)
{
	uint64_t u64Acc;
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

	u64Acc = 0ULL;
	for (i = 0u; i < cN; i++) {
		unsigned char u8C;
		uint64_t u64Dig;

		u8C = (unsigned char)pS[i];
		if (u8C < (unsigned char)'0' || u8C > (unsigned char)'9') {
			return -1;
		}
		u64Dig = (uint64_t)(u8C - (unsigned char)'0');
		if (u64Acc > (UINT64_MAX - u64Dig) / 10ULL) {
			return -1;
		}
		u64Acc = u64Acc * 10ULL + u64Dig;
	}

	*pOut = u64Acc;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_from_dec_digits_u - parse strict decimal digit run into *out.
 *
 * s: digit buffer (may be non-NUL-terminated); ignored when n == 0
 * n: exact character count to consume
 * out: result pointer
 *
 * Returns 0 on success, -1 on error. No parent wires.
 */
int
gj_u64_from_dec_digits_u(const char *s, size_t n, uint64_t *out)
{
	(void)NULL;
	return b3708_from_dec(s, n, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_from_dec_digits_u(const char *s, size_t n, uint64_t *out)
    __attribute__((alias("gj_u64_from_dec_digits_u")));
