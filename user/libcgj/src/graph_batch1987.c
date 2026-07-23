/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1987: freestanding XDG_DATA_HOME shape check.
 *
 * Surface (unique symbols):
 *   int gj_xdg_data_home_ok(const char *szPath);
 *     — Soft validity for an XDG_DATA_HOME-style value (path product
 *       only; does not read the environment). Same shape rules as
 *       XDG_CONFIG_HOME: non-NULL, non-empty, absolute (leading '/'),
 *       and no ".." path component. Returns 1 when ok, else 0.
 *   int __gj_xdg_data_home_ok  (alias)
 *   __libcgj_batch1987_marker = "libcgj-batch1987"
 *
 * String path product exclusive wave (1981–1990). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch1987_marker[] = "libcgj-batch1987";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1987_is_dotdot(const char *szPath, size_t iStart, size_t iEnd)
{
	if (iEnd != iStart + 2u) {
		return 0;
	}
	if (szPath[iStart] != '.' || szPath[iStart + 1u] != '.') {
		return 0;
	}
	return 1;
}

static int
b1987_has_dotdot(const char *szPath)
{
	size_t i;
	size_t iStart;

	iStart = 0u;
	for (i = 0u; ; i++) {
		if (szPath[i] == '/' || szPath[i] == '\0') {
			if (i > iStart && b1987_is_dotdot(szPath, iStart, i)) {
				return 1;
			}
			if (szPath[i] == '\0') {
				break;
			}
			iStart = i + 1u;
		}
	}
	return 0;
}

static int
b1987_ok(const char *szPath)
{
	if (szPath == NULL || szPath[0] == '\0') {
		return 0;
	}
	if (szPath[0] != '/') {
		return 0;
	}
	if (b1987_has_dotdot(szPath)) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_data_home_ok — soft XDG_DATA_HOME path shape check.
 *
 * szPath: candidate data-home path (not env lookup)
 * Returns 1 when absolute and free of ".." components, else 0.
 */
int
gj_xdg_data_home_ok(const char *szPath)
{
	(void)NULL;
	return b1987_ok(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_xdg_data_home_ok(const char *szPath)
    __attribute__((alias("gj_xdg_data_home_ok")));
