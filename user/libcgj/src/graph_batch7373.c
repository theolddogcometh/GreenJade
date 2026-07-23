/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7373: readahead default-length id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ra_def_len_id_7373(void);
 *     - Return soft readahead default window (131072 / 128 KiB).
 *   uint32_t __gj_ra_def_len_id_7373  (alias)
 *   __libcgj_batch7373_marker = "libcgj-batch7373"
 *
 * Exclusive continuum CREATE-ONLY (7371-7380: readahead length stubs —
 * page_id, min_id, def_id, max_id, is_zero, len_ok, len_clamp,
 * len_align, len_errorish, batch_id_7380).
 * Unique gj_ra_def_len_id_7373 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7373_marker[] = "libcgj-batch7373";

/* Soft default readahead window: 128 KiB. */
#define B7373_RA_DEF_LEN ((uint32_t)131072u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7373_ra_def_len_id(void)
{
	return B7373_RA_DEF_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ra_def_len_id_7373 - soft readahead default length constant.
 *
 * Always returns 131072 (128 KiB). Catalog id only; does not call
 * readahead. No parent wires.
 */
uint32_t
gj_ra_def_len_id_7373(void)
{
	(void)NULL;
	return b7373_ra_def_len_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ra_def_len_id_7373(void)
    __attribute__((alias("gj_ra_def_len_id_7373")));
