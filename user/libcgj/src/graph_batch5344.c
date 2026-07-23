/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5344: dyn smoke soft-green gate (wave 5350).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_5350(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 5350
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_5350  (alias)
 *   __libcgj_batch5344_marker = "libcgj-batch5344"
 *
 * Milestone 5350 exclusive continuum CREATE-ONLY (5341-5350). Unique
 * gj_dyn_green_5350 surface only; no multi-def. Distinct from
 * gj_dyn_green_5300 (batch5294), gj_dyn_green_5200 (batch5194),
 * gj_dyn_green_5100 (batch5094), and gj_smoke_green_5350
 * (batch5343 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5344_marker[] = "libcgj-batch5344";

/* Dyn soft-green lamp for wave 5350 (always green). */
#define B5344_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5344_green(void)
{
	return B5344_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_5350 - report dyn soft-green lamp for wave 5350.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch5343) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_5350(void)
{
	(void)NULL;
	return b5344_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_5350(void)
    __attribute__((alias("gj_dyn_green_5350")));
