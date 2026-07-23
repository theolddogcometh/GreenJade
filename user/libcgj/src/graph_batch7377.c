/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7377: readahead length clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ra_len_clamp_u_7377(uint32_t count);
 *     - Clamp count to [0, 16777216].
 *   uint32_t __gj_ra_len_clamp_u_7377  (alias)
 *   __libcgj_batch7377_marker = "libcgj-batch7377"
 *
 * Exclusive continuum CREATE-ONLY (7371-7380: readahead length stubs —
 * page_id, min_id, def_id, max_id, is_zero, len_ok, len_clamp,
 * len_align, len_errorish, batch_id_7380).
 * Unique gj_ra_len_clamp_u_7377 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7377_marker[] = "libcgj-batch7377";

/* Soft catalog max readahead count: 16 MiB. */
#define B7377_RA_MAX_LEN ((uint32_t)16777216u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7377_ra_len_clamp(uint32_t u32Count)
{
	if (u32Count > B7377_RA_MAX_LEN)
		return B7377_RA_MAX_LEN;
	return u32Count;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ra_len_clamp_u_7377 - clamp readahead count to soft max.
 *
 * count: readahead(2) count argument (soft)
 *
 * Returns count if <= 16777216, else 16777216. Soft clamp only; does
 * not call readahead. No parent wires.
 */
uint32_t
gj_ra_len_clamp_u_7377(uint32_t u32Count)
{
	(void)NULL;
	return b7377_ra_len_clamp(u32Count);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ra_len_clamp_u_7377(uint32_t u32Count)
    __attribute__((alias("gj_ra_len_clamp_u_7377")));
