/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2953: freestanding ASCII case-insensitive equality.
 *
 * Surface (unique symbols):
 *   int gj_str_eq_ci_u(const char *a, const char *b);
 *     - Return 1 if a and b are equal as C strings under ASCII
 *       case-insensitive compare (A-Z folds to a-z only), else 0.
 *       Both NULL → 1; exactly one NULL → 0.
 *   int __gj_str_eq_ci_u  (alias)
 *   __libcgj_batch2953_marker = "libcgj-batch2953"
 *
 * Post-2950 exclusive string helpers wave (2951-2960). Distinct from
 * gj_str_eq_ci (batch506) and gj_str_eq_ci_n (batch2411) — unique
 * gj_str_eq_ci_u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2953_marker[] = "libcgj-batch2953";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold A-Z to a-z; leave all other octets unchanged (ASCII only). */
static unsigned char
b2953_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

static int
b2953_eq_ci(const char *a, const char *b)
{
	size_t i;
	unsigned char uCa;
	unsigned char uCb;

	if (a == NULL && b == NULL) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}

	i = 0u;
	for (;;) {
		uCa = b2953_fold((unsigned char)a[i]);
		uCb = b2953_fold((unsigned char)b[i]);
		if (uCa != uCb) {
			return 0;
		}
		if (uCa == (unsigned char)0) {
			return 1;
		}
		i++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_eq_ci_u - 1 if a equals b as C strings, ASCII case-insensitive.
 *
 * Edge cases:
 *   a == NULL && b == NULL → 1
 *   exactly one of a, b is NULL → 0
 *   empty strings equal
 *
 * Does not call libc strcasecmp.
 */
int
gj_str_eq_ci_u(const char *a, const char *b)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b2953_eq_ci(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_eq_ci_u(const char *a, const char *b)
    __attribute__((alias("gj_str_eq_ci_u")));
