/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6234: HDR PQ normalized linear → nits.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_pq_norm_milli_to_nits_6234(uint32_t norm_milli);
 *     - Map milli-normalized linear (0..1000 → 0..1) back to absolute
 *       luminance against the ST.2084 peak of 10000 nits:
 *         nits = (clamp(norm_milli, 0..1000) * 10000) / 1000
 *   uint32_t __gj_hdr_pq_norm_milli_to_nits_6234  (alias)
 *   __libcgj_batch6234_marker = "libcgj-batch6234"
 *
 * Exclusive continuum CREATE-ONLY (6231-6240: HDR PQ/HLG transfer stubs).
 * Unique gj_hdr_pq_norm_milli_to_nits_6234 surface only; no multi-def.
 * Inverse companion of gj_hdr_pq_nits_to_norm_milli_6233. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6234_marker[] = "libcgj-batch6234";

#define B6234_PQ_PEAK_NITS  10000u
#define B6234_MILLI         1000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6234_milli_to_nits(uint32_t u32NormMilli)
{
	uint32_t u32Clamped;

	if (u32NormMilli > B6234_MILLI) {
		u32Clamped = B6234_MILLI;
	} else {
		u32Clamped = u32NormMilli;
	}
	return (u32Clamped * B6234_PQ_PEAK_NITS) / B6234_MILLI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_pq_norm_milli_to_nits_6234 - milli-linear → absolute PQ nits.
 *
 * norm_milli: milli-normalized linear [0, 1000] (values above clamped).
 * Returns absolute nits in [0, 10000]. Soft pure-integer post-EOTF scale;
 * not a full PQ EOTF. No parent wires.
 */
uint32_t
gj_hdr_pq_norm_milli_to_nits_6234(uint32_t u32NormMilli)
{
	(void)NULL;
	return b6234_milli_to_nits(u32NormMilli);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_pq_norm_milli_to_nits_6234(uint32_t u32NormMilli)
    __attribute__((alias("gj_hdr_pq_norm_milli_to_nits_6234")));
