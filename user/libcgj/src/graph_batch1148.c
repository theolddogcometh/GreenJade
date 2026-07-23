/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1148: gzip member magic check.
 *
 * Surface (unique symbols):
 *   int gj_gzip_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with gzip ID1/ID2 0x1F 0x8B
 *       and len >= 2; else 0 (FAIL). NULL / short → 0.
 *   int __gj_gzip_is_magic  (alias)
 *   __libcgj_batch1148_marker = "libcgj-batch1148"
 *
 * Distinct from gzip inflate helpers (batch76) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1148_marker[] = "libcgj-batch1148";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gzip_is_magic — 1 if ID1/ID2 are 1f 8b.
 */
int
gj_gzip_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 2u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0x1fu || p[1] != 0x8bu) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_gzip_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_gzip_is_magic")));
