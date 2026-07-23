/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1322: first index of a character not in a reject set (find_first_not_of).
 *
 * Surface (unique symbols):
 *   size_t gj_str_find_first_not_of(const char *s, const char *reject);
 *     — Return the lowest index i where s[i] does not appear in reject.
 *       (size_t)-1 if every character is in reject, or s/reject NULL.
 *       Empty reject → 0 when s is non-empty (no forbidden bytes).
 *       Empty s → (size_t)-1.
 *   size_t __gj_str_find_first_not_of  (alias)
 *   __libcgj_batch1322_marker = "libcgj-batch1322"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1322_marker[] = "libcgj-batch1322";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1322_in_set(unsigned char ch, const char *szReject)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_find_first_not_of — first index of a character outside reject.
 */
size_t
gj_str_find_first_not_of(const char *sz, const char *szReject)
{
	size_t iIdx;
	const unsigned char *p;

	if (sz == NULL || szReject == NULL) {
		return (size_t)-1;
	}

	p = (const unsigned char *)sz;
	iIdx = 0u;
	while (p[iIdx] != 0u) {
		if (!b1322_in_set(p[iIdx], szReject)) {
			return iIdx;
		}
		iIdx++;
	}
	return (size_t)-1;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_find_first_not_of(const char *sz, const char *szReject)
    __attribute__((alias("gj_str_find_first_not_of")));
