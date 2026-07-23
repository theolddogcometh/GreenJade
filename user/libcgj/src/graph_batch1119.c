/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1119: DNS presentation-name validation.
 *
 * Surface (unique symbols):
 *   int gj_dns_name_ok(const char *name);
 *     — Return 1 if name is a valid dotted LDH hostname: one or more
 *       labels (1..63 each, LDH, no leading/trailing hyphen per label),
 *       total presentation length 1..253, optional single trailing dot
 *       for FQDN. NULL / empty / empty labels → 0.
 *   int __gj_dns_name_ok  (alias)
 *   __libcgj_batch1119_marker = "libcgj-batch1119"
 *
 * Distinct from gj_dns_label_ok (batch1118) / gj_dns_name_encode —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1119_marker[] = "libcgj-batch1119";

/* ---- helpers ------------------------------------------------------------ */

/*
 * b1119_label_ok — validate one LDH label of length cb (1..63).
 */
static int
b1119_label_ok(const char *sz, size_t cb)
{
	size_t i;
	unsigned char uCh;

	if (cb == 0u || cb > 63u) {
		return 0;
	}
	if (sz[0] == '-' || sz[cb - 1u] == '-') {
		return 0;
	}
	for (i = 0u; i < cb; i++) {
		uCh = (unsigned char)sz[i];
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_name_ok — 1 if szName is a valid dotted presentation hostname.
 */
int
gj_dns_name_ok(const char *szName)
{
	const char *pCur;
	size_t cbTotal;
	size_t cbLabel;
	size_t cLabels;

	if (szName == NULL) {
		return 0;
	}

	/* Measure total length (excluding optional trailing dot later). */
	cbTotal = 0u;
	while (szName[cbTotal] != '\0') {
		cbTotal++;
		if (cbTotal > 254u) {
			return 0;
		}
	}
	if (cbTotal == 0u) {
		return 0;
	}

	/* Lone "." is root — accept as valid empty-FQDN form. */
	if (szName[0] == '.' && szName[1] == '\0') {
		return 1;
	}

	/* Strip one trailing FQDN dot from the span we validate. */
	if (szName[cbTotal - 1u] == '.') {
		cbTotal--;
		if (cbTotal == 0u) {
			return 0;
		}
	}

	/* Presentation hostnames without root: 1..253 chars. */
	if (cbTotal > 253u) {
		return 0;
	}

	pCur = szName;
	cLabels = 0u;
	while (cbTotal > 0u) {
		cbLabel = 0u;
		while (cbLabel < cbTotal && pCur[cbLabel] != '.') {
			cbLabel++;
		}
		if (!b1119_label_ok(pCur, cbLabel)) {
			return 0;
		}
		cLabels++;
		pCur += cbLabel;
		if (cbLabel == cbTotal) {
			break;
		}
		/* Skip the separator dot; empty label → fail. */
		if (cbLabel + 1u >= cbTotal) {
			/* Trailing already stripped; this is empty. */
			return 0;
		}
		pCur++;
		cbTotal -= cbLabel + 1u;
	}

	return (cLabels > 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_name_ok(const char *szName)
    __attribute__((alias("gj_dns_name_ok")));
