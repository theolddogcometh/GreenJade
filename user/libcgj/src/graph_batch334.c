/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch334: IPv6 text → 16-octet binary (basic ::).
 *
 * Surface (unique symbols — gj_* only; do NOT redefine inet_pton):
 *   int gj_inet6_pton(const char *s, uint8_t out[16]);
 *     — Parse IPv6 presentation into 16 network-order octets.
 *       Returns 1 on success, 0 on invalid presentation, -1 on NULL args.
 *   __gj_inet6_pton  (alias)
 *   __libcgj_batch334_marker = "libcgj-batch334"
 *
 * Presentation accepted (basic):
 *   - 1..8 hextets of 1..4 hex digits (0-9a-fA-F), colon-separated
 *   - single "::" zero-run compression (leading / middle / trailing)
 *   - bare "::" → all zeros
 *   - no embedded IPv4 dotted-quad (use a fuller pton for that)
 *
 * Note: batch156 also exports gj_inet6_pton / __gj_inet6_pton (with
 * embedded IPv4 tail). When both TUs are linked, resolve the shared
 * name at the product level (map / weak / rename); this TU stands alone
 * as specified.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch334_marker[] = "libcgj-batch334";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b334_hex_val(unsigned char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0');
	}
	if (ch >= 'a' && ch <= 'f') {
		return (int)(ch - 'a') + 10;
	}
	if (ch >= 'A' && ch <= 'F') {
		return (int)(ch - 'A') + 10;
	}
	return -1;
}

static void
b334_zero16(uint8_t *pOut)
{
	unsigned iByte;

	for (iByte = 0u; iByte < 16u; iByte++) {
		pOut[iByte] = 0u;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inet6_pton — IPv6 text → 16 network-order octets.
 *
 * Supports basic hextet form and a single "::" compression run.
 *
 * Examples (success → 1):
 *   "::"                         all zeros
 *   "::1"                        loopback
 *   "2001:db8::1"                compressed middle
 *   "fe80::"                     trailing ::
 *   "2001:0db8:0000:0000:0000:0000:0000:0001"
 *
 * Rejects empty, second "::", leading single ':', trailing single ':',
 * more than 4 hex digits per hextet, and wrong hextet counts.
 */
int
gj_inet6_pton(const char *s, uint8_t out[16])
{
	uint16_t aWord[8];
	int nWords;
	int nCompress; /* hextet index where :: expands; -1 = none */
	const char *pCur;
	int iWord;

	if (s == NULL || out == NULL) {
		return -1;
	}

	for (iWord = 0; iWord < 8; iWord++) {
		aWord[iWord] = 0u;
	}

	nWords = 0;
	nCompress = -1;
	pCur = s;

	/* Leading "::" */
	if (pCur[0] == ':' && pCur[1] == ':') {
		nCompress = 0;
		pCur += 2;
		if (*pCur == '\0') {
			/* Bare "::" → all zeros. */
			b334_zero16(out);
			return 1;
		}
	} else if (pCur[0] == ':') {
		/* Leading single ':' is invalid. */
		return 0;
	}

	while (*pCur != '\0' && nWords < 8) {
		unsigned uVal;
		int nDig;
		int nHv;

		if (*pCur == ':') {
			/* "::" compression (exactly once). */
			if (nCompress >= 0) {
				return 0; /* second :: */
			}
			nCompress = nWords;
			pCur++;
			if (*pCur == '\0') {
				break; /* trailing :: */
			}
			continue;
		}

		uVal = 0u;
		nDig = 0;
		while ((nHv = b334_hex_val((unsigned char)*pCur)) >= 0 &&
		    nDig < 4) {
			uVal = (uVal << 4) | (unsigned)nHv;
			pCur++;
			nDig++;
		}
		if (nDig == 0) {
			return 0;
		}
		/* Reject trailing junk on a hextet (must be ':' or end). */
		if (*pCur != '\0' && *pCur != ':') {
			return 0;
		}

		aWord[nWords++] = (uint16_t)uVal;
		if (*pCur == '\0') {
			break;
		}
		/* *pCur == ':' */
		pCur++;
		if (*pCur == '\0') {
			/* Trailing single ':' is invalid. */
			return 0;
		}
	}

	if (*pCur != '\0') {
		return 0;
	}

	if (nCompress >= 0) {
		int nTail = nWords - nCompress;
		int nZ = 8 - nWords;
		int iShift;

		if (nZ < 0) {
			return 0;
		}
		/* Shift tail words right; zero the expanded gap. */
		for (iShift = nTail - 1; iShift >= 0; iShift--) {
			aWord[nCompress + nZ + iShift] =
			    aWord[nCompress + iShift];
		}
		for (iShift = 0; iShift < nZ; iShift++) {
			aWord[nCompress + iShift] = 0u;
		}
		nWords = 8;
	}

	if (nWords != 8) {
		return 0;
	}

	for (iWord = 0; iWord < 8; iWord++) {
		out[iWord * 2] = (uint8_t)(aWord[iWord] >> 8);
		out[iWord * 2 + 1] = (uint8_t)(aWord[iWord] & 0xffu);
	}
	return 1;
}

int __gj_inet6_pton(const char *s, uint8_t out[16])
    __attribute__((alias("gj_inet6_pton")));
