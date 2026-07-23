/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5794: dyn smoke soft-green gate (wave 5800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_5800(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 5800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_5800  (alias)
 *   __libcgj_batch5794_marker = "libcgj-batch5794"
 *
 * Milestone 5800 exclusive continuum CREATE-ONLY (5791-5800). Unique
 * gj_dyn_green_5800 surface only; no multi-def. Distinct from
 * gj_dyn_green_5700 (batch5694), gj_dyn_green_5650 (batch5644),
 * gj_dyn_green_5600 (batch5594), and gj_smoke_green_5800
 * (batch5793 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5794_marker[] = "libcgj-batch5794";

/* Dyn soft-green lamp for wave 5800 (always green). */
#define B5794_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5794_green(void)
{
	return B5794_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_5800 - report dyn soft-green lamp for wave 5800.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch5793) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_5800(void)
{
	(void)NULL;
	return b5794_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_5800(void)
    __attribute__((alias("gj_dyn_green_5800")));
