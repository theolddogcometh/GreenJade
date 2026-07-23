/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5494: dyn smoke soft-green gate (wave 5500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_5500(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 5500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_5500  (alias)
 *   __libcgj_batch5494_marker = "libcgj-batch5494"
 *
 * Milestone 5500 exclusive continuum CREATE-ONLY (5491-5500). Unique
 * gj_dyn_green_5500 surface only; no multi-def. Distinct from
 * gj_dyn_green_5450 (batch5444), gj_dyn_green_5400 (batch5394),
 * gj_dyn_green_5350 (batch5344), and gj_smoke_green_5500
 * (batch5493 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5494_marker[] = "libcgj-batch5494";

/* Dyn soft-green lamp for wave 5500 (always green). */
#define B5494_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5494_green(void)
{
	return B5494_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_5500 - report dyn soft-green lamp for wave 5500.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch5493) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_5500(void)
{
	(void)NULL;
	return b5494_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_5500(void)
    __attribute__((alias("gj_dyn_green_5500")));
