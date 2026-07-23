/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch165: Bech32 encode / decode lite (BIP-0173).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2; this TU does not require SSE). No third-party BIP
 * reference implementation or wallet source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   base32_encode / base32_decode           → graph_batch41.c (RFC 4648)
 *   crockford32_encode / crockford32_decode → graph_batch119.c
 *   base64 / base85 / z85 / base58…         → earlier graph batches
 *
 * This TU adds only unique symbols (BIP-0173 Bech32, not Bech32m):
 *   int gj_bech32_encode(const char *hrp, const unsigned char *data,
 *                        size_t dlen, char *out, size_t cap);
 *     Encode lowercase Bech32: hrp + '1' + charset(data||checksum6).
 *     data[i] must each be a 5-bit value 0..31 (caller converts 8→5).
 *     Total string length must be <= 90 (BIP-0173). out receives the
 *     NUL-terminated string; cap must fit length+1. Returns 0 / -1.
 *
 *   int gj_bech32_decode(const char *str, char *hrp, size_t hcap,
 *                        unsigned char *data, size_t dcap, size_t *dlen);
 *     Decode a Bech32 string. Rejects mixed case, non-printable, bad
 *     charset, missing separator, truncated HRP/payload, length > 90,
 *     or failed polymod checksum (constant 1 = classic Bech32).
 *     On success: lowercase HRP into hrp (NUL-terminated), payload
 *     5-bit values into data (checksum stripped), *dlen set. 0 / -1.
 *
 *   __gj_bech32_encode / __gj_bech32_decode  (aliases)
 *   __libcgj_batch165_marker = "libcgj-batch165"
 *
 * BIP-0173 notes (informative):
 *   charset  = "qpzry9x8gf2tvdw0s3jn54khce6mua7l"
 *   separator = '1'  (last occurrence splits HRP from data+checksum)
 *   checksum  = 6×5-bit symbols; polymod(hrp_expand||data||chk) == 1
 *   HRP chars in ASCII 33..126; encoders emit all-lowercase.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch165_marker[] = "libcgj-batch165";

/* BIP-0173 Bech32 character set (index == 5-bit value). */
static const char s_b165_charset[32] =
    "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

/* Polymod generator coefficients (BIP-0173). */
static const uint32_t s_b165_gen[5] = {
	0x3b6a57b2u,
	0x26508e6du,
	0x1ea119fau,
	0x3d4233ddu,
	0x2a1462b3u
};

/* Max BIP-0173 string length (including HRP, separator, payload, chk). */
#define B165_MAX_LEN 90u

/* Max polymod values: 2*hrp + 1 + data + 6 ≤ ~2*83+1+6 ≈ 173. */
#define B165_POLY_MAX 256u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b165_strlen(const char *sz)
{
	size_t n = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static int
b165_is_lower(unsigned char ch)
{
	return (ch >= 'a' && ch <= 'z') ? 1 : 0;
}

static int
b165_is_upper(unsigned char ch)
{
	return (ch >= 'A' && ch <= 'Z') ? 1 : 0;
}

static unsigned char
b165_to_lower(unsigned char ch)
{
	if (b165_is_upper(ch) != 0) {
		return (unsigned char)(ch + ('a' - 'A'));
	}
	return ch;
}

/*
 * Map charset glyph → 5-bit value (0..31), or -1 if not in charset.
 * Linear scan is fine: 32 symbols, decode is not a hot path.
 */
static int
b165_charset_val(unsigned char ch)
{
	unsigned i;

	for (i = 0u; i < 32u; i++) {
		if ((unsigned char)s_b165_charset[i] == ch) {
			return (int)i;
		}
	}
	return -1;
}

/*
 * BIP-0173 polymod over 5-bit values. chk is a 30-bit BCH state.
 */
static uint32_t
b165_polymod(const unsigned char *pVal, size_t cVal)
{
	uint32_t uChk = 1u;
	size_t i;

	for (i = 0u; i < cVal; i++) {
		uint32_t uB = uChk >> 25;
		unsigned j;

		uChk = ((uChk & 0x1ffffffu) << 5) ^ (uint32_t)pVal[i];
		for (j = 0u; j < 5u; j++) {
			if ((uB >> j) & 1u) {
				uChk ^= s_b165_gen[j];
			}
		}
	}
	return uChk;
}

/*
 * Expand HRP into 5-bit polymod inputs: high3(chars) || 0 || low5(chars).
 * Returns number of values written, or (size_t)-1 if cap too small.
 */
static size_t
b165_hrp_expand(const char *szHrp, size_t cHrp,
                unsigned char *pOut, size_t cCap)
{
	size_t i;
	size_t cNeed = cHrp + 1u + cHrp;

	if (cNeed > cCap) {
		return (size_t)-1;
	}
	for (i = 0u; i < cHrp; i++) {
		pOut[i] = (unsigned char)(((unsigned char)szHrp[i]) >> 5);
	}
	pOut[cHrp] = 0u;
	for (i = 0u; i < cHrp; i++) {
		pOut[cHrp + 1u + i] =
		    (unsigned char)(((unsigned char)szHrp[i]) & 31u);
	}
	return cNeed;
}

/*
 * Create 6-symbol Bech32 checksum (polymod constant 1) into aChk[6].
 * pScratch holds hrp_expand || data || 6 zero pads for the polymod pass.
 */
static int
b165_create_checksum(const char *szHrp, size_t cHrp,
                     const unsigned char *pData, size_t cData,
                     unsigned char aChk[6])
{
	unsigned char aPoly[B165_POLY_MAX];
	size_t cExp;
	size_t cBase;
	size_t i;
	uint32_t uMod;

	cExp = b165_hrp_expand(szHrp, cHrp, aPoly, B165_POLY_MAX);
	if (cExp == (size_t)-1) {
		return -1;
	}
	if (cExp + cData + 6u > B165_POLY_MAX) {
		return -1;
	}
	for (i = 0u; i < cData; i++) {
		aPoly[cExp + i] = pData[i];
	}
	cBase = cExp + cData;
	for (i = 0u; i < 6u; i++) {
		aPoly[cBase + i] = 0u;
	}

	uMod = b165_polymod(aPoly, cBase + 6u) ^ 1u;
	for (i = 0u; i < 6u; i++) {
		aChk[i] = (unsigned char)((uMod >> (5u * (5u - i))) & 31u);
	}
	return 0;
}

/*
 * Verify Bech32 checksum: polymod(hrp_expand || data_incl_chk) == 1.
 */
static int
b165_verify_checksum(const char *szHrp, size_t cHrp,
                     const unsigned char *pData, size_t cData)
{
	unsigned char aPoly[B165_POLY_MAX];
	size_t cExp;
	size_t i;

	cExp = b165_hrp_expand(szHrp, cHrp, aPoly, B165_POLY_MAX);
	if (cExp == (size_t)-1) {
		return 0;
	}
	if (cExp + cData > B165_POLY_MAX) {
		return 0;
	}
	for (i = 0u; i < cData; i++) {
		aPoly[cExp + i] = pData[i];
	}
	return (b165_polymod(aPoly, cExp + cData) == 1u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bech32_encode — BIP-0173 Bech32 encode (classic checksum constant 1).
 *
 * hrp: human-readable part (ASCII 33..126); emitted lowercase.
 * data[dlen]: 5-bit payload values (each 0..31); no 8→5 conversion here.
 * out/cap: destination buffer (NUL-terminated on success).
 *
 * Returns 0 on success, -1 on NULL args, empty/invalid HRP, data value
 * out of range, total length > 90, or insufficient cap.
 */
int
gj_bech32_encode(const char *hrp, const unsigned char *data, size_t dlen,
                 char *out, size_t cap)
{
	size_t cHrp;
	size_t cTotal;
	size_t i;
	unsigned char aChk[6];
	char aHrpLow[B165_MAX_LEN];

	if (hrp == NULL || out == NULL) {
		return -1;
	}
	/* data may be NULL only when dlen == 0 (empty payload + checksum). */
	if (data == NULL && dlen != 0u) {
		return -1;
	}

	cHrp = b165_strlen(hrp);
	if (cHrp < 1u || cHrp > B165_MAX_LEN) {
		return -1;
	}

	/* Validate HRP printable range; build lowercase copy for emit. */
	for (i = 0u; i < cHrp; i++) {
		unsigned char ch = (unsigned char)hrp[i];

		if (ch < 33u || ch > 126u) {
			return -1;
		}
		aHrpLow[i] = (char)b165_to_lower(ch);
	}

	/* Each payload unit is a 5-bit group. */
	for (i = 0u; i < dlen; i++) {
		if (data[i] > 31u) {
			return -1;
		}
	}

	/* hrp + '1' + data + 6 checksum symbols */
	cTotal = cHrp + 1u + dlen + 6u;
	if (cTotal > B165_MAX_LEN) {
		return -1;
	}
	if (cap < cTotal + 1u) {
		return -1;
	}

	if (b165_create_checksum(aHrpLow, cHrp, data, dlen, aChk) != 0) {
		return -1;
	}

	for (i = 0u; i < cHrp; i++) {
		out[i] = aHrpLow[i];
	}
	out[cHrp] = '1';
	for (i = 0u; i < dlen; i++) {
		out[cHrp + 1u + i] = s_b165_charset[data[i] & 31u];
	}
	for (i = 0u; i < 6u; i++) {
		out[cHrp + 1u + dlen + i] = s_b165_charset[aChk[i] & 31u];
	}
	out[cTotal] = '\0';
	return 0;
}

int __gj_bech32_encode(const char *hrp, const unsigned char *data,
                       size_t dlen, char *out, size_t cap)
    __attribute__((alias("gj_bech32_encode")));

/*
 * gj_bech32_decode — BIP-0173 Bech32 decode with checksum verification.
 *
 * str: full Bech32 string (NUL-terminated). Mixed case rejected.
 * hrp/hcap: lowercase HRP + NUL; hcap must be cHrp+1.
 * data/dcap/dlen: 5-bit payload (checksum not included).
 *
 * Returns 0 on success, -1 on any validation / buffer failure.
 */
int
gj_bech32_decode(const char *str, char *hrp, size_t hcap,
                 unsigned char *data, size_t dcap, size_t *dlen)
{
	size_t cStr;
	size_t i;
	size_t iSep;
	size_t cHrp;
	size_t cTail;
	size_t cPayload;
	int fLower = 0;
	int fUpper = 0;
	char aHrpLow[B165_MAX_LEN];
	unsigned char aVals[B165_MAX_LEN];

	if (str == NULL || hrp == NULL || dlen == NULL) {
		return -1;
	}
	/* data may be NULL only when caller only wants length via empty
	 * payload path — still require a buffer when payload is present.
	 * Require data non-NULL always for a simple strict API. */
	if (data == NULL) {
		return -1;
	}

	cStr = b165_strlen(str);
	if (cStr < 8u || cStr > B165_MAX_LEN) {
		/* min: 1 HRP + '1' + 6 checksum */
		return -1;
	}

	/* No mixed case; all chars printable ASCII 33..126. */
	for (i = 0u; i < cStr; i++) {
		unsigned char ch = (unsigned char)str[i];

		if (ch < 33u || ch > 126u) {
			return -1;
		}
		if (b165_is_lower(ch) != 0) {
			fLower = 1;
		}
		if (b165_is_upper(ch) != 0) {
			fUpper = 1;
		}
	}
	if (fLower != 0 && fUpper != 0) {
		return -1;
	}

	/* Find last '1' separator (after case-fold). */
	iSep = (size_t)-1;
	for (i = 0u; i < cStr; i++) {
		unsigned char ch = b165_to_lower((unsigned char)str[i]);

		if (ch == (unsigned char)'1') {
			iSep = i;
		}
	}
	if (iSep == (size_t)-1 || iSep < 1u) {
		return -1;
	}
	/* Need at least 6 checksum symbols after separator. */
	if (iSep + 7u > cStr) {
		return -1;
	}

	cHrp = iSep;
	cTail = cStr - iSep - 1u; /* data + checksum */
	if (cTail < 6u) {
		return -1;
	}
	cPayload = cTail - 6u;

	/* Lowercase HRP for expand + output. */
	for (i = 0u; i < cHrp; i++) {
		aHrpLow[i] = (char)b165_to_lower((unsigned char)str[i]);
	}

	/* Decode data+checksum charset symbols to 5-bit values. */
	for (i = 0u; i < cTail; i++) {
		unsigned char ch =
		    b165_to_lower((unsigned char)str[iSep + 1u + i]);
		int nVal = b165_charset_val(ch);

		if (nVal < 0) {
			return -1;
		}
		aVals[i] = (unsigned char)nVal;
	}

	if (b165_verify_checksum(aHrpLow, cHrp, aVals, cTail) == 0) {
		return -1;
	}

	if (hcap < cHrp + 1u) {
		return -1;
	}
	if (dcap < cPayload) {
		return -1;
	}

	for (i = 0u; i < cHrp; i++) {
		hrp[i] = aHrpLow[i];
	}
	hrp[cHrp] = '\0';

	for (i = 0u; i < cPayload; i++) {
		data[i] = aVals[i];
	}
	*dlen = cPayload;
	return 0;
}

int __gj_bech32_decode(const char *str, char *hrp, size_t hcap,
                       unsigned char *data, size_t dcap, size_t *dlen)
    __attribute__((alias("gj_bech32_decode")));
