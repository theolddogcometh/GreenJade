/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1321: first index of any character from an accept set (C++ find_first_of).
 *
 * Surface (unique symbols):
 *   size_t gj_str_find_first_of(const char *s, const char *accept);
 *     — Return the lowest index i where s[i] appears in accept (unsigned
 *       octet compare). (size_t)-1 if none, or if s/accept is NULL.
 *       Empty accept → (size_t)-1. Empty s → (size_t)-1.
 *   size_t __gj_str_find_first_of  (alias)
 *   __libcgj_batch1321_marker = "libcgj-batch1321"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1321_marker[] = "libcgj-batch1321";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1321_in_set(unsigned char ch, const char *szAccept)
{
	const unsigned char *p;

	if (szAccept == NULL) {
		return 0;
	}
	p = (const unsigned char *)szAccept;
	while (*p != 0u) {
		if (*p == ch) {
			return 1;
		}
		p++;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_find_first_of — first index of a character from accept, or npos.
 */
size_t
gj_str_find_first_of(const char *sz, const char *szAccept)
{
	size_t iIdx;
	const unsigned char *p;

	if (sz == NULL || szAccept == NULL) {
		return (size_t)-1;
	}
	if (szAccept[0] == '\0') {
		return (size_t)-1;
	}

	p = (const unsigned char *)sz;
	iIdx = 0u;
	while (p[iIdx] != 0u) {
		if (b1321_in_set(p[iIdx], szAccept)) {
			return iIdx;
		}
		iIdx++;
	}
	return (size_t)-1;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_find_first_of(const char *sz, const char *szAccept)
    __attribute__((alias("gj_str_find_first_of")));
