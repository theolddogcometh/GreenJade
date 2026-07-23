/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4392: milestone 4400 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_4400(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 4400 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_4400  (alias)
 *   __libcgj_batch4392_marker = "libcgj-batch4392"
 *
 * Milestone 4400 exclusive continuum CREATE-ONLY (4391-4400). Unique
 * gj_export_hint_4400 surface only; no multi-def. Distinct from
 * gj_export_hint_4300 (batch4292), gj_export_hint_4100 (batch4092),
 * gj_export_hint_3950 (batch3942), and gj_export_hint_3900
 * (batch3892). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4392_marker[] = "libcgj-batch4392";

/* Product export hint ready tag for wave 4400. */
#define B4392_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4392_export_hint(void)
{
	return B4392_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_4400 - report the milestone-4400 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 4400-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_4400(void)
{
	(void)NULL;
	return b4392_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_4400(void)
    __attribute__((alias("gj_export_hint_4400")));
