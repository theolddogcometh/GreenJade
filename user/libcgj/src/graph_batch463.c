/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch463: freestanding BIP-173 Bech32 encode (short API).
 *
 * Surface (unique symbols — distinct from batch165 encode/decode wrappers):
 *   int gj_bech32_enc(const char *hrp, const uint8_t *data5, size_t n,
 *                     char *out, size_t cap);
 *     Encode lowercase Bech32: hrp + '1' + charset(data5||checksum6).
 *     data5[i] must each be a 5-bit value 0..31 (caller converts 8→5).
 *     Total string length must be <= 90 (BIP-0173). out receives the
 *     NUL-terminated string; cap must fit length+1. Returns 0 / -1.
 *   int __gj_bech32_enc  (alias)
 *   __libcgj_batch463_marker = "libcgj-batch463"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_bech32_encode / gj_bech32_decode → graph_batch165.c
 *   gj_bech32_polymod / gj_bech32_charset_idx → graph_batch299.c
 *
 * BIP-0173 notes (informative):
 *   charset  = "qpzry9x8gf2tvdw0s3jn54khce6mua7l"
 *   separator = '1'  (last occurrence splits HRP from data+checksum)
 *   checksum  = 6×5-bit symbols; polymod(hrp_expand||data||chk) == 1
 *   HRP chars in ASCII 33..126; encoders emit all-lowercase.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party BIP reference implementation or wallet source was
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch463_marker[] = "libcgj-batch463";

/* BIP-0173 Bech32 character set (index == 5-bit value). */
static const char s_b463_charset[32] =
    "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

/* Polymod generator coefficients (BIP-0173). */
static const uint32_t s_b463_gen[5] = {
	0x3b6a57b2u,
	0x26508e6du,
	0x1ea119fau,
	0x3d4233ddu,
	0x2a1462b3u
};

/* Max BIP-0173 string length (including HRP, separator, payload, chk). */
#define B463_MAX_LEN 90u

/* Max polymod values: 2*hrp + 1 + data + 6 ≤ ~2*83+1+6 ≈ 173. */
#define B463_POLY_MAX 256u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b463_strlen(const char *sz)
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

static unsigned char
b463_to_lower(unsigned char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (unsigned char)(ch + ('a' - 'A'));
	}
	return ch;
}

/*
 * BIP-0173 polymod over 5-bit values. chk is a 30-bit BCH state.
 */
static uint32_t
b463_polymod(const uint8_t *pVal, size_t cVal)
{
	uint32_t uChk = 1u;
	size_t i;

	for (i = 0u; i < cVal; i++) {
		uint32_t uB = uChk >> 25;
		unsigned j;

		uChk = ((uChk & 0x1ffffffu) << 5) ^ (uint32_t)pVal[i];
		for (j = 0u; j < 5u; j++) {
			if ((uB >> j) & 1u) {
				uChk ^= s_b463_gen[j];
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
b463_hrp_expand(const char *szHrp, size_t cHrp,
                uint8_t *pOut, size_t cCap)
{
	size_t i;
	size_t cNeed = cHrp + 1u + cHrp;

	if (cNeed > cCap) {
		return (size_t)-1;
	}
	for (i = 0u; i < cHrp; i++) {
		pOut[i] = (uint8_t)(((unsigned char)szHrp[i]) >> 5);
	}
	pOut[cHrp] = 0u;
	for (i = 0u; i < cHrp; i++) {
		pOut[cHrp + 1u + i] =
		    (uint8_t)(((unsigned char)szHrp[i]) & 31u);
	}
	return cNeed;
}

/*
 * Create 6-symbol Bech32 checksum (polymod constant 1) into aChk[6].
 */
static int
b463_create_checksum(const char *szHrp, size_t cHrp,
                     const uint8_t *pData, size_t cData,
                     uint8_t aChk[6])
{
	uint8_t aPoly[B463_POLY_MAX];
	size_t cExp;
	size_t cBase;
	size_t i;
	uint32_t uMod;

	cExp = b463_hrp_expand(szHrp, cHrp, aPoly, B463_POLY_MAX);
	if (cExp == (size_t)-1) {
		return -1;
	}
	if (cExp + cData + 6u > B463_POLY_MAX) {
		return -1;
	}
	for (i = 0u; i < cData; i++) {
		aPoly[cExp + i] = pData[i];
	}
	cBase = cExp + cData;
	for (i = 0u; i < 6u; i++) {
		aPoly[cBase + i] = 0u;
	}

	uMod = b463_polymod(aPoly, cBase + 6u) ^ 1u;
	for (i = 0u; i < 6u; i++) {
		aChk[i] = (uint8_t)((uMod >> (5u * (5u - i))) & 31u);
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bech32_enc — BIP-0173 Bech32 encode (classic checksum constant 1).
 *
 * hrp: human-readable part (ASCII 33..126); emitted lowercase.
 * data5[n]: 5-bit payload values (each 0..31); no 8→5 conversion here.
 * out/cap: destination buffer (NUL-terminated on success).
 *
 * Returns 0 on success, -1 on NULL args, empty/invalid HRP, data value
 * out of range, total length > 90, or insufficient cap.
 *
 * Does NOT redefine gj_bech32_encode (graph_batch165.c).
 */
int
gj_bech32_enc(const char *hrp, const uint8_t *data5, size_t n,
              char *out, size_t cap)
{
	size_t cHrp;
	size_t cTotal;
	size_t i;
	uint8_t aChk[6];
	char aHrpLow[B463_MAX_LEN];

	if (hrp == NULL || out == NULL) {
		return -1;
	}
	/* data5 may be NULL only when n == 0 (empty payload + checksum). */
	if (data5 == NULL && n != 0u) {
		return -1;
	}

	cHrp = b463_strlen(hrp);
	if (cHrp < 1u || cHrp > B463_MAX_LEN) {
		return -1;
	}

	/* Validate HRP printable range; build lowercase copy for emit. */
	for (i = 0u; i < cHrp; i++) {
		unsigned char ch = (unsigned char)hrp[i];

		if (ch < 33u || ch > 126u) {
			return -1;
		}
		aHrpLow[i] = (char)b463_to_lower(ch);
	}

	/* Each payload unit is a 5-bit group. */
	for (i = 0u; i < n; i++) {
		if (data5[i] > 31u) {
			return -1;
		}
	}

	/* hrp + '1' + data + 6 checksum symbols */
	cTotal = cHrp + 1u + n + 6u;
	if (cTotal > B463_MAX_LEN) {
		return -1;
	}
	if (cap < cTotal + 1u) {
		return -1;
	}

	if (b463_create_checksum(aHrpLow, cHrp, data5, n, aChk) != 0) {
		return -1;
	}

	for (i = 0u; i < cHrp; i++) {
		out[i] = aHrpLow[i];
	}
	out[cHrp] = '1';
	for (i = 0u; i < n; i++) {
		out[cHrp + 1u + i] = s_b463_charset[data5[i] & 31u];
	}
	for (i = 0u; i < 6u; i++) {
		out[cHrp + 1u + n + i] = s_b463_charset[aChk[i] & 31u];
	}
	out[cTotal] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bech32_enc(const char *hrp, const uint8_t *data5, size_t n,
                    char *out, size_t cap)
    __attribute__((alias("gj_bech32_enc")));
