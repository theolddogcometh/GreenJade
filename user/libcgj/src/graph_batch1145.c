/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1145: PE NT signature check via e_lfanew.
 *
 * Surface (unique symbols):
 *   int gj_pe_is_pe(const void *buf, size_t len);
 *     — Return 1 (OK) if buffer is MZ and the PE signature at
 *       e_lfanew (LE u32 at offset 0x3C) is "PE\0\0"; else 0 (FAIL).
 *       Requires len >= e_lfanew + 4 and e_lfanew reachable from DOS
 *       header (len >= 0x40 for the e_lfanew field). NULL / short → 0.
 *   int __gj_pe_is_pe  (alias)
 *   __libcgj_batch1145_marker = "libcgj-batch1145"
 *
 * Distinct from gj_pe_is_mz / pe32_parse — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1145_marker[] = "libcgj-batch1145";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_is_pe — 1 if MZ + PE\0\0 at e_lfanew.
 */
int
gj_pe_is_pe(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	uint32_t u32Lfa;
	const unsigned char *pSig;

	if (pBuf == NULL || cb < 0x40u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'M' || p[1] != (unsigned char)'Z') {
		return 0;
	}
	u32Lfa = (uint32_t)p[0x3c] |
	         ((uint32_t)p[0x3d] << 8) |
	         ((uint32_t)p[0x3e] << 16) |
	         ((uint32_t)p[0x3f] << 24);
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_pe_is_pe(const void *pBuf, size_t cb)
    __attribute__((alias("gj_pe_is_pe")));
