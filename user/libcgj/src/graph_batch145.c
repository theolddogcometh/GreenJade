/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch145: IMAP untagged line helpers (RFC 3501 lite).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party IMAP client source was copied.
 *
 * IMAP server response lines (untagged form):
 *   "* " data-or-status CRLF
 * Examples:
 *   * 23 EXISTS
 *   * 1 RECENT
 *   * 12 FETCH (FLAGS (\Seen) UID 100)
 *
 * Surface (unique symbols):
 *   int gj_imap_is_untagged(const char *line);
 *     Return 1 if line is an untagged response (starts with '*'), else 0.
 *     NULL → 0. Leading CR/LF are not accepted.
 *
 *   int gj_imap_parse_exists(const char *line, unsigned long *n);
 *     Parse "* <number> EXISTS" (EXISTS case-insensitive). Writes the
 *     message-count into *n. Trailing SP / CR / LF after EXISTS are
 *     allowed. Returns 0 on success, -1 on error.
 *
 *   int gj_imap_parse_uid(const char *line, unsigned long *uid);
 *     Scan line for a whole-word "UID" token (case-insensitive) followed
 *     by a decimal number (as in FETCH attribute lists). Does not match
 *     UIDVALIDITY / UIDNEXT prefixes. Writes the first UID into *uid.
 *     Returns 0 on success, -1 on error.
 *
 *   __gj_imap_is_untagged / __gj_imap_parse_exists / __gj_imap_parse_uid
 *   __libcgj_batch145_marker = "libcgj-batch145"
 */

#include <stddef.h>
#include <stdint.h>

/* ULONG_MAX without limits.h — freestanding. */
#ifndef ULONG_MAX
#define ULONG_MAX ((unsigned long)-1)
#endif

const char __libcgj_batch145_marker[] = "libcgj-batch145";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b145_is_digit(unsigned char ch)
{
	return (ch >= (unsigned char)'0' && ch <= (unsigned char)'9') ? 1 : 0;
}

static int
b145_is_space(unsigned char ch)
{
	return (ch == (unsigned char)' ' || ch == (unsigned char)'\t') ? 1 : 0;
}

/* ASCII fold A-Z → a-z; other octets unchanged. */
static unsigned char
b145_lower(unsigned char ch)
{
	if (ch >= (unsigned char)'A' && ch <= (unsigned char)'Z') {
		return (unsigned char)(ch - (unsigned char)'A' + (unsigned char)'a');
	}
	return ch;
}

/*
 * Case-insensitive match of keyword szKey (NUL-terminated ASCII) at p.
 * Returns 1 on match; does not require a word boundary after the key.
 */
static int
b145_kw_eq(const char *p, const char *szKey)
{
	size_t i;

	if (p == NULL || szKey == NULL) {
		return 0;
	}
	for (i = 0u; szKey[i] != '\0'; i++) {
		if (p[i] == '\0') {
			return 0;
		}
		if (b145_lower((unsigned char)p[i]) !=
		    b145_lower((unsigned char)szKey[i])) {
			return 0;
		}
	}
	return 1;
}

/* Skip SP / HTAB. */
static const char *
b145_skip_ws(const char *p)
{
	if (p == NULL) {
		return NULL;
	}
	while (*p != '\0' && b145_is_space((unsigned char)*p)) {
		p++;
	}
	return p;
}

/*
 * Parse one unsigned decimal into *pu. Requires at least one digit.
 * Overflow → -1. On success advances *pp past the digits.
 */
static int
b145_parse_ulong(const char **pp, unsigned long *pu)
{
	const char *p;
	unsigned long uAcc = 0ul;
	unsigned long uDig;
	int nSaw = 0;

	if (pp == NULL || *pp == NULL || pu == NULL) {
		return -1;
	}
	p = *pp;
	while (b145_is_digit((unsigned char)*p)) {
		uDig = (unsigned long)((unsigned char)*p - (unsigned char)'0');
		if (uAcc > (ULONG_MAX - uDig) / 10ul) {
			return -1;
		}
		uAcc = uAcc * 10ul + uDig;
		nSaw = 1;
		p++;
	}
	if (!nSaw) {
		return -1;
	}
	*pu = uAcc;
	*pp = p;
	return 0;
}

/*
 * True if ch cannot appear inside an IMAP atom / keyword (word boundary).
 * Treats end-of-string, whitespace, CR/LF, and common list delimiters as
 * boundaries so "UID" does not match inside "UIDVALIDITY" / "UIDNEXT".
 */
static int
b145_is_word_end(unsigned char ch)
{
	if (ch == (unsigned char)'\0' || b145_is_space(ch) ||
	    ch == (unsigned char)'\r' || ch == (unsigned char)'\n') {
		return 1;
	}
	if (ch == (unsigned char)'(' || ch == (unsigned char)')' ||
	    ch == (unsigned char)'[' || ch == (unsigned char)']' ||
	    ch == (unsigned char)'{' || ch == (unsigned char)'}' ||
	    ch == (unsigned char)'<' || ch == (unsigned char)'>' ||
	    ch == (unsigned char)'"' || ch == (unsigned char)'%' ||
	    ch == (unsigned char)'*' || ch == (unsigned char)'\\') {
		return 1;
	}
	/* Also stop on non-alnum for safety (IMAP atoms are restrictive). */
	if (!((ch >= (unsigned char)'A' && ch <= (unsigned char)'Z') ||
	      (ch >= (unsigned char)'a' && ch <= (unsigned char)'z') ||
	      (ch >= (unsigned char)'0' && ch <= (unsigned char)'9') ||
	      ch == (unsigned char)'-' || ch == (unsigned char)'.' ||
	      ch == (unsigned char)'_')) {
		return 1;
	}
	return 0;
}

/* True if p[-1] (when p > start) is a word boundary before a keyword. */
static int
b145_is_word_start(const char *pStart, const char *p)
{
	if (p <= pStart) {
		return 1;
	}
	return b145_is_word_end((unsigned char)p[-1]);
}

/* ---- gj_imap_is_untagged ----------------------------------------------- */

/*
 * Untagged responses begin with '*' (RFC 3501 §2.2.2). Tagged responses
 * begin with a tag atom; continuations begin with '+'.
 */
int
gj_imap_is_untagged(const char *line)
{
	if (line == NULL) {
		return 0;
	}
	if (line[0] != '*') {
		return 0;
	}
	/* Accept bare "*", "* ", or "*\r" / "*\n" (CRLF-stripped buffers). */
	if (line[1] == '\0' || b145_is_space((unsigned char)line[1]) ||
	    line[1] == '\r' || line[1] == '\n') {
		return 1;
	}
	return 0;
}

int __gj_imap_is_untagged(const char *line)
    __attribute__((alias("gj_imap_is_untagged")));

/* ---- gj_imap_parse_exists ---------------------------------------------- */

/*
 * Match: "*" SP number SP "EXISTS" [SP / CR / LF / end]
 * Number is one or more decimal digits (message count, may be 0).
 */
int
gj_imap_parse_exists(const char *line, unsigned long *n)
{
	const char *p;
	unsigned long uCount;

	if (line == NULL || n == NULL) {
		return -1;
	}
	if (!gj_imap_is_untagged(line)) {
		return -1;
	}

	p = line + 1; /* skip '*' */
	p = b145_skip_ws(p);
	if (b145_parse_ulong(&p, &uCount) != 0) {
		return -1;
	}
	p = b145_skip_ws(p);

	if (!b145_kw_eq(p, "EXISTS")) {
		return -1;
	}
	p += 6; /* strlen("EXISTS") */

	/* Trailing junk must only be whitespace / line ending. */
	p = b145_skip_ws(p);
	if (*p != '\0' && *p != '\r' && *p != '\n') {
		return -1;
	}

	*n = uCount;
	return 0;
}

int __gj_imap_parse_exists(const char *line, unsigned long *n)
    __attribute__((alias("gj_imap_parse_exists")));

/* ---- gj_imap_parse_uid ------------------------------------------------- */

/*
 * Scan for the first whole-word "UID" followed by a decimal number.
 * Typical FETCH:  * 12 FETCH (FLAGS (\Seen) UID 100 BODY[])
 * Rejects UIDVALIDITY / UIDNEXT by requiring a word end after "UID".
 */
int
gj_imap_parse_uid(const char *line, unsigned long *uid)
{
	const char *pStart;
	const char *p;
	const char *pNum;
	unsigned long uVal;

	if (line == NULL || uid == NULL) {
		return -1;
	}

	pStart = line;
	p = line;
	while (*p != '\0') {
		if (b145_kw_eq(p, "UID") && b145_is_word_start(pStart, p) &&
		    b145_is_word_end((unsigned char)p[3])) {
			pNum = b145_skip_ws(p + 3);
			if (b145_parse_ulong(&pNum, &uVal) == 0) {
				/* Number must be a discrete token. */
				if (b145_is_word_end((unsigned char)*pNum) ||
				    b145_is_space((unsigned char)*pNum) ||
				    *pNum == '\0' || *pNum == '\r' ||
				    *pNum == '\n') {
					*uid = uVal;
					return 0;
				}
			}
			/* Keyword matched but number bad — keep scanning. */
		}
		p++;
	}
	return -1;
}

int __gj_imap_parse_uid(const char *line, unsigned long *uid)
    __attribute__((alias("gj_imap_parse_uid")));
