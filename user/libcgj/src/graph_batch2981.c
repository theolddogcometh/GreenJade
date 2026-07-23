/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2981: PE NT signature check via e_lfanew (_u).
 *
 * Surface (unique symbols):
 *   int gj_pe_is_pe_u(const void *buf, size_t len);
 *     - Return 1 if buffer is MZ and the PE signature at e_lfanew
 *       (LE u32 at offset 0x3C) is "PE\0\0"; else 0. Requires
 *       len >= e_lfanew + 4 and e_lfanew field reachable (len >= 0x40).
 *       NULL / short / non-PE → 0.
 *   int __gj_pe_is_pe_u  (alias)
 *   __libcgj_batch2981_marker = "libcgj-batch2981"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990). Distinct from
 * gj_pe_is_pe (batch1145) — unique gj_pe_is_pe_u surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2981_marker[] = "libcgj-batch2981";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2981_rd_u32_le(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

/* 1 if MZ + PE\0\0 at e_lfanew within cb. */
static int
b2981_is_pe(const unsigned char *p, size_t cb)
{
	uint32_t u32Lfa;
	const unsigned char *pSig;

	if (cb < 0x40u) {
		return 0;
	}
	if (p[0] != (unsigned char)'M' || p[1] != (unsigned char)'Z') {
		return 0;
	}
	u32Lfa = b2981_rd_u32_le(p + 0x3c);
	if ((uint64_t)u32Lfa + 4u > (uint64_t)cb) {
		return 0;
	}
	pSig = p + u32Lfa;
	if (pSig[0] != (unsigned char)'P' || pSig[1] != (unsigned char)'E' ||
	    pSig[2] != 0u || pSig[3] != 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_is_pe_u - 1 if MZ + PE\0\0 at e_lfanew.
 *
 * buf: image bytes; len: available length. NULL/short → 0.
 */
int
gj_pe_is_pe_u(const void *pBuf, size_t cb)
{
	(void)NULL;
	if (pBuf == NULL) {
		return 0;
	}
	return b2981_is_pe((const unsigned char *)pBuf, cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pe_is_pe_u(const void *pBuf, size_t cb)
    __attribute__((alias("gj_pe_is_pe_u")));
