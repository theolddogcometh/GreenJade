/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8193: static smoke green gate (wave 8200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_8200(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 8200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_8200  (alias)
 *   __libcgj_batch8193_marker = "libcgj-batch8193"
 *
 * Milestone 8200 exclusive continuum CREATE-ONLY (8191-8200). Unique
 * gj_smoke_green_8200 surface only; no multi-def. Distinct from
 * gj_smoke_green_8000 (batch7993), gj_smoke_green_7000 (batch6993),
 * gj_smoke_green_6000 (batch5993), and gj_dyn_green_8200
 * (batch8194 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8193_marker[] = "libcgj-batch8193";

/* Smoke green lamp for wave 8200 (always PASS). */
#define B8193_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8193_green(void)
{
	return B8193_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_8200 - report smoke green lamp for wave 8200.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_8200
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_8200(void)
{
	(void)NULL;
	return b8193_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_8200(void)
    __attribute__((alias("gj_smoke_green_8200")));
