/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4431: absolute Unix path predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_is_abs_u2(const char *s);
 *     - Return 1 if s is non-NULL and begins with '/'; else 0
 *       (NULL or relative). Unix-style only; no drive-letter / UNC.
 *   int __gj_path_is_abs_u2  (alias)
 *   __libcgj_batch4431_marker = "libcgj-batch4431"
 *
 * Exclusive continuum CREATE-ONLY (4431-4440: path_is_abs_u,
 * path_is_dot_u, path_has_slash_u, path_ext_idx_u, path_base_idx_u,
 * str_spn_u, str_cspn_u, str_trim_left_idx_u, str_is_all_digits_u,
 * batch_id_4440). Path/string pathlike unique wave. Distinct from
 * gj_path_is_abs (batch480) and gj_path_is_abs_unix (batch1413) —
 * unique _u surface; no multi-def of non-_u parents. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4431_marker[] = "libcgj-batch4431";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff s is non-NULL and begins with '/'. */
static int
b4431_is_abs(const char *s)
{
	if (s == NULL) {
		return 0;
	}
	if (s[0] == '/') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_abs_u2 - absolute Unix path predicate (leading '/').
 *
 * s: NUL-terminated path, or NULL
 *
 * Returns 1 when s is non-NULL and s[0] == '/', else 0.
 * Does not accept Windows drive-letter or UNC forms.
 */
int
gj_path_is_abs_u2(const char *s)
{
	(void)NULL;
	return b4431_is_abs(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_abs_u2(const char *s)
    __attribute__((alias("gj_path_is_abs_u2")));
