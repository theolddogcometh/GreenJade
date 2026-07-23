/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3692: milestone 3700 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3700(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3700 continuum (always 3700). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3700  (alias)
 *   __libcgj_batch3692_marker = "libcgj-batch3692"
 *
 * Milestone 3700 exclusive continuum CREATE-ONLY (3691-3700). Unique
 * gj_export_hint_3700 surface only; no multi-def. Distinct from
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

const char __libcgj_batch3692_marker[] = "libcgj-batch3692";

/* Product export hint tag for wave 3700. */
#define B3692_EXPORT_HINT  3700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3692_export_hint(void)
{
	return B3692_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3700 - report the milestone-3700 product export hint.
 *
 * Always returns 3700 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3700-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3700(void)
{
	(void)NULL;
	return b3692_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3700(void)
    __attribute__((alias("gj_export_hint_3700")));
