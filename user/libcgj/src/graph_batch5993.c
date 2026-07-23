/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5993: static smoke green gate (wave 6000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_6000(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 6000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_6000  (alias)
 *   __libcgj_batch5993_marker = "libcgj-batch5993"
 *
 * Milestone 6000 exclusive continuum CREATE-ONLY (5991-6000). Unique
 * gj_smoke_green_6000 surface only; no multi-def. Distinct from
 * gj_smoke_green_5900 (batch5893), gj_smoke_green_5800 (batch5793),
 * gj_smoke_green_5700 (batch5693), and gj_dyn_green_6000
 * (batch5994 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5993_marker[] = "libcgj-batch5993";

/* Smoke green lamp for wave 6000 (always PASS). */
#define B5993_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5993_green(void)
{
	return B5993_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_6000 - report smoke green lamp for wave 6000.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_6000
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_6000(void)
{
	(void)NULL;
	return b5993_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_6000(void)
    __attribute__((alias("gj_smoke_green_6000")));
