/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4842: milestone 4850 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_4850(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 4850 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_4850  (alias)
 *   __libcgj_batch4842_marker = "libcgj-batch4842"
 *
 * Milestone 4850 exclusive continuum CREATE-ONLY (4841-4850). Unique
 * gj_export_hint_4850 surface only; no multi-def. Distinct from
 * gj_export_hint_4800 (batch4792), gj_export_hint_4750 (batch4742),
 * gj_export_hint_4700 (batch4692), and gj_export_hint_4650
 * (batch4642). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4842_marker[] = "libcgj-batch4842";

/* Product export hint ready tag for wave 4850. */
#define B4842_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4842_export_hint(void)
{
	return B4842_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_4850 - report the milestone-4850 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 4850-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_4850(void)
{
	(void)NULL;
	return b4842_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_4850(void)
    __attribute__((alias("gj_export_hint_4850")));
