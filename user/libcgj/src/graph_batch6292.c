/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6292: milestone 6300 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_6300(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 6300 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_6300  (alias)
 *   __libcgj_batch6292_marker = "libcgj-batch6292"
 *
 * Milestone 6300 exclusive continuum CREATE-ONLY (6291-6300). Unique
 * gj_export_hint_6300 surface only; no multi-def. Distinct from
 * gj_export_hint_6200 (batch6192), gj_export_hint_6100 (batch6092),
 * gj_export_hint_6000 (batch5992), and gj_export_hint_5900
 * (batch5892). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6292_marker[] = "libcgj-batch6292";

/* Product export hint ready tag for wave 6300. */
#define B6292_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6292_export_hint(void)
{
	return B6292_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_6300 - report the milestone-6300 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 6300-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_6300(void)
{
	(void)NULL;
	return b6292_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_6300(void)
    __attribute__((alias("gj_export_hint_6300")));
