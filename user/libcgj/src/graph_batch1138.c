/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1138: YAML leading-indent space counter.
 *
 * Surface (unique symbols):
 *   size_t gj_yaml_indent_spaces(const char *line);
 *     — Count leading ASCII space (0x20) characters on line until a
 *       non-space or NUL. HTAB does not count (stops). NULL → 0.
 *       Cap scan at 4096 to bound work.
 *   size_t __gj_yaml_indent_spaces  (alias)
 *   __libcgj_batch1138_marker = "libcgj-batch1138"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1138_marker[] = "libcgj-batch1138";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_yaml_indent_spaces — number of leading SP characters on szLine.
 */
size_t
gj_yaml_indent_spaces(const char *szLine)
{
	size_t cSpaces;

	if (szLine == NULL) {
		return 0u;
	}

	cSpaces = 0u;
	while (szLine[cSpaces] == ' ') {
		cSpaces++;
		if (cSpaces >= 4096u) {
			return cSpaces;
		}
	}
	return cSpaces;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_yaml_indent_spaces(const char *szLine)
    __attribute__((alias("gj_yaml_indent_spaces")));
