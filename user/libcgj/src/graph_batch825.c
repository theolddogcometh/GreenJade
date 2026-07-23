/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch825: string starts with ASCII digit predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_starts_digit(const char *s);
 *     — Return 1 if s is non-NULL and s[0] is in '0'..'9', else 0.
 *       Empty string → 0.
 *   int __gj_str_starts_digit  (alias)
 *   __libcgj_batch825_marker = "libcgj-batch825"
 *
 * Distinct from gj_str_starts (batch601) — unique name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch825_marker[] = "libcgj-batch825";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_starts_digit — 1 if first character is ASCII '0'..'9'.
 */
int
gj_str_starts_digit(const char *szS)
{
	unsigned char u8C;

	if (szS == NULL || szS[0] == '\0') {
		return 0;
	}
	u8C = (unsigned char)szS[0];
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_starts_digit(const char *szS)
    __attribute__((alias("gj_str_starts_digit")));
