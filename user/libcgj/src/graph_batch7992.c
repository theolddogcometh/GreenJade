/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7992: milestone 8000 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_8000(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 8000 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_8000  (alias)
 *   __libcgj_batch7992_marker = "libcgj-batch7992"
 *
 * Milestone 8000 exclusive continuum CREATE-ONLY (7991-8000). Unique
 * gj_export_hint_8000 surface only; no multi-def. Distinct from
 * gj_export_hint_7000 (batch6992), gj_export_hint_6000 (batch5992),
 * gj_export_hint_5900 (batch5892), and gj_export_hint_5800
 * (batch5792). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7992_marker[] = "libcgj-batch7992";

/* Product export hint ready tag for wave 8000. */
#define B7992_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7992_export_hint(void)
{
	return B7992_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_8000 - report the milestone-8000 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 8000-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_8000(void)
{
	(void)NULL;
	return b7992_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_8000(void)
    __attribute__((alias("gj_export_hint_8000")));
