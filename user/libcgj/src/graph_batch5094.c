/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5094: dyn smoke soft-green gate (wave 5100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_5100(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 5100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_5100  (alias)
 *   __libcgj_batch5094_marker = "libcgj-batch5094"
 *
 * Milestone 5100 exclusive continuum CREATE-ONLY (5091-5100). Unique
 * gj_dyn_green_5100 surface only; no multi-def. Distinct from
 * gj_dyn_green_5000 (batch4994), gj_dyn_green_4900 (batch4894),
 * gj_dyn_green_4850 (batch4844), and gj_smoke_green_5100
 * (batch5093 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5094_marker[] = "libcgj-batch5094";

/* Dyn soft-green lamp for wave 5100 (always green). */
#define B5094_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5094_green(void)
{
	return B5094_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_5100 - report dyn soft-green lamp for wave 5100.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch5093) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_5100(void)
{
	(void)NULL;
	return b5094_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_5100(void)
    __attribute__((alias("gj_dyn_green_5100")));
