/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4142: milestone 4150 product export hint flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_4150(void);
 *     - Returns the product dynamic-export hint status flag for the
 *       milestone 4150 continuum (always 1 = hint active). Soft
 *       compile-time product status lamp.
 *   uint32_t __gj_export_hint_4150  (alias)
 *   __libcgj_batch4142_marker = "libcgj-batch4142"
 *
 * Milestone 4150 exclusive continuum CREATE-ONLY (4141-4150). Unique
 * gj_export_hint_4150 surface only; no multi-def. Distinct from
 * gj_export_hint_4100 (batch4092), gj_export_hint_4050 (batch4042),
 * and prior export_hint_* symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4142_marker[] = "libcgj-batch4142";

/* Product export hint status flag for wave 4150 (1 = active). */
#define B4142_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4142_export_hint(void)
{
	return B4142_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_4150 - report the milestone-4150 product export hint flag.
 *
 * Always returns 1 (export hint active for wave 4150). Link-time
 * presence of this symbol tags the 4150-wave export hint. Does not
 * parse ELF. Does not call libc. No parent wires.
 */
uint32_t
gj_export_hint_4150(void)
{
	(void)NULL;
	return b4142_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_4150(void)
    __attribute__((alias("gj_export_hint_4150")));
