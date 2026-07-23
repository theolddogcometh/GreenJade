/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch762: freestanding ASCII case-insensitive strcmp.
 *
 * Surface (unique symbols):
 *   int gj_str_icmp(const char *a, const char *b);
 *     — strcmp-like ordering under ASCII case fold (A-Z → a-z):
 *       return -1 if a < b, 0 if equal, 1 if a > b.
 *       Both NULL → 0; NULL sorts as empty string vs non-NULL.
 *   int __gj_str_icmp  (alias)
 *   __libcgj_batch762_marker = "libcgj-batch762"
 *
 * Distinct from strcasecmp (signed libc order), gj_str_eq_ci (boolean),
 * and strcasecmp_l — unique gj_str_icmp surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch762_marker[] = "libcgj-batch762";

/* ---- helpers ----------------------------------------------------------- */

/* Fold A-Z to a-z; leave all other octets unchanged (ASCII only). */
static unsigned char
b762_fold_ascii(unsigned char uCh)
{
	if (uCh >= (unsigned char)'A' && uCh <= (unsigned char)'Z') {
		return (unsigned char)(uCh - (unsigned char)'A'
		    + (unsigned char)'a');
	}
	return uCh;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_icmp — ASCII case-insensitive strcmp-like order: -1 / 0 / 1.
 *
 * a, b: NUL-terminated C strings (NULL treated as empty "")
 *
 * Comparison walks folded unsigned octets until a mismatch or joint
 * terminator. Does not call libc strcasecmp.
 */
int
gj_str_icmp(const char *szA, const char *szB)
{
	size_t i;
	unsigned char uA;
	unsigned char uB;

	if (szA == NULL) {
		szA = "";
	}
	if (szB == NULL) {
		szB = "";
	}
	if (szA == szB) {
		return 0;
	}

	i = 0u;
	for (;;) {
		uA = b762_fold_ascii((unsigned char)szA[i]);
		uB = b762_fold_ascii((unsigned char)szB[i]);
		if (uA < uB) {
			return -1;
		}
		if (uA > uB) {
			return 1;
		}
		if (uA == 0u) {
			return 0;
		}
		i++;
	}
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_icmp(const char *szA, const char *szB)
    __attribute__((alias("gj_str_icmp")));
