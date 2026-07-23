/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7885: ASCII case-insensitive strcoll stub.
 *
 * Surface (unique symbols):
 *   int gj_strcoll_ascii_ci_7885(const char *a, const char *b);
 *     - Collate NUL strings after folding A-Z to a-z. Non-ASCII octets
 *       compared as unsigned bytes without fold. NULL handling matches
 *       gj_strcoll_c_7881.
 *   int __gj_strcoll_ascii_ci_7885  (alias)
 *   __libcgj_batch7885_marker = "libcgj-batch7885"
 *
 * Exclusive continuum CREATE-ONLY (7881-7890: strcoll/strxfrm stubs).
 * Unique gj_strcoll_ascii_ci_7885 surface only; no multi-def. Distinct
 * from gj_strcoll_c_7881. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7885_marker[] = "libcgj-batch7885";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold ASCII uppercase A-Z to lowercase; leave other octets unchanged. */
static unsigned char
b7885_fold(unsigned char u)
{
	if (u >= (unsigned char)'A' && u <= (unsigned char)'Z') {
		return (unsigned char)(u + (unsigned char)('a' - 'A'));
	}
	return u;
}

static int
b7885_coll_ci(const unsigned char *pA, const unsigned char *pB)
{
	size_t i;
	unsigned char uA;
	unsigned char uB;

	i = 0u;
	for (;;) {
		uA = b7885_fold(pA[i]);
		uB = b7885_fold(pB[i]);
		if (uA != uB || uA == 0u) {
			return (int)uA - (int)uB;
		}
		i++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcoll_ascii_ci_7885 - ASCII case-insensitive collate stub.
 *
 * Soft freestanding only; not a full locale-aware strcasecmp. No parent
 * wires.
 */
int
gj_strcoll_ascii_ci_7885(const char *a, const char *b)
{
	(void)NULL;
	if (a == NULL || b == NULL) {
		if (a == b) {
			return 0;
		}
		return (a == NULL) ? -1 : 1;
	}
	return b7885_coll_ci((const unsigned char *)a,
	    (const unsigned char *)b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_strcoll_ascii_ci_7885(const char *a, const char *b)
    __attribute__((alias("gj_strcoll_ascii_ci_7885")));
