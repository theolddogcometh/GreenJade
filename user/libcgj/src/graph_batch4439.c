/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4439: all-ASCII-digits predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_str_is_all_digits_u(const char *s);
 *     - Return 1 if s is non-NULL, non-empty, and every octet is in
 *       '0'..'9'; else 0 (NULL, empty, or any non-digit).
 *   int __gj_str_is_all_digits_u  (alias)
 *   __libcgj_batch4439_marker = "libcgj-batch4439"
 *
 * Exclusive continuum CREATE-ONLY (4431-4440: path_is_abs_u,
 * path_is_dot_u, path_has_slash_u, path_ext_idx_u, path_base_idx_u,
 * str_spn_u, str_cspn_u, str_trim_left_idx_u, str_is_all_digits_u,
 * batch_id_4440). Path/string pathlike unique wave. Unique
 * gj_str_is_all_digits_u surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4439_marker[] = "libcgj-batch4439";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff s non-NULL, non-empty, all octets in '0'..'9'. */
static int
b4439_is_all_digits(const char *s)
{
	size_t i;

	if (s == NULL || s[0] == '\0') {
		return 0;
	}

	for (i = 0u; s[i] != '\0'; i++) {
		if (s[i] < '0' || s[i] > '9') {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_all_digits_u - true iff s is a non-empty decimal digit run.
 *
 * s: NUL-terminated C string, or NULL
 *
 * Returns 1 when every character is '0'..'9' and length >= 1; else 0.
 * Does not accept signs, whitespace, or empty strings.
 */
int
gj_str_is_all_digits_u(const char *s)
{
	(void)NULL;
	return b4439_is_all_digits(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_all_digits_u(const char *s)
    __attribute__((alias("gj_str_is_all_digits_u")));
