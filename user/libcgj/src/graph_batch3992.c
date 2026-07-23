/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3992: milestone 4000 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_4000(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       4000 continuum (always 4000). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_4000  (alias)
 *   __libcgj_batch3992_marker = "libcgj-batch3992"
 *
 * Milestone 4000 exclusive continuum CREATE-ONLY (3991-4000). Unique
 * gj_export_hint_4000 surface only; no multi-def. Distinct from
 * gj_export_hint_3950 (batch3942), gj_export_hint_3900 (batch3892),
 * gj_export_hint_3700 (batch3692), and gj_export_hint_3600
 * (batch3592). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3992_marker[] = "libcgj-batch3992";

/* Product export hint tag for wave 4000. */
#define B3992_EXPORT_HINT  4000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3992_export_hint(void)
{
	return B3992_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_4000 - report the milestone-4000 product export hint.
 *
 * Always returns 4000 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 4000-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_4000(void)
{
	(void)NULL;
	return b3992_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_4000(void)
    __attribute__((alias("gj_export_hint_4000")));
