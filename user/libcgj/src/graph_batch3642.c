/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3642: milestone 3650 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3650(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3650 continuum (always 3650). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3650  (alias)
 *   __libcgj_batch3642_marker = "libcgj-batch3642"
 *
 * Milestone 3650 exclusive continuum CREATE-ONLY (3641-3650). Unique
 * gj_export_hint_3650 surface only; no multi-def. Distinct from
 * gj_export_hint_3600 (batch3592), gj_export_hint_3550 (batch3542),
 * gj_export_hint_3500 (batch3492), and gj_export_hint_3400
 * (batch3392). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3642_marker[] = "libcgj-batch3642";

/* Product export hint tag for wave 3650. */
#define B3642_EXPORT_HINT  3650u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3642_export_hint(void)
{
	return B3642_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3650 - report the milestone-3650 product export hint.
 *
 * Always returns 3650 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3650-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3650(void)
{
	(void)NULL;
	return b3642_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3650(void)
    __attribute__((alias("gj_export_hint_3650")));
