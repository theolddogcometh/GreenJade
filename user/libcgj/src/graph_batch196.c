/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch196: multipart/form-data boundary find +
 * body-part iteration (RFC 2046 §5.1 / RFC 7578 subset).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party MIME/multipart source
 * was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   gj_http_header_get / gj_http_content_length → graph_batch114.c
 *   gj_form_get (x-www-form-urlencoded)         → graph_batch197.c
 *   gj_cookie_get                               → graph_batch195.c
 *
 * This TU adds only unique symbols:
 *   int gj_multipart_find_boundary(const char *ctype, char *bound,
 *                                  size_t cap);
 *     — Extract the boundary parameter from a Content-Type header value
 *       (the field-value only; no leading "Content-Type:"). Accepts e.g.
 *         multipart/form-data; boundary=----WebKitFormBoundary7MA4
 *         multipart/mixed; boundary="simple boundary"
 *       Parameter name match is case-insensitive. Quoted values strip the
 *       surrounding quotes; a minimal \" and \\ escape is honoured inside
 *       quotes. Unquoted tokens stop at tspecials / OWS. Empty boundary
 *       is rejected. On success bound receives a NUL-terminated copy
 *       (cap includes NUL). Returns 0 / -1.
 *
 *   int gj_multipart_next_part(const char **pp, const char *end,
 *                              const char *bound,
 *                              const char **part, size_t *plen);
 *     — Iterate MIME body-parts in a multipart body buffer [start, end).
 *       *pp is an in/out cursor (initially the body start). bound is the
 *       bare boundary token from find_boundary (no leading "--").
 *       On success: *part points at the start of the body-part (headers
 *       and payload after the delimiter line), *plen is the number of
 *       bytes until the CRLF/LF that precedes the next delimiter (or the
 *       remaining length if no further delimiter is present). *pp is
 *       advanced to the start of the next delimiter ("--" bound) so a
 *       subsequent call continues cleanly. Returns 0 when a part was
 *       produced, -1 when there are no more parts or on bad args.
 *       Closing delimiter ("--" bound "--") ends iteration without a part.
 *
 *   __gj_multipart_find_boundary / __gj_multipart_next_part  (aliases)
 *   __libcgj_batch196_marker = "libcgj-batch196"
 *
 * Notes:
 *   - Delimiter grammar (RFC 2046):  [CRLF] "--" boundary [transport-pad]
 *     then CRLF (or bare LF). First delimiter may omit the leading CRLF.
 *   - No malloc, no errno (freestanding-safe). Compiles with
 *     -ffreestanding -msse2.
 */

#include <stddef.h>

const char __libcgj_batch196_marker[] = "libcgj-batch196";

/* RFC 2046: boundary is 1..70 characters. */
#define B196_BOUND_MAX 70u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b196_is_ows(unsigned char ch)
{
	return ch == (unsigned char)' ' || ch == (unsigned char)'\t';
}

static unsigned char
b196_to_lower(unsigned char ch)
{
	if (ch >= (unsigned char)'A' && ch <= (unsigned char)'Z') {
		return (unsigned char)(ch + ((unsigned char)'a' -
		    (unsigned char)'A'));
	}
	return ch;
}

/*
 * RFC 2045 tspecials plus CTL/SPACE: end an unquoted parameter value.
 * tspecials = ()<>@,;:\"/[]?=
 */
static int
b196_is_token_end(unsigned char ch)
{
	if (ch == (unsigned char)'\0' || ch <= 0x20u || ch == 0x7fu) {
		return 1;
	}
	switch (ch) {
	case '(':
	case ')':
	case '<':
	case '>':
	case '@':
	case ',':
	case ';':
	case ':':
	case '\\':
	case '"':
	case '/':
	case '[':
	case ']':
	case '?':
	case '=':
		return 1;
	default:
		return 0;
	}
}

static size_t
b196_strlen(const char *sz)
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

/*
 * True if szNeedle (NUL-terminated) matches p[0..) case-insensitively.
 */
static int
b196_iprefix(const char *p, const char *szNeedle)
{
	size_t i;

	if (p == NULL || szNeedle == NULL) {
		return 0;
	}
	for (i = 0u; szNeedle[i] != '\0'; i++) {
		if (p[i] == '\0') {
			return 0;
		}
		if (b196_to_lower((unsigned char)p[i]) !=
		    b196_to_lower((unsigned char)szNeedle[i])) {
			return 0;
		}
	}
	return 1;
}

/*
 * Copy cb bytes then NUL into szDst (cap includes NUL).
 * Returns 0 / -1.
 */
static int
b196_copy_n(char *szDst, size_t cbCap, const char *pSrc, size_t cb)
{
	size_t i;

	if (szDst == NULL || cbCap == 0u) {
		return -1;
	}
	if (cb >= cbCap) {
		return -1;
	}
	if (cb > 0u && pSrc == NULL) {
		return -1;
	}
	for (i = 0u; i < cb; i++) {
		szDst[i] = pSrc[i];
	}
	szDst[cb] = '\0';
	return 0;
}

/* ---- gj_multipart_find_boundary ---------------------------------------- */

/*
 * Scan a Content-Type field-value for the boundary parameter and copy
 * the token into bound. Cap includes the trailing NUL.
 */
int
gj_multipart_find_boundary(const char *ctype, char *bound, size_t cap)
{
	const char *p;
	const char *pVal;
	char aTmp[B196_BOUND_MAX + 1u];
	size_t cbOut;
	size_t i;

	if (ctype == NULL || bound == NULL || cap == 0u) {
		return -1;
	}

	p = ctype;

	/*
	 * Walk parameters. Prefer a "; boundary=" style parameter, but
	 * also accept a bare "boundary=" if that is all the caller passed.
	 */
	for (;;) {
		/* Skip OWS and separators until a name start. */
		while (*p != '\0' &&
		    (b196_is_ows((unsigned char)*p) || *p == ';')) {
			p++;
		}
		if (*p == '\0') {
			return -1;
		}

		/* Attribute name: token characters until '=' or separator. */
		if (b196_iprefix(p, "boundary")) {
			const char *pNameEnd = p + 8; /* strlen("boundary") */

			/* Name must end here (not "boundaryfoo"). */
			if (*pNameEnd == '\0' || *pNameEnd == '=' ||
			    b196_is_ows((unsigned char)*pNameEnd) ||
			    *pNameEnd == ';') {
				p = pNameEnd;
				while (b196_is_ows((unsigned char)*p)) {
					p++;
				}
				if (*p != '=') {
					/* Not a parameter assignment; rescan. */
					goto skip_token;
				}
				p++;
				while (b196_is_ows((unsigned char)*p)) {
					p++;
				}

				cbOut = 0u;
				if (*p == '"') {
					/* quoted-string */
					p++;
					while (*p != '\0' && *p != '"') {
						unsigned char ch;

						ch = (unsigned char)*p;
						if (ch == '\\' && p[1] != '\0') {
							p++;
							ch = (unsigned char)*p;
						}
						if (cbOut >= B196_BOUND_MAX) {
							return -1;
						}
						aTmp[cbOut] = (char)ch;
						cbOut++;
						p++;
					}
					if (*p != '"') {
						return -1;
					}
					/* p at closing quote; done with value */
				} else {
					/* token */
					pVal = p;
					while (*p != '\0' &&
					    !b196_is_token_end(
					        (unsigned char)*p)) {
						p++;
					}
					cbOut = (size_t)(p - pVal);
					if (cbOut == 0u ||
					    cbOut > B196_BOUND_MAX) {
						return -1;
					}
					for (i = 0u; i < cbOut; i++) {
						aTmp[i] = pVal[i];
					}
				}

				if (cbOut == 0u) {
					return -1;
				}
				aTmp[cbOut] = '\0';
				return b196_copy_n(bound, cap, aTmp, cbOut);
			}
		}

skip_token:
		/*
		 * Skip this parameter (name[=value]) until the next ';'
		 * or end. Handle a quoted value so ';' inside quotes does
		 * not split parameters.
		 */
		while (*p != '\0' && *p != ';') {
			if (*p == '"') {
				p++;
				while (*p != '\0' && *p != '"') {
					if (*p == '\\' && p[1] != '\0') {
						p++;
					}
					p++;
				}
				if (*p == '"') {
					p++;
				}
			} else {
				p++;
			}
		}
	}
}

int __gj_multipart_find_boundary(const char *ctype, char *bound, size_t cap)
    __attribute__((alias("gj_multipart_find_boundary")));

/* ---- delimiter scan helpers -------------------------------------------- */

/*
 * True if p is a valid start-of-line position relative to body cursor
 * pBase: either p == pBase (first delimiter may omit leading CRLF) or
 * the byte immediately before p is '\n'.
 */
static int
b196_at_line_start(const char *p, const char *pBase)
{
	if (p == pBase) {
		return 1;
	}
	if (p > pBase && p[-1] == '\n') {
		return 1;
	}
	return 0;
}

/*
 * Match "--" + bound at p within [p, end). On success stores whether the
 * delimiter is the closing form ("--" bound "--") in *pfClose and returns
 * a pointer to the first byte after the boundary token (after optional
 * close dashes are consumed into *pfClose). Returns NULL if no match.
 *
 * Does not consume transport-padding or the trailing CRLF.
 */
static const char *
b196_match_bound_at(const char *p, const char *end, const char *bound,
                    size_t cbBound, int *pfClose)
{
	size_t i;
	const char *pAfter;
	size_t cbNeed;

	if (pfClose != NULL) {
		*pfClose = 0;
	}
	if (p == NULL || end == NULL || bound == NULL) {
		return NULL;
	}
	/* Need "--" + bound. */
	cbNeed = 2u + cbBound;
	if ((size_t)(end - p) < cbNeed) {
		return NULL;
	}
	if (p[0] != '-' || p[1] != '-') {
		return NULL;
	}
	for (i = 0u; i < cbBound; i++) {
		if (p[2u + i] != bound[i]) {
			return NULL;
		}
	}
	pAfter = p + cbNeed;

	/* Closing delimiter: extra "--" immediately after the boundary. */
	if ((size_t)(end - pAfter) >= 2u && pAfter[0] == '-' &&
	    pAfter[1] == '-') {
		if (pfClose != NULL) {
			*pfClose = 1;
		}
		pAfter += 2;
	}
	return pAfter;
}

/*
 * After a matched boundary token (pAfter from b196_match_bound_at), skip
 * optional transport-padding (SP / HTAB) and a required CRLF or LF.
 * Returns pointer to the first byte of the body-part, or NULL if the
 * delimiter line is truncated/malformed. For a closing delimiter the
 * trailing CRLF is optional at end-of-buffer.
 */
static const char *
b196_skip_delim_line(const char *pAfter, const char *end, int fClose)
{
	const char *p = pAfter;

	if (p == NULL || end == NULL || p > end) {
		return NULL;
	}
	while (p < end && b196_is_ows((unsigned char)*p)) {
		p++;
	}
	if (p >= end) {
		/* Closing delimiter may end the entity without a final CRLF. */
		return fClose ? end : NULL;
	}
	if (*p == '\r') {
		p++;
		if (p < end && *p == '\n') {
			p++;
		}
		return p;
	}
	if (*p == '\n') {
		return p + 1;
	}
	/*
	 * Non-OW whitespace garbage after boundary is malformed for an
	 * open delimiter. Closing delimiters still tolerate epilogue
	 * starting immediately (treat rest as after the close).
	 */
	return fClose ? p : NULL;
}

/*
 * Find the next delimiter at or after pCur within [pBase, end).
 * pBase anchors line-start checks (first delimiter may sit at pBase).
 * On success: *ppDelim = start of "--"bound, *ppAfter = past boundary
 * token (+ close dashes), *pfClose set. Returns 0 / -1.
 */
static int
b196_find_delim(const char *pCur, const char *pBase, const char *end,
                const char *bound, size_t cbBound, const char **ppDelim,
                const char **ppAfter, int *pfClose)
{
	const char *p;
	const char *pAfter;
	int fClose;
	size_t cbNeed;

	if (pCur == NULL || pBase == NULL || end == NULL || bound == NULL ||
	    ppDelim == NULL || ppAfter == NULL) {
		return -1;
	}
	if (pCur < pBase || pCur > end) {
		return -1;
	}

	cbNeed = 2u + cbBound;
	p = pCur;
	while ((size_t)(end - p) >= cbNeed) {
		if (b196_at_line_start(p, pBase) != 0) {
			pAfter = b196_match_bound_at(p, end, bound, cbBound,
			    &fClose);
			if (pAfter != NULL) {
				/*
				 * Require that after optional pad we either
				 * hit EOL or (for close) end/epilogue.
				 * Reject false positives mid-token.
				 */
				const char *pChk = pAfter;

				while (pChk < end &&
				    b196_is_ows((unsigned char)*pChk)) {
					pChk++;
				}
				if (pChk >= end || *pChk == '\r' ||
				    *pChk == '\n' || fClose != 0) {
					*ppDelim = p;
					*ppAfter = pAfter;
					if (pfClose != NULL) {
						*pfClose = fClose;
					}
					return 0;
				}
			}
		}
		p++;
	}
	return -1;
}

/* ---- gj_multipart_next_part -------------------------------------------- */

/*
 * Produce the next multipart body-part. See file header for cursor
 * semantics. Returns 0 on success, -1 when finished or on bad args.
 */
int
gj_multipart_next_part(const char **pp, const char *end, const char *bound,
                       const char **part, size_t *plen)
{
	const char *pCur;
	const char *pBase;
	const char *pDelim;
	const char *pAfter;
	const char *pPart;
	const char *pDelim2;
	const char *pAfter2;
	const char *pPartEnd;
	size_t cbBound;
	int fClose;
	int fClose2;

	if (pp == NULL || *pp == NULL || end == NULL || bound == NULL ||
	    part == NULL || plen == NULL) {
		return -1;
	}
	pCur = *pp;
	if (pCur > end) {
		return -1;
	}

	cbBound = b196_strlen(bound);
	if (cbBound == 0u || cbBound > B196_BOUND_MAX) {
		return -1;
	}

	/*
	 * pBase for line-start: the caller's current cursor. The first
	 * delimiter of a body may sit at the body start without a
	 * preceding CRLF; subsequent delimiters are always at line
	 * starts relative to that same buffer walk because we leave
	 * *pp on the "--" of the next delimiter (which follows a CRLF).
	 *
	 * When searching for the *end* of a part we use pPart as the
	 * scan base so a delimiter must be preceded by '\n' (or be at
	 * pPart only if the part itself begins with "--bound", which
	 * is not a valid body-part start — still OK to require newline
	 * for the second search by using a base that forces mid-part
	 * matches to need '\n').
	 */
	pBase = pCur;

	if (b196_find_delim(pCur, pBase, end, bound, cbBound, &pDelim, &pAfter,
	        &fClose) != 0) {
		return -1;
	}

	if (fClose != 0) {
		/* Closing delimiter: no more parts. Advance past it. */
		pPart = b196_skip_delim_line(pAfter, end, 1);
		*pp = (pPart != NULL) ? pPart : end;
		return -1;
	}

	pPart = b196_skip_delim_line(pAfter, end, 0);
	if (pPart == NULL) {
		return -1;
	}

	/*
	 * Find the next delimiter after the part bytes. Require a real
	 * line break before it: use pPart as base so only pPart itself
	 * (degenerate) or bytes after '\n' match.
	 */
	if (b196_find_delim(pPart, pPart, end, bound, cbBound, &pDelim2,
	        &pAfter2, &fClose2) == 0) {
		/*
		 * Part payload ends at the CRLF/LF immediately before
		 * pDelim2. If neither is present (should not happen for
		 * a line-start match past pPart), end at pDelim2.
		 */
		pPartEnd = pDelim2;
		if (pPartEnd > pPart && pPartEnd[-1] == '\n') {
			pPartEnd--;
			if (pPartEnd > pPart && pPartEnd[-1] == '\r') {
				pPartEnd--;
			}
		}
		*part = pPart;
		*plen = (size_t)(pPartEnd - pPart);
		*pp = pDelim2;
		(void)pAfter2;
		(void)fClose2;
		return 0;
	}

	/* No further delimiter: remainder is the final part (lenient). */
	*part = pPart;
	*plen = (size_t)(end - pPart);
	*pp = end;
	return 0;
}

int __gj_multipart_next_part(const char **pp, const char *end,
                             const char *bound, const char **part,
                             size_t *plen)
    __attribute__((alias("gj_multipart_next_part")));
