/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7392: milestone 7400 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_7400(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 7400 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_7400  (alias)
 *   __libcgj_batch7392_marker = "libcgj-batch7392"
 *
 * Milestone 7400 exclusive continuum CREATE-ONLY (7391-7400). Unique
 * gj_export_hint_7400 surface only; no multi-def. Distinct from
 * gj_export_hint_7300 (batch7292), gj_export_hint_7200 (batch7192),
 * gj_export_hint_7100 (batch7092), and gj_export_hint_7000
 * (batch6992). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7392_marker[] = "libcgj-batch7392";

/* Product export-hint ready lamp for wave 7400. */
#define B7392_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7392_hint(void)
{
	return B7392_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_7400 - report product export hint for wave 7400.
 *
 * Always returns 1 (export-hint ready). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_export_hint_7400(void)
{
	(void)NULL;
	return b7392_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_7400(void)
    __attribute__((alias("gj_export_hint_7400")));
