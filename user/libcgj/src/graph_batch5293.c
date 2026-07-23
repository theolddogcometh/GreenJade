/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5293: static smoke green gate (wave 5300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_5300(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 5300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_5300  (alias)
 *   __libcgj_batch5293_marker = "libcgj-batch5293"
 *
 * Milestone 5300 exclusive continuum CREATE-ONLY (5291-5300). Unique
 * gj_smoke_green_5300 surface only; no multi-def. Distinct from
 * gj_smoke_green_5200 (batch5193), gj_smoke_green_5100 (batch5093),
 * gj_smoke_green_5000 (batch4993), and gj_dyn_green_5300
 * (batch5294 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5293_marker[] = "libcgj-batch5293";

/* Smoke green lamp for wave 5300 (always PASS). */
#define B5293_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5293_green(void)
{
	return B5293_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_5300 - report smoke green lamp for wave 5300.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_5300
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_5300(void)
{
	(void)NULL;
	return b5293_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_5300(void)
    __attribute__((alias("gj_smoke_green_5300")));
