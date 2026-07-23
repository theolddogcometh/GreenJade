/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4433: path contains '/' predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_has_slash_u(const char *s);
 *     - Return 1 if s is non-NULL and contains at least one '/';
 *       else 0 (NULL or no slash). Unix separator only; '\\' does not
 *       count.
 *   int __gj_path_has_slash_u  (alias)
 *   __libcgj_batch4433_marker = "libcgj-batch4433"
 *
 * Exclusive continuum CREATE-ONLY (4431-4440: path_is_abs_u,
 * path_is_dot_u, path_has_slash_u, path_ext_idx_u, path_base_idx_u,
 * str_spn_u, str_cspn_u, str_trim_left_idx_u, str_is_all_digits_u,
 * batch_id_4440). Path/string pathlike unique wave. Distinct from
 * gj_path_has_sep (batch683, '/' or '\\') — unique slash-only _u
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4433_marker[] = "libcgj-batch4433";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff s is non-NULL and contains '/'. */
static int
b4433_has_slash(const char *s)
{
	size_t i;

	if (s == NULL) {
		return 0;
	}
	for (i = 0u; s[i] != '\0'; i++) {
		if (s[i] == '/') {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_has_slash_u - true iff path contains a Unix directory separator.
 *
 * s: NUL-terminated path, or NULL
 *
 * Returns 1 when s is non-NULL and contains '/', else 0.
 * Does not treat '\\' as a separator.
 */
int
gj_path_has_slash_u(const char *s)
{
	(void)NULL;
	return b4433_has_slash(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_has_slash_u(const char *s)
    __attribute__((alias("gj_path_has_slash_u")));
