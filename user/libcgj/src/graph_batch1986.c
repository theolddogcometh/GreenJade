/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1986: freestanding XDG_CONFIG_HOME shape check.
 *
 * Surface (unique symbols):
 *   int gj_xdg_config_home_ok(const char *szPath);
 *     — Soft validity for an XDG_CONFIG_HOME-style value (path product
 *       only; does not read the environment). Per XDG Base Directory
 *       Spec shape: must be non-NULL, non-empty, absolute (leading '/'),
 *       and must not contain a ".." path component. Returns 1 when ok,
 *       else 0. Empty string and relative paths fail.
 *   int __gj_xdg_config_home_ok  (alias)
 *   __libcgj_batch1986_marker = "libcgj-batch1986"
 *
 * String path product exclusive wave (1981–1990). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch1986_marker[] = "libcgj-batch1986";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1986_is_dotdot(const char *szPath, size_t iStart, size_t iEnd)
{
	if (iEnd != iStart + 2u) {
		return 0;
	}
	if (szPath[iStart] != '.' || szPath[iStart + 1u] != '.') {
		return 0;
	}
	return 1;
}

/* 1 if any slash-bounded component is exactly "..". */
static int
b1986_has_dotdot(const char *szPath)
{
	size_t i;
	size_t iStart;

	iStart = 0u;
	for (i = 0u; ; i++) {
		if (szPath[i] == '/' || szPath[i] == '\0') {
			if (i > iStart && b1986_is_dotdot(szPath, iStart, i)) {
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
b1986_ok(const char *szPath)
{
	if (szPath == NULL || szPath[0] == '\0') {
		return 0;
	}
	/* Absolute only (XDG requires absolute when set). */
	if (szPath[0] != '/') {
		return 0;
	}
	if (b1986_has_dotdot(szPath)) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdg_config_home_ok — soft XDG_CONFIG_HOME path shape check.
 *
 * szPath: candidate config-home path (not env lookup)
 * Returns 1 when absolute and free of ".." components, else 0.
 */
int
gj_xdg_config_home_ok(const char *szPath)
{
	(void)NULL;
	return b1986_ok(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_xdg_config_home_ok(const char *szPath)
    __attribute__((alias("gj_xdg_config_home_ok")));
