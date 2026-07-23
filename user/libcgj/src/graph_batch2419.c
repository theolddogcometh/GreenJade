/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2419: scalar ASCII case-folded octet equality.
 *
 * Surface (unique symbols):
 *   int gj_ascii_fold_eq_u(unsigned char a, unsigned char b);
 *     - Return 1 if a and b are equal after ASCII case fold (A-Z ->
 *       a-z), else 0. Pure scalar; no string walk, no locale.
 *   int __gj_ascii_fold_eq_u  (alias)
 *   __libcgj_batch2419_marker = "libcgj-batch2419"
 *
 * Distinct from gj_ascii_fold_eq (NUL strings, batch1024),
 * gj_ascii_fold_eq_n (n-byte regions, batch1629), and gj_str_eq_ci
 * (batch506) — unique scalar u-suffix surface; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2419_marker[] = "libcgj-batch2419";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z -> a-z; all other octets unchanged. */
static unsigned char
b2419_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_fold_eq_u - 1 if two octets match under ASCII case fold.
 *
 * a, b: input octets
 *
 * Folds A-Z to a-z on both sides, then compares for equality.
 */
int
gj_ascii_fold_eq_u(unsigned char u8A, unsigned char u8B)
{
	(void)NULL;
	return (b2419_fold(u8A) == b2419_fold(u8B)) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_fold_eq_u(unsigned char u8A, unsigned char u8B)
    __attribute__((alias("gj_ascii_fold_eq_u")));
