/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2208: initial span outside a reject character set.
 *
 * Surface (unique symbols):
 *   size_t gj_str_cspn_u(const char *s, const char *reject);
 *     - Return the length of the initial segment of s consisting only
 *       of octets that do not appear in reject (strcspn-shaped). Stops
 *       at the first octet of s found in reject, or at s's NUL. Empty
 *       reject yields the full length of s. NULL s or NULL reject
 *       yields 0.
 *   size_t __gj_str_cspn_u  (alias)
 *   __libcgj_batch2208_marker = "libcgj-batch2208"
 *
 * Post-2200 str exclusive wave (2201-2210). Distinct from gj_strcspn_set
 * (batch363) and gj_str_cspan (batch633) - unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2208_marker[] = "libcgj-batch2208";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if unsigned octet ch appears in reject; 0 otherwise. */
static int
b2208_in_set(unsigned char ch, const char *reject)
{
	size_t i;

	for (i = 0u; reject[i] != '\0'; i++) {
		if ((unsigned char)reject[i] == ch) {
			return 1;
		}
	}
	return 0;
}

static size_t
b2208_cspn(const char *s, const char *reject)
{
	size_t cb;

	if (s == NULL || reject == NULL) {
		return 0u;
	}

	cb = 0u;
	while (s[cb] != '\0') {
		if (b2208_in_set((unsigned char)s[cb], reject) != 0) {
			break;
		}
		cb++;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cspn_u - length of leading s octets none of which are in reject.
 *
 * s:      NUL-terminated subject (NULL → 0)
 * reject: NUL-terminated set of rejected octets (NULL → 0)
 *
 * Does not call libc strcspn. Empty reject accepts every octet of s.
 */
size_t
gj_str_cspn_u(const char *s, const char *reject)
{
	(void)NULL;
	return b2208_cspn(s, reject);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_cspn_u(const char *s, const char *reject)
    __attribute__((alias("gj_str_cspn_u")));
