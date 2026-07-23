/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2477: freestanding Windows drive-letter prefix.
 *
 * Surface (unique symbols):
 *   unsigned gj_path_has_drive_u(const char *path);
 *     — Return 1u if path begins with an ASCII drive letter followed by
 *       ':', e.g. "C:", "C:\\foo", "d:/bar", "E:rel". Case-insensitive
 *       letter. Does not require a trailing separator (unlike
 *       gj_path_is_abs_win_drive). NULL or short/non-matching → 0u.
 *   unsigned __gj_path_has_drive_u  (alias)
 *   __libcgj_batch2477_marker = "libcgj-batch2477"
 *
 * Path exclusive wave (2471–2480). Distinct from
 * gj_path_is_abs_win_drive (batch1414, requires sep after ':').
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2477_marker[] = "libcgj-batch2477";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2477_is_alpha(unsigned char uCh)
{
	return (uCh >= 'A' && uCh <= 'Z') || (uCh >= 'a' && uCh <= 'z');
}

static unsigned
b2477_has_drive(const char *szPath)
{
	unsigned char uDrive;

	if (szPath == NULL) {
		return 0u;
	}

	uDrive = (unsigned char)szPath[0];
	if (!b2477_is_alpha(uDrive)) {
		return 0u;
	}
	if (szPath[1] != ':') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_has_drive_u — 1u if path has an X: drive-letter prefix.
 *
 * Examples:
 *   "C:\\foo"  → 1u
 *   "d:/bar"   → 1u
 *   "C:"       → 1u
 *   "C:foo"    → 1u  (drive-relative; still has a drive prefix)
 *   "/unix"    → 0u
 *   "\\\\unc"  → 0u
 *   NULL       → 0u
 */
unsigned
gj_path_has_drive_u(const char *szPath)
{
	(void)NULL;
	return b2477_has_drive(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_path_has_drive_u(const char *szPath)
    __attribute__((alias("gj_path_has_drive_u")));
