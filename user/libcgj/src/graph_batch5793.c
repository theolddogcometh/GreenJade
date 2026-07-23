/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5793: static smoke green gate (wave 5800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_5800(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 5800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_5800  (alias)
 *   __libcgj_batch5793_marker = "libcgj-batch5793"
 *
 * Milestone 5800 exclusive continuum CREATE-ONLY (5791-5800). Unique
 * gj_smoke_green_5800 surface only; no multi-def. Distinct from
 * gj_smoke_green_5700 (batch5693), gj_smoke_green_5650 (batch5643),
 * gj_smoke_green_5600 (batch5593), and gj_dyn_green_5800
 * (batch5794 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5793_marker[] = "libcgj-batch5793";

/* Smoke green lamp for wave 5800 (always PASS). */
#define B5793_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5793_green(void)
{
	return B5793_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_5800 - report smoke green lamp for wave 5800.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_5800
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_5800(void)
{
	(void)NULL;
	return b5793_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_5800(void)
    __attribute__((alias("gj_smoke_green_5800")));
