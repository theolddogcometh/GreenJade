/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2592: milestone 2600 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_2600(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       2600 continuum (always 2600). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_2600  (alias)
 *   __libcgj_batch2592_marker = "libcgj-batch2592"
 *
 * Milestone 2600 exclusive product helpers (2591-2600). Unique
 * gj_export_hint_2600 surface only; no multi-def. Distinct from
 * gj_export_target_2500 (batch2492), gj_export_floor_2400 (batch2392),
 * gj_export_band_ok (batch2292), and gj_export_floor_ok (batch2194).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2592_marker[] = "libcgj-batch2592";

/* Product export hint tag for wave 2600. */
#define B2592_EXPORT_HINT  2600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2592_export_hint(void)
{
	return B2592_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_2600 - report the milestone-2600 product export hint.
 *
 * Always returns 2600 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 2600-wave export hint. Does not
 * parse ELF. Does not call libc.
 */
uint32_t
gj_export_hint_2600(void)
{
	(void)NULL;
	return b2592_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_2600(void)
    __attribute__((alias("gj_export_hint_2600")));
