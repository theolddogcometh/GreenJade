/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch146: FTP reply-code and PASV endpoint parse.
 * Clean-room freestanding pure C from the public FTP control-channel
 * reply forms (RFC 959 §4.2 / §4.1.2 PASV). Builds under userspace
 * -msse2. No third-party FTP client source was copied.
 *
 * Surface (unique symbols):
 *   int gj_ftp_parse_code(const char *line, int *code, int *is_final);
 *     Parse a single FTP reply line. Expects three leading digits; sets
 *     *code to the 0..999 reply value. *is_final is 0 when the fourth
 *     character is '-', else 1 (space / CR / LF / NUL / other → final).
 *     Returns 0 on success, -1 on NULL args or non-digit code.
 *   int gj_ftp_parse_pasv(const char *line, unsigned char ip[4],
 *                         unsigned *port);
 *     Parse a 227 PASV reply: six comma-separated octets in parentheses
 *       (h1,h2,h3,h4,p1,p2)
 *     → ip = {h1,h2,h3,h4}, port = p1*256 + p2. Scans for the first '('
 *     and requires a matching ')' after the six fields. Returns 0 ok,
 *     -1 on NULL args, missing fields, or out-of-range octets.
 *   __gj_ftp_parse_code / __gj_ftp_parse_pasv  (aliases)
 *   __libcgj_batch146_marker = "libcgj-batch146"
 *
 * Note: these helpers only decode wire text. They do not open sockets,
 * issue commands, or validate that *code is 227 for PASV.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch146_marker[] = "libcgj-batch146";

/* ---- freestanding digit helpers ---------------------------------------- */

static int
b146_is_digit(char c)
{
	return (c >= '0' && c <= '9') ? 1 : 0;
}

/*
 * Parse an unsigned decimal field of at most 3 digits into *pu (0..255).
 * Advances *pp past the digits. Rejects empty runs and values > 255.
 */
static int
b146_parse_u8(const char **pp, unsigned *pu)
{
	const char *p = *pp;
	unsigned u = 0u;
	unsigned cDig = 0u;

	if (!b146_is_digit(*p)) {
		return -1;
	}
	while (b146_is_digit(*p)) {
		unsigned d = (unsigned)(*p - '0');

		if (cDig >= 3u) {
			return -1;
		}
		u = u * 10u + d;
		if (u > 255u) {
			return -1;
		}
		cDig++;
		p++;
	}
	*pp = p;
	*pu = u;
	return 0;
}

/* ---- gj_ftp_parse_code ------------------------------------------------- */

/*
 * FTP reply line: "xyz " final, "xyz-" multi-line continuation (RFC 959).
 * Leading whitespace is not accepted; the three digits start at line[0].
 */
int
gj_ftp_parse_code(const char *line, int *code, int *is_final)
{
	unsigned u0, u1, u2;

	if (line == NULL || code == NULL || is_final == NULL) {
		return -1;
	}
	if (!b146_is_digit(line[0]) || !b146_is_digit(line[1]) ||
	    !b146_is_digit(line[2])) {
		return -1;
	}

	u0 = (unsigned)(line[0] - '0');
	u1 = (unsigned)(line[1] - '0');
	u2 = (unsigned)(line[2] - '0');
	*code = (int)(u0 * 100u + u1 * 10u + u2);

	/* '-' = intermediate multi-line; anything else is treated final. */
	*is_final = (line[3] == '-') ? 0 : 1;
	return 0;
}

int __gj_ftp_parse_code(const char *line, int *code, int *is_final)
    __attribute__((alias("gj_ftp_parse_code")));

/* ---- gj_ftp_parse_pasv ------------------------------------------------- */

/*
 * Locate "(h1,h2,h3,h4,p1,p2)" anywhere in the reply text (standard
 * PASV form after the 227 code and human message). Each hi/pi is a
 * decimal octet 0..255; port is network-order p1:p2.
 */
int
gj_ftp_parse_pasv(const char *line, unsigned char ip[4], unsigned *port)
{
	const char *p;
	unsigned a[6];
	unsigned i;

	if (line == NULL || ip == NULL || port == NULL) {
		return -1;
	}

	/* Find first '('. */
	p = line;
	while (*p != '\0' && *p != '(') {
		p++;
	}
	if (*p != '(') {
		return -1;
	}
	p++; /* past '(' */

	for (i = 0u; i < 6u; i++) {
		if (b146_parse_u8(&p, &a[i]) != 0) {
			return -1;
		}
		if (i < 5u) {
			if (*p != ',') {
				return -1;
			}
			p++; /* past ',' */
		}
	}

	if (*p != ')') {
		return -1;
	}

	ip[0] = (unsigned char)a[0];
	ip[1] = (unsigned char)a[1];
	ip[2] = (unsigned char)a[2];
	ip[3] = (unsigned char)a[3];
	*port = a[4] * 256u + a[5];
	return 0;
}

int __gj_ftp_parse_pasv(const char *line, unsigned char ip[4], unsigned *port)
    __attribute__((alias("gj_ftp_parse_pasv")));
