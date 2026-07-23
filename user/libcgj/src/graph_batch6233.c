/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6233: HDR PQ nits → normalized linear (milli).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_pq_nits_to_norm_milli_6233(uint32_t nits);
 *     - Map absolute luminance (nits) to a milli-normalized linear value
 *       relative to the ST.2084 peak of 10000 nits:
 *         milli = (clamp(nits, 0..10000) * 1000) / 10000
 *       Result is in [0, 1000] (1000 = peak linear 1.0).
 *   uint32_t __gj_hdr_pq_nits_to_norm_milli_6233  (alias)
 *   __libcgj_batch6233_marker = "libcgj-batch6233"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs).
 * Unique gj_hdr_pq_nits_to_norm_milli_6233 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6233_marker[] = "libcgj-batch6233";

#define B6233_PQ_PEAK_NITS  10000u
#define B6233_MILLI         1000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6233_nits_to_milli(uint32_t u32Nits)
{
	uint32_t u32Clamped;

	if (u32Nits > B6233_PQ_PEAK_NITS) {
		u32Clamped = B6233_PQ_PEAK_NITS;
	} else {
		u32Clamped = u32Nits;
	}
	/* (nits * 1000) / 10000 — fits in uint32 for nits <= 10000. */
	return (u32Clamped * B6233_MILLI) / B6233_PQ_PEAK_NITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_pq_nits_to_norm_milli_6233 - nits → milli-normalized PQ linear.
 *
 * nits: absolute luminance in cd/m^2 (clamped to peak before scale).
 * Returns milli-linear [0, 1000]. Soft pure-integer pre-EOTF normalize;
 * not a full PQ OETF. No parent wires.
 */
uint32_t
gj_hdr_pq_nits_to_norm_milli_6233(uint32_t u32Nits)
{
	(void)NULL;
	return b6233_nits_to_milli(u32Nits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_pq_nits_to_norm_milli_6233(uint32_t u32Nits)
    __attribute__((alias("gj_hdr_pq_nits_to_norm_milli_6233")));
