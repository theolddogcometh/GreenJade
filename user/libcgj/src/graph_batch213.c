/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch213: character-level Hamming distance on
 * equal-length C strings.
 *
 * Surface (unique symbols):
 *   int gj_hamming_str(const char *a, const char *b);
 *     — count of positions i where a[i] != b[i] over the common
 *       equal length of a and b (NUL-terminated). Returns -1 if a or
 *       b is NULL or if strlen(a) != strlen(b). Empty equal strings
 *       yield 0. Distinct from bit-level buffer Hamming
 *       (gj_hamming_distance, batch184).
 *   __gj_hamming_str  (alias)
 *   __libcgj_batch213_marker = "libcgj-batch213"
 *
 * Clean-room freestanding pure C — integer/pointer only (no SSE
 * required; builds under userspace -msse2). No libc string dependency
 * (-fno-builtin safe). No third-party source was copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch213_marker[] = "libcgj-batch213";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Measure C-string length; NULL is treated as length 0 by the caller
 * path (we reject NULL before calling this).
 */
static size_t
b213_strlen(const char *s)
{
	size_t n = 0u;

	while (s[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- gj_hamming_str ---------------------------------------------------- */

/*
 * Character Hamming distance for equal-length strings only.
 * Walks both strings once after an equal-length check (two passes
 * total: strlen each, then compare). Returns -1 on NULL or unequal
 * lengths; otherwise the mismatch count (0 .. n).
 */
int
gj_hamming_str(const char *a, const char *b)
{
	size_t na;
	size_t nb;
	size_t i;
	int dist;

	if (a == NULL || b == NULL) {
		return -1;
	}

	na = b213_strlen(a);
	nb = b213_strlen(b);
	if (na != nb) {
		return -1;
	}

	dist = 0;
	for (i = 0u; i < na; i++) {
		if (a[i] != b[i]) {
			dist++;
		}
	}
	return dist;
}

int __gj_hamming_str(const char *a, const char *b)
    __attribute__((alias("gj_hamming_str")));
