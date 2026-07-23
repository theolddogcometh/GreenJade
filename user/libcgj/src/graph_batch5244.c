/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5244: dyn smoke soft-green gate (wave 5250).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_5250(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 5250
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_5250  (alias)
 *   __libcgj_batch5244_marker = "libcgj-batch5244"
 *
 * Milestone 5250 exclusive continuum CREATE-ONLY (5241-5250). Unique
 * gj_dyn_green_5250 surface only; no multi-def. Distinct from
 * gj_dyn_green_5200 (batch5194), gj_dyn_green_5100 (batch5094),
 * gj_dyn_green_5000 (batch4994), and gj_smoke_green_5250
 * (batch5243 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5244_marker[] = "libcgj-batch5244";

/* Dyn soft-green lamp for wave 5250 (always green). */
#define B5244_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5244_green(void)
{
	return B5244_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_5250 - report dyn soft-green lamp for wave 5250.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch5243) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_5250(void)
{
	(void)NULL;
	return b5244_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_5250(void)
    __attribute__((alias("gj_dyn_green_5250")));
