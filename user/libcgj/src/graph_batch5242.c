/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5242: milestone 5250 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_5250(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 5250 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_5250  (alias)
 *   __libcgj_batch5242_marker = "libcgj-batch5242"
 *
 * Milestone 5250 exclusive continuum CREATE-ONLY (5241-5250). Unique
 * gj_export_hint_5250 surface only; no multi-def. Distinct from
 * gj_export_hint_5200 (batch5192), gj_export_hint_5100 (batch5092),
 * gj_export_hint_5000 (batch4992), and gj_export_hint_4900
 * (batch4892). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5242_marker[] = "libcgj-batch5242";

/* Product export hint ready tag for wave 5250. */
#define B5242_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5242_export_hint(void)
{
	return B5242_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_5250 - report the milestone-5250 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 5250-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_5250(void)
{
	(void)NULL;
	return b5242_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_5250(void)
    __attribute__((alias("gj_export_hint_5250")));
