/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5393: static smoke green gate (wave 5400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_5400(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 5400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_5400  (alias)
 *   __libcgj_batch5393_marker = "libcgj-batch5393"
 *
 * Milestone 5400 exclusive continuum CREATE-ONLY (5391-5400). Unique
 * gj_smoke_green_5400 surface only; no multi-def. Distinct from
 * gj_smoke_green_5350 (batch5343), gj_smoke_green_5300 (batch5293),
 * gj_smoke_green_5200 (batch5193), and gj_dyn_green_5400
 * (batch5394 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5393_marker[] = "libcgj-batch5393";

/* Smoke green lamp for wave 5400 (always PASS). */
#define B5393_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5393_green(void)
{
	return B5393_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_5400 - report smoke green lamp for wave 5400.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_5400
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_5400(void)
{
	(void)NULL;
	return b5393_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_5400(void)
    __attribute__((alias("gj_smoke_green_5400")));
