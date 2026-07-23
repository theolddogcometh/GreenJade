/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3842: milestone 3850 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3850(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3850 continuum (always 3850). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3850  (alias)
 *   __libcgj_batch3842_marker = "libcgj-batch3842"
 *
 * Milestone 3850 exclusive continuum CREATE-ONLY (3841-3850). Unique
 * gj_export_hint_3850 surface only; no multi-def. Distinct from
 * gj_export_hint_3750 (batch3742), gj_export_hint_3700 (batch3692),
 * gj_export_hint_3600 (batch3592), and gj_export_hint_3550
 * (batch3542). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3842_marker[] = "libcgj-batch3842";

/* Product export hint tag for wave 3850. */
#define B3842_EXPORT_HINT  3850u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3842_export_hint(void)
{
	return B3842_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3850 - report the milestone-3850 product export hint.
 *
 * Always returns 3850 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3850-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3850(void)
{
	(void)NULL;
	return b3842_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3850(void)
    __attribute__((alias("gj_export_hint_3850")));
