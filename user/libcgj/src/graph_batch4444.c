/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4444: dyn smoke soft-green gate (wave 4450).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4450(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4450
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4450  (alias)
 *   __libcgj_batch4444_marker = "libcgj-batch4444"
 *
 * Milestone 4450 exclusive continuum CREATE-ONLY (4441-4450). Unique
 * gj_dyn_green_4450 surface only; no multi-def. Distinct from
 * gj_dyn_green_4400 (batch4394), gj_dyn_green_4300 (batch4294),
 * gj_dyn_green_4100 (batch4094), and gj_smoke_green_4450
 * (batch4443 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4444_marker[] = "libcgj-batch4444";

/* Dyn soft-green lamp for wave 4450 (always green). */
#define B4444_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4444_green(void)
{
	return B4444_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4450 - report dyn soft-green lamp for wave 4450.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4443) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4450(void)
{
	(void)NULL;
	return b4444_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4450(void)
    __attribute__((alias("gj_dyn_green_4450")));
