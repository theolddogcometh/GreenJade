/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6232: HDR PQ absolute luminance (nits) clamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_pq_nits_clamp_6232(uint32_t nits);
 *     - Clamp absolute display / scene luminance into the classic
 *       ST.2084 peak window [0, 10000] cd/m^2 (nits). Above peak → 10000.
 *   uint32_t __gj_hdr_pq_nits_clamp_6232  (alias)
 *   __libcgj_batch6232_marker = "libcgj-batch6232"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs).
 * Unique gj_hdr_pq_nits_clamp_6232 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6232_marker[] = "libcgj-batch6232";

/* ST.2084 absolute peak luminance (cd/m^2). */
#define B6232_PQ_PEAK_NITS  10000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6232_clamp(uint32_t u32Nits)
{
	if (u32Nits > B6232_PQ_PEAK_NITS) {
		return B6232_PQ_PEAK_NITS;
	}
	return u32Nits;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_pq_nits_clamp_6232 - clamp luminance into the PQ 0..10000 nits band.
 *
 * nits: absolute luminance in cd/m^2.
 * Returns clamped nits. Soft pure-integer HDR transfer bound; does not
 * evaluate the full PQ EOTF curve. No parent wires.
 */
uint32_t
gj_hdr_pq_nits_clamp_6232(uint32_t u32Nits)
{
	(void)NULL;
	return b6232_clamp(u32Nits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_pq_nits_clamp_6232(uint32_t u32Nits)
    __attribute__((alias("gj_hdr_pq_nits_clamp_6232")));
