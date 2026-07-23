/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch144: SMTP response parse (RFC 5321 lite).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party SMTP client source was copied.
 *
 * SMTP reply line shape (public, well-known):
 *   <3-digit-code> ( " " | "-" ) [ text ] [ CR ] [ LF ]
 *   Examples:
 *     "220 mail.example ESMTP"     → code 220, final
 *     "250-PIPELINING"             → code 250, continuation
 *     "250 OK"                     → code 250, final
 *     "550 5.1.1 User unknown"     → code 550, final
 *
 * Surface (unique symbols):
 *   int gj_smtp_parse_code(const char *line, int *code, int *is_final);
 *     — parse the leading three-digit reply code from a single line.
 *       *code = 100..999; *is_final = 1 when the separator is SP (or the
 *       line ends right after the digits), 0 when the separator is '-'.
 *       Leading ASCII whitespace is skipped. Returns 0 on success, -1 on
 *       bad args or a line that does not begin with three digits.
 *   int gj_smtp_is_success(int code);
 *     — non-zero when code is a 2yz Positive Completion reply (200..299).
 *   __gj_smtp_parse_code / __gj_smtp_is_success  (aliases)
 *   __libcgj_batch144_marker = "libcgj-batch144"
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch144_marker[] = "libcgj-batch144";

/* ---- gj_smtp_parse_code ------------------------------------------------ */

/*
 * Parse one SMTP reply line. Does not require a trailing CRLF; stops at
 * NUL or end of the three-digit field. Continuation vs final is decided
 * solely by the character immediately after the code (RFC 5321).
 */
int
gj_smtp_parse_code(const char *line, int *code, int *is_final)
{
	const unsigned char *p;
	int nCode;

	if (line == NULL || code == NULL || is_final == NULL) {
		return -1;
	}

	p = (const unsigned char *)line;

	/* Optional leading ASCII whitespace (SP / HT / CR / LF). */
	while (*p == (unsigned char)' ' || *p == (unsigned char)'\t' ||
	    *p == (unsigned char)'\r' || *p == (unsigned char)'\n') {
		p++;
	}

	/* Exactly three decimal digits required. */
	if (p[0] < (unsigned char)'0' || p[0] > (unsigned char)'9' ||
	    p[1] < (unsigned char)'0' || p[1] > (unsigned char)'9' ||
	    p[2] < (unsigned char)'0' || p[2] > (unsigned char)'9') {
		return -1;
	}

	nCode = (int)(p[0] - (unsigned char)'0') * 100 +
	    (int)(p[1] - (unsigned char)'0') * 10 +
	    (int)(p[2] - (unsigned char)'0');
	p += 3;

	/*
	 * Separator:
	 *   '-'  → multi-line continuation (not final)
	 *   ' '  → final line of this reply (or single-line)
	 *   NUL / CR / LF / end-of-text → treat as final (no text body)
	 *   anything else after digits → malformed
	 */
	if (*p == (unsigned char)'-') {
		*is_final = 0;
	} else if (*p == (unsigned char)' ' || *p == (unsigned char)'\0' ||
	    *p == (unsigned char)'\r' || *p == (unsigned char)'\n') {
		*is_final = 1;
	} else {
		return -1;
	}

	*code = nCode;
	return 0;
}

int __gj_smtp_parse_code(const char *line, int *code, int *is_final)
    __attribute__((alias("gj_smtp_parse_code")));

/* ---- gj_smtp_is_success ------------------------------------------------ */

/*
 * Positive Completion replies are 2yz (RFC 5321 §4.2.1). Preliminary
 * (1yz) and intermediate (3yz) are not completion success for a finished
 * command, so they return 0 here.
 */
int
gj_smtp_is_success(int code)
{
	return (code >= 200 && code <= 299) ? 1 : 0;
}

int __gj_smtp_is_success(int code)
    __attribute__((alias("gj_smtp_is_success")));
