/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2561: RLE first-run length (unsigned byte stream).
 *
 * Surface (unique symbols):
 *   size_t gj_rle_run_len_u(const uint8_t *in, size_t nin);
 *     — Length of the maximal contiguous run of equal bytes starting at
 *       in[0] within in[0..nin). Returns 0 if in is NULL or nin is 0.
 *       Uncapped (full run); encode helpers chunk at 255 separately.
 *   size_t __gj_rle_run_len_u  (alias)
 *   __libcgj_batch2561_marker = "libcgj-batch2561"
 *
 * Compression exclusive wave (2561–2570). Distinct from gj_rle_compress
 * (batch288) / gj_rle_encode_simple (batch1502) — unique symbols only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2561_marker[] = "libcgj-batch2561";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count equal bytes starting at pIn[0] within pIn[0..cbIn). */
static size_t
b2561_run(const uint8_t *pIn, size_t cbIn)
{
	uint8_t u8Val;
	size_t nRun;

	if (pIn == NULL || cbIn == 0u) {
		return 0u;
	}

	u8Val = pIn[0];
	nRun = 1u;
	while (nRun < cbIn && pIn[nRun] == u8Val) {
		nRun++;
	}
	return nRun;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rle_run_len_u — length of the first equal-byte run in in[0..nin).
 *
 * in:  input buffer (may be NULL only when nin == 0)
 * nin: number of input bytes
 * Returns run length in 0..nin (0 when empty or NULL).
 */
size_t
gj_rle_run_len_u(const uint8_t *in, size_t nin)
{
	(void)NULL;
	return b2561_run(in, nin);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_rle_run_len_u(const uint8_t *in, size_t nin)
    __attribute__((alias("gj_rle_run_len_u")));
