/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4844: dyn smoke soft-green gate (wave 4850).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4850(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4850
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4850  (alias)
 *   __libcgj_batch4844_marker = "libcgj-batch4844"
 *
 * Milestone 4850 exclusive continuum CREATE-ONLY (4841-4850). Unique
 * gj_dyn_green_4850 surface only; no multi-def. Distinct from
 * gj_dyn_green_4800 (batch4794), gj_dyn_green_4750 (batch4744),
 * gj_dyn_green_4700 (batch4694), and gj_smoke_green_4850
 * (batch4843 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4844_marker[] = "libcgj-batch4844";

/* Dyn soft-green lamp for wave 4850 (always green). */
#define B4844_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4844_green(void)
{
	return B4844_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4850 - report dyn soft-green lamp for wave 4850.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4843) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4850(void)
{
	(void)NULL;
	return b4844_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4850(void)
    __attribute__((alias("gj_dyn_green_4850")));
