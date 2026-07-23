/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7371: readahead page-length id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ra_page_len_id_7371(void);
 *     - Return soft readahead page length (4096).
 *   uint32_t __gj_ra_page_len_id_7371  (alias)
 *   __libcgj_batch7371_marker = "libcgj-batch7371"
 *
 * Exclusive continuum CREATE-ONLY (7371-7380: readahead length stubs —
 * page_id, min_id, def_id, max_id, is_zero, len_ok, len_clamp,
 * len_align, len_errorish, batch_id_7380).
 * Unique gj_ra_page_len_id_7371 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7371_marker[] = "libcgj-batch7371";

/* Soft readahead page length (typical Linux PAGE_SIZE). */
#define B7371_RA_PAGE_LEN ((uint32_t)4096u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7371_ra_page_len_id(void)
{
	return B7371_RA_PAGE_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ra_page_len_id_7371 - soft readahead page length constant.
 *
 * Always returns 4096. Catalog id only; does not call readahead.
 * No parent wires.
 */
uint32_t
gj_ra_page_len_id_7371(void)
{
	(void)NULL;
	return b7371_ra_page_len_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ra_page_len_id_7371(void)
    __attribute__((alias("gj_ra_page_len_id_7371")));
