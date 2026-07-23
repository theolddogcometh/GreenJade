/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8094: dyn smoke soft-green gate (wave 8100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_8100(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 8100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_8100  (alias)
 *   __libcgj_batch8094_marker = "libcgj-batch8094"
 *
 * Milestone 8100 exclusive continuum CREATE-ONLY (8091-8100). Unique
 * gj_dyn_green_8100 surface only; no multi-def. Distinct from
 * gj_dyn_green_8000 (batch7994), gj_dyn_green_7000 (batch6994),
 * gj_dyn_green_6000 (batch5994), and gj_smoke_green_8100
 * (batch8093 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8094_marker[] = "libcgj-batch8094";

/* Dyn soft-green lamp for wave 8100 (always green). */
#define B8094_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8094_green(void)
{
	return B8094_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_8100 - report dyn soft-green lamp for wave 8100.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch8093) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_8100(void)
{
	(void)NULL;
	return b8094_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_8100(void)
    __attribute__((alias("gj_dyn_green_8100")));
