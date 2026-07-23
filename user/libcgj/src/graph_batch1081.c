/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1081: repeat a string count times into an output buffer
 *
 * Surface (unique symbols):
 *   int gj_str_repeat(const char *s, size_t count, char *out, size_t cap);
 *     — Write s repeated count times into out as a NUL-terminated
 *       string. Returns length excluding NUL on success, or -1 if
 *       out == NULL, cap == 0, s == NULL with needed length > 0, or
 *       the result does not fit (including NUL).
 *   int __gj_str_repeat  (alias)

 *   __libcgj_batch1081_marker = "libcgj-batch1081"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1081_marker[] = "libcgj-batch1081";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1081_strlen(const char *sz)
{
	size_t n;

	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_repeat — write sz repeated cCount times into pOut.
 *
 * Edge cases:
 *   pOut == NULL or cbCap == 0 → -1
 *   sz == NULL and cCount > 0 → -1
 *   cCount == 0 → empty string (length 0) if cap >= 1
 *   overflow of length → -1
 */
int
gj_str_repeat(const char *sz, size_t cCount, char *pOut, size_t cbCap)
{
	size_t cbSrc;
	size_t cbNeed;
	size_t iRep;
	size_t iCh;
	size_t iOut;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	if (cCount == 0u) {
		pOut[0] = '\0';
		return 0;
	}

	if (sz == NULL) {
		return -1;
	}

	cbSrc = b1081_strlen(sz);
	if (cbSrc == 0u) {
		pOut[0] = '\0';
		return 0;
	}

	/* cbSrc * cCount + 1 must fit and not wrap. */
	if (cCount > (SIZE_MAX - 1u) / cbSrc) {
		return -1;
	}
	cbNeed = cbSrc * cCount + 1u;
	if (cbCap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	for (iRep = 0u; iRep < cCount; iRep++) {
		for (iCh = 0u; iCh < cbSrc; iCh++) {
			pOut[iOut++] = sz[iCh];
		}
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_repeat(const char *sz, size_t cCount, char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_repeat")));
