/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1325: last occurrence of a substring (C++ rfind).
 *
 * Surface (unique symbols):
 *   size_t gj_str_rfind(const char *hay, const char *needle);
 *     — Return the highest index where needle occurs as a contiguous
 *       substring of hay. Empty needle → strlen(hay). NULL hay/needle →
 *       (size_t)-1. No match → (size_t)-1.
 *   size_t __gj_str_rfind  (alias)
 *   __libcgj_batch1325_marker = "libcgj-batch1325"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1325_marker[] = "libcgj-batch1325";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1325_strlen(const char *sz)
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

static int
b1325_eq_n(const char *szA, const char *szB, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		if ((unsigned char)szA[i] != (unsigned char)szB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_rfind — last start index of needle in hay, or npos.
 */
size_t
gj_str_rfind(const char *szHay, const char *szNeedle)
{
	size_t cbHay;
	size_t cbNeedle;
	size_t iIdx;

	if (szHay == NULL || szNeedle == NULL) {
		return (size_t)-1;
	}

	cbHay = b1325_strlen(szHay);
	cbNeedle = b1325_strlen(szNeedle);

	if (cbNeedle == 0u) {
		return cbHay;
	}
	if (cbNeedle > cbHay) {
		return (size_t)-1;
	}

	iIdx = cbHay - cbNeedle + 1u;
	while (iIdx > 0u) {
		iIdx--;
		if (b1325_eq_n(szHay + iIdx, szNeedle, cbNeedle)) {
			return iIdx;
		}
	}
	return (size_t)-1;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_rfind(const char *szHay, const char *szNeedle)
    __attribute__((alias("gj_str_rfind")));
