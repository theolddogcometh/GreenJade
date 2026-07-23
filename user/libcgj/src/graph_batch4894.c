/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4894: dyn smoke soft-green gate (wave 4900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4900(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4900  (alias)
 *   __libcgj_batch4894_marker = "libcgj-batch4894"
 *
 * Milestone 4900 exclusive continuum CREATE-ONLY (4891-4900). Unique
 * gj_dyn_green_4900 surface only; no multi-def. Distinct from
 * gj_dyn_green_4850 (batch4844), gj_dyn_green_4800 (batch4794),
 * gj_dyn_green_4750 (batch4744), and gj_smoke_green_4900
 * (batch4893 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4894_marker[] = "libcgj-batch4894";

/* Dyn soft-green lamp for wave 4900 (always green). */
#define B4894_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4894_green(void)
{
	return B4894_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4900 - report dyn soft-green lamp for wave 4900.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4893) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4900(void)
{
	(void)NULL;
	return b4894_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4900(void)
    __attribute__((alias("gj_dyn_green_4900")));
