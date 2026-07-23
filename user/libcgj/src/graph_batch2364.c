/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2364: dynamic linker soname soft-ok predicate
 * (post-2360 product path exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_ld_so_name_ok(const char *name);
 *     - Return 1 if name is a plausible ld.so basename: non-NULL,
 *       non-empty, length 1..64, no '/', and only ASCII letters,
 *       digits, '.', '-', '_'. Soft shape check for names like
 *       "ld-linux-x86-64.so.2" or "ld-gj.so.1".
 *   int __gj_ld_so_name_ok  (alias)
 *   __libcgj_batch2364_marker = "libcgj-batch2364"
 *
 * Post-2360 product path exclusive wave (2361-2370). Distinct from
 * gj_soname_is_libc_so6 (batch1998) - unique ld.so name surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2364_marker[] = "libcgj-batch2364";

/* Max basename length for an ld.so product name. */
#define B2364_NAME_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ch is a permitted ld.so basename character. */
static int
b2364_ch_ok(char ch)
{
	if (ch >= 'a' && ch <= 'z') {
		return 1;
	}
	if (ch >= 'A' && ch <= 'Z') {
		return 1;
	}
	if (ch >= '0' && ch <= '9') {
		return 1;
	}
	if (ch == '.' || ch == '-' || ch == '_') {
		return 1;
	}
	return 0;
}

/* Soft basename shape check for ld.so product names. */
static int
b2364_name_ok(const char *szName)
{
	size_t nLen;

	if (szName == NULL) {
		return 0;
	}
	nLen = 0u;
	while (szName[nLen] != '\0') {
		if (nLen >= B2364_NAME_MAX) {
			return 0;
		}
		if (szName[nLen] == '/') {
			return 0;
		}
		if (!b2364_ch_ok(szName[nLen])) {
			return 0;
		}
		nLen++;
	}
	if (nLen == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ld_so_name_ok - soft check for a dynamic linker basename.
 *
 * name: candidate ld.so basename (not a path)
 *
 * Returns 1 when name is non-empty, length <= 64, has no '/', and uses
 * only [A-Za-z0-9._-], else 0. Does not call libc.
 */
int
gj_ld_so_name_ok(const char *szName)
{
	(void)NULL;
	return b2364_name_ok(szName);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ld_so_name_ok(const char *szName)
    __attribute__((alias("gj_ld_so_name_ok")));
