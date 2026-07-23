/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2392: milestone 2400 product export floor.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_floor_2400(void);
 *     - Returns the product dynamic-export floor for the milestone 2400
 *       continuum (always 1: non-empty export tables). Soft compile-time
 *       product tag paired with export-band checks.
 *   uint32_t __gj_export_floor_2400  (alias)
 *   __libcgj_batch2392_marker = "libcgj-batch2392"
 *
 * Milestone 2400 exclusive product helpers (2391-2400). Unique
 * gj_export_floor_2400 surface only; no multi-def. Distinct from
 * gj_export_floor_ok (batch2194 predicate) and gj_export_band_ok
 * (batch2292).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2392_marker[] = "libcgj-batch2392";

/* Product minimum non-empty export table size for wave 2400. */
#define B2392_EXPORT_FLOOR  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2392_export_floor(void)
{
	return B2392_EXPORT_FLOOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_floor_2400 - report the milestone-2400 product export floor.
 *
 * Always returns 1 (empty dynamic export tables fail the product floor).
 * Link-time presence of this symbol tags the 2400-wave floor constant.
 * Does not parse ELF. Does not call libc.
 */
uint32_t
gj_export_floor_2400(void)
{
	(void)NULL;
	return b2392_export_floor();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_floor_2400(void)
    __attribute__((alias("gj_export_floor_2400")));
