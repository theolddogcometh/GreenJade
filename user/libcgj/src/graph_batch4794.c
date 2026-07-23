/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4794: dyn smoke soft-green gate (wave 4800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4800(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4800  (alias)
 *   __libcgj_batch4794_marker = "libcgj-batch4794"
 *
 * Milestone 4800 exclusive continuum CREATE-ONLY (4791-4800). Unique
 * gj_dyn_green_4800 surface only; no multi-def. Distinct from
 * gj_dyn_green_4750 (batch4744), gj_dyn_green_4700 (batch4694),
 * gj_dyn_green_4650 (batch4644), and gj_smoke_green_4800
 * (batch4793 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4794_marker[] = "libcgj-batch4794";

/* Dyn soft-green lamp for wave 4800 (always green). */
#define B4794_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4794_green(void)
{
	return B4794_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4800 - report dyn soft-green lamp for wave 4800.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4793) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4800(void)
{
	(void)NULL;
	return b4794_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4800(void)
    __attribute__((alias("gj_dyn_green_4800")));
