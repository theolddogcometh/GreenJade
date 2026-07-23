/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1028: C-string escape into bounded out buffer.
 *
 * Surface (unique symbols):
 *   int gj_graph_escape_c(const char *in, char *out, size_t cap);
 *     — Escape a C string into out for safe C-string embedding:
 *       \\ \" \n \r \t \a \b \f \v, and other controls / non-ASCII
 *       as \\xHH. Printable ASCII (0x20..0x7E except \\ and \") pass
 *       through. Returns 0 on success (NUL-terminated out), -1 on
 *       error (NULL args or insufficient cap).
 *   int __gj_graph_escape_c  (alias)
 *   __libcgj_batch1028_marker = "libcgj-batch1028"
 *
 * Does NOT redefine json_escape / gj_html_escape.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1028_marker[] = "libcgj-batch1028";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1028_putc(char *szOut, size_t cbCap, size_t *pcb, char ch)
{
	if (*pcb + 1u >= cbCap) {
		return -1;
	}
	szOut[(*pcb)++] = ch;
	return 0;
}

static int
b1028_puts(char *szOut, size_t cbCap, size_t *pcb, const char *szLit,
           size_t cbLit)
{
	size_t i;

	if (*pcb + cbLit >= cbCap) {
		return -1;
	}
	for (i = 0u; i < cbLit; i++) {
		szOut[*pcb + i] = szLit[i];
	}
	*pcb += cbLit;
	return 0;
}

static char
b1028_hex(unsigned uNibble)
{
	return (char)((uNibble < 10u) ? ('0' + (int)uNibble)
	                              : ('a' + (int)uNibble - 10));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graph_escape_c — escape C string body into out.
 *
 * szIn:  NUL-terminated source (NULL → -1)
 * szOut: destination (NULL → -1)
 * cbCap: capacity including final NUL (0 → -1)
 */
int
gj_graph_escape_c(const char *szIn, char *szOut, size_t cbCap)
{
	size_t cbOut;
	size_t iIn;

	if (szIn == NULL || szOut == NULL || cbCap == 0u) {
		return -1;
	}

	cbOut = 0u;
	for (iIn = 0u; szIn[iIn] != '\0'; iIn++) {
		unsigned char u8Ch = (unsigned char)szIn[iIn];
		const char *szEsc;
		size_t cbEsc;
		char aHex[4];

		switch (u8Ch) {
		case (unsigned char)'\\':
			szEsc = "\\\\";
			cbEsc = 2u;
			break;
		case (unsigned char)'\"':
			szEsc = "\\\"";
			cbEsc = 2u;
			break;
		case (unsigned char)'\n':
			szEsc = "\\n";
			cbEsc = 2u;
			break;
		case (unsigned char)'\r':
			szEsc = "\\r";
			cbEsc = 2u;
			break;
		case (unsigned char)'\t':
			szEsc = "\\t";
			cbEsc = 2u;
			break;
		case (unsigned char)'\a':
			szEsc = "\\a";
			cbEsc = 2u;
			break;
		case (unsigned char)'\b':
			szEsc = "\\b";
			cbEsc = 2u;
			break;
		case (unsigned char)'\f':
			szEsc = "\\f";
			cbEsc = 2u;
			break;
		case (unsigned char)'\v':
			szEsc = "\\v";
			cbEsc = 2u;
			break;
		default:
			if (u8Ch >= 0x20u && u8Ch <= 0x7Eu) {
				if (b1028_putc(szOut, cbCap, &cbOut,
				               (char)u8Ch) != 0) {
					return -1;
				}
				continue;
			}
			/* Control / non-ASCII → \xHH */
			aHex[0] = '\\';
			aHex[1] = 'x';
			aHex[2] = b1028_hex((unsigned)(u8Ch >> 4));
			aHex[3] = b1028_hex((unsigned)(u8Ch & 0x0Fu));
			szEsc = aHex;
			cbEsc = 4u;
			break;
		}

		if (b1028_puts(szOut, cbCap, &cbOut, szEsc, cbEsc) != 0) {
			return -1;
		}
	}

	szOut[cbOut] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_graph_escape_c(const char *szIn, char *szOut, size_t cbCap)
    __attribute__((alias("gj_graph_escape_c")));
