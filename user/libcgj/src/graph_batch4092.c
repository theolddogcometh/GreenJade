/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4092: milestone 4100 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_4100(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       4100 continuum (always 4100). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_4100  (alias)
 *   __libcgj_batch4092_marker = "libcgj-batch4092"
 *
 * Milestone 4100 exclusive continuum CREATE-ONLY (4091-4100). Unique
 * gj_export_hint_4100 surface only; no multi-def. Distinct from
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

const char __libcgj_batch4092_marker[] = "libcgj-batch4092";

/* Product export hint tag for wave 4100. */
#define B4092_EXPORT_HINT  4100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4092_export_hint(void)
{
	return B4092_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_4100 - report the milestone-4100 product export hint.
 *
 * Always returns 4100 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 4100-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_4100(void)
{
	(void)NULL;
	return b4092_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_4100(void)
    __attribute__((alias("gj_export_hint_4100")));
