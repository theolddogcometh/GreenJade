/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1331: byte histogram into a fixed 256-bin table.
 *
 * Surface (unique symbols):
 *   void gj_u8_histogram256(const uint8_t *data, size_t n, uint32_t hist[256]);
 *     — Zero hist[0..255], then count each data[i] into hist[data[i]].
 *       NULL hist → no-op. NULL data or n == 0 → hist zeroed.
 *   void __gj_u8_histogram256  (alias)
 *
 * Distinct from gj_hist_u8 (batch306) — unique name; avoid multi-def.
 *   __libcgj_batch1331_marker = "libcgj-batch1331"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1331_marker[] = "libcgj-batch1331";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_histogram256 — frequency count of each octet value into hist[256].
 */
void
gj_u8_histogram256(const uint8_t *pData, size_t cN, uint32_t aHist[256])
{
	size_t i;

	if (aHist == NULL) {
		return;
	}

	for (i = 0u; i < 256u; i++) {
		aHist[i] = 0u;
	}

	if (pData == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		aHist[pData[i]]++;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u8_histogram256(const uint8_t *pData, size_t cN, uint32_t aHist[256])
    __attribute__((alias("gj_u8_histogram256")));
