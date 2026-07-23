/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1988: freestanding home-relative path probe.
 *
 * Surface (unique symbols):
 *   int gj_home_rel_path_p(const char *szPath);
 *     — Return 1 if szPath is home-relative shell shape: bare "~",
 *       "~/", or "~/..." (tilde as first byte, then end or '/').
 *       Does not expand HOME. "~user" is not treated as home-rel here
 *       (product limited to self-home tilde). NULL or empty → 0.
 *   int __gj_home_rel_path_p  (alias)
 *   __libcgj_batch1988_marker = "libcgj-batch1988"
 *
 * String path product exclusive wave (1981–1990). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch1988_marker[] = "libcgj-batch1988";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1988_home_rel(const char *szPath)
{
	if (szPath == NULL) {
		return 0;
	}
	if (szPath[0] != '~') {
		return 0;
	}
	/* "~" alone */
	if (szPath[1] == '\0') {
		return 1;
	}
	/* "~/..." */
	if (szPath[1] == '/') {
		return 1;
	}
	/* "~user" etc. — out of scope for this product */
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_home_rel_path_p — 1 if path is "~" or "~/...".
 *
 * szPath: NUL-terminated path, or NULL
 * Returns 1 for self-home tilde form, else 0.
 */
int
gj_home_rel_path_p(const char *szPath)
{
	(void)NULL;
	return b1988_home_rel(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_home_rel_path_p(const char *szPath)
    __attribute__((alias("gj_home_rel_path_p")));
