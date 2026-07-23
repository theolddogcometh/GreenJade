/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1029: simple C-string unescape into bounded out.
 *
 * Surface (unique symbols):
 *   int gj_graph_unescape_c(const char *in, char *out, size_t cap);
 *     — Simple reverse of gj_graph_escape_c: \\, \", \n, \r, \t,
 *       \a, \b, \f, \v, and \xHH (two hex digits). Unknown escape
 *       or truncated sequence → -1. Returns 0 on success.
 *   int __gj_graph_unescape_c  (alias)
 *   __libcgj_batch1029_marker = "libcgj-batch1029"
 *
 * Does NOT redefine gj_graph_escape_c / gj_json_unescape.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1029_marker[] = "libcgj-batch1029";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1029_hexval(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'0' && u8Ch <= (unsigned char)'9') {
		return (int)(u8Ch - (unsigned char)'0');
	}
	if (u8Ch >= (unsigned char)'a' && u8Ch <= (unsigned char)'f') {
		return (int)(u8Ch - (unsigned char)'a' + 10);
	}
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'F') {
		return (int)(u8Ch - (unsigned char)'A' + 10);
	}
	return -1;
}

static int
b1029_putc(char *szOut, size_t cbCap, size_t *pcb, char ch)
{
	if (*pcb + 1u >= cbCap) {
		return -1;
	}
	szOut[(*pcb)++] = ch;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graph_unescape_c — simple C-string unescape into out.
 *
 * szIn:  NUL-terminated escaped text
 * szOut: destination buffer
 * cbCap: capacity including final NUL
 *
 * Supported: \\ \" \n \r \t \a \b \f \v \xHH. Bare non-backslash
 * octets copy through. Truncated/unknown escapes return -1.
 */
int
gj_graph_unescape_c(const char *szIn, char *szOut, size_t cbCap)
{
	size_t cbOut;
	size_t iIn;

	if (szIn == NULL || szOut == NULL || cbCap == 0u) {
		return -1;
	}

	cbOut = 0u;
	iIn = 0u;
	while (szIn[iIn] != '\0') {
		unsigned char u8Ch = (unsigned char)szIn[iIn];

		if (u8Ch != (unsigned char)'\\') {
			if (b1029_putc(szOut, cbCap, &cbOut,
			               (char)u8Ch) != 0) {
				return -1;
			}
			iIn++;
			continue;
		}

		iIn++;
		if (szIn[iIn] == '\0') {
			return -1;
		}
		u8Ch = (unsigned char)szIn[iIn];
		iIn++;

		switch (u8Ch) {
		case (unsigned char)'\\':
		case (unsigned char)'\"':
		case (unsigned char)'\'':
			if (b1029_putc(szOut, cbCap, &cbOut,
			               (char)u8Ch) != 0) {
				return -1;
			}
			break;
		case (unsigned char)'n':
			if (b1029_putc(szOut, cbCap, &cbOut, '\n') != 0) {
				return -1;
			}
			break;
		case (unsigned char)'r':
			if (b1029_putc(szOut, cbCap, &cbOut, '\r') != 0) {
				return -1;
			}
			break;
		case (unsigned char)'t':
			if (b1029_putc(szOut, cbCap, &cbOut, '\t') != 0) {
				return -1;
			}
			break;
		case (unsigned char)'a':
			if (b1029_putc(szOut, cbCap, &cbOut, '\a') != 0) {
				return -1;
			}
			break;
		case (unsigned char)'b':
			if (b1029_putc(szOut, cbCap, &cbOut, '\b') != 0) {
				return -1;
			}
			break;
		case (unsigned char)'f':
			if (b1029_putc(szOut, cbCap, &cbOut, '\f') != 0) {
				return -1;
			}
			break;
		case (unsigned char)'v':
			if (b1029_putc(szOut, cbCap, &cbOut, '\v') != 0) {
				return -1;
			}
			break;
		case (unsigned char)'x': {
			int nHi;
			int nLo;

			if (szIn[iIn] == '\0' || szIn[iIn + 1u] == '\0') {
				return -1;
			}
			nHi = b1029_hexval((unsigned char)szIn[iIn]);
			nLo = b1029_hexval((unsigned char)szIn[iIn + 1u]);
			if (nHi < 0 || nLo < 0) {
				return -1;
			}
			iIn += 2u;
			if (b1029_putc(szOut, cbCap, &cbOut,
			               (char)(unsigned char)((nHi << 4) |
			                                     nLo)) != 0) {
				return -1;
			}
			break;
		}
		default:
			return -1;
		}
	}

	szOut[cbOut] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_graph_unescape_c(const char *szIn, char *szOut, size_t cbCap)
    __attribute__((alias("gj_graph_unescape_c")));
