/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5792: milestone 5800 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_5800(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 5800 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_5800  (alias)
 *   __libcgj_batch5792_marker = "libcgj-batch5792"
 *
 * Milestone 5800 exclusive continuum CREATE-ONLY (5791-5800). Unique
 * gj_export_hint_5800 surface only; no multi-def. Distinct from
 * gj_export_hint_5700 (batch5692), gj_export_hint_5650 (batch5642),
 * gj_export_hint_5600 (batch5592), and gj_export_hint_5550
 * (batch5542). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5792_marker[] = "libcgj-batch5792";

/* Product export hint ready tag for wave 5800. */
#define B5792_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5792_export_hint(void)
{
	return B5792_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_5800 - report the milestone-5800 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 5800-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_5800(void)
{
	(void)NULL;
	return b5792_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_5800(void)
    __attribute__((alias("gj_export_hint_5800")));
