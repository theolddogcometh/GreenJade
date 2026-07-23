/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2992: milestone 3000 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3000(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3000 continuum (always 3000). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3000  (alias)
 *   __libcgj_batch2992_marker = "libcgj-batch2992"
 *
 * Milestone 3000 exclusive continuum CREATE-ONLY (2991-3000). Unique
 * gj_export_hint_3000 surface only; no multi-def. Distinct from
 * gj_export_hint_2700 (batch2692), gj_export_hint_2600 (batch2592),
 * gj_export_target_2500 (batch2492), and gj_export_band_ok (batch2292).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2992_marker[] = "libcgj-batch2992";

/* Product export hint tag for wave 3000. */
#define B2992_EXPORT_HINT  3000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2992_export_hint(void)
{
	return B2992_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3000 - report the milestone-3000 product export hint.
 *
 * Always returns 3000 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3000-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3000(void)
{
	(void)NULL;
	return b2992_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3000(void)
    __attribute__((alias("gj_export_hint_3000")));
