/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch332: freestanding strict UUID parse (canonical
 * 8-4-4-4-12 hyphenated hex only).
 *
 * Surface (unique symbols only — does NOT redefine batch116/36 UUID APIs):
 *   int gj_uuid_parse_strict(const char *s, uint8_t out[16]);
 *     — Parse exactly xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (36 chars + NUL).
 *       Hyphens fixed at offsets 8,13,18,23. Hex digits case-insensitive.
 *       Returns 0 on success, -1 on NULL args or any format deviation.
 *   int __gj_uuid_parse_strict  (alias)
 *   __libcgj_batch332_marker = "libcgj-batch332"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_uuid_parse / gj_uuid_unparse       → graph_batch116.c
 *   uuid_parse / uuid_unparse             → graph_batch36.c
 *   gj_uuid_v4_format                     → graph_batch262.c
 *   gj_uuid_v7_set_timestamp / version    → graph_batch163.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party UUID source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch332_marker[] = "libcgj-batch332";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b332_hex_nibble(unsigned char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0');
	}
	if (ch >= 'a' && ch <= 'f') {
		return (int)(ch - 'a' + 10);
	}
	if (ch >= 'A' && ch <= 'F') {
		return (int)(ch - 'A' + 10);
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_parse_strict — parse canonical hyphenated UUID text into 16 octets.
 *
 * Accepts only the strict 8-4-4-4-12 form (exactly 36 characters, then NUL).
 * Rejects braces, URN prefixes, missing/extra hyphens, short/long strings,
 * and non-hex content. Case-insensitive A–F/a–f.
 *
 * Returns 0 on success, -1 on error.
 */
int
gj_uuid_parse_strict(const char *s, uint8_t out[16])
{
	unsigned i;
	unsigned nNib = 0u;
	unsigned char b = 0u;
	/* required hyphen character offsets in the 36-char form */
	static const unsigned aHyphenAt[] = { 8u, 13u, 18u, 23u };
	unsigned uPos;

	if (s == NULL || out == NULL) {
		return -1;
	}

	/* clear output first */
	for (i = 0u; i < 16u; i++) {
		out[i] = 0u;
	}

	/* must be exactly 36 form chars, no extra trailer */
	for (uPos = 0u; uPos < 36u; uPos++) {
		if (s[uPos] == '\0') {
			return -1;
		}
	}
	if (s[36] != '\0') {
		return -1;
	}

	/* hyphens at fixed offsets only */
	for (i = 0u; i < 4u; i++) {
		if (s[aHyphenAt[i]] != '-') {
			return -1;
		}
	}

	i = 0u;
	nNib = 0u;
	for (uPos = 0u; uPos < 36u; uPos++) {
		int n;
		unsigned char ch = (unsigned char)s[uPos];

		if (ch == (unsigned char)'-') {
			/* only the four fixed positions may be hyphens */
			if (uPos != 8u && uPos != 13u && uPos != 18u &&
			    uPos != 23u) {
				return -1;
			}
			continue;
		}
		n = b332_hex_nibble(ch);
		if (n < 0) {
			return -1;
		}
		if ((nNib & 1u) == 0u) {
			b = (unsigned char)((unsigned)n << 4);
		} else {
			b = (unsigned char)(b | (unsigned char)n);
			if (i >= 16u) {
				return -1;
			}
			out[i++] = (uint8_t)b;
		}
		nNib++;
	}

	if (i != 16u || nNib != 32u) {
		return -1;
	}
	return 0;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_uuid_parse_strict(const char *s, uint8_t out[16])
    __attribute__((alias("gj_uuid_parse_strict")));
