/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1959: AppImage magic soft hint (first bytes AI).
 *
 * Surface (unique symbols):
 *   int gj_appimage_magic_hint(const void *buf, size_t len);
 *     — Soft magic hint: return 1 if buf is non-NULL, len >= 2, and the
 *       first two octets are ASCII 'A' then 'I' (AppImage product-path
 *       marker probe). Does not parse ELF headers, type-1/type-2
 *       offsets, or squashfs payloads.
 *   int __gj_appimage_magic_hint  (alias)
 *   __libcgj_batch1959_marker = "libcgj-batch1959"
 *
 * Package/install exclusive pure-data wave (1951–1960). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1959_marker[] = "libcgj-batch1959";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if first two bytes are 'A' 'I'. */
static int
b1959_magic_ai(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 2u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'A') {
		return 0;
	}
	if (p[1] != (unsigned char)'I') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_appimage_magic_hint — 1 if buf begins with ASCII "AI".
 *
 * buf: candidate file head (NULL → 0)
 * len: available byte count (must be >= 2)
 * Soft product-path probe only; not a full AppImage validator.
 */
int
gj_appimage_magic_hint(const void *pBuf, size_t cb)
{
	(void)NULL;
	return b1959_magic_ai(pBuf, cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_appimage_magic_hint(const void *pBuf, size_t cb)
    __attribute__((alias("gj_appimage_magic_hint")));
