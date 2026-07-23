/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch114: HTTP/1.x request line + header get (no body).
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * No third-party HTTP parser source was copied.
 *
 * Surface:
 *   gj_http_req_line(const char *req, char *method, size_t mcap,
 *                    char *path, size_t pcap, char *ver, size_t vcap)
 *     Parse first request-line: method SP request-target SP HTTP-version CRLF.
 *     Copies NUL-terminated fields into the provided buffers (caps include NUL).
 *   gj_http_header_get(const char *req, const char *name, char *out, size_t outcap)
 *     Case-insensitive header lookup; trims OWS around the field-value.
 *     First matching header wins. Stops at the blank line before the body.
 *   gj_http_content_length(const char *req, long *out)
 *     Parse Content-Length (1*DIGIT) into *out.
 *   __gj_http_req_line / __gj_http_header_get / __gj_http_content_length
 *   __libcgj_batch114_marker = "libcgj-batch114"
 *
 * Returns: 0 success, -1 error (errno set).
 *   EINVAL  bad args / malformed message
 *   ENOENT  header not present
 *   ENOSPC  output buffer too small
 *   ERANGE  Content-Length overflow
 *
 * Grammar (RFC 7230 subset; obs-fold not accepted):
 *   request-line = method SP request-target SP HTTP-version CRLF
 *   header-field = field-name ":" OWS field-value OWS CRLF
 *   blank line ends the header section (body is ignored).
 * Bare LF is accepted in place of CRLF.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

/* Freestanding LONG_MAX (avoid host limits.h dependency). */
#ifndef LONG_MAX
#if defined(__LP64__) || defined(_LP64) || defined(__x86_64__)
#define LONG_MAX 0x7fffffffffffffffL
#else
#define LONG_MAX 0x7fffffffL
#endif
#endif

const char __libcgj_batch114_marker[] = "libcgj-batch114";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b114_is_sp(unsigned char ch)
{
	return ch == (unsigned char)' ';
}

static int
b114_is_ows(unsigned char ch)
{
	return ch == (unsigned char)' ' || ch == (unsigned char)'\t';
}

static int
b114_is_digit(unsigned char ch)
{
	return ch >= (unsigned char)'0' && ch <= (unsigned char)'9';
}

static int
b114_is_alpha(unsigned char ch)
{
	return (ch >= (unsigned char)'A' && ch <= (unsigned char)'Z') ||
	       (ch >= (unsigned char)'a' && ch <= (unsigned char)'z');
}

/* RFC 7230 tchar (token character). */
static int
b114_is_tchar(unsigned char ch)
{
	if (b114_is_digit(ch) || b114_is_alpha(ch)) {
		return 1;
	}
	switch (ch) {
	case '!':
	case '#':
	case '$':
	case '%':
	case '&':
	case '\'':
	case '*':
	case '+':
	case '-':
	case '.':
	case '^':
	case '_':
	case '`':
	case '|':
	case '~':
		return 1;
	default:
		return 0;
	}
}

static unsigned char
b114_tolower(unsigned char ch)
{
	if (ch >= (unsigned char)'A' && ch <= (unsigned char)'Z') {
		return (unsigned char)(ch - (unsigned char)'A' + (unsigned char)'a');
	}
	return ch;
}

/*
 * Case-insensitive equality of [pName, pName+cbName) against NUL-terminated
 * szKey. Returns 1 on match.
 */
static int
b114_name_eq(const char *pName, size_t cbName, const char *szKey)
{
	size_t i;

	if (pName == NULL || szKey == NULL) {
		return 0;
	}
	for (i = 0; i < cbName; i++) {
		if (szKey[i] == '\0') {
			return 0;
		}
		if (b114_tolower((unsigned char)pName[i]) !=
		    b114_tolower((unsigned char)szKey[i])) {
			return 0;
		}
	}
	return szKey[cbName] == '\0';
}

/* Copy cb bytes then NUL. Requires cbCap >= cb + 1. */
static int
b114_copy_n(char *szDst, size_t cbCap, const char *pSrc, size_t cb)
{
	size_t i;

	if (szDst == NULL || cbCap == 0u) {
		errno = EINVAL;
		return -1;
	}
	if (cb >= cbCap) {
		errno = ENOSPC;
		return -1;
	}
	for (i = 0; i < cb; i++) {
		szDst[i] = pSrc[i];
	}
	szDst[cb] = '\0';
	return 0;
}

/*
 * Advance past one line ending (CRLF or LF). Returns pointer past it, or NULL.
 */
static const char *
b114_skip_eol(const char *pSz)
{
	if (pSz == NULL) {
		return NULL;
	}
	if (pSz[0] == '\r' && pSz[1] == '\n') {
		return pSz + 2;
	}
	if (pSz[0] == '\n') {
		return pSz + 1;
	}
	return NULL;
}

/*
 * Parse request-line tokens in place. On success:
 *   *ppMethod..*pcbMethod, *ppPath..*pcbPath, *ppVer..*pcbVer
 * and *ppAfter points past the request-line EOL.
 */
static int
b114_parse_req_line(const char *szReq,
                    const char **ppMethod, size_t *pcbMethod,
                    const char **ppPath, size_t *pcbPath,
                    const char **ppVer, size_t *pcbVer,
                    const char **ppAfter)
{
	const char *p;
	const char *pMethod;
	const char *pPath;
	const char *pVer;
	size_t cbMethod = 0;
	size_t cbPath = 0;
	size_t cbVer = 0;

	if (szReq == NULL || ppMethod == NULL || pcbMethod == NULL ||
	    ppPath == NULL || pcbPath == NULL || ppVer == NULL ||
	    pcbVer == NULL || ppAfter == NULL) {
		errno = EINVAL;
		return -1;
	}

	p = szReq;

	/* method = token */
	pMethod = p;
	while (*p != '\0' && b114_is_tchar((unsigned char)*p)) {
		p++;
		cbMethod++;
	}
	if (cbMethod == 0u || !b114_is_sp((unsigned char)*p)) {
		errno = EINVAL;
		return -1;
	}
	p++; /* SP */

	/* request-target: non-empty, no SP/CR/LF (origin, absolute, authority, star) */
	pPath = p;
	while (*p != '\0' && *p != ' ' && *p != '\r' && *p != '\n') {
		p++;
		cbPath++;
	}
	if (cbPath == 0u || !b114_is_sp((unsigned char)*p)) {
		errno = EINVAL;
		return -1;
	}
	p++; /* SP */

	/* HTTP-version: "HTTP/" DIGIT "." DIGIT  (HTTP/1.x family) */
	pVer = p;
	if (p[0] != 'H' || p[1] != 'T' || p[2] != 'T' || p[3] != 'P' ||
	    p[4] != '/') {
		errno = EINVAL;
		return -1;
	}
	p += 5;
	cbVer = 5;
	if (!b114_is_digit((unsigned char)*p)) {
		errno = EINVAL;
		return -1;
	}
	p++;
	cbVer++;
	if (*p != '.') {
		errno = EINVAL;
		return -1;
	}
	p++;
	cbVer++;
	if (!b114_is_digit((unsigned char)*p)) {
		errno = EINVAL;
		return -1;
	}
	p++;
	cbVer++;

	/* No trailing junk before EOL (single-digit minor only for 1.x). */
	if (*p != '\r' && *p != '\n') {
		errno = EINVAL;
		return -1;
	}

	*ppAfter = b114_skip_eol(p);
	if (*ppAfter == NULL) {
		errno = EINVAL;
		return -1;
	}

	*ppMethod = pMethod;
	*pcbMethod = cbMethod;
	*ppPath = pPath;
	*pcbPath = cbPath;
	*ppVer = pVer;
	*pcbVer = cbVer;
	return 0;
}

/*
 * Find first header named szName (case-insensitive). On success sets
 * *ppVal / *pcbVal to the trimmed field-value (may be zero length).
 */
static int
b114_find_header(const char *szReq, const char *szName,
                 const char **ppVal, size_t *pcbVal)
{
	const char *p;
	const char *pMethod;
	const char *pPath;
	const char *pVer;
	size_t cbMethod;
	size_t cbPath;
	size_t cbVer;

	if (szReq == NULL || szName == NULL || szName[0] == '\0' ||
	    ppVal == NULL || pcbVal == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (b114_parse_req_line(szReq, &pMethod, &cbMethod, &pPath, &cbPath,
	                        &pVer, &cbVer, &p) != 0) {
		return -1;
	}
	(void)pMethod;
	(void)cbMethod;
	(void)pPath;
	(void)cbPath;
	(void)pVer;
	(void)cbVer;

	for (;;) {
		const char *pName;
		size_t cbName = 0;
		const char *pVal;
		const char *pValEnd;
		const char *pEol;
		size_t cbVal;

		/* Blank line → end of headers (no body parse). */
		if (*p == '\r' || *p == '\n') {
			if (b114_skip_eol(p) == NULL) {
				errno = EINVAL;
				return -1;
			}
			errno = ENOENT;
			return -1;
		}
		if (*p == '\0') {
			/* Truncated message without blank line. */
			errno = EINVAL;
			return -1;
		}

		/* field-name = token */
		pName = p;
		while (*p != '\0' && b114_is_tchar((unsigned char)*p)) {
			p++;
			cbName++;
		}
		if (cbName == 0u || *p != ':') {
			errno = EINVAL;
			return -1;
		}
		p++; /* ':' */

		/* OWS before field-value */
		while (b114_is_ows((unsigned char)*p)) {
			p++;
		}
		pVal = p;

		/* field-value until EOL (obs-fold not accepted) */
		while (*p != '\0' && *p != '\r' && *p != '\n') {
			/* reject CTL except HTAB (already allowed via loop) */
			{
				unsigned char ch = (unsigned char)*p;
				if (ch < 0x20u && ch != (unsigned char)'\t') {
					errno = EINVAL;
					return -1;
				}
				if (ch == 0x7fu) {
					errno = EINVAL;
					return -1;
				}
			}
			p++;
		}
		pValEnd = p;

		/* trim trailing OWS */
		while (pValEnd > pVal &&
		       b114_is_ows((unsigned char)pValEnd[-1])) {
			pValEnd--;
		}
		cbVal = (size_t)(pValEnd - pVal);

		pEol = b114_skip_eol(p);
		if (pEol == NULL) {
			errno = EINVAL;
			return -1;
		}
		p = pEol;

		if (b114_name_eq(pName, cbName, szName)) {
			*ppVal = pVal;
			*pcbVal = cbVal;
			return 0;
		}
	}
}

/* Parse 1*DIGIT into long; overflow → ERANGE. */
static int
b114_parse_long_digits(const char *pVal, size_t cbVal, long *pOut)
{
	size_t i;
	unsigned long uAcc = 0ul;
	const unsigned long uLim = (unsigned long)LONG_MAX;

	if (pVal == NULL || pOut == NULL || cbVal == 0u) {
		errno = EINVAL;
		return -1;
	}

	for (i = 0; i < cbVal; i++) {
		unsigned char ch = (unsigned char)pVal[i];
		unsigned long uDig;

		if (!b114_is_digit(ch)) {
			errno = EINVAL;
			return -1;
		}
		uDig = (unsigned long)(ch - (unsigned char)'0');
		if (uAcc > (uLim - uDig) / 10ul) {
			errno = ERANGE;
			return -1;
		}
		uAcc = uAcc * 10ul + uDig;
	}

	*pOut = (long)uAcc;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * Parse HTTP/1.x request-line into method / request-target / version buffers.
 * Caps include the trailing NUL. Body and headers are not examined.
 */
int
gj_http_req_line(const char *szReq, char *szMethod, size_t cbMethod,
                 char *szPath, size_t cbPath, char *szVer, size_t cbVer)
{
	const char *pMethod;
	const char *pPath;
	const char *pVer;
	const char *pAfter;
	size_t nMethod;
	size_t nPath;
	size_t nVer;

	if (szReq == NULL || szMethod == NULL || cbMethod == 0u ||
	    szPath == NULL || cbPath == 0u || szVer == NULL || cbVer == 0u) {
		errno = EINVAL;
		return -1;
	}

	if (b114_parse_req_line(szReq, &pMethod, &nMethod, &pPath, &nPath,
	                        &pVer, &nVer, &pAfter) != 0) {
		return -1;
	}
	(void)pAfter;

	if (b114_copy_n(szMethod, cbMethod, pMethod, nMethod) != 0) {
		return -1;
	}
	if (b114_copy_n(szPath, cbPath, pPath, nPath) != 0) {
		return -1;
	}
	if (b114_copy_n(szVer, cbVer, pVer, nVer) != 0) {
		return -1;
	}
	return 0;
}

/*
 * Copy the first matching header field-value (OWS-trimmed) into szOut.
 */
int
gj_http_header_get(const char *szReq, const char *szName, char *szOut,
                   size_t cbOut)
{
	const char *pVal;
	size_t cbVal;

	if (szOut == NULL || cbOut == 0u) {
		errno = EINVAL;
		return -1;
	}

	if (b114_find_header(szReq, szName, &pVal, &cbVal) != 0) {
		return -1;
	}
	return b114_copy_n(szOut, cbOut, pVal, cbVal);
}

/*
 * Parse Content-Length header as an unsigned decimal into *pOut (as long).
 */
int
gj_http_content_length(const char *szReq, long *pOut)
{
	const char *pVal;
	size_t cbVal;

	if (pOut == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (b114_find_header(szReq, "Content-Length", &pVal, &cbVal) != 0) {
		return -1;
	}
	return b114_parse_long_digits(pVal, cbVal, pOut);
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_http_req_line(const char *szReq, char *szMethod, size_t cbMethod,
                       char *szPath, size_t cbPath, char *szVer, size_t cbVer)
    __attribute__((alias("gj_http_req_line")));

int __gj_http_header_get(const char *szReq, const char *szName, char *szOut,
                         size_t cbOut)
    __attribute__((alias("gj_http_header_get")));

int __gj_http_content_length(const char *szReq, long *pOut)
    __attribute__((alias("gj_http_content_length")));
