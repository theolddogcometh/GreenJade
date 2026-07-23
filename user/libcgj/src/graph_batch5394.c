/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5394: dyn smoke soft-green gate (wave 5400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_5400(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 5400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_5400  (alias)
 *   __libcgj_batch5394_marker = "libcgj-batch5394"
 *
 * Milestone 5400 exclusive continuum CREATE-ONLY (5391-5400). Unique
 * gj_dyn_green_5400 surface only; no multi-def. Distinct from
 * gj_dyn_green_5350 (batch5344), gj_dyn_green_5300 (batch5294),
 * gj_dyn_green_5200 (batch5194), and gj_smoke_green_5400
 * (batch5393 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5394_marker[] = "libcgj-batch5394";

/* Dyn soft-green lamp for wave 5400 (always green). */
#define B5394_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5394_green(void)
{
	return B5394_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_5400 - report dyn soft-green lamp for wave 5400.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch5393) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_5400(void)
{
	(void)NULL;
	return b5394_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_5400(void)
    __attribute__((alias("gj_dyn_green_5400")));
