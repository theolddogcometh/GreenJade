/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4437: initial span outside a reject set (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_str_cspn_u2(const char *s, const char *reject);
 *     - Return the length of the initial segment of s consisting only
 *       of octets that do not appear in reject (strcspn-shaped). Stops
 *       at the first octet of s found in reject, or at s's NUL. Empty
 *       reject yields the full length of s. NULL s or NULL reject
 *       yields 0.
 *   size_t __gj_str_cspn_u2  (alias)
 *   __libcgj_batch4437_marker = "libcgj-batch4437"
 *
 * Exclusive continuum CREATE-ONLY (4431-4440: path_is_abs_u,
 * path_is_dot_u, path_has_slash_u, path_ext_idx_u, path_base_idx_u,
 * str_spn_u, str_cspn_u, str_trim_left_idx_u, str_is_all_digits_u,
 * batch_id_4440). Path/string pathlike unique wave. Distinct from
 * gj_strcspn_set (batch363) and gj_str_cspan (batch633) — unique _u
 * surface. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4437_marker[] = "libcgj-batch4437";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if unsigned octet ch appears in reject; 0 otherwise. */
static int
b4437_in_set(unsigned char ch, const char *reject)
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
b4437_cspn(const char *s, const char *reject)
{
	size_t cb;

	if (s == NULL || reject == NULL) {
		return 0u;
	}

	cb = 0u;
	while (s[cb] != '\0') {
		if (b4437_in_set((unsigned char)s[cb], reject) != 0) {
			break;
		}
		cb++;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cspn_u2 - length of leading s octets none of which are in reject.
 *
 * s:      NUL-terminated subject (NULL → 0)
 * reject: NUL-terminated set of rejected octets (NULL → 0)
 *
 * Does not call libc strcspn. Empty reject accepts every octet of s.
 */
size_t
gj_str_cspn_u2(const char *s, const char *reject)
{
	(void)NULL;
	return b4437_cspn(s, reject);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_cspn_u2(const char *s, const char *reject)
    __attribute__((alias("gj_str_cspn_u2")));
