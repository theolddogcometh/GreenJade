/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1998: soname equals libc.so.6 predicate.
 *
 * Surface (unique symbols):
 *   int gj_soname_is_libc_so6(const char *soname);
 *     — Return 1 if soname is exactly "libc.so.6" (case-sensitive,
 *       full-string match), else 0. NULL → 0. Soft product soname
 *       check for glibc-compat rootfs wiring; no path parsing.
 *   int __gj_soname_is_libc_so6  (alias)
 *   __libcgj_batch1998_marker = "libcgj-batch1998"
 *
 * Product bar helpers exclusive pure-data wave (1991–2000). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1998_marker[] = "libcgj-batch1998";

/* Canonical glibc product soname. */
static const char b1998_soname[] = "libc.so.6";

/* ---- freestanding helpers ---------------------------------------------- */

/* Length of a NUL-terminated string (no libc). */
static size_t
b1998_len(const char *sz)
{
	size_t n;

	n = 0;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/* Exact equality of two NUL-terminated strings. */
static int
b1998_eq(const char *szA, const char *szB)
{
	size_t i;

	i = 0;
	for (;;) {
		if (szA[i] != szB[i]) {
			return 0;
		}
		if (szA[i] == '\0') {
			return 1;
		}
		i++;
	}
}

/* 1 if soname equals "libc.so.6". */
static int
b1998_is_libc_so6(const char *szSoname)
{
	if (b1998_len(szSoname) != b1998_len(b1998_soname)) {
		return 0;
	}
	return b1998_eq(szSoname, b1998_soname);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_soname_is_libc_so6 — 1 iff soname is exactly "libc.so.6".
 *
 * soname: DT_SONAME-style C string, or NULL.
 * Examples:
 *   "libc.so.6"   → 1
 *   "libc.so.6.1" → 0
 *   "libc.so"     → 0
 *   NULL          → 0
 */
int
gj_soname_is_libc_so6(const char *szSoname)
{
	(void)NULL;
	if (szSoname == NULL) {
		return 0;
	}
	return b1998_is_libc_so6(szSoname);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_soname_is_libc_so6(const char *szSoname)
    __attribute__((alias("gj_soname_is_libc_so6")));
