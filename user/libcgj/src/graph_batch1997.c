/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1997: ELF export count soft-ok hint.
 *
 * Surface (unique symbols):
 *   int gj_export_count_ok_hint(uint32_t count);
 *     — Return 1 if count is in (0, 65536] (plausible dynamic export
 *       table size for a product shared object), else 0. Soft pure-data
 *       range hint; does not parse ELF.
 *   int __gj_export_count_ok_hint  (alias)
 *   __libcgj_batch1997_marker = "libcgj-batch1997"
 *
 * Product bar helpers exclusive pure-data wave (1991–2000). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1997_marker[] = "libcgj-batch1997";

/* Soft upper bound for export-symbol counts (16-bit table style). */
#define B1997_EXPORT_MAX  65536u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if count is in (0, B1997_EXPORT_MAX]. */
static int
b1997_export_ok(uint32_t uCount)
{
	if (uCount == 0u) {
		return 0;
	}
	return (uCount <= B1997_EXPORT_MAX) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_count_ok_hint — 1 iff export count is in (0, 65536].
 *
 * count: reported dynamic export symbol count.
 * Returns 0 for empty or oversized tables.
 */
int
gj_export_count_ok_hint(uint32_t uCount)
{
	(void)NULL;
	return b1997_export_ok(uCount);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_export_count_ok_hint(uint32_t uCount)
    __attribute__((alias("gj_export_count_ok_hint")));
