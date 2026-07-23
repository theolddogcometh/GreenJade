/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2399: libcgj soname major soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   int gj_libcgj_soname_ok_u(uint32_t major);
 *     - Return 1 if major is the product libcgj DT_SONAME major
 *       (exactly 1, as in libcgj.so.1 / libc.so.6-shaped major), else 0.
 *       Soft pure-data check; does not parse soname strings.
 *   int __gj_libcgj_soname_ok_u  (alias)
 *   __libcgj_batch2399_marker = "libcgj-batch2399"
 *
 * Milestone 2400 exclusive product helpers (2391-2400). Unique
 * gj_libcgj_soname_ok_u surface only; no multi-def. Distinct from
 * gj_soname_major_ok (batch2297 band [1,99]) and gj_soname_is_libc_so6
 * (batch1998 string equality).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2399_marker[] = "libcgj-batch2399";

/* Product libcgj DT_SONAME major (libcgj.so.1 style). */
#define B2399_LIBCGJ_MAJOR  1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if major is the product libcgj soname major. */
static int
b2399_soname_ok(uint32_t uMajor)
{
	if (uMajor == B2399_LIBCGJ_MAJOR) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_soname_ok_u - soft check that major is product libcgj major.
 *
 * major: DT_SONAME-style major version number (1 for libcgj.so.1)
 *
 * Returns 1 when major is exactly 1, else 0. Does not parse soname
 * strings and does not call libc.
 */
int
gj_libcgj_soname_ok_u(uint32_t uMajor)
{
	(void)NULL;
	return b2399_soname_ok(uMajor);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_libcgj_soname_ok_u(uint32_t uMajor)
    __attribute__((alias("gj_libcgj_soname_ok_u")));
