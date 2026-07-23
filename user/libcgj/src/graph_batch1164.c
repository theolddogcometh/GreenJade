/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1164: ISO BMFF/MP4 ftyp brand magic check.
 *
 * Surface (unique symbols):
 *   int gj_video_is_mp4_ftyp(const void *buf, size_t len);
 *     — Return 1 (OK) if len >= 8 and octets [4..7] are "ftyp"
 *       (ISO base media file format major brand box); else 0.
 *       Does not validate the size field at [0..3].
 *   int __gj_video_is_mp4_ftyp  (alias)
 *   __libcgj_batch1164_marker = "libcgj-batch1164"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1164_marker[] = "libcgj-batch1164";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_video_is_mp4_ftyp — 1 if brand type at offset 4 is "ftyp".
 */
int
gj_video_is_mp4_ftyp(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 8u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[4] != (unsigned char)'f' || p[5] != (unsigned char)'t' ||
	    p[6] != (unsigned char)'y' || p[7] != (unsigned char)'p') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_video_is_mp4_ftyp(const void *pBuf, size_t cb)
    __attribute__((alias("gj_video_is_mp4_ftyp")));
