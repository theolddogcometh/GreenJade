/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3892: milestone 3900 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3900(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3900 continuum (always 3900). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3900  (alias)
 *   __libcgj_batch3892_marker = "libcgj-batch3892"
 *
 * Milestone 3900 exclusive continuum CREATE-ONLY (3891-3900). Unique
 * gj_export_hint_3900 surface only; no multi-def. Distinct from
 * gj_export_hint_3700 (batch3692), gj_export_hint_3600 (batch3592),
 * gj_export_hint_3550 (batch3542), and gj_export_hint_3500
 * (batch3492). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3892_marker[] = "libcgj-batch3892";

/* Product export hint tag for wave 3900. */
#define B3892_EXPORT_HINT  3900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3892_export_hint(void)
{
	return B3892_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3900 - report the milestone-3900 product export hint.
 *
 * Always returns 3900 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3900-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3900(void)
{
	(void)NULL;
	return b3892_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3900(void)
    __attribute__((alias("gj_export_hint_3900")));
