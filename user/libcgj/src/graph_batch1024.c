/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1024: ASCII case-folded string equality.
 *
 * Surface (unique symbols):
 *   int gj_ascii_fold_eq(const char *a, const char *b);
 *     — Case-insensitive ASCII equality of two NUL-terminated
 *       strings (A–Z folded to a–z). Returns 1 if equal, 0 if not.
 *       NULL a or b → 0 (unless both NULL → 1).
 *   int __gj_ascii_fold_eq  (alias)
 *   __libcgj_batch1024_marker = "libcgj-batch1024"
 *
 * Does NOT redefine strcasecmp / gj_ct_eq.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1024_marker[] = "libcgj-batch1024";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1024_fold — map ASCII A–Z to a–z; leave all other octets unchanged.
 */
static unsigned char
b1024_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_fold_eq — ASCII case-insensitive string equality.
 *
 * szA, szB: NUL-terminated inputs (may be empty).
 *
 * Both NULL → 1. Exactly one NULL → 0. Otherwise walk until mismatch
 * (after A–Z fold) or both hit NUL.
 */
int
gj_ascii_fold_eq(const char *szA, const char *szB)
{
	size_t i;

	if (szA == NULL && szB == NULL) {
		return 1;
	}
	if (szA == NULL || szB == NULL) {
		return 0;
	}

	i = 0u;
	for (;;) {
		unsigned char u8A = b1024_fold((unsigned char)szA[i]);
		unsigned char u8B = b1024_fold((unsigned char)szB[i]);

		if (u8A != u8B) {
			return 0;
		}
		if (u8A == 0u) {
			return 1;
		}
		i++;
	}
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_fold_eq(const char *szA, const char *szB)
    __attribute__((alias("gj_ascii_fold_eq")));
