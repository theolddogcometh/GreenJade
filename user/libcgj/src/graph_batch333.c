/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch333: Ethernet MAC parse with colon or dash
 * separators.
 *
 * Surface (unique symbols for this TU's object file):
 *   int gj_mac_parse_strict(const char *s, uint8_t out[6]);
 *     — parse "aa:bb:cc:dd:ee:ff" or "aa-bb-cc-dd-ee-ff" (exactly 6×2
 *       hex digits; separators may be ':' or '-' between octets).
 *       Accepts mixed-case hex. Both nibbles required per octet.
 *       Must end at the sixth octet (no trailing junk). Returns 0 on
 *       success, -1 on failure or NULL args.
 *   __gj_mac_parse_strict  (alias)
 *   __libcgj_batch333_marker = "libcgj-batch333"
 *
 * Note: batch157 also exports gj_mac_parse_strict (colon-only). When both TUs
 * are linked, resolve the shared name at the product level (map / weak /
 * rename); this TU stands alone as specified.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch333_marker[] = "libcgj-batch333";

/* ---- hex helper -------------------------------------------------------- */

static int
b333_hex_nibble(int ch)
{
	if (ch >= '0' && ch <= '9') {
		return ch - '0';
	}
	if (ch >= 'a' && ch <= 'f') {
		return ch - 'a' + 10;
	}
	if (ch >= 'A' && ch <= 'F') {
		return ch - 'A' + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mac_parse_strict — strict MAC string → 6-byte binary.
 *
 * Accepts separators ':' or '-' between octets (per position). Requires
 * two hex digits per octet and a trailing NUL after the last octet.
 */
int
gj_mac_parse_strict(const char *s, uint8_t out[6])
{
	const char *pCur;
	int iOctet;

	if (s == NULL || out == NULL) {
		return -1;
	}

	pCur = s;
	for (iOctet = 0; iOctet < 6; iOctet++) {
		int nHi;
		int nLo;

		nHi = b333_hex_nibble((unsigned char)pCur[0]);
		nLo = b333_hex_nibble((unsigned char)pCur[1]);
		if (nHi < 0 || nLo < 0) {
			return -1;
		}
		out[iOctet] = (uint8_t)((nHi << 4) | nLo);
		pCur += 2;
		if (iOctet < 5) {
			char chSep = *pCur;

			if (chSep != ':' && chSep != '-') {
				return -1;
			}
			pCur++;
		}
	}
	if (*pCur != '\0') {
		return -1;
	}
	return 0;
}

int __gj_mac_parse_strict(const char *s, uint8_t out[6])
    __attribute__((alias("gj_mac_parse_strict")));
