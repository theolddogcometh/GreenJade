/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1129: HTTP Accept qvalue parser (RFC 7231).
 *
 * Surface (unique symbols):
 *   int gj_accept_qvalue_parse(const char *s, unsigned *out_milli);
 *     — Parse a weight / qvalue: "q="? optional, then
 *       ("0" [ "." 0*3DIGIT ]) / ("1" [ "." 0*3("0") ])
 *       Writes milli-q in [0,1000] to *out_milli on success and
 *       returns 0. On failure returns -1 and does not write.
 *       Leading OWS allowed. NULL s or out_milli → -1.
 *   int __gj_accept_qvalue_parse  (alias)
 *   __libcgj_batch1129_marker = "libcgj-batch1129"
 *
 * Distinct from general float parsers — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1129_marker[] = "libcgj-batch1129";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_accept_qvalue_parse — parse qvalue into milli units [0,1000].
 *
 * Accepts optional "q=" prefix (case-sensitive q), optional leading
 * SP/HTAB. Forms: 0, 0.x, 0.xx, 0.xxx, 1, 1.0, 1.00, 1.000.
 */
int
gj_accept_qvalue_parse(const char *sz, unsigned *pOutMilli)
{
	const char *pCur;
	unsigned uMilli;
	unsigned uFrac;
	unsigned uScale;
	unsigned cDigits;
	unsigned char uCh;

	if (sz == NULL || pOutMilli == NULL) {
		return -1;
	}

	pCur = sz;
	while (*pCur == ' ' || *pCur == '\t') {
		pCur++;
	}

	/* Optional "q=" (case-sensitive 'q' per common practice). */
	if (pCur[0] == 'q' && pCur[1] == '=') {
		pCur += 2;
	}

	uCh = (unsigned char)*pCur;
	if (uCh != '0' && uCh != '1') {
		return -1;
	}

	if (uCh == '1') {
		pCur++;
		uMilli = 1000u;
		if (*pCur == '.') {
			pCur++;
			cDigits = 0u;
			while (*pCur == '0') {
				cDigits++;
				pCur++;
				if (cDigits > 3u) {
					return -1;
				}
			}
			/* After 1., only zeros allowed (and at most 3). */
			if (cDigits == 0u && *pCur != '\0' &&
			    *pCur != ' ' && *pCur != '\t' &&
			    *pCur != ';' && *pCur != ',') {
				/* non-zero frac digit after 1. → invalid */
				uCh = (unsigned char)*pCur;
				if (uCh >= '0' && uCh <= '9') {
					return -1;
				}
			}
			if (*pCur != '\0' && *pCur != ' ' && *pCur != '\t' &&
			    *pCur != ';' && *pCur != ',') {
				uCh = (unsigned char)*pCur;
				if (uCh >= '0' && uCh <= '9') {
					return -1;
				}
			}
		}
		/* Trailing OWS ok; other garbage → fail. */
		while (*pCur == ' ' || *pCur == '\t') {
			pCur++;
		}
		if (*pCur != '\0' && *pCur != ';' && *pCur != ',') {
			return -1;
		}
		*pOutMilli = uMilli;
		return 0;
	}

	/* uCh == '0' */
	pCur++;
	uMilli = 0u;
	if (*pCur == '.') {
		pCur++;
		uFrac = 0u;
		uScale = 100u;
		cDigits = 0u;
		while (cDigits < 3u) {
			uCh = (unsigned char)*pCur;
			if (uCh < '0' || uCh > '9') {
				break;
			}
			uFrac += (unsigned)(uCh - '0') * uScale;
			uScale /= 10u;
			pCur++;
			cDigits++;
		}
		/* More than 3 fraction digits → invalid. */
		uCh = (unsigned char)*pCur;
		if (uCh >= '0' && uCh <= '9') {
			return -1;
		}
		uMilli = uFrac;
	}

	while (*pCur == ' ' || *pCur == '\t') {
		pCur++;
	}
	if (*pCur != '\0' && *pCur != ';' && *pCur != ',') {
		return -1;
	}

	*pOutMilli = uMilli;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_accept_qvalue_parse(const char *sz, unsigned *pOutMilli)
    __attribute__((alias("gj_accept_qvalue_parse")));
