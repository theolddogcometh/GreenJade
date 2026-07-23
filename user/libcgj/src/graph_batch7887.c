/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7887: fixed-length memcoll C-locale stub.
 *
 * Surface (unique symbols):
 *   int gj_memcoll_c_7887(const void *a, const void *b, size_t n);
 *     - Collate n unsigned octets of a and b (no early NUL stop).
 *       n == 0 -> 0. NULL with n > 0: both NULL -> 0; only a NULL -> -1;
 *       only b NULL -> 1.
 *   int __gj_memcoll_c_7887  (alias)
 *   __libcgj_batch7887_marker = "libcgj-batch7887"
 *
 * Exclusive continuum CREATE-ONLY (7881-7890: strcoll/strxfrm stubs).
 * Unique gj_memcoll_c_7887 surface only; no multi-def. Distinct from
 * gj_strncoll_c_7883 (NUL-aware) and libc memcmp. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7887_marker[] = "libcgj-batch7887";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b7887_memcoll(const unsigned char *pA, const unsigned char *pB, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pA[i] != pB[i]) {
			return (int)pA[i] - (int)pB[i];
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcoll_c_7887 - fixed-length C-locale collate of raw memory.
 *
 * a, b: regions of at least n bytes when n > 0
 * n:    octet count (0 -> equal)
 *
 * Soft freestanding memcmp-shaped collate. No parent wires.
 */
int
gj_memcoll_c_7887(const void *a, const void *b, size_t n)
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
	if (a == b) {
		return 0;
	}
	return b7887_memcoll((const unsigned char *)a,
	    (const unsigned char *)b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_memcoll_c_7887(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_memcoll_c_7887")));
