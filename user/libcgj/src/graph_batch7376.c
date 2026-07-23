/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7376: readahead length validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ra_len_ok_u_7376(uint32_t count);
 *     - Return 1 if count is in (0, 16777216], else 0.
 *   uint32_t __gj_ra_len_ok_u_7376  (alias)
 *   __libcgj_batch7376_marker = "libcgj-batch7376"
 *
 * Exclusive continuum CREATE-ONLY (7371-7380: readahead length stubs —
 * page_id, min_id, def_id, max_id, is_zero, len_ok, len_clamp,
 * len_align, len_errorish, batch_id_7380).
 * Unique gj_ra_len_ok_u_7376 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7376_marker[] = "libcgj-batch7376";

/* Soft catalog max readahead count: 16 MiB. */
#define B7376_RA_MAX_LEN ((uint32_t)16777216u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7376_ra_len_ok(uint32_t u32Count)
{
	return (u32Count > 0u && u32Count <= B7376_RA_MAX_LEN) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ra_len_ok_u_7376 - 1 if readahead count is a useful soft length.
 *
 * count: readahead(2) count argument (soft)
 *
 * Returns 1 when count is in (0, 16777216], else 0. Soft catalog
 * check; does not call readahead. No parent wires.
 */
uint32_t
gj_ra_len_ok_u_7376(uint32_t u32Count)
{
	(void)NULL;
	return b7376_ra_len_ok(u32Count);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ra_len_ok_u_7376(uint32_t u32Count)
    __attribute__((alias("gj_ra_len_ok_u_7376")));
