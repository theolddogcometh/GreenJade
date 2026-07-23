/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4434: path extension start index (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_path_ext_idx_u(const char *s);
 *     - Index of the last '.' in the final path component (after the
 *       last '/'). Leading-dot hidden names without a further '.' do
 *       not count. No extension, empty, or NULL yields SIZE_MAX.
 *   size_t __gj_path_ext_idx_u  (alias)
 *   __libcgj_batch4434_marker = "libcgj-batch4434"
 *
 * Exclusive continuum CREATE-ONLY (4431-4440: path_is_abs_u,
 * path_is_dot_u, path_has_slash_u, path_ext_idx_u, path_base_idx_u,
 * str_spn_u, str_cspn_u, str_trim_left_idx_u, str_is_all_digits_u,
 * batch_id_4440). Path/string pathlike unique wave. Distinct from
 * gj_path_ext (batch479) and gj_path_ext_len (batch793) — unique index
 * _u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4434_marker[] = "libcgj-batch4434";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Index of last '.' after last '/', excluding a lone leading-dot of the
 * final component. SIZE_MAX when absent / NULL.
 */
static size_t
b4434_ext_idx(const char *s)
{
	size_t i;
	size_t base;
	size_t last_dot;

	if (s == NULL) {
		return SIZE_MAX;
	}

	base = 0u;
	for (i = 0u; s[i] != '\0'; i++) {
		if (s[i] == '/') {
			base = i + 1u;
		}
	}

	last_dot = SIZE_MAX;
	for (i = base; s[i] != '\0'; i++) {
		if (s[i] == '.') {
			last_dot = i;
		}
	}

	/* No dot, or only a leading-dot hidden name (dot at base). */
	if (last_dot == SIZE_MAX || last_dot == base) {
		return SIZE_MAX;
	}
	return last_dot;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_ext_idx_u - index of the extension-introducing '.' in path.
 *
 * s: NUL-terminated path, or NULL
 *
 * Returns the index of the last '.' after the last '/', provided it is
 * not the first character of the final component. Returns SIZE_MAX when
 * s is NULL, empty of extension, or only a leading-dot name.
 */
size_t
gj_path_ext_idx_u(const char *s)
{
	(void)NULL;
	return b4434_ext_idx(s);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_ext_idx_u(const char *s)
    __attribute__((alias("gj_path_ext_idx_u")));
