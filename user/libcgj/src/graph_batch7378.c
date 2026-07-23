/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7378: readahead length page-align stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ra_len_align_u_7378(uint32_t count);
 *     - Round count up to a multiple of 4096 (0 stays 0); soft clamp
 *       after align to 16777216.
 *   uint32_t __gj_ra_len_align_u_7378  (alias)
 *   __libcgj_batch7378_marker = "libcgj-batch7378"
 *
 * Exclusive continuum CREATE-ONLY (7371-7380: readahead length stubs —
 * page_id, min_id, def_id, max_id, is_zero, len_ok, len_clamp,
 * len_align, len_errorish, batch_id_7380).
 * Unique gj_ra_len_align_u_7378 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7378_marker[] = "libcgj-batch7378";

#define B7378_RA_PAGE_LEN ((uint32_t)4096u)
#define B7378_RA_MAX_LEN  ((uint32_t)16777216u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7378_ra_len_align(uint32_t u32Count)
{
	uint32_t u32Rem;
	uint32_t u32Aligned;

	if (u32Count == 0u)
		return 0u;
	if (u32Count >= B7378_RA_MAX_LEN)
		return B7378_RA_MAX_LEN;

	u32Rem = u32Count % B7378_RA_PAGE_LEN;
	if (u32Rem == 0u)
		return u32Count;

	/* Round up; u32Count < MAX so add cannot wrap past UINT32_MAX. */
	u32Aligned = u32Count + (B7378_RA_PAGE_LEN - u32Rem);
	if (u32Aligned > B7378_RA_MAX_LEN || u32Aligned < u32Count)
		return B7378_RA_MAX_LEN;
	return u32Aligned;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ra_len_align_u_7378 - page-align readahead count upward.
 *
 * count: readahead(2) count argument (soft)
 *
 * Returns 0 if count is 0; otherwise rounds up to a multiple of 4096
 * and clamps to 16777216. Soft align only; does not call readahead.
 * No parent wires.
 */
uint32_t
gj_ra_len_align_u_7378(uint32_t u32Count)
{
	(void)NULL;
	return b7378_ra_len_align(u32Count);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ra_len_align_u_7378(uint32_t u32Count)
    __attribute__((alias("gj_ra_len_align_u_7378")));
