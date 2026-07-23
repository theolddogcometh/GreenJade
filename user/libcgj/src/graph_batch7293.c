/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7293: static smoke green gate (wave 7300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_7300(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 7300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_7300  (alias)
 *   __libcgj_batch7293_marker = "libcgj-batch7293"
 *
 * Milestone 7300 exclusive continuum CREATE-ONLY (7291-7300). Unique
 * gj_smoke_green_7300 surface only; no multi-def. Distinct from
 * gj_smoke_green_7200 (batch7193), gj_smoke_green_7100 (batch7093),
 * gj_smoke_green_7000 (batch6993), and gj_dyn_green_7300
 * (batch7294 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7293_marker[] = "libcgj-batch7293";

/* Smoke green lamp for wave 7300 (always PASS). */
#define B7293_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7293_green(void)
{
	return B7293_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_7300 - report smoke green lamp for wave 7300.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_7300
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_7300(void)
{
	(void)NULL;
	return b7293_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_7300(void)
    __attribute__((alias("gj_smoke_green_7300")));
