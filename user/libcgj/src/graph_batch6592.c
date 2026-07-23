/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6592: milestone 6600 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_6600(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 6600 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_6600  (alias)
 *   __libcgj_batch6592_marker = "libcgj-batch6592"
 *
 * Milestone 6600 exclusive continuum CREATE-ONLY (6591-6600). Unique
 * gj_export_hint_6600 surface only; no multi-def. Distinct from
 * gj_export_hint_6500 (batch6492), gj_export_hint_6400 (batch6392),
 * gj_export_hint_6300 (batch6292), and gj_export_hint_6200
 * (batch6192). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6592_marker[] = "libcgj-batch6592";

/* Product export-hint ready lamp for wave 6600. */
#define B6592_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6592_hint(void)
{
	return B6592_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_6600 - report product export-hint ready tag for wave 6600.
 *
 * Always returns 1 (export hint ready). Link-time presence of this
 * symbol tags the product export band. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_export_hint_6600(void)
{
	(void)NULL;
	return b6592_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_6600(void)
    __attribute__((alias("gj_export_hint_6600")));
