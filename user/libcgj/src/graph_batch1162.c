/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1162: MPEG audio frame-sync bit check.
 *
 * Surface (unique symbols):
 *   int gj_audio_is_mp3_frame_sync(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with an MPEG audio frame sync:
 *       first octet 0xFF and second octet has top 3 bits set
 *       ((b1 & 0xE0) == 0xE0), with len >= 2; else 0 (FAIL).
 *   int __gj_audio_is_mp3_frame_sync  (alias)
 *   __libcgj_batch1162_marker = "libcgj-batch1162"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1162_marker[] = "libcgj-batch1162";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_is_mp3_frame_sync — 1 if 11-bit MPEG frame sync present.
 */
int
gj_audio_is_mp3_frame_sync(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 2u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0xffu) {
		return 0;
	}
	if ((p[1] & 0xe0u) != 0xe0u) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_audio_is_mp3_frame_sync(const void *pBuf, size_t cb)
    __attribute__((alias("gj_audio_is_mp3_frame_sync")));
