/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1144: PE/DOS MZ magic check.
 *
 * Surface (unique symbols):
 *   int gj_pe_is_mz(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with DOS "MZ" (0x4D 0x5A) and
 *       len >= 2; else 0 (FAIL). NULL / short → 0.
 *   int __gj_pe_is_mz  (alias)
 *   __libcgj_batch1144_marker = "libcgj-batch1144"
 *
 * Distinct from pe32_parse (kernel) / gj_pe_is_pe — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1144_marker[] = "libcgj-batch1144";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_is_mz — 1 if IMAGE_DOS_HEADER.e_magic is "MZ".
 */
int
gj_pe_is_mz(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 2u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'M' || p[1] != (unsigned char)'Z') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pe_is_mz(const void *pBuf, size_t cb)
    __attribute__((alias("gj_pe_is_mz")));
