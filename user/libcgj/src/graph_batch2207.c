/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2207: initial span over an accept character set.
 *
 * Surface (unique symbols):
 *   size_t gj_str_spn_u(const char *s, const char *accept);
 *     - Return the length of the initial segment of s consisting only
 *       of octets that appear in accept (strspn-shaped). Stops at the
 *       first octet of s not found in accept, or at s's NUL. Empty
 *       accept yields 0. NULL s or NULL accept yields 0.
 *   size_t __gj_str_spn_u  (alias)
 *   __libcgj_batch2207_marker = "libcgj-batch2207"
 *
 * Post-2200 str exclusive wave (2201-2210). Distinct from gj_strspn_set
 * (batch362) and gj_str_span (batch632) - unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2207_marker[] = "libcgj-batch2207";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if unsigned octet ch appears in accept; 0 otherwise. */
static int
b2207_in_set(unsigned char ch, const char *accept)
{
	size_t i;

	for (i = 0u; accept[i] != '\0'; i++) {
		if ((unsigned char)accept[i] == ch) {
			return 1;
		}
	}
	return 0;
}

static size_t
b2207_spn(const char *s, const char *accept)
{
	size_t cb;

	if (s == NULL || accept == NULL) {
		return 0u;
	}

	cb = 0u;
	while (s[cb] != '\0') {
		if (b2207_in_set((unsigned char)s[cb], accept) == 0) {
			break;
		}
		cb++;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_spn_u - length of leading s octets all present in accept.
 *
 * s:      NUL-terminated subject (NULL → 0)
 * accept: NUL-terminated set of accepted octets (NULL → 0)
 *
 * Does not call libc strspn. Empty accept means no octet is accepted.
 */
size_t
gj_str_spn_u(const char *s, const char *accept)
{
	(void)NULL;
	return b2207_spn(s, accept);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_spn_u(const char *s, const char *accept)
    __attribute__((alias("gj_str_spn_u")));
