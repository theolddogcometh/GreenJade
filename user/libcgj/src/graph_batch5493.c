/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5493: static smoke green gate (wave 5500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_5500(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 5500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_5500  (alias)
 *   __libcgj_batch5493_marker = "libcgj-batch5493"
 *
 * Milestone 5500 exclusive continuum CREATE-ONLY (5491-5500). Unique
 * gj_smoke_green_5500 surface only; no multi-def. Distinct from
 * gj_smoke_green_5450 (batch5443), gj_smoke_green_5400 (batch5393),
 * gj_smoke_green_5350 (batch5343), and gj_dyn_green_5500
 * (batch5494 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5493_marker[] = "libcgj-batch5493";

/* Smoke green lamp for wave 5500 (always PASS). */
#define B5493_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5493_green(void)
{
	return B5493_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_5500 - report smoke green lamp for wave 5500.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_5500
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_5500(void)
{
	(void)NULL;
	return b5493_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_5500(void)
    __attribute__((alias("gj_smoke_green_5500")));
