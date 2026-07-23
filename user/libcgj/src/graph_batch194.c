/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch194: HTTP/1.x status-line parse.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Builds under userspace -ffreestanding -msse2. No third-party HTTP parser
 * source was copied.
 *
 * Surface (unique symbols):
 *   int gj_http_status_line(const char *line, char *ver, size_t vcap,
 *                           int *code, char *reason, size_t rcap);
 *     Parse a single status-line (RFC 7230 subset):
 *       status-line = HTTP-version SP status-code SP reason-phrase [CR]LF
 *     HTTP-version = "HTTP/" DIGIT "." DIGIT
 *     status-code  = 3DIGIT  (stored as int in *code)
 *     reason-phrase may be empty; SP after status-code is required unless
 *     the line ends immediately after the code (empty reason accepted).
 *     Copies NUL-terminated version and reason into caller buffers
 *     (caps include the trailing NUL). Bare LF accepted in place of CRLF.
 *     Returns 0 on success, -1 on bad args / malformed / buffer too small.
 *   __gj_http_status_line  (alias)
 *   __libcgj_batch194_marker = "libcgj-batch194"
 *
 * No malloc, no errno (freestanding-safe).
 */

#include <stddef.h>

const char __libcgj_batch194_marker[] = "libcgj-batch194";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b194_is_digit(unsigned char ch)
{
	return ch >= (unsigned char)'0' && ch <= (unsigned char)'9';
}

/*
 * Copy cb bytes then NUL. Requires cbCap >= cb + 1.
 * Returns 0 on success, -1 if dst/cap invalid or buffer too small.
 */
static int
b194_copy_n(char *szDst, size_t cbCap, const char *pSrc, size_t cb)
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

/* ---- gj_http_status_line ----------------------------------------------- */

/*
 * Parse HTTP/1.x status-line into version buffer, status code, and reason.
 * Caps include the trailing NUL. Only the first line is examined; anything
 * after the EOL is ignored.
 */
int
gj_http_status_line(const char *line, char *ver, size_t vcap, int *code,
                    char *reason, size_t rcap)
{
	const char *p;
	const char *pVer;
	const char *pReason;
	size_t cbVer;
	size_t cbReason;
	int nCode;
	int i;

	if (line == NULL || ver == NULL || vcap == 0u || code == NULL ||
	    reason == NULL || rcap == 0u) {
		return -1;
	}

	p = line;

	/* HTTP-version: "HTTP/" DIGIT "." DIGIT */
	pVer = p;
	if (p[0] != 'H' || p[1] != 'T' || p[2] != 'T' || p[3] != 'P' ||
	    p[4] != '/') {
		return -1;
	}
	p += 5;
	cbVer = 5u;
	if (!b194_is_digit((unsigned char)*p)) {
		return -1;
	}
	p++;
	cbVer++;
	if (*p != '.') {
		return -1;
	}
	p++;
	cbVer++;
	if (!b194_is_digit((unsigned char)*p)) {
		return -1;
	}
	p++;
	cbVer++;

	/* SP after version */
	if (*p != ' ') {
		return -1;
	}
	p++;

	/* status-code: exactly 3 DIGIT */
	nCode = 0;
	for (i = 0; i < 3; i++) {
		if (!b194_is_digit((unsigned char)*p)) {
			return -1;
		}
		nCode = nCode * 10 + (int)((unsigned char)*p - (unsigned char)'0');
		p++;
	}

	/*
	 * After code: either EOL (empty reason) or SP then reason-phrase
	 * until CR/LF. reason-phrase may be empty after SP.
	 */
	if (*p == '\r' || *p == '\n' || *p == '\0') {
		pReason = p;
		cbReason = 0u;
	} else if (*p == ' ') {
		p++;
		pReason = p;
		cbReason = 0u;
		while (*p != '\0' && *p != '\r' && *p != '\n') {
			p++;
			cbReason++;
		}
	} else {
		return -1;
	}

	/* Require a real line ending (CRLF or LF). Bare NUL is not enough. */
	if (*p == '\r') {
		if (p[1] != '\n') {
			return -1;
		}
	} else if (*p != '\n') {
		return -1;
	}

	if (b194_copy_n(ver, vcap, pVer, cbVer) != 0) {
		return -1;
	}
	if (b194_copy_n(reason, rcap, pReason, cbReason) != 0) {
		return -1;
	}
	*code = nCode;
	return 0;
}

int __gj_http_status_line(const char *line, char *ver, size_t vcap, int *code,
                          char *reason, size_t rcap)
    __attribute__((alias("gj_http_status_line")));
