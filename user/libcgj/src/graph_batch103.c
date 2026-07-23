/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch103: URL helpers (RFC 3986 subset).
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * No third-party URL parser was copied.
 *
 * Surface:
 *   gj_url_parse(const char *url,
 *                char *scheme, size_t sch_cap,
 *                char *host,   size_t host_cap,
 *                int *port,
 *                char *path,   size_t path_cap);
 *     Parse absolute URI with authority form:
 *       scheme "://" [ userinfo "@" ] host [ ":" port ] path-abempty
 *       [ "?" query ] [ "#" fragment ]
 *     path receives path-abempty plus optional "?" query and "#" fragment
 *     (everything after authority). Empty path-abempty yields "".
 *     *port is set to the explicit decimal port, or -1 if omitted.
 *     IPv6 literals in brackets are accepted: http://[::1]:8080/x
 *     Returns 0 on success, -1 on error (errno set).
 *
 *   gj_url_encode(const char *in, char *out, size_t outcap);
 *     Percent-encode (RFC 3986 §2.1): unreserved stay as-is; all other
 *     octets become %HH (uppercase hex). Space is %20 (not '+').
 *     Returns 0 on success, -1 on error.
 *
 *   gj_url_decode(const char *in, char *out, size_t outcap);
 *     Percent-decode: %HH → octet; '+' left as '+'. Invalid % sequences
 *     are EINVAL. Returns 0 on success, -1 on error.
 *
 *   __gj_url_parse / __gj_url_encode / __gj_url_decode  (aliases)
 *   __libcgj_batch103_marker = "libcgj-batch103"
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch103_marker[] = "libcgj-batch103";

/* ---- freestanding character class -------------------------------------- */

static int
b103_is_alpha(unsigned char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

static int
b103_is_digit(unsigned char ch)
{
	return ch >= '0' && ch <= '9';
}

static int
b103_is_alnum(unsigned char ch)
{
	return b103_is_alpha(ch) || b103_is_digit(ch);
}

/* RFC 3986 unreserved: ALPHA / DIGIT / "-" / "." / "_" / "~" */
static int
b103_is_unreserved(unsigned char ch)
{
	return b103_is_alnum(ch) || ch == '-' || ch == '.' || ch == '_' ||
	       ch == '~';
}

/* scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
static int
b103_is_scheme_cont(unsigned char ch)
{
	return b103_is_alnum(ch) || ch == '+' || ch == '-' || ch == '.';
}

/* host reg-name / IPv4-ish / unreserved + sub-delims subset used on wire */
static int
b103_is_host_char(unsigned char ch)
{
	/* unreserved / sub-delims / pct-encoded handled by caller */
	return b103_is_unreserved(ch) || ch == '!' || ch == '$' || ch == '&' ||
	       ch == '\'' || ch == '(' || ch == ')' || ch == '*' || ch == '+' ||
	       ch == ',' || ch == ';' || ch == '=' || ch == '%';
}

static int
b103_hex_val(unsigned char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0');
	}
	if (ch >= 'A' && ch <= 'F') {
		return (int)(ch - 'A') + 10;
	}
	if (ch >= 'a' && ch <= 'f') {
		return (int)(ch - 'a') + 10;
	}
	return -1;
}

static char
b103_hex_digit(unsigned uN)
{
	return (char)((uN < 10u) ? ('0' + (char)uN) : ('A' + (char)(uN - 10u)));
}

/* ---- freestanding string helpers --------------------------------------- */

static size_t
b103_strlen(const char *sz)
{
	size_t cb = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

/*
 * Copy cbSrc bytes of pSrc into szDst (capacity cbDstCap including NUL).
 * Always NUL-terminates on success. Returns 0 or -1 (ENOSPC / EINVAL).
 */
static int
b103_copy_cap(char *szDst, size_t cbDstCap, const char *pSrc, size_t cbSrc)
{
	size_t i;

	if (szDst == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (cbDstCap == 0u) {
		errno = ENOSPC;
		return -1;
	}
	if (cbSrc >= cbDstCap) {
		errno = ENOSPC;
		return -1;
	}
	if (pSrc == NULL) {
		if (cbSrc != 0u) {
			errno = EINVAL;
			return -1;
		}
		szDst[0] = '\0';
		return 0;
	}
	for (i = 0u; i < cbSrc; i++) {
		szDst[i] = pSrc[i];
	}
	szDst[cbSrc] = '\0';
	return 0;
}

/* ---- public: gj_url_encode --------------------------------------------- */

/*
 * Percent-encode in → out. outcap includes room for trailing NUL.
 * Unreserved characters pass through; others become %HH (uppercase).
 */
int
gj_url_encode(const char *szIn, char *szOut, size_t cbOutCap)
{
	size_t iIn;
	size_t iOut;
	size_t cbIn;
	unsigned char ch;

	if (szIn == NULL || szOut == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (cbOutCap == 0u) {
		errno = ENOSPC;
		return -1;
	}

	cbIn = b103_strlen(szIn);
	iOut = 0u;
	for (iIn = 0u; iIn < cbIn; iIn++) {
		ch = (unsigned char)szIn[iIn];
		if (b103_is_unreserved(ch)) {
			if (iOut + 1u >= cbOutCap) {
				errno = ENOSPC;
				return -1;
			}
			szOut[iOut++] = (char)ch;
		} else {
			if (iOut + 3u >= cbOutCap) {
				errno = ENOSPC;
				return -1;
			}
			szOut[iOut++] = '%';
			szOut[iOut++] = b103_hex_digit((unsigned)ch >> 4);
			szOut[iOut++] = b103_hex_digit((unsigned)ch & 0x0fu);
		}
	}
	szOut[iOut] = '\0';
	return 0;
}

/* ---- public: gj_url_decode --------------------------------------------- */

/*
 * Percent-decode in → out. %HH (hex) becomes one octet; other chars copy.
 * Incomplete or non-hex % sequences are EINVAL.
 */
int
gj_url_decode(const char *szIn, char *szOut, size_t cbOutCap)
{
	size_t iIn;
	size_t iOut;
	size_t cbIn;
	unsigned char ch;
	int nHi;
	int nLo;

	if (szIn == NULL || szOut == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (cbOutCap == 0u) {
		errno = ENOSPC;
		return -1;
	}

	cbIn = b103_strlen(szIn);
	iOut = 0u;
	iIn = 0u;
	while (iIn < cbIn) {
		ch = (unsigned char)szIn[iIn];
		if (ch == '%') {
			if (iIn + 2u >= cbIn) {
				errno = EINVAL;
				return -1;
			}
			nHi = b103_hex_val((unsigned char)szIn[iIn + 1u]);
			nLo = b103_hex_val((unsigned char)szIn[iIn + 2u]);
			if (nHi < 0 || nLo < 0) {
				errno = EINVAL;
				return -1;
			}
			if (iOut + 1u >= cbOutCap) {
				errno = ENOSPC;
				return -1;
			}
			szOut[iOut++] = (char)((nHi << 4) | nLo);
			iIn += 3u;
		} else {
			if (iOut + 1u >= cbOutCap) {
				errno = ENOSPC;
				return -1;
			}
			szOut[iOut++] = (char)ch;
			iIn++;
		}
	}
	szOut[iOut] = '\0';
	return 0;
}

/* ---- public: gj_url_parse ---------------------------------------------- */

/*
 * Parse absolute URI with // authority (RFC 3986 subset).
 *
 * Required form:
 *   scheme "://" [ userinfo "@" ] host [ ":" port ] [path]["?"query]["#"frag]
 *
 * host may be:
 *   - reg-name / IPv4address characters
 *   - "[" IPv6address "]"  (bracket contents not deeply validated)
 *
 * path buffer receives path-abempty through optional query and fragment.
 * *port is -1 when the port production is absent; else 0..65535.
 * scheme/host/path pointers may be NULL to skip that output field.
 */
int
gj_url_parse(const char *szUrl, char *szScheme, size_t cbSchCap, char *szHost,
             size_t cbHostCap, int *pPort, char *szPath, size_t cbPathCap)
{
	const char *pCur;
	const char *pSchemeStart;
	const char *pHostStart;
	const char *pHostEnd;
	const char *pPathStart;
	const char *pAuthStart;
	const char *pAuthEnd;
	const char *pAt;
	const char *pColon;
	const char *pWalk;
	size_t cbScheme;
	size_t cbHost;
	size_t cbPath;
	size_t cbAuth;
	int nPort;
	int nDigit;
	unsigned uPort;
	int fBracket;

	if (szUrl == NULL) {
		errno = EINVAL;
		return -1;
	}

	pCur = szUrl;

	/* ---- scheme ---------------------------------------------------- */
	if (!b103_is_alpha((unsigned char)pCur[0])) {
		errno = EINVAL;
		return -1;
	}
	pSchemeStart = pCur;
	pCur++;
	while (pCur[0] != '\0' && b103_is_scheme_cont((unsigned char)pCur[0])) {
		pCur++;
	}
	if (pCur[0] != ':') {
		errno = EINVAL;
		return -1;
	}
	cbScheme = (size_t)(pCur - pSchemeStart);
	pCur++; /* skip ':' */

	/* require "//" authority form */
	if (pCur[0] != '/' || pCur[1] != '/') {
		errno = EINVAL;
		return -1;
	}
	pCur += 2;
	pAuthStart = pCur;

	/* authority ends at '/', '?', '#', or NUL */
	while (pCur[0] != '\0' && pCur[0] != '/' && pCur[0] != '?' &&
	       pCur[0] != '#') {
		pCur++;
	}
	pAuthEnd = pCur;
	pPathStart = pCur; /* path-abempty (may be empty) + query/fragment */
	cbAuth = (size_t)(pAuthEnd - pAuthStart);

	if (cbAuth == 0u) {
		/* empty host not accepted in this subset */
		errno = EINVAL;
		return -1;
	}

	/* strip userinfo: last '@' in authority separates userinfo@host[:port] */
	pAt = NULL;
	for (pWalk = pAuthStart; pWalk < pAuthEnd; pWalk++) {
		if (pWalk[0] == '@') {
			pAt = pWalk;
		}
	}
	if (pAt != NULL) {
		pHostStart = pAt + 1;
	} else {
		pHostStart = pAuthStart;
	}
	if (pHostStart >= pAuthEnd) {
		errno = EINVAL;
		return -1;
	}

	/* host / port split; IPv6 brackets protected from colon-as-port */
	fBracket = 0;
	if (pHostStart[0] == '[') {
		fBracket = 1;
		pWalk = pHostStart + 1;
		while (pWalk < pAuthEnd && pWalk[0] != ']') {
			pWalk++;
		}
		if (pWalk >= pAuthEnd || pWalk[0] != ']') {
			errno = EINVAL;
			return -1;
		}
		/* host includes brackets */
		pHostEnd = pWalk + 1;
		if (pHostEnd < pAuthEnd) {
			if (pHostEnd[0] != ':') {
				errno = EINVAL;
				return -1;
			}
			/* port after ":"; fall through with pColon */
			pColon = pHostEnd;
		} else {
			pColon = NULL;
		}
	} else {
		/* find rightmost ':' for port (reg-name / IPv4 has at most one) */
		pColon = NULL;
		for (pWalk = pHostStart; pWalk < pAuthEnd; pWalk++) {
			if (pWalk[0] == ':') {
				pColon = pWalk;
			}
		}
		if (pColon != NULL) {
			pHostEnd = pColon;
		} else {
			pHostEnd = pAuthEnd;
		}
	}

	cbHost = (size_t)(pHostEnd - pHostStart);
	if (cbHost == 0u) {
		errno = EINVAL;
		return -1;
	}

	/* validate host characters (brackets content is free-form hex/colon) */
	if (fBracket) {
		/* require at least "[x]" */
		if (cbHost < 3u || pHostStart[0] != '[' ||
		    pHostStart[cbHost - 1u] != ']') {
			errno = EINVAL;
			return -1;
		}
		for (pWalk = pHostStart + 1; pWalk < pHostEnd - 1; pWalk++) {
			unsigned char ch = (unsigned char)pWalk[0];
			/* IPv6 literal subset: hex, ':', '.', and zone '%" */
			if (!(b103_is_digit(ch) ||
			      (ch >= 'A' && ch <= 'F') ||
			      (ch >= 'a' && ch <= 'f') || ch == ':' ||
			      ch == '.' || ch == '%')) {
				errno = EINVAL;
				return -1;
			}
		}
	} else {
		for (pWalk = pHostStart; pWalk < pHostEnd; pWalk++) {
			if (!b103_is_host_char((unsigned char)pWalk[0])) {
				errno = EINVAL;
				return -1;
			}
		}
	}

	/* port */
	nPort = -1;
	if (pColon != NULL) {
		pWalk = pColon + 1;
		if (pWalk >= pAuthEnd) {
			/* empty port production */
			errno = EINVAL;
			return -1;
		}
		uPort = 0u;
		while (pWalk < pAuthEnd) {
			if (!b103_is_digit((unsigned char)pWalk[0])) {
				errno = EINVAL;
				return -1;
			}
			nDigit = (int)(pWalk[0] - '0');
			/* reject overflow past 65535 */
			if (uPort > 6553u ||
			    (uPort == 6553u && (unsigned)nDigit > 5u)) {
				errno = EINVAL;
				return -1;
			}
			uPort = uPort * 10u + (unsigned)nDigit;
			pWalk++;
		}
		nPort = (int)uPort;
	}

	/* path = remainder of URL (may be empty) */
	cbPath = b103_strlen(pPathStart);

	/* ---- write outputs --------------------------------------------- */
	if (szScheme != NULL) {
		if (b103_copy_cap(szScheme, cbSchCap, pSchemeStart, cbScheme) !=
		    0) {
			return -1;
		}
	} else if (cbSchCap != 0u) {
		errno = EINVAL;
		return -1;
	}

	if (szHost != NULL) {
		if (b103_copy_cap(szHost, cbHostCap, pHostStart, cbHost) != 0) {
			return -1;
		}
	} else if (cbHostCap != 0u) {
		errno = EINVAL;
		return -1;
	}

	if (pPort != NULL) {
		*pPort = nPort;
	}

	if (szPath != NULL) {
		if (b103_copy_cap(szPath, cbPathCap, pPathStart, cbPath) != 0) {
			return -1;
		}
	} else if (cbPathCap != 0u) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_url_parse(const char *szUrl, char *szScheme, size_t cbSchCap,
                   char *szHost, size_t cbHostCap, int *pPort, char *szPath,
                   size_t cbPathCap)
    __attribute__((alias("gj_url_parse")));

int __gj_url_encode(const char *szIn, char *szOut, size_t cbOutCap)
    __attribute__((alias("gj_url_encode")));

int __gj_url_decode(const char *szIn, char *szOut, size_t cbOutCap)
    __attribute__((alias("gj_url_decode")));
