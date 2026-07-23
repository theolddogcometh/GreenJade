/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4192: milestone 4200 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_4200(void);
 *     - Returns the product dynamic-export hint flag for the milestone
 *       4200 continuum (always 1). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_4200  (alias)
 *   __libcgj_batch4192_marker = "libcgj-batch4192"
 *
 * Milestone 4200 exclusive continuum CREATE-ONLY (4191-4200). Unique
 * gj_export_hint_4200 surface only; no multi-def. Distinct from
 * gj_export_hint_4100 (batch4092), gj_export_hint_3950 (batch3942),
 * gj_export_hint_3900 (batch3892), and gj_export_hint_3700
 * (batch3692). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4192_marker[] = "libcgj-batch4192";

/* Product export hint flag for wave 4200. */
#define B4192_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4192_export_hint(void)
{
	return B4192_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_4200 - report the milestone-4200 product export hint.
 *
 * Always returns 1 (wave-tagged export hint present). Link-time
 * presence of this symbol tags the 4200-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_4200(void)
{
	(void)NULL;
	return b4192_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_4200(void)
    __attribute__((alias("gj_export_hint_4200")));
