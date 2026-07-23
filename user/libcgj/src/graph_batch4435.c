/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4435: path basename start index (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_path_base_idx_u(const char *s);
 *     - Index of the first character after the last '/'. If s has no
 *       '/', returns 0. NULL yields 0.
 *   size_t __gj_path_base_idx_u  (alias)
 *   __libcgj_batch4435_marker = "libcgj-batch4435"
 *
 * Exclusive continuum CREATE-ONLY (4431-4440: path_is_abs_u,
 * path_is_dot_u, path_has_slash_u, path_ext_idx_u, path_base_idx_u,
 * str_spn_u, str_cspn_u, str_trim_left_idx_u, str_is_all_digits_u,
 * batch_id_4440). Path/string pathlike unique wave. Distinct from
 * gj_path_basename (batch476) and gj_path_basename_start — unique index
 * _u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4435_marker[] = "libcgj-batch4435";

/* ---- freestanding helpers ---------------------------------------------- */

/* Index after last '/'; 0 when none or s is NULL. */
static size_t
b4435_base_idx(const char *s)
{
	size_t i;
	size_t base;

	if (s == NULL) {
		return 0u;
	}

	base = 0u;
	for (i = 0u; s[i] != '\0'; i++) {
		if (s[i] == '/') {
			base = i + 1u;
		}
	}
	return base;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_base_idx_u - index of the final path component.
 *
 * s: NUL-terminated path, or NULL
 *
 * Returns the index of the character immediately after the last '/',
 * or 0 when s is NULL or contains no '/'. The returned index may equal
 * strlen(s) when s ends with '/'.
 */
size_t
gj_path_base_idx_u(const char *s)
{
	(void)NULL;
	return b4435_base_idx(s);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_base_idx_u(const char *s)
    __attribute__((alias("gj_path_base_idx_u")));
