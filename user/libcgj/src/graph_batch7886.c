/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7886: C-locale strcoll equality predicate.
 *
 * Surface (unique symbols):
 *   int gj_strcoll_eq_7886(const char *a, const char *b);
 *     - Return 1 if C-locale collate of a and b is equal, else 0.
 *       Both NULL -> 1; one NULL -> 0.
 *   int __gj_strcoll_eq_7886  (alias)
 *   __libcgj_batch7886_marker = "libcgj-batch7886"
 *
 * Exclusive continuum CREATE-ONLY (7881-7890: strcoll/strxfrm stubs).
 * Unique gj_strcoll_eq_7886 surface only; no multi-def. Distinct from
 * gj_strcoll_c_7881 (signed compare). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7886_marker[] = "libcgj-batch7886";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b7886_eq(const unsigned char *pA, const unsigned char *pB)
{
	size_t i;

	i = 0u;
	while (pA[i] != 0u && pA[i] == pB[i]) {
		i++;
	}
	return (pA[i] == pB[i]) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcoll_eq_7886 - true if C-locale collate considers a and b equal.
 *
 * Soft freestanding predicate only. No parent wires.
 */
int
gj_strcoll_eq_7886(const char *a, const char *b)
{
	(void)NULL;
	if (a == NULL || b == NULL) {
		return (a == b) ? 1 : 0;
	}
	if (a == b) {
		return 1;
	}
	return b7886_eq((const unsigned char *)a,
	    (const unsigned char *)b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_strcoll_eq_7886(const char *a, const char *b)
    __attribute__((alias("gj_strcoll_eq_7886")));
