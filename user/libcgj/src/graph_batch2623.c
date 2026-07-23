/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2623: freestanding C-string lexicographic compare.
 *
 * Surface (unique symbols):
 *   int gj_str_cmp_u(const char *a, const char *b);
 *     - strcmp-shaped: 0 if equal; negative if a < b; positive if a > b
 *       (unsigned-char difference). Both NULL → 0; a NULL → -1; b NULL
 *       → 1 when the other is non-NULL.
 *   int __gj_str_cmp_u  (alias)
 *   __libcgj_batch2623_marker = "libcgj-batch2623"
 *
 * Post-2620 classic str exclusive wave (2621-2630). Distinct from
 * gj_str_cmp_n (batch862), gj_str_ncmp_u (batch2203), and gj_str_eq_u
 * (batch2622 boolean) — unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2623_marker[] = "libcgj-batch2623";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2623_cmp(const char *szA, const char *szB)
{
	const unsigned char *pA;
	const unsigned char *pB;
	size_t i;

	if (szA == NULL || szB == NULL) {
		if (szA == szB) {
			return 0;
		}
		return (szA == NULL) ? -1 : 1;
	}

	pA = (const unsigned char *)szA;
	pB = (const unsigned char *)szB;
	i = 0u;
	for (;;) {
		unsigned char u8A = pA[i];
		unsigned char u8B = pB[i];

		if (u8A != u8B) {
			return (int)u8A - (int)u8B;
		}
		if (u8A == 0u) {
			return 0;
		}
		i++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cmp_u - lexicographic compare of two C strings (unsigned char).
 *
 * Compares until mismatch or shared NUL. Does not call libc strcmp.
 */
int
gj_str_cmp_u(const char *a, const char *b)
{
	(void)NULL;
	return b2623_cmp(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_cmp_u(const char *a, const char *b)
    __attribute__((alias("gj_str_cmp_u")));
