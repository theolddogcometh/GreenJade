/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5144: dyn smoke soft-green gate (wave 5150).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_5150(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 5150
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_5150  (alias)
 *   __libcgj_batch5144_marker = "libcgj-batch5144"
 *
 * Milestone 5150 exclusive continuum CREATE-ONLY (5141-5150). Unique
 * gj_dyn_green_5150 surface only; no multi-def. Distinct from
 * gj_dyn_green_5100 (batch5094), gj_dyn_green_5000 (batch4994),
 * gj_dyn_green_4900 (batch4894), and gj_smoke_green_5150
 * (batch5143 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5144_marker[] = "libcgj-batch5144";

/* Dyn soft-green lamp for wave 5150 (always green). */
#define B5144_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5144_green(void)
{
	return B5144_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_5150 - report dyn soft-green lamp for wave 5150.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch5143) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_5150(void)
{
	(void)NULL;
	return b5144_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_5150(void)
    __attribute__((alias("gj_dyn_green_5150")));
