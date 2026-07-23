/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6239: HDR PQ/HLG peak luminance ratio (milli).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_pq_hlg_peak_ratio_milli_6239(uint32_t pq_peak_nits,
 *                                                uint32_t hlg_lw_nits);
 *     - Soft milli-ratio of a PQ peak (nits) to an HLG Lw (nits):
 *         milli = (pq_peak_nits * 1000) / hlg_lw_nits
 *       hlg_lw_nits == 0 → 0. Overflow-safe when product would exceed
 *       UINT32_MAX. Used as a crude cross-transfer tone-map scale hint.
 *   uint32_t __gj_hdr_pq_hlg_peak_ratio_milli_6239  (alias)
 *   __libcgj_batch6239_marker = "libcgj-batch6239"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs).
 * Unique gj_hdr_pq_hlg_peak_ratio_milli_6239 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6239_marker[] = "libcgj-batch6239";

#define B6239_MILLI  1000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6239_ratio_milli(uint32_t u32PqPeak, uint32_t u32HlgLw)
{
	if (u32HlgLw == 0u || u32PqPeak == 0u) {
		return 0u;
	}
	if (u32PqPeak > UINT32_MAX / B6239_MILLI) {
		return (u32PqPeak / u32HlgLw) * B6239_MILLI +
		    ((u32PqPeak % u32HlgLw) * B6239_MILLI) / u32HlgLw;
	}
	return (u32PqPeak * B6239_MILLI) / u32HlgLw;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_pq_hlg_peak_ratio_milli_6239 - milli-ratio of PQ peak to HLG Lw.
 *
 * pq_peak_nits: PQ-side absolute peak (often 10000).
 * hlg_lw_nits:  HLG display peak white Lw.
 * Returns milli-ratio (1000 = equal peaks). Soft pure-integer cross-
 * transfer scale hint. No parent wires.
 */
uint32_t
gj_hdr_pq_hlg_peak_ratio_milli_6239(uint32_t u32PqPeakNits,
    uint32_t u32HlgLwNits)
{
	(void)NULL;
	return b6239_ratio_milli(u32PqPeakNits, u32HlgLwNits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_pq_hlg_peak_ratio_milli_6239(uint32_t u32PqPeakNits,
    uint32_t u32HlgLwNits)
    __attribute__((alias("gj_hdr_pq_hlg_peak_ratio_milli_6239")));
