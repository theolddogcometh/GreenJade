/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3442: milestone 3450 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3450(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3450 continuum (always 3450). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3450  (alias)
 *   __libcgj_batch3442_marker = "libcgj-batch3442"
 *
 * Milestone 3450 exclusive continuum CREATE-ONLY (3441-3450). Unique
 * gj_export_hint_3450 surface only; no multi-def. Distinct from
 * gj_export_hint_3400 (batch3392), gj_export_hint_3300 (batch3292),
 * gj_export_hint_3200 (batch3193), and gj_export_hint_3100
 * (batch3094). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3442_marker[] = "libcgj-batch3442";

/* Product export hint tag for wave 3450. */
#define B3442_EXPORT_HINT  3450u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3442_export_hint(void)
{
	return B3442_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3450 - report the milestone-3450 product export hint.
 *
 * Always returns 3450 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3450-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3450(void)
{
	(void)NULL;
	return b3442_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3450(void)
    __attribute__((alias("gj_export_hint_3450")));
