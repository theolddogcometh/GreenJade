/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch731: empty / NULL C-string predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_empty(const char *s);
 *     — Return 1 if s is NULL or s[0] == '\0', else 0.
 *   int __gj_str_is_empty  (alias)
 *   __libcgj_batch731_marker = "libcgj-batch731"
 *
 * Unique gj_str_is_empty surface — no multi-def with other str predicates.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch731_marker[] = "libcgj-batch731";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_empty — 1 if s is NULL or the empty string, else 0.
 *
 * s: NUL-terminated C string, or NULL
 *
 * Does not walk past the first character. Non-NULL non-empty strings
 * always return 0 regardless of later content.
 */
int
gj_str_is_empty(const char *s)
{
	if (s == NULL) {
		return 1;
	}
	if (s[0] == '\0') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_empty(const char *s)
    __attribute__((alias("gj_str_is_empty")));
