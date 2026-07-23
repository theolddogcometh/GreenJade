/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1161: WAV RIFF/WAVE container magic check.
 *
 * Surface (unique symbols):
 *   int gj_audio_is_wav(const void *buf, size_t len);
 *     — Return 1 (OK) if buf has RIFF at 0 and WAVE at 8 and
 *       len >= 12; else 0 (FAIL). NULL / short → 0.
 *   int __gj_audio_is_wav  (alias)
 *   __libcgj_batch1161_marker = "libcgj-batch1161"
 *
 * Distinct from gj_wav_is_wav (batch112) / gj_riff_is_magic (batch1153)
 * — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1161_marker[] = "libcgj-batch1161";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_is_wav — 1 if RIFF....WAVE outer header.
 */
int
gj_audio_is_wav(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 12u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'R' || p[1] != (unsigned char)'I' ||
	    p[2] != (unsigned char)'F' || p[3] != (unsigned char)'F') {
		return 0;
	}
	if (p[8] != (unsigned char)'W' || p[9] != (unsigned char)'A' ||
	    p[10] != (unsigned char)'V' || p[11] != (unsigned char)'E') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_audio_is_wav(const void *pBuf, size_t cb)
    __attribute__((alias("gj_audio_is_wav")));
