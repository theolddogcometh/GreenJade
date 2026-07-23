/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1153: RIFF container magic check.
 *
 * Surface (unique symbols):
 *   int gj_riff_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with "RIFF" and len >= 4;
 *       else 0 (FAIL). Does not require a form type (WAVE/AVI/…).
 *   int __gj_riff_is_magic  (alias)
 *   __libcgj_batch1153_marker = "libcgj-batch1153"
 *
 * Distinct from gj_wav_is_wav (batch112) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1153_marker[] = "libcgj-batch1153";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_riff_is_magic — 1 if first four octets are "RIFF".
 */
int
gj_riff_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'R' || p[1] != (unsigned char)'I' ||
	    p[2] != (unsigned char)'F' || p[3] != (unsigned char)'F') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_riff_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_riff_is_magic")));
