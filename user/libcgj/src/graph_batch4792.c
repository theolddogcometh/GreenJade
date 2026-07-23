/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4792: milestone 4800 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_4800(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 4800 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_4800  (alias)
 *   __libcgj_batch4792_marker = "libcgj-batch4792"
 *
 * Milestone 4800 exclusive continuum CREATE-ONLY (4791-4800). Unique
 * gj_export_hint_4800 surface only; no multi-def. Distinct from
 * gj_export_hint_4750 (batch4742), gj_export_hint_4700 (batch4692),
 * gj_export_hint_4650 (batch4642), and gj_export_hint_4600
 * (batch4592). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4792_marker[] = "libcgj-batch4792";

/* Product export hint ready tag for wave 4800. */
#define B4792_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4792_export_hint(void)
{
	return B4792_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_4800 - report the milestone-4800 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 4800-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_4800(void)
{
	(void)NULL;
	return b4792_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_4800(void)
    __attribute__((alias("gj_export_hint_4800")));
