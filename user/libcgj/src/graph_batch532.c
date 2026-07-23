/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch532: freestanding classic hexdump line formatter.
 *
 * Surface (unique symbols):
 *   int gj_hexdump_line(char *out, size_t cap, const void *data, size_t n,
 *                       uint64_t addr);
 *     — Format one classic hexdump -C style line into out (NUL-term):
 *         AAAAAAAA  XX XX XX XX XX XX XX XX  XX XX XX XX XX XX XX XX  |ascii....|
 *       Up to 16 octets of data; n > 16 is treated as 16. Address is the
 *       low 32 bits printed as 8 lowercase hex digits. Non-printable
 *       octets (outside 0x20..0x7E) render as '.' in the ASCII column.
 *       Hex columns are fixed-width (pad missing slots with spaces) so
 *       the '|' gutter lines up. Returns 0 on success, -1 if out is NULL,
 *       cap is too small, or data is NULL with n > 0.
 *   int __gj_hexdump_line  (alias)
 *   __libcgj_batch532_marker = "libcgj-batch532"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc (no ctype/isprint). No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch532_marker[] = "libcgj-batch532";

/* Lowercase hex digit table (classic hexdump -C). */
static const char s_b532_hex[] = "0123456789abcdef";

/* ---- freestanding helpers ---------------------------------------------- */

/* Emit one lowercase hex digit nibble (0..15) at *pOut; advance. */
static void
b532_put_nibble(char **ppOut, unsigned uNibble)
{
	**ppOut = s_b532_hex[uNibble & 0xfu];
	(*ppOut)++;
}

/* Emit two lowercase hex digits for one octet. */
static void
b532_put_hex_byte(char **ppOut, unsigned char u8Byte)
{
	b532_put_nibble(ppOut, (unsigned)(u8Byte >> 4) & 0xfu);
	b532_put_nibble(ppOut, (unsigned)u8Byte & 0xfu);
}

/* Emit ch at *pOut; advance. */
static void
b532_put_ch(char **ppOut, char ch)
{
	**ppOut = ch;
	(*ppOut)++;
}

/* ASCII printable for classic dump: 0x20..0x7E (no locale / ctype). */
static int
b532_is_print(unsigned char u8Byte)
{
	return (u8Byte >= 0x20u && u8Byte <= 0x7eu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hexdump_line — one classic hexdump -C line into caller buffer.
 *
 * out:  destination buffer; must be non-NULL.
 * cap:  capacity of out including room for the terminating NUL.
 * data: input octets (NULL only legal when n == 0, or when n will be
 *       clamped but still requires a valid pointer if original n > 0 —
 *       NULL with n > 0 is always -1 before clamping).
 * n:    number of input octets; values above 16 are treated as 16.
 * addr: line address; low 32 bits rendered as 8 hex digits.
 *
 * Layout (fixed hex gutter width, variable ASCII length = shown count):
 *   %08x  + 16×("%02x "|pad) with mid-group space after byte 8
 *   + " |" + ascii + "|" + NUL
 *
 * Need size: 63 + nShow  (including NUL), nShow = min(n, 16).
 * Full 16-byte line needs 79 bytes.
 *
 * Returns 0 on success with a NUL-terminated line in out.
 * Returns -1 if out is NULL, data is NULL with n > 0, or cap is too small.
 * Does not partially commit a short buffer (measure pass first).
 */
int
gj_hexdump_line(char *out, size_t cap, const void *data, size_t n,
                uint64_t addr)
{
	const unsigned char *pData;
	char *pOut;
	size_t cShow;
	size_t cbNeed;
	size_t i;
	unsigned char u8Byte;
	uint32_t u32Addr;

	if (out == NULL) {
		return -1;
	}
	if (data == NULL && n > 0u) {
		return -1;
	}

	cShow = n;
	if (cShow > 16u) {
		cShow = 16u;
	}

	/*
	 * Fixed layout:
	 *   8 addr + 2 spaces + 1 mid-gap + 16*3 hex slots
	 *   + 2 (" |") + cShow ascii + 1 ("|") + 1 NUL
	 * = 63 + cShow
	 */
	cbNeed = 63u + cShow;
	if (cap < cbNeed) {
		return -1;
	}

	pData = (const unsigned char *)data;
	pOut = out;
	u32Addr = (uint32_t)(addr & 0xffffffffu);

	/* 8 hex digits of address (high nibble first). */
	b532_put_nibble(&pOut, (unsigned)(u32Addr >> 28) & 0xfu);
	b532_put_nibble(&pOut, (unsigned)(u32Addr >> 24) & 0xfu);
	b532_put_nibble(&pOut, (unsigned)(u32Addr >> 20) & 0xfu);
	b532_put_nibble(&pOut, (unsigned)(u32Addr >> 16) & 0xfu);
	b532_put_nibble(&pOut, (unsigned)(u32Addr >> 12) & 0xfu);
	b532_put_nibble(&pOut, (unsigned)(u32Addr >> 8) & 0xfu);
	b532_put_nibble(&pOut, (unsigned)(u32Addr >> 4) & 0xfu);
	b532_put_nibble(&pOut, (unsigned)u32Addr & 0xfu);

	/* Two spaces after address. */
	b532_put_ch(&pOut, ' ');
	b532_put_ch(&pOut, ' ');

	/* 16 hex slots; extra space between groups of 8. */
	for (i = 0u; i < 16u; i++) {
		if (i == 8u) {
			b532_put_ch(&pOut, ' ');
		}
		if (i < cShow) {
			u8Byte = pData[i];
			b532_put_hex_byte(&pOut, u8Byte);
			b532_put_ch(&pOut, ' ');
		} else {
			b532_put_ch(&pOut, ' ');
			b532_put_ch(&pOut, ' ');
			b532_put_ch(&pOut, ' ');
		}
	}

	/* " |" then ASCII then "|". */
	b532_put_ch(&pOut, ' ');
	b532_put_ch(&pOut, '|');
	for (i = 0u; i < cShow; i++) {
		u8Byte = pData[i];
		if (b532_is_print(u8Byte)) {
			b532_put_ch(&pOut, (char)u8Byte);
		} else {
			b532_put_ch(&pOut, '.');
		}
	}
	b532_put_ch(&pOut, '|');
	*pOut = '\0';

	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hexdump_line(char *out, size_t cap, const void *data, size_t n,
                      uint64_t addr)
    __attribute__((alias("gj_hexdump_line")));
