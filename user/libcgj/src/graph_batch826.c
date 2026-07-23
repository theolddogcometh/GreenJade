/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch826: string starts with ASCII letter predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_starts_alpha(const char *s);
 *     — Return 1 if s is non-NULL and s[0] is A-Z or a-z, else 0.
 *       Empty string → 0.
 *   int __gj_str_starts_alpha  (alias)
 *   __libcgj_batch826_marker = "libcgj-batch826"
 *
 * Distinct from gj_str_starts_digit (batch825) / gj_str_starts (batch601)
 * — unique name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch826_marker[] = "libcgj-batch826";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_starts_alpha — 1 if first character is ASCII A-Z or a-z.
 */
int
gj_str_starts_alpha(const char *szS)
{
	unsigned char u8C;

	if (szS == NULL || szS[0] == '\0') {
		return 0;
	}
	u8C = (unsigned char)szS[0];
	if ((u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ||
	    (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z')) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_starts_alpha(const char *szS)
    __attribute__((alias("gj_str_starts_alpha")));
