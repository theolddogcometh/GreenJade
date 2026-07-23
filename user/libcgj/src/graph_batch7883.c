/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7883: bounded C-locale strncoll stub.
 *
 * Surface (unique symbols):
 *   int gj_strncoll_c_7883(const char *a, const char *b, size_t n);
 *     - Collate at most n unsigned octets of a and b (NUL ends early).
 *       n == 0 -> 0. NULL handling matches gj_strcoll_c_7881 when n > 0.
 *   int __gj_strncoll_c_7883  (alias)
 *   __libcgj_batch7883_marker = "libcgj-batch7883"
 *
 * Exclusive continuum CREATE-ONLY (7881-7890: strcoll/strxfrm stubs).
 * Unique gj_strncoll_c_7883 surface only; no multi-def. Distinct from
 * gj_strcoll_c_7881. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7883_marker[] = "libcgj-batch7883";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Bounded collate: compare up to n octets; stop at first mismatch or
 * NUL. Both pA and pB non-NULL; n > 0.
 */
static int
b7883_ncoll(const unsigned char *pA, const unsigned char *pB, size_t n)
{
	size_t i;
	unsigned char uA;
	unsigned char uB;

	for (i = 0u; i < n; i++) {
		uA = pA[i];
		uB = pB[i];
		if (uA != uB) {
			return (int)uA - (int)uB;
		}
		if (uA == 0u) {
			return 0;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncoll_c_7883 - bounded C-locale collate stub.
 *
 * a, b: strings (may be shorter than n; NUL ends compare)
 * n:    max octets to inspect (0 -> equal)
 *
 * Soft freestanding only. No parent wires.
 */
int
gj_strncoll_c_7883(const char *a, const char *b, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 0;
	}
	if (a == NULL || b == NULL) {
		if (a == b) {
			return 0;
		}
		return (a == NULL) ? -1 : 1;
	}
	return b7883_ncoll((const unsigned char *)a,
	    (const unsigned char *)b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_strncoll_c_7883(const char *a, const char *b, size_t n)
    __attribute__((alias("gj_strncoll_c_7883")));
