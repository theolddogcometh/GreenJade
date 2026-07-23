/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6892: milestone 6900 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_6900(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 6900 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_6900  (alias)
 *   __libcgj_batch6892_marker = "libcgj-batch6892"
 *
 * Milestone 6900 exclusive continuum CREATE-ONLY (6891-6900). Unique
 * gj_export_hint_6900 surface only; no multi-def. Distinct from
 * gj_export_hint_6800 (batch6792), gj_export_hint_6700 (batch6692),
 * gj_export_hint_6600 (batch6592), and gj_export_hint_6500
 * (batch6492). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6892_marker[] = "libcgj-batch6892";

/* Product export-hint ready lamp for wave 6900. */
#define B6892_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6892_hint(void)
{
	return B6892_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_6900 - report product export-hint ready tag for wave 6900.
 *
 * Always returns 1 (export hint ready). Link-time presence of this
 * symbol tags the product export band. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_export_hint_6900(void)
{
	(void)NULL;
	return b6892_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_6900(void)
    __attribute__((alias("gj_export_hint_6900")));
