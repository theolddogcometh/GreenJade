/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7362: fallocate FALLOC_FL_PUNCH_HOLE mode id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_falloc_punch_hole_id_7362(void);
 *     - Return soft FALLOC_FL_PUNCH_HOLE constant (0x02).
 *   uint32_t __gj_falloc_punch_hole_id_7362  (alias)
 *   __libcgj_batch7362_marker = "libcgj-batch7362"
 *
 * Exclusive continuum CREATE-ONLY (7361-7370: fallocate mode stubs —
 * keep_size_id, punch_hole_id, collapse_range_id, zero_range_id,
 * has_keep_size, has_punch_hole, modes_ok, modes_pack, modes_errorish,
 * batch_id_7370).
 * Unique gj_falloc_punch_hole_id_7362 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7362_marker[] = "libcgj-batch7362";

/* FALLOC_FL_PUNCH_HOLE: deallocate range (hole punch) (0x02). */
#define B7362_FALLOC_FL_PUNCH_HOLE ((uint32_t)0x02u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7362_falloc_punch_hole_id(void)
{
	return B7362_FALLOC_FL_PUNCH_HOLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_falloc_punch_hole_id_7362 - soft FALLOC_FL_PUNCH_HOLE mode constant.
 *
 * Always returns 0x02 (Linux FALLOC_FL_PUNCH_HOLE). Catalog id only;
 * does not call fallocate. No parent wires.
 */
uint32_t
gj_falloc_punch_hole_id_7362(void)
{
	(void)NULL;
	return b7362_falloc_punch_hole_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_falloc_punch_hole_id_7362(void)
    __attribute__((alias("gj_falloc_punch_hole_id_7362")));
