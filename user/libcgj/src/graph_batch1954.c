/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1954: install-prefix absolute-path predicate.
 *
 * Surface (unique symbols):
 *   int gj_install_prefix_abs_p(const char *prefix);
 *     — Return 1 if prefix is a non-NULL Unix absolute path suitable
 *       as a packaging install root: begins with '/' ("/" alone is
 *       accepted; empty and relative paths are rejected). Pure
 *       path-shape data check; does not resolve, canonicalize, or
 *       touch the filesystem.
 *   int __gj_install_prefix_abs_p  (alias)
 *   __libcgj_batch1954_marker = "libcgj-batch1954"
 *
 * Distinct from gj_path_is_abs (batch480) — avoid multi-def.
 *
 * Package/install exclusive pure-data wave (1951–1960). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1954_marker[] = "libcgj-batch1954";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if sz is absolute (leading '/') and non-empty path text. */
static int
b1954_prefix_abs_p(const char *szPrefix)
{
	if (szPrefix == NULL) {
		return 0;
	}
	if (szPrefix[0] != '/') {
		return 0;
	}
	/* "/" alone is a valid install prefix; just require present. */
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_prefix_abs_p — 1 if prefix is an absolute install root.
 *
 * prefix: NUL-terminated path (NULL → 0; relative → 0)
 * Returns 1 when prefix[0] == '/', else 0.
 */
int
gj_install_prefix_abs_p(const char *szPrefix)
{
	(void)NULL;
	return b1954_prefix_abs_p(szPrefix);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_install_prefix_abs_p(const char *szPrefix)
    __attribute__((alias("gj_install_prefix_abs_p")));
