/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1332: modal (most frequent) octet of a byte buffer.
 *
 * Surface (unique symbols):
 *   int gj_u8_mode(const uint8_t *data, size_t n, uint8_t *out_mode);
 *     — Write the most frequent byte value into *out_mode. Ties break
 *       toward the lowest byte value. Returns 1 (OK) on success;
 *       0 (FAIL) when data is NULL, n == 0, or out_mode is NULL.
 *   int __gj_u8_mode  (alias)
 *   __libcgj_batch1332_marker = "libcgj-batch1332"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1332_marker[] = "libcgj-batch1332";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_mode — most frequent octet (lowest value on ties).
 */
int
gj_u8_mode(const uint8_t *pData, size_t cN, uint8_t *pOutMode)
{
	uint32_t aHist[256];
	size_t i;
	uint32_t uBest;
	unsigned uMode;

	if (pData == NULL || cN == 0u || pOutMode == NULL) {
		return 0;
	}

	for (i = 0u; i < 256u; i++) {
		aHist[i] = 0u;
	}
	for (i = 0u; i < cN; i++) {
		aHist[pData[i]]++;
	}

	uBest = 0u;
	uMode = 0u;
	for (i = 0u; i < 256u; i++) {
		if (aHist[i] > uBest) {
			uBest = aHist[i];
			uMode = (unsigned)i;
		}
	}

	*pOutMode = (uint8_t)uMode;
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u8_mode(const uint8_t *pData, size_t cN, uint8_t *pOutMode)
    __attribute__((alias("gj_u8_mode")));
