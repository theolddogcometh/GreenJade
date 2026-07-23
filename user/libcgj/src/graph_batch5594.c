/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5594: dyn smoke soft-green gate (wave 5600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_5600(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 5600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_5600  (alias)
 *   __libcgj_batch5594_marker = "libcgj-batch5594"
 *
 * Milestone 5600 exclusive continuum CREATE-ONLY (5591-5600). Unique
 * gj_dyn_green_5600 surface only; no multi-def. Distinct from
 * gj_dyn_green_5550 (batch5544), gj_dyn_green_5500 (batch5494),
 * gj_dyn_green_5450 (batch5444), and gj_smoke_green_5600
 * (batch5593 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5594_marker[] = "libcgj-batch5594";

/* Dyn soft-green lamp for wave 5600 (always green). */
#define B5594_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5594_green(void)
{
	return B5594_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_5600 - report dyn soft-green lamp for wave 5600.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch5593) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_5600(void)
{
	(void)NULL;
	return b5594_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_5600(void)
    __attribute__((alias("gj_dyn_green_5600")));
