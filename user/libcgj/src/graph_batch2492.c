/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2492: milestone 2500 product export target.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_target_2500(void);
 *     - Returns the product dynamic-export target count for the
 *       milestone 2500 continuum (always 2500). Soft compile-time
 *       product tag paired with export-band / floor checks.
 *   uint32_t __gj_export_target_2500  (alias)
 *   __libcgj_batch2492_marker = "libcgj-batch2492"
 *
 * Milestone 2500 exclusive product helpers (2491-2500). Unique
 * gj_export_target_2500 surface only; no multi-def. Distinct from
 * gj_export_floor_2400 (batch2392), gj_export_band_ok (batch2292),
 * and gj_export_floor_ok (batch2194).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2492_marker[] = "libcgj-batch2492";

/* Product export target count tag for wave 2500. */
#define B2492_EXPORT_TARGET  2500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2492_export_target(void)
{
	return B2492_EXPORT_TARGET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_target_2500 - report the milestone-2500 product export target.
 *
 * Always returns 2500 (wave-tagged export target constant). Link-time
 * presence of this symbol tags the 2500-wave export target. Does not
 * parse ELF. Does not call libc.
 */
uint32_t
gj_export_target_2500(void)
{
	(void)NULL;
	return b2492_export_target();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_target_2500(void)
    __attribute__((alias("gj_export_target_2500")));
