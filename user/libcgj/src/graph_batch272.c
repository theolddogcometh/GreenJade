/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch272: JSON string body unescape (simple).
 *
 * Surface (unique symbols — distinct from batch77 json_unescape):
 *   int gj_json_unescape(const char *in, char *out, size_t out_cap);
 *     — Unescape a JSON string body (no surrounding quotes) into a
 *       NUL-terminated C string. Supported escapes only:
 *         \"  \\  \/  \b  \f  \n  \r  \t  \u00XX
 *       where XX is two hex digits; \u00XX emits a single byte 0xXX.
 *       in is NUL-terminated. Returns 0 on success, -1 on error
 *       (NULL args, short cap, illegal escape, bare control char, or
 *       truncated escape). On error out is undefined.
 *   __gj_json_unescape  (alias)
 *   __libcgj_batch272_marker = "libcgj-batch272"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   json_escape / json_unescape / gj_json_get_*  → graph_batch77.c
 *   gj_json_get_bool / array helpers             → graph_batch107.c
 *   gj_json_pointer_get                          → graph_batch161.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party JSON source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch272_marker[] = "libcgj-batch272";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b272_hexval — map one ASCII hex digit to 0..15, or -1 if not hex.
 */
static int
b272_hexval(unsigned char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0');
	}
	if (ch >= 'a' && ch <= 'f') {
		return (int)(ch - 'a' + 10);
	}
	if (ch >= 'A' && ch <= 'F') {
		return (int)(ch - 'A' + 10);
	}
	return -1;
}

/*
 * b272_putc — append one byte; leave room for final NUL. Returns 0 / -1.
 */
static int
b272_putc(char *szOut, size_t cbCap, size_t *pcb, char ch)
{
	if (*pcb + 1u >= cbCap) {
		return -1;
	}
	szOut[(*pcb)++] = ch;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_unescape — JSON string body → C string (simple escapes).
 *
 * in:       NUL-terminated escaped body (no surrounding '"').
 * out:      destination buffer; must be non-NULL.
 * out_cap:  capacity of out including room for NUL; must be >= 1.
 *
 * Supported escapes: \" \\ \/ \b \f \n \r \t and \u00XX (single byte).
 * Unescaped bytes below 0x20 are rejected (JSON string body rule).
 * Unknown escapes and truncated sequences return -1.
 *
 * Success: out is NUL-terminated unescaped text; return 0.
 * Failure: return -1; out contents undefined.
 */
int
gj_json_unescape(const char *in, char *out, size_t out_cap)
{
	const char *pSz;
	size_t cbOut;

	if (in == NULL || out == NULL || out_cap < 1u) {
		return -1;
	}

	pSz = in;
	cbOut = 0u;

	while (*pSz != '\0') {
		unsigned char ch = (unsigned char)*pSz;

		/* Unescaped control chars are illegal in JSON strings. */
		if (ch < 0x20u) {
			return -1;
		}

		if (ch != (unsigned char)'\\') {
			if (b272_putc(out, out_cap, &cbOut, (char)ch) != 0) {
				return -1;
			}
			pSz++;
			continue;
		}

		/* Escape sequence. */
		pSz++;
		if (*pSz == '\0') {
			return -1;
		}
		ch = (unsigned char)*pSz;
		pSz++;

		switch (ch) {
		case '"':
		case '\\':
		case '/':
			if (b272_putc(out, out_cap, &cbOut, (char)ch) != 0) {
				return -1;
			}
			break;
		case 'b':
			if (b272_putc(out, out_cap, &cbOut, '\b') != 0) {
				return -1;
			}
			break;
		case 'f':
			if (b272_putc(out, out_cap, &cbOut, '\f') != 0) {
				return -1;
			}
			break;
		case 'n':
			if (b272_putc(out, out_cap, &cbOut, '\n') != 0) {
				return -1;
			}
			break;
		case 'r':
			if (b272_putc(out, out_cap, &cbOut, '\r') != 0) {
				return -1;
			}
			break;
		case 't':
			if (b272_putc(out, out_cap, &cbOut, '\t') != 0) {
				return -1;
			}
			break;
		case 'u': {
			/*
			 * Simple form only: \u00XX → single byte 0xXX.
			 * Require four hex digits with high byte forced 00.
			 */
			int n0, n1, n2, n3;
			unsigned uByte;

			if (pSz[0] == '\0' || pSz[1] == '\0' ||
			    pSz[2] == '\0' || pSz[3] == '\0') {
				return -1;
			}
			n0 = b272_hexval((unsigned char)pSz[0]);
			n1 = b272_hexval((unsigned char)pSz[1]);
			n2 = b272_hexval((unsigned char)pSz[2]);
			n3 = b272_hexval((unsigned char)pSz[3]);
			if (n0 < 0 || n1 < 0 || n2 < 0 || n3 < 0) {
				return -1;
			}
			/* High byte must be 0x00 (simple \u00XX only). */
			if (n0 != 0 || n1 != 0) {
				return -1;
			}
			uByte = ((unsigned)n2 << 4) | (unsigned)n3;
			pSz += 4;
			if (b272_putc(out, out_cap, &cbOut,
				      (char)(unsigned char)uByte) != 0) {
				return -1;
			}
			break;
		}
		default:
			return -1;
		}
	}

	out[cbOut] = '\0';
	return 0;
}

int __gj_json_unescape(const char *in, char *out, size_t out_cap)
    __attribute__((alias("gj_json_unescape")));
