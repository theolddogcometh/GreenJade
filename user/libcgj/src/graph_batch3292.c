/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3292: milestone 3300 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3300(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3300 continuum (always 3300). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3300  (alias)
 *   __libcgj_batch3292_marker = "libcgj-batch3292"
 *
 * Milestone 3300 exclusive continuum CREATE-ONLY (3291-3300). Unique
 * gj_export_hint_3300 surface only; no multi-def. Distinct from
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

const char __libcgj_batch3292_marker[] = "libcgj-batch3292";

/* Product export hint tag for wave 3300. */
#define B3292_EXPORT_HINT  3300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3292_export_hint(void)
{
	return B3292_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3300 - report the milestone-3300 product export hint.
 *
 * Always returns 3300 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3300-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3300(void)
{
	(void)NULL;
	return b3292_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3300(void)
    __attribute__((alias("gj_export_hint_3300")));
