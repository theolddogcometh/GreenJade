/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2194: dynamic export floor predicate.
 *
 * Surface (unique symbols):
 *   int gj_export_floor_ok(uint32_t count);
 *     - Return 1 if count is at least the product export floor (1),
 *       else 0. Soft lower-bound check for dynamic symbol tables.
 *       Distinct from gj_export_count_ok_hint (batch1997 upper band).
 *   int __gj_export_floor_ok  (alias)
 *   __libcgj_batch2194_marker = "libcgj-batch2194"
 *
 * Milestone 2200 exclusive product helpers (2191-2200). Unique
 * gj_export_floor_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2194_marker[] = "libcgj-batch2194";

/* Product minimum non-empty export table size. */
#define B2194_EXPORT_FLOOR  1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if count meets or exceeds the export floor. */
static int
b2194_floor_ok(uint32_t uCount)
{
	return (uCount >= B2194_EXPORT_FLOOR) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_floor_ok - 1 iff export count is at least the product floor.
 *
 * count: reported dynamic export symbol count.
 * Floor is 1 (empty tables fail). Pair with gj_export_count_ok_hint for
 * upper-band checks. Does not parse ELF. Does not call libc.
 */
int
gj_export_floor_ok(uint32_t uCount)
{
	(void)NULL;
	return b2194_floor_ok(uCount);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_export_floor_ok(uint32_t uCount)
    __attribute__((alias("gj_export_floor_ok")));
