/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3358: PE TimeDateStamp reader (_u exclusive).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_timestamp_u(const void *buf, size_t len);
 *     - If buf is MZ + PE\0\0 at e_lfanew and len covers
 *       TimeDateStamp (LE u32 at e_lfanew+8), return that field.
 *       Else 0. Requires len >= 0x40 and len >= e_lfanew + 12.
 *       NULL / short / non-PE → 0.
 *   uint32_t __gj_pe_timestamp_u  (alias)
 *   __libcgj_batch3358_marker = "libcgj-batch3358"
 *
 * CREATE-ONLY exclusive continuum wave (3351-3360). Distinct from
 * gj_pe_is_pe / gj_pe_machine_u / gj_pe_sections_u — unique
 * gj_pe_timestamp_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3358_marker[] = "libcgj-batch3358";

#define B3358_TIMESTAMP_OFF  8u
#define B3358_COFF_MIN       12u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3358_rd_u32_le(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_timestamp_u - PE TimeDateStamp when MZ + PE valid; else 0.
 *
 * buf: image bytes; len: available length. NULL/short/non-PE → 0.
 * Note: a true TimeDateStamp of 0 is indistinguishable from the
 * failure return of 0.
 */
uint32_t
gj_pe_timestamp_u(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	uint32_t u32Lfa;
	const unsigned char *pNt;

	(void)NULL;
	if (pBuf == NULL || cb < 0x40u) {
		return 0u;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'M' || p[1] != (unsigned char)'Z') {
		return 0u;
	}
	u32Lfa = b3358_rd_u32_le(p + 0x3c);
	if ((uint64_t)u32Lfa + (uint64_t)B3358_COFF_MIN > (uint64_t)cb) {
		return 0u;
	}
	pNt = p + u32Lfa;
	if (pNt[0] != (unsigned char)'P' || pNt[1] != (unsigned char)'E' ||
	    pNt[2] != 0u || pNt[3] != 0u) {
		return 0u;
	}
	return b3358_rd_u32_le(pNt + B3358_TIMESTAMP_OFF);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_timestamp_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_pe_timestamp_u")));
