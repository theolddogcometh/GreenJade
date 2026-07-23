/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1598: INI section-name character predicate.
 *
 * Surface (unique symbols):
 *   int gj_ini_is_section_char(int c);
 *     — Return 1 if c may appear inside an INI [section] name body:
 *       printable ASCII excluding ']' and control chars. Else 0.
 *       (Common practical set: alnum, '_', '-', '.', space.)
 *   int __gj_ini_is_section_char  (alias)
 *   __libcgj_batch1598_marker = "libcgj-batch1598"
 *
 * Distinct from gj_ini_is_comment_char (batch914) / gj_ini_get —
 * unique name. Section header brackets are not included.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1598_marker[] = "libcgj-batch1598";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Practical INI section body chars: letters, digits, and a small
 * punctuation set used by desktop .ini / .conf section titles.
 */
static int
b1598_is_section_char(int nC)
{
	if (nC >= 'A' && nC <= 'Z') {
		return 1;
	}
	if (nC >= 'a' && nC <= 'z') {
		return 1;
	}
	if (nC >= '0' && nC <= '9') {
		return 1;
	}
	if (nC == '_' || nC == '-' || nC == '.' || nC == ' ') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ini_is_section_char — true for a safe INI section-name body octet.
 *
 * Rejects ']', controls, and shell/INI structural markers. Callers
 * strip surrounding '[' ']' and trim outer spaces as needed.
 */
int
gj_ini_is_section_char(int nC)
{
	return b1598_is_section_char(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ini_is_section_char(int nC)
    __attribute__((alias("gj_ini_is_section_char")));
