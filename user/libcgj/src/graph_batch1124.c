/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1124: HTTP-version string validation.
 *
 * Surface (unique symbols):
 *   int gj_http_version_ok(const char *version);
 *     — Return 1 if version matches HTTP-version:
 *       "HTTP/" DIGIT "." DIGIT  (exactly 8 octets, e.g. "HTTP/1.1").
 *       NULL / other → 0.
 *   int __gj_http_version_ok  (alias)
 *   __libcgj_batch1124_marker = "libcgj-batch1124"
 *
 * Distinct from gj_http_method_ok — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1124_marker[] = "libcgj-batch1124";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_http_version_ok — 1 if szVersion is "HTTP/" DIGIT "." DIGIT.
 */
int
gj_http_version_ok(const char *szVersion)
{
	unsigned char uMaj;
	unsigned char uMin;

	if (szVersion == NULL) {
		return 0;
	}

	if (szVersion[0] != 'H' || szVersion[1] != 'T' ||
	    szVersion[2] != 'T' || szVersion[3] != 'P' ||
	    szVersion[4] != '/') {
		return 0;
	}

	uMaj = (unsigned char)szVersion[5];
	if (uMaj < '0' || uMaj > '9') {
		return 0;
	}
	if (szVersion[6] != '.') {
		return 0;
	}
	uMin = (unsigned char)szVersion[7];
	if (uMin < '0' || uMin > '9') {
		return 0;
	}
	if (szVersion[8] != '\0') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_http_version_ok(const char *szVersion)
    __attribute__((alias("gj_http_version_ok")));
