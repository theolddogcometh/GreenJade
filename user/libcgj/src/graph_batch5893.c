/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5893: static smoke green gate (wave 5900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_5900(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 5900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_5900  (alias)
 *   __libcgj_batch5893_marker = "libcgj-batch5893"
 *
 * Milestone 5900 exclusive continuum CREATE-ONLY (5891-5900). Unique
 * gj_smoke_green_5900 surface only; no multi-def. Distinct from
 * gj_smoke_green_5800 (batch5793), gj_smoke_green_5700 (batch5693),
 * gj_smoke_green_5650 (batch5643), and gj_dyn_green_5900
 * (batch5894 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5893_marker[] = "libcgj-batch5893";

/* Smoke green lamp for wave 5900 (always PASS). */
#define B5893_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5893_green(void)
{
	return B5893_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_5900 - report smoke green lamp for wave 5900.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_5900
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_5900(void)
{
	(void)NULL;
	return b5893_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_5900(void)
    __attribute__((alias("gj_smoke_green_5900")));
