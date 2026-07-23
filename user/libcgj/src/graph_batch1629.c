/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1629: ASCII case-insensitive n-byte memory equality.
 *
 * Surface (unique symbols):
 *   int gj_ascii_fold_eq_n(const void *a, const void *b, size_t n);
 *     — Case-insensitive ASCII equality of two n-byte regions (A–Z
 *       folded to a–z). Returns 1 if equal, 0 if not. n == 0 → 1
 *       (vacuous). Either NULL with n > 0 → 0. Both NULL with n == 0 → 1.
 *   int __gj_ascii_fold_eq_n  (alias)
 *   __libcgj_batch1629_marker = "libcgj-batch1629"
 *
 * Distinct from gj_ascii_fold_eq (NUL-terminated strings, batch1024),
 * gj_mem_eq_n (case-sensitive), and strncasecmp. Unique gj_ name only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1629_marker[] = "libcgj-batch1629";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1629_fold — map ASCII A–Z to a–z; leave all other octets unchanged.
 */
static unsigned char
b1629_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_fold_eq_n — ASCII case-insensitive equality of n octets.
 *
 * a, b: regions of at least n bytes when n > 0
 * n:    number of octets to compare
 *
 * Folds A–Z → a–z on both sides, then compares. Does not stop at NUL.
 * Early-exits on the first folded mismatch.
 */
int
gj_ascii_fold_eq_n(const void *a, const void *b, size_t n)
{
	const unsigned char *pa;
	const unsigned char *pb;
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}
	if (a == b) {
		return 1;
	}

	pa = (const unsigned char *)a;
	pb = (const unsigned char *)b;
	for (i = 0u; i < n; i++) {
		if (b1629_fold(pa[i]) != b1629_fold(pb[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_fold_eq_n(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_ascii_fold_eq_n")));
