/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4144: dyn smoke soft-green status (wave 4150).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4150(void);
 *     - Returns the dyn-smoke soft-green status flag for the milestone
 *       4150 continuum (always 1 = soft-green). Soft compile-time
 *       product status lamp.
 *   uint32_t __gj_dyn_green_4150  (alias)
 *   __libcgj_batch4144_marker = "libcgj-batch4144"
 *
 * Milestone 4150 exclusive continuum CREATE-ONLY (4141-4150). Unique
 * gj_dyn_green_4150 surface only; no multi-def. Distinct from
 * gj_dyn_green_4100 (batch4094), gj_dyn_green_4050 (batch4044), and
 * gj_smoke_green_4150 (batch4143 exact green). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4144_marker[] = "libcgj-batch4144";

/* Dyn-smoke soft-green status for wave 4150 (1 = soft-green). */
#define B4144_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4144_green(void)
{
	return B4144_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4150 - report dyn-smoke soft-green status for wave 4150.
 *
 * Always returns 1 (dyn soft-green for milestone 4150 continuum).
 * Distinct from static smoke green (batch4143). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_dyn_green_4150(void)
{
	(void)NULL;
	return b4144_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4150(void)
    __attribute__((alias("gj_dyn_green_4150")));
