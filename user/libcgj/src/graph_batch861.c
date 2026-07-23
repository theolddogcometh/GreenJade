/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch861: bounded C-string equality (n octets).
 *
 * Surface (unique symbols):
 *   int gj_str_eq_bounded(const char *a, const char *b, size_t n);
 *     — 1 if a and b match for up to n octets (stop at first mismatch
 *       or shared NUL within the window), else 0. n==0 → 1.
 *       both NULL → 1; exactly one NULL and n>0 → 0.
 *   int __gj_str_eq_bounded  (alias)
 *   __libcgj_batch861_marker = "libcgj-batch861"
 *
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch861_marker[] = "libcgj-batch861";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_eq_bounded — 1 if a and b match for up to n octets as C-string prefix.
 *
 * Stops at first mismatch or after n octets. Shared NUL within the
 * window ends the comparison as equal. Does not call libc strncmp.
 */
int
gj_str_eq_bounded(const char *szA, const char *szB, size_t cN)
{
	size_t i;

	if (cN == 0u) {
		return 1;
	}
	if (szA == NULL && szB == NULL) {
		return 1;
	}
	if (szA == NULL || szB == NULL) {
		return 0;
	}

	for (i = 0u; i < cN; i++) {
		if (szA[i] != szB[i]) {
			return 0;
		}
		if (szA[i] == '\0') {
			return 1;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_eq_bounded(const char *szA, const char *szB, size_t cN)
    __attribute__((alias("gj_str_eq_bounded")));
