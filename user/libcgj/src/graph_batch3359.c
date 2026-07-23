/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3359: bare COFF NumberOfSections reader (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_coff_nscns_u(const void *buf, size_t len);
 *     - Read NumberOfSections (LE u16 at offset 2) from a bare COFF
 *       IMAGE_FILE_HEADER start. Requires len >= 4. Does not validate
 *       Machine or reject MZ images; pure field extract. NULL/short → 0.
 *   uint16_t __gj_coff_nscns_u  (alias)
 *   __libcgj_batch3359_marker = "libcgj-batch3359"
 *
 * CREATE-ONLY exclusive continuum wave (3351-3360). Distinct from
 * gj_coff_is_coff_u / gj_pe_sections_u — unique gj_coff_nscns_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3359_marker[] = "libcgj-batch3359";

#define B3359_NSCNS_OFF  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b3359_rd_u16_le(const unsigned char *p)
{
	return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_coff_nscns_u - bare COFF NumberOfSections (offset 2); else 0.
 *
 * buf: start of IMAGE_FILE_HEADER (or PE COFF header after signature).
 * len: available length (need >= 4). NULL/short → 0.
 *
 * Note: a true NumberOfSections of 0 is indistinguishable from the
 * failure return of 0.
 */
uint16_t
gj_coff_nscns_u(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	(void)NULL;
	if (pBuf == NULL || cb < (B3359_NSCNS_OFF + 2u)) {
		return 0u;
	}
	p = (const unsigned char *)pBuf;
	return b3359_rd_u16_le(p + B3359_NSCNS_OFF);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_coff_nscns_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_coff_nscns_u")));
