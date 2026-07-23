/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1163: Matroska/WebM EBML header magic check.
 *
 * Surface (unique symbols):
 *   int gj_video_is_mkv(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with EBML header ID
 *       0x1A 0x45 0xDF 0xA3 and len >= 4; else 0 (FAIL).
 *   int __gj_video_is_mkv  (alias)
 *   __libcgj_batch1163_marker = "libcgj-batch1163"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1163_marker[] = "libcgj-batch1163";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_video_is_mkv — 1 if EBML ID 1A 45 DF A3 (Matroska/WebM).
 */
int
gj_video_is_mkv(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0x1au || p[1] != 0x45u || p[2] != 0xdfu ||
	    p[3] != 0xa3u) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_video_is_mkv(const void *pBuf, size_t cb)
    __attribute__((alias("gj_video_is_mkv")));
