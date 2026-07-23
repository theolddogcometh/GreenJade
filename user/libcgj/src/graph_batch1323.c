/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1323: last index of any character from an accept set (find_last_of).
 *
 * Surface (unique symbols):
 *   size_t gj_str_find_last_of(const char *s, const char *accept);
 *     — Return the highest index i where s[i] appears in accept.
 *       (size_t)-1 if none, or if s/accept is NULL. Empty accept/s → npos.
 *   size_t __gj_str_find_last_of  (alias)
 *   __libcgj_batch1323_marker = "libcgj-batch1323"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1323_marker[] = "libcgj-batch1323";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1323_in_set(unsigned char ch, const char *szAccept)
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

static size_t
b1323_strlen(const char *sz)
{
	size_t cb = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_find_last_of — last index of a character from accept, or npos.
 */
size_t
gj_str_find_last_of(const char *sz, const char *szAccept)
{
	size_t cb;
	size_t iIdx;
	const unsigned char *p;

	if (sz == NULL || szAccept == NULL) {
		return (size_t)-1;
	}
	if (szAccept[0] == '\0') {
		return (size_t)-1;
	}

	cb = b1323_strlen(sz);
	if (cb == 0u) {
		return (size_t)-1;
	}

	p = (const unsigned char *)sz;
	iIdx = cb;
	while (iIdx > 0u) {
		iIdx--;
		if (b1323_in_set(p[iIdx], szAccept)) {
			return iIdx;
		}
	}
	return (size_t)-1;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_find_last_of(const char *sz, const char *szAccept)
    __attribute__((alias("gj_str_find_last_of")));
