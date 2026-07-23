/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7792: milestone 7800 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_7800(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 7800 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_7800  (alias)
 *   __libcgj_batch7792_marker = "libcgj-batch7792"
 *
 * Milestone 7800 exclusive continuum CREATE-ONLY (7791-7800). Unique
 * gj_export_hint_7800 surface only; no multi-def. Distinct from
 * gj_export_hint_7700 (batch7692), gj_export_hint_7600 (batch7592),
 * gj_export_hint_7500 (batch7492), and gj_export_hint_7400
 * (batch7392). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7792_marker[] = "libcgj-batch7792";

/* Product export-hint ready lamp for wave 7800. */
#define B7792_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7792_hint(void)
{
	return B7792_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_7800 - report product export hint for wave 7800.
 *
 * Always returns 1 (export-hint ready). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_export_hint_7800(void)
{
	(void)NULL;
	return b7792_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_7800(void)
    __attribute__((alias("gj_export_hint_7800")));
