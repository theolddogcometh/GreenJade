/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7794: dyn smoke soft-green gate (wave 7800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_7800(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 7800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_7800  (alias)
 *   __libcgj_batch7794_marker = "libcgj-batch7794"
 *
 * Milestone 7800 exclusive continuum CREATE-ONLY (7791-7800). Unique
 * gj_dyn_green_7800 surface only; no multi-def. Distinct from
 * gj_dyn_green_7700 (batch7694), gj_dyn_green_7600 (batch7594),
 * gj_dyn_green_7500 (batch7494), and gj_smoke_green_7800
 * (batch7793 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7794_marker[] = "libcgj-batch7794";

/* Dyn-smoke soft-green lamp for wave 7800. */
#define B7794_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7794_green(void)
{
	return B7794_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_7800 - report dyn-smoke soft-green for wave 7800.
 *
 * Always returns 1 (dyn soft-green). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_dyn_green_7800(void)
{
	(void)NULL;
	return b7794_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_7800(void)
    __attribute__((alias("gj_dyn_green_7800")));
