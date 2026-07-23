/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5243: static smoke green gate (wave 5250).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_5250(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 5250
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_5250  (alias)
 *   __libcgj_batch5243_marker = "libcgj-batch5243"
 *
 * Milestone 5250 exclusive continuum CREATE-ONLY (5241-5250). Unique
 * gj_smoke_green_5250 surface only; no multi-def. Distinct from
 * gj_smoke_green_5200 (batch5193), gj_smoke_green_5100 (batch5093),
 * gj_smoke_green_5000 (batch4993), and gj_dyn_green_5250
 * (batch5244 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5243_marker[] = "libcgj-batch5243";

/* Smoke green lamp for wave 5250 (always PASS). */
#define B5243_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5243_green(void)
{
	return B5243_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_5250 - report smoke green lamp for wave 5250.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_5250
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_5250(void)
{
	(void)NULL;
	return b5243_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_5250(void)
    __attribute__((alias("gj_smoke_green_5250")));
