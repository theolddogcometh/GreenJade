/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7379: readahead length errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ra_len_errorish_u_7379(uint32_t count);
 *     - Return 1 if count is 0 or greater than 16777216, else 0.
 *   uint32_t __gj_ra_len_errorish_u_7379  (alias)
 *   __libcgj_batch7379_marker = "libcgj-batch7379"
 *
 * Exclusive continuum CREATE-ONLY (7371-7380: readahead length stubs —
 * page_id, min_id, def_id, max_id, is_zero, len_ok, len_clamp,
 * len_align, len_errorish, batch_id_7380).
 * Unique gj_ra_len_errorish_u_7379 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7379_marker[] = "libcgj-batch7379";

/* Soft catalog max readahead count: 16 MiB. */
#define B7379_RA_MAX_LEN ((uint32_t)16777216u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7379_ra_len_errorish(uint32_t u32Count)
{
	return (u32Count == 0u || u32Count > B7379_RA_MAX_LEN) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ra_len_errorish_u_7379 - 1 if readahead count is zero or over max.
 *
 * count: readahead(2) count argument (soft)
 *
 * Returns 1 when count is 0 or > 16777216, else 0. Soft inverse of
 * len_ok; does not call readahead. No parent wires.
 */
uint32_t
gj_ra_len_errorish_u_7379(uint32_t u32Count)
{
	(void)NULL;
	return b7379_ra_len_errorish(u32Count);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ra_len_errorish_u_7379(uint32_t u32Count)
    __attribute__((alias("gj_ra_len_errorish_u_7379")));
