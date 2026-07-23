/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1727: PE COFF Characteristics EXECUTABLE_IMAGE.
 *
 * Surface (unique symbols):
 *   int gj_pe_is_exe_char(const void *buf, size_t len);
 *     — Return 1 if buffer is MZ + PE\0\0 at e_lfanew and the COFF
 *       Characteristics word (LE u16 at e_lfanew+22) has bit
 *       IMAGE_FILE_EXECUTABLE_IMAGE (0x0002) set. Else 0. Requires
 *       len >= e_lfanew + 24. NULL / short / non-PE → 0.
 *   int __gj_pe_is_exe_char  (alias)
 *   __libcgj_batch1727_marker = "libcgj-batch1727"
 *
 * Beyond-magic exclusive: Characteristics bit, not MZ/PE alone.
 * Distinct from gj_pe_is_mz / gj_pe_is_pe / pe32_parse / is_dll_char.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1727_marker[] = "libcgj-batch1727";

#define B1727_IMAGE_FILE_EXECUTABLE_IMAGE 0x0002u
#define B1727_CHARS_OFF                   22u
#define B1727_COFF_MIN                    24u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1727_rd_u32_le(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

static uint16_t
b1727_rd_u16_le(const unsigned char *p)
{
	return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static int
b1727_pe_chars(const void *pBuf, size_t cb, uint16_t *pU16Chars)
{
	const unsigned char *p;
	uint32_t u32Lfa;
	const unsigned char *pNt;

	if (pBuf == NULL || cb < 0x40u || pU16Chars == NULL) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'M' || p[1] != (unsigned char)'Z') {
		return 0;
	}
	u32Lfa = b1727_rd_u32_le(p + 0x3c);
	if ((uint64_t)u32Lfa + (uint64_t)B1727_COFF_MIN > (uint64_t)cb) {
		return 0;
	}
	pNt = p + u32Lfa;
	if (pNt[0] != (unsigned char)'P' || pNt[1] != (unsigned char)'E' ||
	    pNt[2] != 0u || pNt[3] != 0u) {
		return 0;
	}
	*pU16Chars = b1727_rd_u16_le(pNt + B1727_CHARS_OFF);
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_is_exe_char — 1 if PE Characteristics has EXECUTABLE_IMAGE.
 */
int
gj_pe_is_exe_char(const void *pBuf, size_t cb)
{
	uint16_t u16Chars;

	if (!b1727_pe_chars(pBuf, cb, &u16Chars)) {
		return 0;
	}
	if ((u16Chars & B1727_IMAGE_FILE_EXECUTABLE_IMAGE) != 0u) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pe_is_exe_char(const void *pBuf, size_t cb)
    __attribute__((alias("gj_pe_is_exe_char")));
