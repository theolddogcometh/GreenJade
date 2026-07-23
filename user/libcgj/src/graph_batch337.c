/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch337: DNS wire name encode from dotted
 * presentation form (RFC 1035 §3.1 labels, no compression).
 *
 * Surface (unique symbols):
 *   int gj_dns_name_encode(const char *dotted, uint8_t *out, size_t cap,
 *                          size_t *nout);
 *     — Encode presentation-form domain name (dot-separated labels) into
 *       DNS wire form at out[0..cap): each label as <len><bytes>, then a
 *       root terminator 0. Empty string or "." alone is the root name
 *       (single 0). A trailing '.' (FQDN) is accepted and does not emit
 *       an empty label. No compression pointers. On success writes *nout
 *       to the encoded byte length (including the root 0) and returns 0.
 *       Returns -1 on NULL dotted/out/nout, empty interior labels, label
 *       length > 63, total wire length > 255, or insufficient cap.
 *       Does not set errno (freestanding). *nout is unchanged on failure.
 *   __gj_dns_name_encode  (alias)
 *   __libcgj_batch337_marker = "libcgj-batch337"
 *
 * Distinct from gj_dns_pack_query / dn_comp (full query / compression
 * pointer tables): this TU is the freestanding name-only encoder.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch337_marker[] = "libcgj-batch337";

/* RFC 1035 §2.3.4 / §3.1 limits */
#define B337_MAX_LABEL 63u
#define B337_MAX_NAME  255u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_name_encode — dotted presentation → uncompressed DNS wire name.
 *
 * dotted: NUL-terminated presentation name (e.g. "www.example.com" or
 *         "www.example.com."); NULL is an error. "" and "." are root.
 * out:    destination buffer of at least cap bytes; NULL is an error.
 * cap:    capacity of out in bytes.
 * nout:   on success, receives encoded length including root 0.
 *
 * Returns 0 on success, -1 on invalid input or insufficient capacity.
 */
int
gj_dns_name_encode(const char *szDotted, uint8_t *pOut, size_t cbCap,
                   size_t *pNout)
{
	const char *pCur;
	size_t cbWire;
	size_t cbLabel;
	size_t iByte;

	if (szDotted == NULL || pOut == NULL || pNout == NULL) {
		return -1;
	}

	/* Root: empty or lone "." → single zero label. */
	if (szDotted[0] == '\0' ||
	    (szDotted[0] == '.' && szDotted[1] == '\0')) {
		if (cbCap < 1u) {
			return -1;
		}
		pOut[0] = 0u;
		*pNout = 1u;
		return 0;
	}

	pCur = szDotted;
	cbWire = 0u;

	while (*pCur != '\0') {
		/* Measure one label up to '.' or end. */
		cbLabel = 0u;
		while (pCur[cbLabel] != '\0' && pCur[cbLabel] != '.') {
			cbLabel++;
		}

		/* Empty label: leading, consecutive, or stray interior. */
		if (cbLabel == 0u) {
			return -1;
		}
		if (cbLabel > B337_MAX_LABEL) {
			return -1;
		}

		/*
		 * Wire total so far + this label's length byte and bytes +
		 * eventual root 0 must stay within the 255-octet name limit.
		 */
		if (cbWire + 1u + cbLabel + 1u > B337_MAX_NAME) {
			return -1;
		}
		if (cbWire + 1u + cbLabel > cbCap) {
			return -1;
		}

		pOut[cbWire] = (uint8_t)cbLabel;
		for (iByte = 0u; iByte < cbLabel; iByte++) {
			pOut[cbWire + 1u + iByte] = (uint8_t)pCur[iByte];
		}
		cbWire += 1u + cbLabel;
		pCur += cbLabel;

		if (*pCur == '.') {
			pCur++;
			if (*pCur == '\0') {
				/* Trailing dot: FQDN, stop before empty label. */
				break;
			}
		}
	}

	/* Root terminator. */
	if (cbWire + 1u > B337_MAX_NAME) {
		return -1;
	}
	if (cbWire + 1u > cbCap) {
		return -1;
	}
	pOut[cbWire] = 0u;
	*pNout = cbWire + 1u;
	return 0;
}

int __gj_dns_name_encode(const char *szDotted, uint8_t *pOut, size_t cbCap,
                         size_t *pNout)
    __attribute__((alias("gj_dns_name_encode")));
