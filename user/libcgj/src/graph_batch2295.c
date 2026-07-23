/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2295: product shell version soft-ok hint.
 *
 * Surface (unique symbols):
 *   int gj_shell_ver_ok_hint(uint32_t major, uint32_t minor);
 *     - Return 1 if major is in [1, 99] and minor is in [0, 99]
 *       (plausible greenjade-shell product version pair), else 0.
 *       Soft pure-data hint; not a package query.
 *   int __gj_shell_ver_ok_hint  (alias)
 *   __libcgj_batch2295_marker = "libcgj-batch2295"
 *
 * Milestone 2300 exclusive product helpers (2291-2300). Unique
 * gj_shell_ver_ok_hint surface only; no multi-def. Distinct from
 * gj_shell_cmd_budget_hint (batch2193 byte budget).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2295_marker[] = "libcgj-batch2295";

/* Inclusive soft bounds for product shell major.minor. */
#define B2295_MAJOR_MIN  1u
#define B2295_MAJOR_MAX  99u
#define B2295_MINOR_MAX  99u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if major/minor form a plausible product shell version. */
static int
b2295_ver_ok(uint32_t uMajor, uint32_t uMinor)
{
	if (uMajor < B2295_MAJOR_MIN) {
		return 0;
	}
	if (uMajor > B2295_MAJOR_MAX) {
		return 0;
	}
	if (uMinor > B2295_MINOR_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_ver_ok_hint - soft check for product shell major.minor.
 *
 * major: shell major version component (must be >= 1)
 * minor: shell minor version component
 *
 * Returns 1 when major in [1, 99] and minor in [0, 99], else 0.
 * Does not call libc.
 */
int
gj_shell_ver_ok_hint(uint32_t uMajor, uint32_t uMinor)
{
	(void)NULL;
	return b2295_ver_ok(uMajor, uMinor);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shell_ver_ok_hint(uint32_t uMajor, uint32_t uMinor)
    __attribute__((alias("gj_shell_ver_ok_hint")));
