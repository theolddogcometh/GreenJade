/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch156: IPv6 text ↔ binary (pton / ntop).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party inet source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   inet_pton / inet_ntop     → inet.c  (AF_INET + AF_INET6 dispatcher)
 *   inet_aton / inet_ntoa     → inet.c
 *   inet_net_pton / ntop      → graph_batch17.c
 *   __inet_pton_length        → graph_batch25.c
 *
 * This TU adds only unique symbols (gj_* names only — no inet_pton*):
 *   int gj_inet_pton6(const char *src, unsigned char dst[16]);
 *     Text → 16-octet network-order IPv6. Returns 1 on success, 0 on
 *     invalid presentation, -1 on NULL args.
 *   int gj_inet_ntop6(const unsigned char src[16], char *dst, size_t size);
 *     16-octet binary → presentation (RFC 5952-style :: compression,
 *     lowercase hex). Returns 0 on success, -1 on NULL args or size
 *     too small (needs at most 46 bytes including trailing NUL).
 *   __gj_inet_pton6 / __gj_inet_ntop6  (aliases)
 *   __libcgj_batch156_marker = "libcgj-batch156"
 *
 * Presentation accepted by pton6:
 *   - 1..8 hextets of 1..4 hex digits, colon-separated
 *   - single "::" zero-run compression
 *   - trailing dotted-quad IPv4 (e.g. ::ffff:192.0.2.1) → last 32 bits
 *   - empty string and malformed forms rejected
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch156_marker[] = "libcgj-batch156";

/* INET6_ADDRSTRLEN including trailing NUL. */
#define B156_ADDRSTRLEN 46u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b156_hex_val(unsigned char ch)
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
b156_zero16(unsigned char *p)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		p[i] = 0u;
	}
}

/*
 * Parse one decimal octet 0..255. Advances *pp. At least one digit
 * required; leading zeros allowed (e.g. 001). Rejects overflow.
 */
static int
b156_parse_dec_octet(const char **pp, unsigned *puVal)
{
	const char *p = *pp;
	unsigned u = 0u;
	int nDig = 0;

	if (p == NULL || *p < '0' || *p > '9') {
		return -1;
	}
	while (*p >= '0' && *p <= '9') {
		if (nDig >= 3) {
			return -1;
		}
		u = u * 10u + (unsigned)(*p - '0');
		p++;
		nDig++;
	}
	if (nDig == 0 || u > 255u) {
		return -1;
	}
	*puVal = u;
	*pp = p;
	return 0;
}

/*
 * Parse dotted-quad IPv4 into four bytes. Used for embedded IPv4 tail.
 * Returns 0 on success, -1 on failure. Does not require advancing a
 * caller's cursor beyond sz (sz must be a full remaining string).
 */
static int
b156_parse_v4(const char *sz, unsigned char aOut[4])
{
	const char *p = sz;
	unsigned i;
	unsigned u;

	for (i = 0u; i < 4u; i++) {
		if (b156_parse_dec_octet(&p, &u) != 0) {
			return -1;
		}
		aOut[i] = (unsigned char)u;
		if (i < 3u) {
			if (*p != '.') {
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
 * Write lowercase hex hextet (no leading zeros except for zero itself)
 * into pOut. Returns bytes written (1..4).
 */
static size_t
b156_fmt_hextet(char *pOut, unsigned uVal)
{
	char aTmp[4];
	int n = 0;
	int i;

	if (uVal == 0u) {
		pOut[0] = '0';
		return 1u;
	}
	while (uVal > 0u && n < 4) {
		unsigned d = uVal & 0xfu;

		aTmp[n++] = (char)(d < 10u ? '0' + d : 'a' + (d - 10u));
		uVal >>= 4;
	}
	for (i = 0; i < n; i++) {
		pOut[i] = aTmp[n - 1 - i];
	}
	return (size_t)n;
}

/* True when sz looks like the start of a dotted-quad (digit+). */
static int
b156_looks_like_v4(const char *sz)
{
	const char *q = sz;
	int fDigits = 0;

	while (*q >= '0' && *q <= '9') {
		q++;
		fDigits = 1;
	}
	return fDigits && *q == '.';
}

/* ---- gj_inet_pton6 ----------------------------------------------------- */

/*
 * Text IPv6 → 16 network-order octets.
 * Supports :: compression and trailing IPv4 dotted-quad.
 */
int
gj_inet_pton6(const char *szSrc, unsigned char aDst[16])
{
	uint16_t aWord[8];
	int nWords = 0;
	int nCompress = -1; /* index where :: expands; -1 = none */
	const char *p;
	int i;

	if (szSrc == NULL || aDst == NULL) {
		return -1;
	}

	for (i = 0; i < 8; i++) {
		aWord[i] = 0u;
	}

	p = szSrc;

	/* Leading "::" */
	if (p[0] == ':' && p[1] == ':') {
		nCompress = 0;
		p += 2;
		if (*p == '\0') {
			/* Bare "::" → all zeros. */
			b156_zero16(aDst);
			return 1;
		}
	} else if (p[0] == ':') {
		/* Leading single ':' is invalid. */
		return 0;
	}

	while (*p != '\0' && nWords < 8) {
		unsigned u = 0u;
		int nDig = 0;
		int d;
		unsigned char aV4[4];

		if (*p == ':') {
			/* "::" compression (exactly once). */
			if (nCompress >= 0) {
				return 0; /* second :: */
			}
			nCompress = nWords;
			p++;
			if (*p == '\0') {
				break; /* trailing :: */
			}
			continue;
		}

		/* Trailing dotted-quad IPv4 occupies two hextets. */
		if (b156_looks_like_v4(p)) {
			if (b156_parse_v4(p, aV4) != 0) {
				return 0;
			}
			if (nWords > 6) {
				return 0;
			}
			aWord[nWords++] =
			    (uint16_t)(((unsigned)aV4[0] << 8) |
			               (unsigned)aV4[1]);
			aWord[nWords++] =
			    (uint16_t)(((unsigned)aV4[2] << 8) |
			               (unsigned)aV4[3]);
			/* Entire remainder was the IPv4 string. */
			p = "";
			break;
		}

		while ((d = b156_hex_val((unsigned char)*p)) >= 0 && nDig < 4) {
			u = (u << 4) | (unsigned)d;
			p++;
			nDig++;
		}
		if (nDig == 0) {
			return 0;
		}
		/* Reject trailing junk on a hextet. */
		if (*p != '\0' && *p != ':') {
			return 0;
		}

		aWord[nWords++] = (uint16_t)u;
		if (*p == '\0') {
			break;
		}
		/* *p == ':' */
		p++;
		if (*p == '\0') {
			/* Trailing single ':' is invalid. */
			return 0;
		}
	}

	if (*p != '\0') {
		return 0;
	}

	if (nCompress >= 0) {
		int nTail = nWords - nCompress;
		int nZ = 8 - nWords;
		int j;

		if (nZ < 0) {
			return 0;
		}
		/* Shift tail words right; zero the expanded gap. */
		for (j = nTail - 1; j >= 0; j--) {
			aWord[nCompress + nZ + j] = aWord[nCompress + j];
		}
		for (j = 0; j < nZ; j++) {
			aWord[nCompress + j] = 0u;
		}
		nWords = 8;
	}

	if (nWords != 8) {
		return 0;
	}

	for (i = 0; i < 8; i++) {
		aDst[i * 2] = (unsigned char)(aWord[i] >> 8);
		aDst[i * 2 + 1] = (unsigned char)(aWord[i] & 0xffu);
	}
	return 1;
}

int __gj_inet_pton6(const char *src, unsigned char dst[16])
    __attribute__((alias("gj_inet_pton6")));

/* ---- gj_inet_ntop6 ----------------------------------------------------- */

/*
 * 16 network-order octets → presentation string with RFC 5952 compression:
 * longest run of zero hextets collapsed to "::" (first run on ties,
 * length must be >= 2); lowercase hex; no leading zeros in hextets.
 */
int
gj_inet_ntop6(const unsigned char aSrc[16], char *szDst, size_t cbSize)
{
	uint16_t aWord[8];
	int i;
	/* Longest zero run [nBest, nBest+nBestLen). */
	int nBest = -1;
	int nBestLen = 0;
	int nRun = -1;
	int nRunLen = 0;
	char aTmp[B156_ADDRSTRLEN];
	size_t cb = 0u;

	if (aSrc == NULL || szDst == NULL || cbSize == 0u) {
		return -1;
	}

	for (i = 0; i < 8; i++) {
		aWord[i] = (uint16_t)(((unsigned)aSrc[i * 2] << 8) |
		                      (unsigned)aSrc[i * 2 + 1]);
	}

	/* Find longest zero run (length >= 2). First wins on ties. */
	for (i = 0; i < 8; i++) {
		if (aWord[i] == 0u) {
			if (nRun < 0) {
				nRun = i;
				nRunLen = 1;
			} else {
				nRunLen++;
			}
		} else {
			if (nRunLen > nBestLen) {
				nBest = nRun;
				nBestLen = nRunLen;
			}
			nRun = -1;
			nRunLen = 0;
		}
	}
	if (nRunLen > nBestLen) {
		nBest = nRun;
		nBestLen = nRunLen;
	}
	if (nBestLen < 2) {
		nBest = -1;
		nBestLen = 0;
	}

	if (nBest < 0) {
		/* Full form: 8 hextets. */
		for (i = 0; i < 8; i++) {
			size_t n;

			if (i > 0) {
				if (cb + 1u >= B156_ADDRSTRLEN) {
					return -1;
				}
				aTmp[cb++] = ':';
			}
			n = b156_fmt_hextet(aTmp + cb, (unsigned)aWord[i]);
			if (cb + n >= B156_ADDRSTRLEN) {
				return -1;
			}
			cb += n;
		}
	} else {
		/* Before the compressed run. */
		for (i = 0; i < nBest; i++) {
			size_t n;

			if (i > 0) {
				if (cb + 1u >= B156_ADDRSTRLEN) {
					return -1;
				}
				aTmp[cb++] = ':';
			}
			n = b156_fmt_hextet(aTmp + cb, (unsigned)aWord[i]);
			if (cb + n >= B156_ADDRSTRLEN) {
				return -1;
			}
			cb += n;
		}
		/* "::" */
		if (cb + 2u >= B156_ADDRSTRLEN) {
			return -1;
		}
		aTmp[cb++] = ':';
		aTmp[cb++] = ':';
		/* After the compressed run. */
		for (i = nBest + nBestLen; i < 8; i++) {
			size_t n = b156_fmt_hextet(aTmp + cb,
			    (unsigned)aWord[i]);

			if (cb + n >= B156_ADDRSTRLEN) {
				return -1;
			}
			cb += n;
			if (i + 1 < 8) {
				if (cb + 1u >= B156_ADDRSTRLEN) {
					return -1;
				}
				aTmp[cb++] = ':';
			}
		}
	}
	aTmp[cb] = '\0';

	if (cb + 1u > cbSize) {
		return -1;
	}
	{
		size_t j;

		for (j = 0u; j <= cb; j++) {
			szDst[j] = aTmp[j];
		}
	}
	return 0;
}

int __gj_inet_ntop6(const unsigned char src[16], char *dst, size_t size)
    __attribute__((alias("gj_inet_ntop6")));
