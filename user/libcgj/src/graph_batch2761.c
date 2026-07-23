/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2761: bounded initial span outside a reject set.
 *
 * Surface (unique symbols):
 *   size_t gj_str_cspn_n_u(const char *s, const char *reject, size_t n);
 *     - Length of the leading segment of s[0..n) consisting only of
 *       octets that do not appear in reject (bounded strcspn-shaped).
 *       Stops at the first octet of s found in reject, at s's NUL, or
 *       at n octets. Empty reject yields min(n, strlen(s)). NULL s or
 *       NULL reject yields 0. n == 0 yields 0.
 *   size_t __gj_str_cspn_n_u  (alias)
 *   __libcgj_batch2761_marker = "libcgj-batch2761"
 *
 * Post-2760 str/mem/int exclusive wave (2761-2770). Distinct from
 * gj_str_cspn_u (batch2208) / gj_strcspn_set (batch363) - unique
 * bounded _n_u surface only; no multi-def. Suggested continuum name
 * gj_str_cspn_u already reserved by batch2208.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2761_marker[] = "libcgj-batch2761";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if unsigned octet ch appears in reject; 0 otherwise. */
static int
b2761_in_set(unsigned char ch, const char *reject)
{
	size_t i;

	for (i = 0u; reject[i] != '\0'; i++) {
		if ((unsigned char)reject[i] == ch) {
			return 1;
		}
	}
	return 0;
}

/* Leading span of s[0..n) outside reject. Caller: s, reject non-NULL. */
static size_t
b2761_cspn_n(const char *s, const char *reject, size_t n)
{
	size_t cb;

	cb = 0u;
	while (cb < n && s[cb] != '\0') {
		if (b2761_in_set((unsigned char)s[cb], reject) != 0) {
			break;
		}
		cb++;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cspn_n_u - length of leading s octets none of which are in reject,
 *                   bounded by n.
 *
 * s:      subject string (NULL -> 0); scanned at most n octets
 * reject: NUL-terminated set of rejected octets (NULL -> 0)
 * n:      maximum number of subject octets to examine (0 -> 0)
 *
 * Does not call libc strcspn. Empty reject accepts every examined octet.
 */
size_t
gj_str_cspn_n_u(const char *s, const char *reject, size_t n)
{
	(void)NULL;
	if (s == NULL || reject == NULL || n == 0u) {
		return 0u;
	}
	return b2761_cspn_n(s, reject, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_cspn_n_u(const char *s, const char *reject, size_t n)
    __attribute__((alias("gj_str_cspn_n_u")));
