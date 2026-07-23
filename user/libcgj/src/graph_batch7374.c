/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7374: readahead max-length id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ra_max_len_id_7374(void);
 *     - Return soft readahead max count (16777216 / 16 MiB).
 *   uint32_t __gj_ra_max_len_id_7374  (alias)
 *   __libcgj_batch7374_marker = "libcgj-batch7374"
 *
 * Exclusive continuum CREATE-ONLY (7371-7380: readahead length stubs —
 * page_id, min_id, def_id, max_id, is_zero, len_ok, len_clamp,
 * len_align, len_errorish, batch_id_7380).
 * Unique gj_ra_max_len_id_7374 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7374_marker[] = "libcgj-batch7374";

/* Soft catalog max readahead count: 16 MiB. */
#define B7374_RA_MAX_LEN ((uint32_t)16777216u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7374_ra_max_len_id(void)
{
	return B7374_RA_MAX_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ra_max_len_id_7374 - soft readahead maximum length constant.
 *
 * Always returns 16777216 (16 MiB). Catalog cap only; does not call
 * readahead. No parent wires.
 */
uint32_t
gj_ra_max_len_id_7374(void)
{
	(void)NULL;
	return b7374_ra_max_len_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ra_max_len_id_7374(void)
    __attribute__((alias("gj_ra_max_len_id_7374")));
