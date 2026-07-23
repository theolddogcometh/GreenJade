/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2297: shared-object soname major soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_soname_major_ok(uint32_t major);
 *     - Return 1 if major is a plausible DT_SONAME major version in
 *       [1, 99] (covers libcgj.so.1 style majors with headroom), else 0.
 *       Soft pure-data check; does not parse soname strings.
 *   int __gj_soname_major_ok  (alias)
 *   __libcgj_batch2297_marker = "libcgj-batch2297"
 *
 * Milestone 2300 exclusive product helpers (2291-2300). Unique
 * gj_soname_major_ok surface only; no multi-def. Distinct from
 * gj_soname_is_libc_so6 (batch1998 string equality).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2297_marker[] = "libcgj-batch2297";

/* Inclusive soft bounds for product soname major. */
#define B2297_MAJOR_MIN  1u
#define B2297_MAJOR_MAX  99u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if major is in [B2297_MAJOR_MIN, B2297_MAJOR_MAX]. */
static int
b2297_major_ok(uint32_t uMajor)
{
	if (uMajor < B2297_MAJOR_MIN) {
		return 0;
	}
	if (uMajor > B2297_MAJOR_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_soname_major_ok - soft check that major is a plausible soname major.
 *
 * major: DT_SONAME-style major version number (e.g. 1 for libcgj.so.1)
 *
 * Returns 1 when major is in [1, 99], else 0. Does not call libc.
 */
int
gj_soname_major_ok(uint32_t uMajor)
{
	(void)NULL;
	return b2297_major_ok(uMajor);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_soname_major_ok(uint32_t uMajor)
    __attribute__((alias("gj_soname_major_ok")));
