/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch510: freestanding POSIX shell single-quote escape.
 *
 * Surface (unique symbols):
 *   int gj_shell_quote(const char *in, char *out, size_t cap);
 *     — Quote a string for safe use as one word in a POSIX sh command
 *       line. Always surround with single quotes; every embedded '\''
 *       becomes '\'' (close quote, backslash-quote, open quote).
 *       Writes a NUL-terminated result into out when cap is large
 *       enough. Empty input yields ''. Returns 0 on success, -1 on
 *       NULL args, cap too small, or size overflow.
 *   int __gj_shell_quote  (alias)
 *   __libcgj_batch510_marker = "libcgj-batch510"
 *
 * Encoding (classic shell single-quote form):
 *   out = '\'' + map(c → c=='\'' ? "'\\''" : c for c in in) + '\'' + '\0'
 *   e.g. "hello" → 'hello'
 *        "it's"  → 'it'\''s'
 *        ""      → ''
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch510_marker[] = "libcgj-batch510";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_quote — POSIX sh single-quote escape into caller buffer.
 *
 * in:   NUL-terminated source text (may be empty; may contain '\'').
 * out:  destination buffer; must be non-NULL on call.
 * cap:  capacity of out including room for the terminating NUL.
 *       Minimum successful write is 3 bytes: '' + NUL.
 *
 * Returns 0 on success with a NUL-terminated quoted string in out.
 * Returns -1 if in/out is NULL, cap cannot hold the result, or the
 * measured body length would overflow size_t. Does not partially
 * commit a short buffer (measure pass first).
 */
int
gj_shell_quote(const char *in, char *out, size_t cap)
{
	const char *pSrc;
	size_t cbBody;
	size_t cbNeed;
	size_t iOut;
	char ch;

	if (in == NULL || out == NULL) {
		return -1;
	}
	/* Empty quoted string is ''; needs 3 bytes including NUL. */
	if (cap < 3u) {
		return -1;
	}

	/*
	 * Pass 1 — measure escaped body length (bytes between the outer
	 * quotes). Each non-quote is 1; each '\'' expands to four bytes
	 * in the wire form: '\''  (close, backslash, quote, open).
	 * That expansion straddles the outer quotes, but the net body
	 * growth relative to a plain copy is +3 per embedded quote
	 * (the close/open are still part of the total string).
	 *
	 * Total string layout for one embedded quote:
	 *   '  ...  ' \ ' '  ...  '
	 * so each '\'' in input contributes 4 output bytes in place of 1.
	 */
	cbBody = 0u;
	for (pSrc = in; *pSrc != '\0'; pSrc++) {
		if (*pSrc == '\'') {
			if (cbBody > (size_t)-1 - 4u) {
				return -1;
			}
			cbBody += 4u;
		} else {
			if (cbBody == (size_t)-1) {
				return -1;
			}
			cbBody += 1u;
		}
	}

	/* Total: opening ', body (with expansions), closing ', NUL. */
	if (cbBody > (size_t)-1 - 3u) {
		return -1;
	}
	cbNeed = cbBody + 3u;
	if (cbNeed > cap) {
		return -1;
	}

	/* Pass 2 — emit single-quoted form. */
	iOut = 0u;
	out[iOut++] = '\'';
	for (pSrc = in; (ch = *pSrc) != '\0'; pSrc++) {
		if (ch == '\'') {
			/* Close, backslash-quote, reopen: '\'' */
			out[iOut++] = '\'';
			out[iOut++] = '\\';
			out[iOut++] = '\'';
			out[iOut++] = '\'';
		} else {
			out[iOut++] = ch;
		}
	}
	out[iOut++] = '\'';
	out[iOut] = '\0';

	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shell_quote(const char *in, char *out, size_t cap)
    __attribute__((alias("gj_shell_quote")));
