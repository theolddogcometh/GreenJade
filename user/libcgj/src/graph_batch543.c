/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch543: RFC 4180 CSV field unquote.
 *
 * Surface (unique symbols):
 *   int gj_csv_unquote(const char *in, char *out, size_t cap);
 *     — Inverse of gj_csv_quote (graph_batch276). If in starts and ends
 *       with double-quote, strip the outer pair and collapse every
 *       internal "" to a single ". Otherwise copy in to out as-is.
 *       Writes a NUL-terminated result when cap is large enough.
 *       Returns 0 on success, -1 on NULL args or cap too small.
 *   __gj_csv_unquote  (alias)
 *   __libcgj_batch543_marker = "libcgj-batch543"
 *
 * Pair of graph_batch276 (gj_csv_quote): this TU decodes a quoted field;
 * batch276 encodes. Does NOT define gj_csv_quote / __gj_csv_quote.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party CSV source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch543_marker[] = "libcgj-batch543";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_csv_unquote — RFC 4180 field decoding (outer-quote strip form).
 *
 * in:   NUL-terminated field text (may be empty; may contain ").
 * out:  destination buffer; must be non-NULL on call.
 * cap:  capacity of out including room for the terminating NUL.
 *       Minimum successful write is 1 byte (empty string + NUL).
 *
 * Decoding:
 *   if len(in) >= 2 and in[0] == '"' and in[len-1] == '"':
 *     body = in[1 .. len-2]; map each "" → single "; copy other bytes
 *   else:
 *     copy in verbatim
 *   out = result + '\0'
 *
 * Returns 0 on success with a NUL-terminated unquoted field in out.
 * Returns -1 if in/out is NULL, cap is 0, or cap cannot hold the
 * result. Does not partially commit a short buffer (measure first).
 */
int
gj_csv_unquote(const char *in, char *out, size_t cap)
{
	const char *p;
	size_t len;
	size_t need;
	size_t i;
	size_t iOut;
	int quoted;

	if (in == NULL || out == NULL) {
		return -1;
	}
	if (cap == 0u) {
		return -1;
	}

	/* Measure input length (no libc strlen). */
	len = 0u;
	for (p = in; *p != '\0'; p++) {
		if (len == (size_t)-1) {
			return -1;
		}
		len += 1u;
	}

	quoted = (len >= 2u && in[0] == '"' && in[len - 1u] == '"') ? 1 : 0;

	if (!quoted) {
		/* Copy as-is: need len body bytes + NUL. */
		if (len >= cap) {
			return -1;
		}
		for (i = 0u; i < len; i++) {
			out[i] = in[i];
		}
		out[len] = '\0';
		return 0;
	}

	/*
	 * Pass 1 — measure unescaped body length between the outer
	 * quotes. Each non-pair byte is 1; each "" pair collapses to 1.
	 */
	need = 0u;
	for (i = 1u; i + 1u < len; ) {
		if (in[i] == '"' && (i + 1u) < (len - 1u) && in[i + 1u] == '"') {
			if (need == (size_t)-1) {
				return -1;
			}
			need += 1u;
			i += 2u;
		} else {
			if (need == (size_t)-1) {
				return -1;
			}
			need += 1u;
			i += 1u;
		}
	}

	/* Total: body + NUL. */
	if (need >= cap) {
		return -1;
	}

	/* Pass 2 — emit unquoted field. */
	iOut = 0u;
	for (i = 1u; i + 1u < len; ) {
		if (in[i] == '"' && (i + 1u) < (len - 1u) && in[i + 1u] == '"') {
			out[iOut++] = '"';
			i += 2u;
		} else {
			out[iOut++] = in[i];
			i += 1u;
		}
	}
	out[iOut] = '\0';

	return 0;
}

int __gj_csv_unquote(const char *in, char *out, size_t cap)
    __attribute__((alias("gj_csv_unquote")));
