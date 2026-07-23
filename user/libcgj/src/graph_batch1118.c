/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1118: DNS label LDH validation.
 *
 * Surface (unique symbols):
 *   int gj_dns_label_ok(const char *label);
 *     — Return 1 if label is a single valid DNS hostname label:
 *       length 1..63, LDH [A-Za-z0-9-], no leading/trailing hyphen,
 *       no dots. NULL / empty → 0.
 *   int __gj_dns_label_ok  (alias)
 *   __libcgj_batch1118_marker = "libcgj-batch1118"
 *
 * Distinct from gj_dns_name_encode (batch337) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1118_marker[] = "libcgj-batch1118";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_label_ok — 1 if szLabel is a valid single LDH DNS label.
 */
int
gj_dns_label_ok(const char *szLabel)
{
	size_t cb;
	size_t i;
	unsigned char uCh;

	if (szLabel == NULL) {
		return 0;
	}

	cb = 0u;
	while (szLabel[cb] != '\0') {
		cb++;
		if (cb > 63u) {
			return 0;
		}
	}
	if (cb == 0u || cb > 63u) {
		return 0;
	}

	/* No leading or trailing hyphen. */
	if (szLabel[0] == '-' || szLabel[cb - 1u] == '-') {
		return 0;
	}

	for (i = 0u; i < cb; i++) {
		uCh = (unsigned char)szLabel[i];
		if (uCh == '.') {
			return 0;
		}
		if ((uCh >= 'A' && uCh <= 'Z') ||
		    (uCh >= 'a' && uCh <= 'z') ||
		    (uCh >= '0' && uCh <= '9') ||
		    uCh == '-') {
			continue;
		}
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_label_ok(const char *szLabel)
    __attribute__((alias("gj_dns_label_ok")));
