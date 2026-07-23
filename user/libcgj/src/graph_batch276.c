/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch276: RFC 4180 CSV field quote.
 *
 * Surface (unique symbols):
 *   int gj_csv_quote(const char *in, char *out, size_t out_cap);
 *     — Quote a single CSV field per RFC 4180: always surround with
 *       double quotes, and double every internal " ( " → "" ).
 *       Writes a NUL-terminated result into out when out_cap is large
 *       enough. Empty input yields "". Returns 0 on success, -1 on
 *       NULL args, out_cap too small, or size overflow.
 *   __gj_csv_quote  (alias)
 *   __libcgj_batch276_marker = "libcgj-batch276"
 *
 * Pair of graph_batch138 (gj_csv_next_field / gj_csv_count_fields): this
 * TU emits a quoted field; batch138 consumes RFC 4180 fields.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party CSV source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch276_marker[] = "libcgj-batch276";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_csv_quote — RFC 4180 field encoding (always-quoted form).
 *
 * in:       NUL-terminated field text (may be empty; may contain ").
 * out:      destination buffer; must be non-NULL on call.
 * out_cap:  capacity of out including room for the terminating NUL.
 *           Minimum successful write is 3 bytes: "" + NUL.
 *
 * Encoding:
 *   out = '"' + map(c → c=='"' ? "\"\"" : c for c in in) + '"' + '\0'
 *
 * Returns 0 on success with a NUL-terminated quoted field in out.
 * Returns -1 if in/out is NULL, out_cap cannot hold the result, or the
 * measured body length would overflow size_t. Does not partially
 * commit a short buffer (measure pass first).
 */
int
gj_csv_quote(const char *in, char *out, size_t out_cap)
{
	const char *pSrc;
	size_t cbBody;
	size_t cbNeed;
	size_t iOut;
	char ch;

	if (in == NULL || out == NULL) {
		return -1;
	}
	/* Empty quoted field is ""; needs 3 bytes including NUL. */
	if (out_cap < 3u) {
		return -1;
	}

	/*
	 * Pass 1 — measure escaped body length (bytes between the outer
	 * quotes). Each non-quote is 1; each " becomes two bytes.
	 */
	cbBody = 0u;
	for (pSrc = in; *pSrc != '\0'; pSrc++) {
		if (*pSrc == '"') {
			if (cbBody > (size_t)-1 - 2u) {
				return -1;
			}
			cbBody += 2u;
		} else {
			if (cbBody == (size_t)-1) {
				return -1;
			}
			cbBody += 1u;
		}
	}

	/* Total: opening ", body, closing ", NUL. */
	if (cbBody > (size_t)-1 - 3u) {
		return -1;
	}
	cbNeed = cbBody + 3u;
	if (cbNeed > out_cap) {
		return -1;
	}

	/* Pass 2 — emit always-quoted field. */
	iOut = 0u;
	out[iOut++] = '"';
	for (pSrc = in; (ch = *pSrc) != '\0'; pSrc++) {
		if (ch == '"') {
			out[iOut++] = '"';
			out[iOut++] = '"';
		} else {
			out[iOut++] = ch;
		}
	}
	out[iOut++] = '"';
	out[iOut] = '\0';

	return 0;
}

int __gj_csv_quote(const char *in, char *out, size_t out_cap)
    __attribute__((alias("gj_csv_quote")));
