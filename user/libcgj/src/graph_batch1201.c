/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1201: n-bounded printable ASCII predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_printable_n(const char *s, size_t n);
 *     — Return 1 (OK) if every octet in s[0..n) is ASCII printable
 *       (0x20..0x7E inclusive); else 0 (FAIL). NULL with n > 0 → 0.
 *       n == 0 → 1 (vacuous). Does not require a terminating NUL.
 *   int __gj_str_is_printable_n  (alias)
 *   __libcgj_batch1201_marker = "libcgj-batch1201"
 *
 * Distinct from isprint / ctype — unique gj_ name only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1201_marker[] = "libcgj-batch1201";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_printable_n — 1 if s[0..n) are all printable ASCII.
 */
int
gj_str_is_printable_n(const char *sz, size_t cbN)
{
	const unsigned char *p;
	size_t i;

	if (cbN == 0u) {
		return 1;
	}
	if (sz == NULL) {
		return 0;
	}

	p = (const unsigned char *)sz;
	for (i = 0u; i < cbN; i++) {
		if (p[i] < 0x20u || p[i] > 0x7eu) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_printable_n(const char *sz, size_t cbN)
    __attribute__((alias("gj_str_is_printable_n")));
