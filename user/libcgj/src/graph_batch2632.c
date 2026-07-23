/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2632: product ld-gj dynamic linker name soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_ld_gj_name_ok(const char *name);
 *     - Return 1 if name is exactly "ld-gj.so.1" (case-sensitive full
 *       string match), else 0. NULL → 0. Soft product PT_INTERP /
 *       soname check for the GreenJade dynamic linker; no path parse.
 *   int __gj_ld_gj_name_ok  (alias)
 *   __libcgj_batch2632_marker = "libcgj-batch2632"
 *
 * Product path exclusive wave (2631-2640). Distinct from
 * gj_ld_so_name_ok (batch2364 general basename shape) and
 * gj_soname_is_libc_so6 (batch1998) — unique gj_ld_gj_name_ok surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2632_marker[] = "libcgj-batch2632";

/* Canonical product dynamic linker basename. */
static const char b2632_name[] = "ld-gj.so.1";

/* ---- freestanding helpers ---------------------------------------------- */

/* Exact equality of two NUL-terminated strings. */
static int
b2632_eq(const char *szA, const char *szB)
{
	size_t i;

	i = 0u;
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

/* 1 if name equals "ld-gj.so.1". */
static int
b2632_name_ok(const char *szName)
{
	if (szName == NULL) {
		return 0;
	}
	return b2632_eq(szName, b2632_name);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ld_gj_name_ok - 1 iff name is exactly "ld-gj.so.1".
 *
 * name: candidate dynamic linker basename (not a full path)
 * Examples:
 *   "ld-gj.so.1"            → 1
 *   "ld-linux-x86-64.so.2"  → 0
 *   "ld-gj.so"              → 0
 *   NULL                    → 0
 */
int
gj_ld_gj_name_ok(const char *szName)
{
	(void)NULL;
	return b2632_name_ok(szName);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ld_gj_name_ok(const char *szName)
    __attribute__((alias("gj_ld_gj_name_ok")));
