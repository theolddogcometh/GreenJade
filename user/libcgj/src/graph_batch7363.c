/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7363: fallocate FALLOC_FL_COLLAPSE_RANGE mode id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_falloc_collapse_range_id_7363(void);
 *     - Return soft FALLOC_FL_COLLAPSE_RANGE constant (0x08).
 *   uint32_t __gj_falloc_collapse_range_id_7363  (alias)
 *   __libcgj_batch7363_marker = "libcgj-batch7363"
 *
 * Exclusive continuum CREATE-ONLY (7361-7370: fallocate mode stubs —
 * keep_size_id, punch_hole_id, collapse_range_id, zero_range_id,
 * has_keep_size, has_punch_hole, modes_ok, modes_pack, modes_errorish,
 * batch_id_7370).
 * Unique gj_falloc_collapse_range_id_7363 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7363_marker[] = "libcgj-batch7363";

/* FALLOC_FL_COLLAPSE_RANGE: remove range, shift following data (0x08). */
#define B7363_FALLOC_FL_COLLAPSE_RANGE ((uint32_t)0x08u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7363_falloc_collapse_range_id(void)
{
	return B7363_FALLOC_FL_COLLAPSE_RANGE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_falloc_collapse_range_id_7363 - soft FALLOC_FL_COLLAPSE_RANGE id.
 *
 * Always returns 0x08 (Linux FALLOC_FL_COLLAPSE_RANGE). Catalog id only;
 * does not call fallocate. No parent wires.
 */
uint32_t
gj_falloc_collapse_range_id_7363(void)
{
	(void)NULL;
	return b7363_falloc_collapse_range_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_falloc_collapse_range_id_7363(void)
    __attribute__((alias("gj_falloc_collapse_range_id_7363")));
