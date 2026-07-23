/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2692: milestone 2700 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_2700(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       2700 continuum (always 2700). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_2700  (alias)
 *   __libcgj_batch2692_marker = "libcgj-batch2692"
 *
 * Milestone 2700 exclusive product helpers (2691-2700). Unique
 * gj_export_hint_2700 surface only; no multi-def. Distinct from
 * gj_export_hint_2600 (batch2592), gj_export_target_2500 (batch2492),
 * gj_export_floor_2400 (batch2392), and gj_export_band_ok (batch2292).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2692_marker[] = "libcgj-batch2692";

/* Product export hint tag for wave 2700. */
#define B2692_EXPORT_HINT  2700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2692_export_hint(void)
{
	return B2692_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_2700 - report the milestone-2700 product export hint.
 *
 * Always returns 2700 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 2700-wave export hint. Does not
 * parse ELF. Does not call libc.
 */
uint32_t
gj_export_hint_2700(void)
{
	(void)NULL;
	return b2692_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_2700(void)
    __attribute__((alias("gj_export_hint_2700")));
