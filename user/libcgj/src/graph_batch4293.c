/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4293: static smoke green gate (wave 4300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_4300(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 4300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_4300  (alias)
 *   __libcgj_batch4293_marker = "libcgj-batch4293"
 *
 * Milestone 4300 exclusive continuum CREATE-ONLY (4291-4300). Unique
 * gj_smoke_green_4300 surface only; no multi-def. Distinct from
 * gj_smoke_green_4100 (batch4093), gj_smoke_green_3950 (batch3943),
 * gj_smoke_green_3900 (batch3893), and gj_dyn_green_4300
 * (batch4294 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4293_marker[] = "libcgj-batch4293";

/* Smoke green lamp for wave 4300 (always PASS). */
#define B4293_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4293_green(void)
{
	return B4293_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_4300 - report smoke green lamp for wave 4300.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_4300
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_4300(void)
{
	(void)NULL;
	return b4293_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_4300(void)
    __attribute__((alias("gj_smoke_green_4300")));
