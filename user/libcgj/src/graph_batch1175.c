/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1175: semver core numeric digit predicate.
 *
 * Surface (unique symbols):
 *   int gj_semver_is_digit_core(int c);
 *     — Return 1 if c is an ASCII decimal digit '0'..'9' (valid in
 *       semver major/minor/patch numeric identifiers); else 0.
 *   int __gj_semver_is_digit_core  (alias)
 *   __libcgj_batch1175_marker = "libcgj-batch1175"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1175_marker[] = "libcgj-batch1175";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_semver_is_digit_core — 1 iff c is '0'..'9'.
 */
int
gj_semver_is_digit_core(int c)
{
	if (c >= '0' && c <= '9') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_semver_is_digit_core(int c)
    __attribute__((alias("gj_semver_is_digit_core")));
