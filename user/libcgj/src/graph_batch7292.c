/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7292: milestone 7300 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_7300(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 7300 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_7300  (alias)
 *   __libcgj_batch7292_marker = "libcgj-batch7292"
 *
 * Milestone 7300 exclusive continuum CREATE-ONLY (7291-7300). Unique
 * gj_export_hint_7300 surface only; no multi-def. Distinct from
 * gj_export_hint_7200 (batch7192), gj_export_hint_7100 (batch7092),
 * gj_export_hint_7000 (batch6992), and gj_export_hint_6000
 * (batch5992). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7292_marker[] = "libcgj-batch7292";

/* Product export hint ready tag for wave 7300. */
#define B7292_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7292_export_hint(void)
{
	return B7292_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_7300 - report the milestone-7300 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 7300-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_7300(void)
{
	(void)NULL;
	return b7292_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_7300(void)
    __attribute__((alias("gj_export_hint_7300")));
