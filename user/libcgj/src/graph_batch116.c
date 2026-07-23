/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch116: freestanding gj_uuid parse/unparse helpers.
 * Clean-room pure C — integer/pointer only (no libc string). Userspace may
 * be built with -msse2; this TU does not require SSE.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   uuid_parse / uuid_unparse / uuid_unparse_lower / uuid_unparse_upper
 *     → graph_batch36.c (libuuid-shaped over batch35)
 *   uuid_from_string / uuid_to_string → graph_batch35.c
 *
 * This TU adds only unique gj_* symbols (uuid_parse exists; keep gj_* names):
 *   int  gj_uuid_parse(const char *in, unsigned char out[16]);
 *   void gj_uuid_unparse(const unsigned char in[16], char out[37]);
 *     — hyphenated 8-4-4-4-12 lowercase hex (36 chars + NUL)
 *   void gj_uuid_unparse_upper(const unsigned char in[16], char out[37]);
 *     — same layout, uppercase hex
 *   __gj_uuid_parse / __gj_uuid_unparse / __gj_uuid_unparse_upper (aliases)
 *   __libcgj_batch116_marker = "libcgj-batch116"
 *
 * Parse accepts the canonical 36-char form with hyphens at positions
 * 8,13,18,23 (case-insensitive hex). Returns 0 on success, -1 on error.
 * No third-party UUID source was copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch116_marker[] = "libcgj-batch116";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b116_hex_nibble(unsigned char ch)
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

/*
 * Format 16 UUID octets into szOut[37] as 8-4-4-4-12 with hyphens.
 * fUpper selects uppercase vs lowercase hex digits.
 */
static void
b116_unparse(const unsigned char aUu[16], char *szOut, int fUpper)
{
	static const char *szLo = "0123456789abcdef";
	static const char *szUp = "0123456789ABCDEF";
	const char *szDigits = fUpper ? szUp : szLo;
	/* dash after octet indices 3,5,7,9  → positions before 4,6,8,10 */
	static const unsigned aDashAfter[] = { 4u, 6u, 8u, 10u };
	unsigned i;
	unsigned j = 0u;

	if (szOut == NULL) {
		return;
	}
	if (aUu == NULL) {
		szOut[0] = '\0';
		return;
	}

	for (i = 0u; i < 16u; i++) {
		unsigned d;

		for (d = 0u; d < 4u; d++) {
			if (aDashAfter[d] == i) {
				szOut[j++] = '-';
			}
		}
		szOut[j++] = szDigits[(aUu[i] >> 4) & 0xfu];
		szOut[j++] = szDigits[aUu[i] & 0xfu];
	}
	szOut[j] = '\0';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * Parse canonical hyphenated UUID text into 16 octets.
 * Expects exactly: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (36 chars).
 * Returns 0 on success, -1 on error (NULL args or bad format).
 */
int
gj_uuid_parse(const char *szIn, unsigned char aOut[16])
{
	unsigned i;
	unsigned nNib = 0u;
	unsigned char b = 0u;
	/* required hyphen character offsets in the 36-char form */
	static const unsigned aHyphenAt[] = { 8u, 13u, 18u, 23u };
	unsigned uPos;

	if (szIn == NULL || aOut == NULL) {
		return -1;
	}

	/* clear output first */
	for (i = 0u; i < 16u; i++) {
		aOut[i] = 0u;
	}

	/* must be exactly 36 printable form chars, no extra trailer */
	for (uPos = 0u; uPos < 36u; uPos++) {
		if (szIn[uPos] == '\0') {
			return -1;
		}
	}
	if (szIn[36] != '\0') {
		return -1;
	}

	/* hyphens at fixed offsets */
	for (i = 0u; i < 4u; i++) {
		if (szIn[aHyphenAt[i]] != '-') {
			return -1;
		}
	}

	i = 0u;
	nNib = 0u;
	for (uPos = 0u; uPos < 36u; uPos++) {
		int n;
		unsigned char ch = (unsigned char)szIn[uPos];

		if (ch == (unsigned char)'-') {
			continue;
		}
		n = b116_hex_nibble(ch);
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
			aOut[i++] = b;
		}
		nNib++;
	}

	if (i != 16u || nNib != 32u) {
		return -1;
	}
	return 0;
}

/*
 * Write lowercase hyphenated UUID into out[37] (36 hex/hyphen + NUL).
 */
void
gj_uuid_unparse(const unsigned char aIn[16], char aOut[37])
{
	b116_unparse(aIn, aOut, 0);
}

/*
 * Write uppercase hyphenated UUID into out[37] (36 hex/hyphen + NUL).
 */
void
gj_uuid_unparse_upper(const unsigned char aIn[16], char aOut[37])
{
	b116_unparse(aIn, aOut, 1);
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_uuid_parse(const char *szIn, unsigned char aOut[16])
    __attribute__((alias("gj_uuid_parse")));

void __gj_uuid_unparse(const unsigned char aIn[16], char aOut[37])
    __attribute__((alias("gj_uuid_unparse")));

void __gj_uuid_unparse_upper(const unsigned char aIn[16], char aOut[37])
    __attribute__((alias("gj_uuid_unparse_upper")));
