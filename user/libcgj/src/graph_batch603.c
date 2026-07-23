/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch603: freestanding C-string substring predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_contains(const char *hay, const char *needle);
 *     — Return 1 if needle occurs as a contiguous substring of hay,
 *       else 0. Empty needle → 1 when hay is non-NULL (conventional
 *       empty-match-at-zero). hay == NULL or needle == NULL → 0.
 *   int __gj_str_contains  (alias)
 *   __libcgj_batch603_marker = "libcgj-batch603"
 *
 * Distinct from libc strstr (pointer return) and from gj_starts_with /
 * gj_ends_with (batch356). Boolean contains only; unique gj_ names.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch603_marker[] = "libcgj-batch603";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_contains — 1 if needle is a substring of hay, else 0.
 *
 * hay:    haystack C string (NULL → 0)
 * needle: required substring (NULL → 0; empty string matches any
 *         non-NULL hay)
 *
 * Comparison is unsigned-byte exact (no locale). Scans every alignment
 * of needle inside hay until a full match or hay is exhausted.
 * Does not call libc strstr.
 */
int
gj_str_contains(const char *hay, const char *needle)
{
	size_t i;
	size_t j;

	if (hay == NULL || needle == NULL) {
		return 0;
	}

	/* Empty needle matches any non-NULL hay (like strstr empty). */
	if (needle[0] == '\0') {
		return 1;
	}

	for (i = 0u; hay[i] != '\0'; i++) {
		if (hay[i] != needle[0]) {
			continue;
		}
		j = 0u;
		for (;;) {
			if (needle[j] == '\0') {
				return 1;
			}
			if (hay[i + j] == '\0' || hay[i + j] != needle[j]) {
				break;
			}
			j++;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_contains(const char *hay, const char *needle)
    __attribute__((alias("gj_str_contains")));
