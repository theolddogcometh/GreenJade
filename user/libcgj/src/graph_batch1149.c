/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1149: PNG file signature magic check.
 *
 * Surface (unique symbols):
 *   int gj_png_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with the 8-byte PNG signature
 *       89 50 4E 47 0D 0A 1A 0A and len >= 8; else 0 (FAIL).
 *   int __gj_png_is_magic  (alias)
 *   __libcgj_batch1149_marker = "libcgj-batch1149"
 *
 * Distinct from gj_png_is_png (batch111) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1149_marker[] = "libcgj-batch1149";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_png_is_magic — 1 if 8-byte PNG signature matches.
 */
int
gj_png_is_magic(const void *pBuf, size_t cb)
{
	static const unsigned char aSig[8] = {
	    0x89u, 0x50u, 0x4eu, 0x47u, 0x0du, 0x0au, 0x1au, 0x0au
	};
	const unsigned char *p;
	unsigned i;

	if (pBuf == NULL || cb < 8u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	for (i = 0u; i < 8u; i++) {
		if (p[i] != aSig[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_png_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_png_is_magic")));
