/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1414: Windows drive absolute-path predicate.
 *
 * Surface (unique symbols):
 *   int gj_path_is_abs_win_drive(const char *path);
 *     — 1 if path matches X:\ or X:/ style (ASCII letter, ':', then
 *       '\\' or '/'). Case-insensitive drive letter. 0 otherwise
 *       (NULL, relative, Unix absolute, bare "C:", UNC, etc.).
 *   int __gj_path_is_abs_win_drive  (alias)
 *   __libcgj_batch1414_marker = "libcgj-batch1414"
 *
 * Distinct from gj_path_is_abs / gj_path_is_abs_unix — unique name.
 * Avoids gj_path_join / has_sep.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1414_marker[] = "libcgj-batch1414";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1414_is_alpha(unsigned char uCh)
{
	return (uCh >= 'A' && uCh <= 'Z') || (uCh >= 'a' && uCh <= 'z');
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_abs_win_drive — Windows drive-letter absolute path.
 *
 * Examples:
 *   "C:\\foo"  → 1
 *   "d:/bar"   → 1
 *   "C:"       → 0  (no trailing separator)
 *   "C:foo"    → 0  (relative to drive)
 *   "/unix"    → 0
 *   NULL       → 0
 */
int
gj_path_is_abs_win_drive(const char *szPath)
{
	unsigned char uDrive;
	unsigned char uSep;

	if (szPath == NULL) {
		return 0;
	}

	uDrive = (unsigned char)szPath[0];
	if (!b1414_is_alpha(uDrive)) {
		return 0;
	}
	if (szPath[1] != ':') {
		return 0;
	}

	uSep = (unsigned char)szPath[2];
	if (uSep == '\\' || uSep == '/') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_abs_win_drive(const char *szPath)
    __attribute__((alias("gj_path_is_abs_win_drive")));
