/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4942: milestone 4950 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_4950(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 4950 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_4950  (alias)
 *   __libcgj_batch4942_marker = "libcgj-batch4942"
 *
 * Milestone 4950 exclusive continuum CREATE-ONLY (4941-4950). Unique
 * gj_export_hint_4950 surface only; no multi-def. Distinct from
 * gj_export_hint_4900 (batch4892), gj_export_hint_4850 (batch4842),
 * gj_export_hint_4800 (batch4792), and gj_export_hint_4750
 * (batch4742). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4942_marker[] = "libcgj-batch4942";

/* Product export hint ready tag for wave 4950. */
#define B4942_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4942_export_hint(void)
{
	return B4942_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_4950 - report the milestone-4950 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 4950-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_4950(void)
{
	(void)NULL;
	return b4942_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_4950(void)
    __attribute__((alias("gj_export_hint_4950")));
