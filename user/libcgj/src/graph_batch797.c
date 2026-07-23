/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch797: decimal TCP/UDP port parse (0..65535).
 *
 * Surface (unique symbols):
 *   int gj_port_parse(const char *s, unsigned *out);
 *     — Parse an unsigned decimal port in [0, 65535] from s into *out.
 *       Returns 0 on success, -1 on failure (NULL s/out, empty, non-
 *       digits, overflow past 65535, trailing junk).
 *   int __gj_port_parse  (alias)
 *   __libcgj_batch797_marker = "libcgj-batch797"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch797_marker[] = "libcgj-batch797";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_parse — strict decimal port 0..65535 into *out.
 *
 * Leading zeros allowed. No sign, no whitespace, entire string must be
 * digits. Empty string and values > 65535 fail.
 */
int
gj_port_parse(const char *sz, unsigned *pOut)
{
	unsigned uVal;
	unsigned uDigits;
	const char *p;

	if (sz == NULL || pOut == NULL) {
		return -1;
	}
	if (sz[0] == '\0') {
		return -1;
	}

	p = sz;
	uVal = 0u;
	uDigits = 0u;
	while (*p >= '0' && *p <= '9') {
		unsigned uDigit;

		uDigit = (unsigned)(*p - '0');
		/* Reject if 10*uVal + digit would exceed 65535. */
		if (uVal > 6553u || (uVal == 6553u && uDigit > 5u)) {
			return -1;
		}
		uVal = uVal * 10u + uDigit;
		uDigits++;
		p++;
	}

	if (uDigits == 0u || *p != '\0') {
		return -1;
	}

	*pOut = uVal;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_parse(const char *sz, unsigned *pOut)
    __attribute__((alias("gj_port_parse")));
