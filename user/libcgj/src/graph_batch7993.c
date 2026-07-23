/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7993: static smoke green gate (wave 8000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_8000(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 8000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_8000  (alias)
 *   __libcgj_batch7993_marker = "libcgj-batch7993"
 *
 * Milestone 8000 exclusive continuum CREATE-ONLY (7991-8000). Unique
 * gj_smoke_green_8000 surface only; no multi-def. Distinct from
 * gj_smoke_green_7000 (batch6993), gj_smoke_green_6000 (batch5993),
 * gj_smoke_green_5900 (batch5893), and gj_dyn_green_8000
 * (batch7994 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7993_marker[] = "libcgj-batch7993";

/* Smoke green lamp for wave 8000 (always PASS). */
#define B7993_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7993_green(void)
{
	return B7993_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_8000 - report smoke green lamp for wave 8000.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_8000
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_8000(void)
{
	(void)NULL;
	return b7993_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_8000(void)
    __attribute__((alias("gj_smoke_green_8000")));
