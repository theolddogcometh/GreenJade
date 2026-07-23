/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7394: dyn smoke soft-green gate (wave 7400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_7400(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 7400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_7400  (alias)
 *   __libcgj_batch7394_marker = "libcgj-batch7394"
 *
 * Milestone 7400 exclusive continuum CREATE-ONLY (7391-7400). Unique
 * gj_dyn_green_7400 surface only; no multi-def. Distinct from
 * gj_dyn_green_7300 (batch7294), gj_dyn_green_7200 (batch7194),
 * gj_dyn_green_7100 (batch7094), and gj_smoke_green_7400
 * (batch7393 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7394_marker[] = "libcgj-batch7394";

/* Dyn-smoke soft-green lamp for wave 7400. */
#define B7394_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7394_green(void)
{
	return B7394_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_7400 - report dyn-smoke soft-green for wave 7400.
 *
 * Always returns 1 (dyn soft-green). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_dyn_green_7400(void)
{
	(void)NULL;
	return b7394_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_7400(void)
    __attribute__((alias("gj_dyn_green_7400")));
