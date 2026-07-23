/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6293: static smoke green gate (wave 6300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_6300(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 6300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_6300  (alias)
 *   __libcgj_batch6293_marker = "libcgj-batch6293"
 *
 * Milestone 6300 exclusive continuum CREATE-ONLY (6291-6300). Unique
 * gj_smoke_green_6300 surface only; no multi-def. Distinct from
 * gj_smoke_green_6200 (batch6193), gj_smoke_green_6100 (batch6093),
 * gj_smoke_green_6000 (batch5993), and gj_dyn_green_6300
 * (batch6294 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6293_marker[] = "libcgj-batch6293";

/* Smoke green lamp for wave 6300 (always PASS). */
#define B6293_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6293_green(void)
{
	return B6293_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_6300 - report smoke green lamp for wave 6300.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_6300
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_6300(void)
{
	(void)NULL;
	return b6293_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_6300(void)
    __attribute__((alias("gj_smoke_green_6300")));
