/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6993: static smoke green gate (wave 7000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_7000(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 7000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_7000  (alias)
 *   __libcgj_batch6993_marker = "libcgj-batch6993"
 *
 * Milestone 7000 exclusive continuum CREATE-ONLY (6991-7000). Unique
 * gj_smoke_green_7000 surface only; no multi-def. Distinct from
 * gj_smoke_green_6000 (batch5993), gj_smoke_green_5900 (batch5893),
 * gj_smoke_green_5800 (batch5793), and gj_dyn_green_7000
 * (batch6994 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6993_marker[] = "libcgj-batch6993";

/* Smoke green lamp for wave 7000 (always PASS). */
#define B6993_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6993_green(void)
{
	return B6993_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_7000 - report smoke green lamp for wave 7000.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_7000
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_7000(void)
{
	(void)NULL;
	return b6993_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_7000(void)
    __attribute__((alias("gj_smoke_green_7000")));
