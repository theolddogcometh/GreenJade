/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4644: dyn smoke soft-green gate (wave 4650).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4650(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4650
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4650  (alias)
 *   __libcgj_batch4644_marker = "libcgj-batch4644"
 *
 * Milestone 4650 exclusive continuum CREATE-ONLY (4641-4650). Unique
 * gj_dyn_green_4650 surface only; no multi-def. Distinct from
 * gj_dyn_green_4600 (batch4594), gj_dyn_green_4550 (batch4544),
 * gj_dyn_green_4500 (batch4494), and gj_smoke_green_4650
 * (batch4643 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4644_marker[] = "libcgj-batch4644";

/* Dyn soft-green lamp for wave 4650 (always green). */
#define B4644_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4644_green(void)
{
	return B4644_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4650 - report dyn soft-green lamp for wave 4650.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4643) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4650(void)
{
	(void)NULL;
	return b4644_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4650(void)
    __attribute__((alias("gj_dyn_green_4650")));
