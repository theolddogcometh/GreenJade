/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1955: .desktop file-extension predicate.
 *
 * Surface (unique symbols):
 *   int gj_desktop_file_ext_p(const char *name);
 *     — Return 1 if name is non-NULL and ends with the exact eight
 *       ASCII octets ".desktop" (case-sensitive), else 0. Does not
 *       parse Desktop Entry keys or validate the file body.
 *   int __gj_desktop_file_ext_p  (alias)
 *   __libcgj_batch1955_marker = "libcgj-batch1955"
 *
 * Package/install exclusive pure-data wave (1951–1960). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1955_marker[] = "libcgj-batch1955";

/* Exact freedesktop desktop-entry suffix length (".desktop"). */
#define B1955_EXT_LEN  8u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1955_strlen(const char *sz)
{
	size_t cb = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

/* 1 if sz ends with ".desktop". */
static int
b1955_desktop_ext_p(const char *szName)
{
	static const char aExt[B1955_EXT_LEN] = {
	    '.', 'd', 'e', 's', 'k', 't', 'o', 'p'
	};
	size_t cb;
	size_t i;
	size_t iBase;

	if (szName == NULL) {
		return 0;
	}
	cb = b1955_strlen(szName);
	if (cb < B1955_EXT_LEN) {
		return 0;
	}
	iBase = cb - B1955_EXT_LEN;
	for (i = 0u; i < B1955_EXT_LEN; i++) {
		if (szName[iBase + i] != aExt[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_desktop_file_ext_p — 1 if name ends with ".desktop".
 *
 * name: NUL-terminated path or basename (NULL / short / mismatch → 0)
 * Exact trailing match; no case folding.
 */
int
gj_desktop_file_ext_p(const char *szName)
{
	(void)NULL;
	return b1955_desktop_ext_p(szName);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_desktop_file_ext_p(const char *szName)
    __attribute__((alias("gj_desktop_file_ext_p")));
