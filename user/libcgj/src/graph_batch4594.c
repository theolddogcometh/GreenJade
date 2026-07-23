/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4594: dyn smoke soft-green gate (wave 4600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4600(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4600  (alias)
 *   __libcgj_batch4594_marker = "libcgj-batch4594"
 *
 * Milestone 4600 exclusive continuum CREATE-ONLY (4591-4600). Unique
 * gj_dyn_green_4600 surface only; no multi-def. Distinct from
 * gj_dyn_green_4500 (batch4494), gj_dyn_green_4400 (batch4394),
 * gj_dyn_green_4300 (batch4294), and gj_smoke_green_4600
 * (batch4593 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4594_marker[] = "libcgj-batch4594";

/* Dyn soft-green lamp for wave 4600 (always green). */
#define B4594_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4594_green(void)
{
	return B4594_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4600 - report dyn soft-green lamp for wave 4600.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4593) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4600(void)
{
	(void)NULL;
	return b4594_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4600(void)
    __attribute__((alias("gj_dyn_green_4600")));
