/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4432: path is-dot / is-dotdot component (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_is_dot_u2(const char *s);
 *     - Return 1 if s is exactly "." or exactly ".."; else 0
 *       (including NULL). Component-level exact match only.
 *   int __gj_path_is_dot_u2  (alias)
 *   __libcgj_batch4432_marker = "libcgj-batch4432"
 *
 * Exclusive continuum CREATE-ONLY (4431-4440: path_is_abs_u,
 * path_is_dot_u, path_has_slash_u, path_ext_idx_u, path_base_idx_u,
 * str_spn_u, str_cspn_u, str_trim_left_idx_u, str_is_all_digits_u,
 * batch_id_4440). Path/string pathlike unique wave. Accepts both "."
 * and ".." (combined component predicate). Distinct from
 * gj_path_is_dot (batch918) and gj_path_is_dotdot (batch919) — unique
 * _u surface. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4432_marker[] = "libcgj-batch4432";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff s is exactly "." or exactly "..". */
static int
b4432_is_dot(const char *s)
{
	if (s == NULL) {
		return 0;
	}
	if (s[0] != '.') {
		return 0;
	}
	/* "." */
	if (s[1] == '\0') {
		return 1;
	}
	/* ".." */
	if (s[1] == '.' && s[2] == '\0') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_dot_u2 - true iff path is exactly "." or "..".
 *
 * s: NUL-terminated path component, or NULL
 *
 * Returns 1 on exact match of either component form, else 0.
 * Does not accept "./", "../", or longer forms.
 */
int
gj_path_is_dot_u2(const char *s)
{
	(void)NULL;
	return b4432_is_dot(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_dot_u2(const char *s)
    __attribute__((alias("gj_path_is_dot_u2")));
