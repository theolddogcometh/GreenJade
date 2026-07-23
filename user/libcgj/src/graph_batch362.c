/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch362: initial span over an accept character set.
 *
 * Surface (unique symbols):
 *   size_t gj_strspn_set(const char *s, const char *accept);
 *     — Return the length of the initial segment of s consisting only
 *       of octets that appear in accept (strspn-shaped). Stops at the
 *       first octet of s not found in accept, or at s's NUL. Empty
 *       accept yields 0. NULL s or NULL accept yields 0.
 *   size_t __gj_strspn_set  (alias)
 *   __libcgj_batch362_marker = "libcgj-batch362"
 *
 * Distinct from libc strspn in string.c — unique gj_ names only; no
 * multi-def with the existing string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch362_marker[] = "libcgj-batch362";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if unsigned octet ch appears in accept; 0 otherwise. */
static int
b362_in_set(unsigned char ch, const char *accept)
{
	size_t i;

	for (i = 0u; accept[i] != '\0'; i++) {
		if ((unsigned char)accept[i] == ch) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strspn_set — length of leading s octets all present in accept.
 *
 * s:      NUL-terminated subject (NULL → 0)
 * accept: NUL-terminated set of accepted octets (NULL → 0)
 *
 * Does not call libc strspn. Empty accept means no octet is accepted.
 */
size_t
gj_strspn_set(const char *s, const char *accept)
{
	size_t cb;

	if (s == NULL || accept == NULL) {
		return 0u;
	}

	cb = 0u;
	while (s[cb] != '\0') {
		if (b362_in_set((unsigned char)s[cb], accept) == 0) {
			break;
		}
		cb++;
	}
	return cb;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_strspn_set(const char *s, const char *accept)
    __attribute__((alias("gj_strspn_set")));
