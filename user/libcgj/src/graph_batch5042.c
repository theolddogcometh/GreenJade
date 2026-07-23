/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5042: milestone 5050 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_5050(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 5050 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_5050  (alias)
 *   __libcgj_batch5042_marker = "libcgj-batch5042"
 *
 * Milestone 5050 exclusive continuum CREATE-ONLY (5041-5050). Unique
 * gj_export_hint_5050 surface only; no multi-def. Distinct from
 * gj_export_hint_5000 (batch4992), gj_export_hint_4900 (batch4892),
 * gj_export_hint_4850 (batch4842), and gj_export_hint_4800
 * (batch4792). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5042_marker[] = "libcgj-batch5042";

/* Product export hint ready tag for wave 5050. */
#define B5042_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5042_export_hint(void)
{
	return B5042_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_5050 - report the milestone-5050 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 5050-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_5050(void)
{
	(void)NULL;
	return b5042_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_5050(void)
    __attribute__((alias("gj_export_hint_5050")));
