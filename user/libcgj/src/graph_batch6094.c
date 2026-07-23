/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6094: dyn smoke soft-green gate (wave 6100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_6100(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 6100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_6100  (alias)
 *   __libcgj_batch6094_marker = "libcgj-batch6094"
 *
 * Milestone 6100 exclusive continuum CREATE-ONLY (6091-6100). Unique
 * gj_dyn_green_6100 surface only; no multi-def. Distinct from
 * gj_dyn_green_6000 (batch5994), gj_dyn_green_5900 (batch5894),
 * gj_dyn_green_5800 (batch5794), and gj_smoke_green_6100
 * (batch6093 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6094_marker[] = "libcgj-batch6094";

/* Dyn soft-green lamp for wave 6100. */
#define B6094_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6094_green(void)
{
	return B6094_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_6100 - report dyn soft-green lamp for wave 6100.
 *
 * Always returns 1 (dyn-smoke soft-green). Pair with gj_smoke_green_6100
 * for the exact PASS counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_6100(void)
{
	(void)NULL;
	return b6094_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_6100(void)
    __attribute__((alias("gj_dyn_green_6100")));
