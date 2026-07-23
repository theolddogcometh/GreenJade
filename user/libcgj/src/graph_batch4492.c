/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4492: milestone 4500 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_export_hint_4500(void);
 *     - Returns the product dynamic-export hint ready tag for the
 *       milestone 4500 continuum (always 1). Soft compile-time
 *       product tag paired with export-band / floor / target checks.
 *   uint32_t __gj_export_hint_4500  (alias)
 *   __libcgj_batch4492_marker = "libcgj-batch4492"
 *
 * Milestone 4500 exclusive continuum CREATE-ONLY (4491-4500). Unique
 * gj_export_hint_4500 surface only; no multi-def. Distinct from
 * gj_export_hint_4400 (batch4392), gj_export_hint_4300 (batch4292),
 * gj_export_hint_4100 (batch4092), and gj_export_hint_3950
 * (batch3942). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4492_marker[] = "libcgj-batch4492";

/* Product export hint ready tag for wave 4500. */
#define B4492_EXPORT_HINT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4492_export_hint(void)
{
	return B4492_EXPORT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_export_hint_4500 - report the milestone-4500 product export hint.
 *
 * Always returns 1 (export-hint ready). Link-time presence of this
 * symbol tags the 4500-wave export hint. Does not parse ELF. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_export_hint_4500(void)
{
	(void)NULL;
	return b4492_export_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_export_hint_4500(void)
    __attribute__((alias("gj_export_hint_4500")));
