/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3592: milestone 3600 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3600(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3600 continuum (always 3600). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3600  (alias)
 *   __libcgj_batch3592_marker = "libcgj-batch3592"
 *
 * Milestone 3600 exclusive continuum CREATE-ONLY (3591-3600). Unique
 * gj_export_hint_3600 surface only; no multi-def. Distinct from
 * gj_export_hint_3550 (batch3542), gj_export_hint_3500 (batch3492),
 * gj_export_hint_3400 (batch3392), and gj_export_hint_3300
 * (batch3292). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3592_marker[] = "libcgj-batch3592";

/* Product export hint tag for wave 3600. */
#define B3592_EXPORT_HINT  3600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3592_export_hint(void)
{
	return B3592_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3600 - report the milestone-3600 product export hint.
 *
 * Always returns 3600 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3600-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3600(void)
{
	(void)NULL;
	return b3592_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3600(void)
    __attribute__((alias("gj_export_hint_3600")));
