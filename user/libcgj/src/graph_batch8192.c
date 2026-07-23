/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8192: milestone 8200 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_8200(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 8200 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_8200  (alias)
 *   __libcgj_batch8192_marker = "libcgj-batch8192"
 *
 * Milestone 8200 exclusive continuum CREATE-ONLY (8191-8200). Unique
 * gj_export_hint_8200 surface only; no multi-def. Distinct from
 * gj_export_hint_8000 (batch7992), gj_export_hint_7000 (batch6992),
 * gj_export_hint_6000 (batch5992), and gj_export_hint_5900
 * (batch5892). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8192_marker[] = "libcgj-batch8192";

/* Product export hint ready tag for wave 8200. */
#define B8192_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8192_export_hint(void)
{
	return B8192_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_8200 - report the milestone-8200 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 8200-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_8200(void)
{
	(void)NULL;
	return b8192_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_8200(void)
    __attribute__((alias("gj_export_hint_8200")));
