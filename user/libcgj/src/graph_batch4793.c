/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4793: static smoke green gate (wave 4800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_4800(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 4800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_4800  (alias)
 *   __libcgj_batch4793_marker = "libcgj-batch4793"
 *
 * Milestone 4800 exclusive continuum CREATE-ONLY (4791-4800). Unique
 * gj_smoke_green_4800 surface only; no multi-def. Distinct from
 * gj_smoke_green_4750 (batch4743), gj_smoke_green_4700 (batch4693),
 * gj_smoke_green_4650 (batch4643), and gj_dyn_green_4800
 * (batch4794 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4793_marker[] = "libcgj-batch4793";

/* Smoke green lamp for wave 4800 (always PASS). */
#define B4793_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4793_green(void)
{
	return B4793_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_4800 - report smoke green lamp for wave 4800.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_4800
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_4800(void)
{
	(void)NULL;
	return b4793_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_4800(void)
    __attribute__((alias("gj_smoke_green_4800")));
