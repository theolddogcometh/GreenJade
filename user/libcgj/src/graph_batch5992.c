/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5992: milestone 6000 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_6000(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 6000 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_6000  (alias)
 *   __libcgj_batch5992_marker = "libcgj-batch5992"
 *
 * Milestone 6000 exclusive continuum CREATE-ONLY (5991-6000). Unique
 * gj_export_hint_6000 surface only; no multi-def. Distinct from
 * gj_export_hint_5900 (batch5892), gj_export_hint_5800 (batch5792),
 * gj_export_hint_5700 (batch5692), and gj_export_hint_5650
 * (batch5642). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5992_marker[] = "libcgj-batch5992";

/* Product export hint ready tag for wave 6000. */
#define B5992_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5992_export_hint(void)
{
	return B5992_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_6000 - report the milestone-6000 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 6000-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_6000(void)
{
	(void)NULL;
	return b5992_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_6000(void)
    __attribute__((alias("gj_export_hint_6000")));
