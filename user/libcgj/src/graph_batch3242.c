/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3242: milestone 3250 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3250(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3250 continuum (always 3250). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3250  (alias)
 *   __libcgj_batch3242_marker = "libcgj-batch3242"
 *
 * Milestone 3250 exclusive continuum CREATE-ONLY (3241-3250). Unique
 * gj_export_hint_3250 surface only; no multi-def. Distinct from
 * gj_export_hint_3200 (batch3193), gj_export_hint_3100 (batch3094),
 * gj_export_hint_3000 (batch2992), and gj_export_hint_2700
 * (batch2692). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3242_marker[] = "libcgj-batch3242";

/* Product export hint tag for wave 3250. */
#define B3242_EXPORT_HINT  3250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3242_export_hint(void)
{
	return B3242_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3250 - report the milestone-3250 product export hint.
 *
 * Always returns 3250 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3250-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3250(void)
{
	(void)NULL;
	return b3242_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3250(void)
    __attribute__((alias("gj_export_hint_3250")));
