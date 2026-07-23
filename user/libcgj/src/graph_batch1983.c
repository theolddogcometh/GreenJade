/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1983: freestanding trailing-slash path probe.
 *
 * Surface (unique symbols):
 *   int gj_path_ends_with_slash(const char *szPath);
 *     — Return 1 if szPath is non-NULL, non-empty, and its last byte
 *       is '/'; else 0. Does not treat '\\' as a slash (Unix product).
 *   int __gj_path_ends_with_slash  (alias)
 *   __libcgj_batch1983_marker = "libcgj-batch1983"
 *
 * String path product exclusive wave (1981–1990). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch1983_marker[] = "libcgj-batch1983";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1983_len(const char *sz)
{
	size_t n = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b1983_ends(const char *szPath)
{
	size_t nLen;

	if (szPath == NULL) {
		return 0;
	}
	nLen = b1983_len(szPath);
	if (nLen == 0u) {
		return 0;
	}
	return (szPath[nLen - 1u] == '/') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_ends_with_slash — 1 if last character is '/'.
 *
 * szPath: NUL-terminated path, or NULL
 * Returns 1 when trailing '/', else 0.
 */
int
gj_path_ends_with_slash(const char *szPath)
{
	(void)NULL;
	return b1983_ends(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_ends_with_slash(const char *szPath)
    __attribute__((alias("gj_path_ends_with_slash")));
