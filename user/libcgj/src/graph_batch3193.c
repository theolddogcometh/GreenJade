/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3193: milestone 3200 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_3200(void);
 *     - Returns the product dynamic-export hint tag for the milestone
 *       3200 continuum (always 3200). Soft compile-time product tag
 *       paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_3200  (alias)
 *   __libcgj_batch3193_marker = "libcgj-batch3193"
 *
 * Milestone 3200 exclusive continuum CREATE-ONLY (3191-3200). Unique
 * gj_export_hint_3200 surface only; no multi-def. Distinct from
 * gj_export_hint_3100 (batch3094), gj_export_hint_3000 (batch2992),
 * gj_export_hint_2700 (batch2692), and gj_export_hint_2600
 * (batch2592). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3193_marker[] = "libcgj-batch3193";

/* Product export hint tag for wave 3200. */
#define B3193_EXPORT_HINT  3200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3193_export_hint(void)
{
return B3193_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_3200 - report the milestone-3200 product export hint.
 *
 * Always returns 3200 (wave-tagged export hint constant). Link-time
 * presence of this symbol tags the 3200-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_3200(void)
{
(void)NULL;
return b3193_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_3200(void)
    __attribute__((alias("gj_export_hint_3200")));
