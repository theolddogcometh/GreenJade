/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5143: static smoke green gate (wave 5150).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_5150(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 5150
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_5150  (alias)
 *   __libcgj_batch5143_marker = "libcgj-batch5143"
 *
 * Milestone 5150 exclusive continuum CREATE-ONLY (5141-5150). Unique
 * gj_smoke_green_5150 surface only; no multi-def. Distinct from
 * gj_smoke_green_5100 (batch5093), gj_smoke_green_5000 (batch4993),
 * gj_smoke_green_4900 (batch4893), and gj_dyn_green_5150
 * (batch5144 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5143_marker[] = "libcgj-batch5143";

/* Smoke green lamp for wave 5150 (always PASS). */
#define B5143_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5143_green(void)
{
	return B5143_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_5150 - report smoke green lamp for wave 5150.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_5150
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_5150(void)
{
	(void)NULL;
	return b5143_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_5150(void)
    __attribute__((alias("gj_smoke_green_5150")));
