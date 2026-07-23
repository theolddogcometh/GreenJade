/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch157: Ethernet MAC parse / format.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   ether_aton / ether_aton_r / ether_ntoa / ether_ntoa_r → ether.c
 *
 * This TU adds only unique symbols:
 *   int  gj_mac_parse(const char *s, unsigned char mac[6]);
 *     — parse "aa:bb:cc:dd:ee:ff" (exactly 6×2 hex digits, colon-sep).
 *       Returns 0 on success, -1 on failure. Strict: both nibbles required
 *       per octet; only ':' separators; must end at sixth octet (no junk).
 *   void gj_mac_format(const unsigned char mac[6], char out[18]);
 *     — write lowercase "aa:bb:cc:dd:ee:ff\0" into out[18].
 *   __gj_mac_parse / __gj_mac_format  (aliases)
 *   __libcgj_batch157_marker = "libcgj-batch157"
 */

#include <stddef.h>

const char __libcgj_batch157_marker[] = "libcgj-batch157";

/* ---- hex helpers ------------------------------------------------------- */

static int
b157_hex_nibble(int c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	return -1;
}

/* ---- parse / format ---------------------------------------------------- */

/*
 * gj_mac_parse — strict "aa:bb:cc:dd:ee:ff" → 6-byte MAC.
 * Accepts mixed-case hex; requires two digits per octet and ':' between.
 * Trailing non-NUL characters after the sixth octet are rejected.
 */
int
gj_mac_parse(const char *s, unsigned char mac[6])
{
	int i;
	const char *p;

	if (s == NULL || mac == NULL) {
		return -1;
	}
	p = s;
	for (i = 0; i < 6; i++) {
		int hi, lo;

		hi = b157_hex_nibble((unsigned char)p[0]);
		lo = b157_hex_nibble((unsigned char)p[1]);
		if (hi < 0 || lo < 0) {
			return -1;
		}
		mac[i] = (unsigned char)((hi << 4) | lo);
		p += 2;
		if (i < 5) {
			if (*p != ':') {
				return -1;
			}
			p++;
		}
	}
	if (*p != '\0') {
		return -1;
	}
	return 0;
}

/*
 * gj_mac_format — 6-byte MAC → lowercase "aa:bb:cc:dd:ee:ff\0" (18 bytes).
 * No-op if mac or out is NULL.
 */
void
gj_mac_format(const unsigned char mac[6], char out[18])
{
	static const char hex[] = "0123456789abcdef";
	int i;

	if (mac == NULL || out == NULL) {
		return;
	}
	for (i = 0; i < 6; i++) {
		unsigned v = mac[i];

		out[i * 3] = hex[(v >> 4) & 0xf];
		out[i * 3 + 1] = hex[v & 0xf];
		out[i * 3 + 2] = (i < 5) ? ':' : '\0';
	}
}

int __gj_mac_parse(const char *s, unsigned char mac[6])
    __attribute__((alias("gj_mac_parse")));
void __gj_mac_format(const unsigned char mac[6], char out[18])
    __attribute__((alias("gj_mac_format")));
