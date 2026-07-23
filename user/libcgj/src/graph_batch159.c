/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch159: UEFI GUID string parse / format.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Builds under userspace -msse2; this TU does not require SSE.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   uuid_parse / uuid_unparse / gj_uuid_parse / gj_uuid_unparse
 *     → graph_batch36.c / graph_batch116.c (RFC 4122 sequential octets)
 *   uuid_from_string / uuid_to_string → graph_batch35.c
 *
 * This TU adds only unique gj_* symbols for the UEFI mixed-endian layout:
 *   int  gj_guid_parse(const char *s, unsigned char guid[16]);
 *   void gj_guid_format(const unsigned char guid[16], char out[37]);
 *   __gj_guid_parse / __gj_guid_format  (aliases)
 *   __libcgj_batch159_marker = "libcgj-batch159"
 *
 * UEFI EFI_GUID on-wire / memory layout (LE fields for Data1/Data2/Data3):
 *   bytes 0..3  = Data1 little-endian
 *   bytes 4..5  = Data2 little-endian
 *   bytes 6..7  = Data3 little-endian
 *   bytes 8..15 = Data4[0..7] as written in the string
 *
 * Canonical text is hyphenated 8-4-4-4-12 hex (36 chars + NUL), the same
 * glyph form as RFC 4122 UUID strings, but the first three fields map to
 * little-endian memory rather than sequential network order.
 *
 * Example:
 *   text  "00112233-4455-6677-8899-aabbccddeeff"
 *   bytes  33 22 11 00  55 44  77 66  88 99 aa bb cc dd ee ff
 *
 * Parse accepts case-insensitive hex; format emits lowercase. Returns 0
 * on success, -1 on error (NULL args or bad format). No third-party UEFI
 * or UUID source was copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch159_marker[] = "libcgj-batch159";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b159_hex_nibble(unsigned char ch)
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
 * Read two hex nibbles from *ppSz into *pByte. Advances *ppSz by 2.
 * Returns 0 on success, -1 on non-hex.
 */
static int
b159_read_byte(const char **ppSz, unsigned char *pByte)
{
	int nHi;
	int nLo;

	nHi = b159_hex_nibble((unsigned char)(*ppSz)[0]);
	nLo = b159_hex_nibble((unsigned char)(*ppSz)[1]);
	if (nHi < 0 || nLo < 0) {
		return -1;
	}
	*pByte = (unsigned char)(((unsigned)nHi << 4) | (unsigned)nLo);
	*ppSz += 2;
	return 0;
}

/*
 * Format one byte as two lowercase hex digits into *ppOut; advances *ppOut.
 */
static void
b159_put_byte(char **ppOut, unsigned char uByte)
{
	static const char szDigits[] = "0123456789abcdef";

	(*ppOut)[0] = szDigits[(uByte >> 4) & 0xfu];
	(*ppOut)[1] = szDigits[uByte & 0xfu];
	*ppOut += 2;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_guid_parse — parse canonical UEFI GUID text into mixed-endian octets.
 *
 * Expects exactly: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (36 chars, NUL
 * terminated). Hyphens at offsets 8,13,18,23. Case-insensitive hex.
 *
 * Output layout matches EFI_GUID on a little-endian host:
 *   Data1 LE | Data2 LE | Data3 LE | Data4[8]
 *
 * Returns 0 on success, -1 on error.
 */
int
gj_guid_parse(const char *szIn, unsigned char aGuid[16])
{
	const char *p;
	unsigned char aSeq[16];
	unsigned i;
	unsigned uPos;
	static const unsigned aHyphenAt[] = { 8u, 13u, 18u, 23u };

	if (szIn == NULL || aGuid == NULL) {
		return -1;
	}

	for (i = 0u; i < 16u; i++) {
		aGuid[i] = 0u;
	}

	/* exactly 36 form characters, no extra trailer */
	for (uPos = 0u; uPos < 36u; uPos++) {
		if (szIn[uPos] == '\0') {
			return -1;
		}
	}
	if (szIn[36] != '\0') {
		return -1;
	}

	for (i = 0u; i < 4u; i++) {
		if (szIn[aHyphenAt[i]] != '-') {
			return -1;
		}
	}

	/*
	 * Read glyph fields left-to-right into sequential logical order,
	 * then rearrange into UEFI mixed-endian wire bytes.
	 * Field widths (bytes): 4, 2, 2, 2, 6  →  8-4-4-4-12 hex digits.
	 */
	p = szIn;
	for (i = 0u; i < 16u; i++) {
		/* skip hyphens when they appear between fields */
		if (*p == '-') {
			p++;
		}
		if (b159_read_byte(&p, &aSeq[i]) != 0) {
			return -1;
		}
	}
	if (*p != '\0') {
		return -1;
	}

	/* Data1: logical aSeq[0..3] → LE memory */
	aGuid[0] = aSeq[3];
	aGuid[1] = aSeq[2];
	aGuid[2] = aSeq[1];
	aGuid[3] = aSeq[0];
	/* Data2: logical aSeq[4..5] → LE */
	aGuid[4] = aSeq[5];
	aGuid[5] = aSeq[4];
	/* Data3: logical aSeq[6..7] → LE */
	aGuid[6] = aSeq[7];
	aGuid[7] = aSeq[6];
	/* Data4: aSeq[8..15] as-is */
	for (i = 8u; i < 16u; i++) {
		aGuid[i] = aSeq[i];
	}

	return 0;
}

/*
 * gj_guid_format — write lowercase hyphenated UEFI GUID into out[37].
 *
 * Input is EFI_GUID mixed-endian wire bytes (see gj_guid_parse).
 * out receives 36 characters + trailing NUL. NULL args are no-ops
 * (NULL out ignored; NULL guid → empty string if out non-NULL).
 */
void
gj_guid_format(const unsigned char aGuid[16], char aOut[37])
{
	char *p;
	unsigned i;

	if (aOut == NULL) {
		return;
	}
	if (aGuid == NULL) {
		aOut[0] = '\0';
		return;
	}

	p = aOut;

	/* Data1 LE → 8 hex digits big-endian glyph order */
	b159_put_byte(&p, aGuid[3]);
	b159_put_byte(&p, aGuid[2]);
	b159_put_byte(&p, aGuid[1]);
	b159_put_byte(&p, aGuid[0]);
	*p++ = '-';

	/* Data2 LE */
	b159_put_byte(&p, aGuid[5]);
	b159_put_byte(&p, aGuid[4]);
	*p++ = '-';

	/* Data3 LE */
	b159_put_byte(&p, aGuid[7]);
	b159_put_byte(&p, aGuid[6]);
	*p++ = '-';

	/* Data4[0..1] */
	b159_put_byte(&p, aGuid[8]);
	b159_put_byte(&p, aGuid[9]);
	*p++ = '-';

	/* Data4[2..7] */
	for (i = 10u; i < 16u; i++) {
		b159_put_byte(&p, aGuid[i]);
	}

	*p = '\0';
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_guid_parse(const char *szIn, unsigned char aGuid[16])
    __attribute__((alias("gj_guid_parse")));

void __gj_guid_format(const unsigned char aGuid[16], char aOut[37])
    __attribute__((alias("gj_guid_format")));
