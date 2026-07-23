/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1324: last index of a character not in a reject set (find_last_not_of).
 *
 * Surface (unique symbols):
 *   size_t gj_str_find_last_not_of(const char *s, const char *reject);
 *     — Return the highest index i where s[i] is not in reject.
 *       (size_t)-1 if every character is in reject, or s/reject NULL.
 *       Empty reject → strlen(s)-1 when s non-empty.
 *   size_t __gj_str_find_last_not_of  (alias)
 *   __libcgj_batch1324_marker = "libcgj-batch1324"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1324_marker[] = "libcgj-batch1324";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1324_in_set(unsigned char ch, const char *szReject)
{
	const unsigned char *p;

	if (szReject == NULL) {
		return 0;
	}
	p = (const unsigned char *)szReject;
	while (*p != 0u) {
		if (*p == ch) {
			return 1;
		}
		p++;
	}
	return 0;
}

static size_t
b1324_strlen(const char *sz)
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
 * gj_str_find_last_not_of — last index of a character outside reject.
 */
size_t
gj_str_find_last_not_of(const char *sz, const char *szReject)
{
	size_t cb;
	size_t iIdx;
	const unsigned char *p;

	if (sz == NULL || szReject == NULL) {
		return (size_t)-1;
	}

	cb = b1324_strlen(sz);
	if (cb == 0u) {
		return (size_t)-1;
	}

	p = (const unsigned char *)sz;
	iIdx = cb;
	while (iIdx > 0u) {
		iIdx--;
		if (!b1324_in_set(p[iIdx], szReject)) {
			return iIdx;
		}
	}
	return (size_t)-1;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_find_last_not_of(const char *sz, const char *szReject)
    __attribute__((alias("gj_str_find_last_not_of")));
