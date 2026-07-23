/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch361: bounded lexicographic string compare.
 *
 * Surface (unique symbols):
 *   int gj_strcmp_n(const char *a, const char *b, size_t n);
 *     — Compare at most n octets of a and b as unsigned bytes. Stops
 *       at the first mismatch, the first NUL in either string, or when
 *       n bytes have been compared. Returns <0 if a < b, 0 if equal
 *       over the compared window, >0 if a > b. NULL a with non-NULL b
 *       yields -1; non-NULL a with NULL b yields 1; both NULL yields 0.
 *       n == 0 yields 0 (empty window is equal).
 *   int __gj_strcmp_n  (alias)
 *   __libcgj_batch361_marker = "libcgj-batch361"
 *
 * Distinct from libc strncmp / strcmp in string.c — unique gj_ names
 * only; no multi-def with the existing string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch361_marker[] = "libcgj-batch361";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcmp_n — compare a and b for at most n bytes (strncmp-shaped).
 *
 * a, b: NUL-terminated C strings (either may be NULL; see header)
 * n:    maximum number of octets to compare
 *
 * Unsigned-byte lexicographic order. Does not call libc strcmp/strncmp.
 */
int
gj_strcmp_n(const char *a, const char *b, size_t n)
{
	size_t i;
	unsigned char ua;
	unsigned char ub;

	if (a == NULL && b == NULL) {
		return 0;
	}
	if (a == NULL) {
		return -1;
	}
	if (b == NULL) {
		return 1;
	}
	if (n == 0u) {
		return 0;
	}

	for (i = 0u; i < n; i++) {
		ua = (unsigned char)a[i];
		ub = (unsigned char)b[i];
		if (ua != ub) {
			return (ua < ub) ? -1 : 1;
		}
		if (ua == 0u) {
			return 0;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_strcmp_n(const char *a, const char *b, size_t n)
    __attribute__((alias("gj_strcmp_n")));
