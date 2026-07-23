/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4438: left-trim whitespace start index (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_str_trim_left_idx_u(const char *s);
 *     - Index of the first character of s that is not space / tab /
 *       CR / LF. All-trim or empty content yields the index of the
 *       terminating NUL (strlen). NULL yields 0.
 *   size_t __gj_str_trim_left_idx_u  (alias)
 *   __libcgj_batch4438_marker = "libcgj-batch4438"
 *
 * Exclusive continuum CREATE-ONLY (4431-4440: path_is_abs_u,
 * path_is_dot_u, path_has_slash_u, path_ext_idx_u, path_base_idx_u,
 * str_spn_u, str_cspn_u, str_trim_left_idx_u, str_is_all_digits_u,
 * batch_id_4440). Path/string pathlike unique wave. Distinct from
 * gj_str_trim_left_u (batch2956, pointer return) — unique index _u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4438_marker[] = "libcgj-batch4438";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if ch is a left-trim target: space, tab, CR, or LF. */
static int
b4438_is_trim(unsigned char u8Ch)
{
	return u8Ch == (unsigned char)' ' ||
	    u8Ch == (unsigned char)'\t' ||
	    u8Ch == (unsigned char)'\r' ||
	    u8Ch == (unsigned char)'\n';
}

static size_t
b4438_trim_left_idx(const char *s)
{
	size_t i;

	if (s == NULL) {
		return 0u;
	}

	i = 0u;
	while (s[i] != '\0' && b4438_is_trim((unsigned char)s[i])) {
		i++;
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_trim_left_idx_u - index of first non-space (space/tab/CR/LF).
 *
 * s: NUL-terminated C string, or NULL
 *
 * Returns the index of the first non-trim character. If every character
 * is a trim character (or s is empty), returns the index of the final
 * NUL. NULL s returns 0.
 */
size_t
gj_str_trim_left_idx_u(const char *s)
{
	(void)NULL;
	return b4438_trim_left_idx(s);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_trim_left_idx_u(const char *s)
    __attribute__((alias("gj_str_trim_left_idx_u")));
