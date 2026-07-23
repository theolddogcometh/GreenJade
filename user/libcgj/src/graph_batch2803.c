/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2803: djb2 classic hash (exclusive _u surface).
 *
 * Surface (unique symbols only — do not export djb2_hash / gj_djb2):
 *   uint32_t gj_djb2_u(const void *data, size_t n);
 *     - Dan Bernstein djb2 over n bytes at data. Seed 5381; step is
 *       hash * 33 + c. NULL data with n != 0 -> 0. Empty (n == 0) -> 5381.
 *   uint32_t __gj_djb2_u  (alias)
 *   __libcgj_batch2803_marker = "libcgj-batch2803"
 *
 * Distinct from batch42 djb2_hash, batch422 gj_djb2, batch620 gj_djb2_n,
 * batch1703 gj_djb2_64_update — no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2803_marker[] = "libcgj-batch2803";

/* djb2 initial seed (classic public algorithm). */
#define B2803_DJB2_SEED 5381u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2803_djb2(const unsigned char *pData, size_t cbN)
{
	uint32_t u32H;
	size_t i;

	u32H = B2803_DJB2_SEED;
	for (i = 0u; i < cbN; i++) {
		/* hash * 33 + c */
		u32H = ((u32H << 5) + u32H) + (uint32_t)pData[i];
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_djb2_u - classic djb2 hash of n bytes at data.
 *
 * data: byte buffer (may be NULL only when n == 0)
 * n:    length in bytes
 *
 * Returns the 32-bit digest. On NULL data with n != 0, returns 0.
 * Empty input (n == 0) returns the seed 5381.
 */
uint32_t
gj_djb2_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return B2803_DJB2_SEED;
	}
	return b2803_djb2(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_djb2_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_djb2_u")));
