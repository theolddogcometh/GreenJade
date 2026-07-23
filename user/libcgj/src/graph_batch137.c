/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch137: netstring encode/decode (DJB netstrings).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party netstring source was copied.
 *
 * Format (public, well-known):
 *   <decimal-length> ":" <payload-bytes> ","
 *   Examples:  "0:,"  "5:hello,"  "12:hello world!,"
 *   Length has no leading zeros except the empty payload ("0:,").
 *   Payload may contain any octet sequence of the declared length.
 *
 * Surface (unique symbols):
 *   int gj_netstring_encode(const void *data, size_t dlen,
 *                           char *out, size_t outcap, size_t *n);
 *     — write length:data, into out. *n = total bytes written.
 *       data may be NULL only when dlen == 0. Returns 0 ok, -1 error.
 *   int gj_netstring_decode(const char *in, size_t len,
 *                           const char **data, size_t *dlen, size_t *n);
 *     — parse one netstring from in[0..len). *data points into in
 *       (no copy); *dlen = payload length; *n = bytes consumed
 *       (digits + ':' + payload + ','). Returns 0 ok, -1 error.
 *   __gj_netstring_encode / __gj_netstring_decode  (aliases)
 *   __libcgj_batch137_marker = "libcgj-batch137"
 *
 * Return: 0 on success, -1 on error (NULL args, truncated input, bad
 * length digits / leading zero, missing comma, insufficient outcap).
 * On success *n is bytes written (encode) or consumed (decode).
 * On error *n is set to 0 when n is non-NULL.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch137_marker[] = "libcgj-batch137";

/* Max decimal digits for a size_t length on a 64-bit host: 20. */
#define B137_DIG_MAX 20u

/* ---- freestanding helpers ---------------------------------------------- */

static void
b137_memcpy(char *pDst, const char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

/*
 * Format unsigned size_t as decimal into pDig[0..*pcbDig). Most-significant
 * digit first. Always writes at least one digit (zero → "0").
 */
static void
b137_fmt_u(size_t uVal, char *pDig, size_t *pcbDig)
{
	char aRev[B137_DIG_MAX];
	size_t cb = 0u;
	size_t i;

	if (uVal == 0u) {
		pDig[0] = '0';
		*pcbDig = 1u;
		return;
	}
	while (uVal > 0u) {
		aRev[cb++] = (char)('0' + (uVal % 10u));
		uVal /= 10u;
	}
	/* Reverse into pDig. */
	for (i = 0u; i < cb; i++) {
		pDig[i] = aRev[cb - 1u - i];
	}
	*pcbDig = cb;
}

/* ---- gj_netstring_encode ----------------------------------------------- */

/*
 * Encode payload as a netstring into out. Total size is
 *   digits(dlen) + 1 (':') + dlen + 1 (',')
 * which is overflow-checked against size_t and outcap.
 */
int
gj_netstring_encode(const void *data, size_t dlen, char *out, size_t outcap,
                    size_t *n)
{
	char aDig[B137_DIG_MAX];
	size_t cbDig = 0u;
	size_t cbNeed;
	const char *pSrc;

	if (n != NULL) {
		*n = 0u;
	}
	if (out == NULL || n == NULL) {
		return -1;
	}
	if (data == NULL && dlen != 0u) {
		return -1;
	}

	b137_fmt_u(dlen, aDig, &cbDig);

	/*
	 * need = cbDig + 1 + dlen + 1. Check stepwise for size_t wrap.
	 * cbDig is in [1, 20], so cbDig + 2 cannot overflow size_t.
	 */
	cbNeed = cbDig + 2u;
	if (cbNeed > (size_t)-1 - dlen) {
		return -1;
	}
	cbNeed += dlen;
	if (cbNeed > outcap) {
		return -1;
	}

	/* digits */
	b137_memcpy(out, aDig, cbDig);
	out[cbDig] = ':';
	pSrc = (const char *)data;
	if (dlen > 0u) {
		b137_memcpy(out + cbDig + 1u, pSrc, dlen);
	}
	out[cbDig + 1u + dlen] = ',';

	*n = cbNeed;
	return 0;
}

/* ---- gj_netstring_decode ----------------------------------------------- */

/*
 * Decode one netstring. Rejects empty digit run, non-digits, leading zeros
 * (except the single digit "0"), truncated payload, and missing trailing
 * comma. *data points at the first payload byte inside in (or at the
 * comma position when dlen == 0 — still a valid interior pointer).
 */
int
gj_netstring_decode(const char *in, size_t len, const char **data,
                    size_t *dlen, size_t *n)
{
	size_t i = 0u;
	size_t cbPayload = 0u;
	size_t cbTotal;
	size_t cDig = 0u;
	int fLeadingZero = 0;

	if (n != NULL) {
		*n = 0u;
	}
	if (in == NULL || data == NULL || dlen == NULL || n == NULL) {
		return -1;
	}
	if (len == 0u) {
		return -1;
	}

	/* Parse decimal length digits until ':'. */
	if (in[0] == '0') {
		fLeadingZero = 1;
	}
	while (i < len && in[i] >= '0' && in[i] <= '9') {
		unsigned uDigit = (unsigned)(in[i] - '0');

		/* Overflow-safe: cbPayload * 10 + digit. */
		if (cbPayload > ((size_t)-1 - (size_t)uDigit) / 10u) {
			return -1;
		}
		cbPayload = cbPayload * 10u + (size_t)uDigit;
		cDig++;
		i++;
		if (cDig > B137_DIG_MAX) {
			return -1;
		}
	}

	if (cDig == 0u) {
		return -1;
	}
	/* Leading zero only legal for the empty payload length "0". */
	if (fLeadingZero && !(cDig == 1u && cbPayload == 0u)) {
		return -1;
	}
	if (i >= len || in[i] != ':') {
		return -1;
	}
	i++; /* skip ':' */

	/* Need cbPayload bytes of payload plus a trailing comma. */
	if (cbPayload > len - i) {
		return -1;
	}
	if (1u > (len - i) - cbPayload) {
		return -1;
	}
	if (in[i + cbPayload] != ',') {
		return -1;
	}

	cbTotal = i + cbPayload + 1u;
	*data = in + i;
	*dlen = cbPayload;
	*n = cbTotal;
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_netstring_encode(const void *data, size_t dlen, char *out,
                          size_t outcap, size_t *n)
    __attribute__((alias("gj_netstring_encode")));

int __gj_netstring_decode(const char *in, size_t len, const char **data,
                          size_t *dlen, size_t *n)
    __attribute__((alias("gj_netstring_decode")));
