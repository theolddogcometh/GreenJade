/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7094: dyn smoke soft-green gate (wave 7100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_7100(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 7100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_7100  (alias)
 *   __libcgj_batch7094_marker = "libcgj-batch7094"
 *
 * Milestone 7100 exclusive continuum CREATE-ONLY (7091-7100). Unique
 * gj_dyn_green_7100 surface only; no multi-def. Distinct from
 * gj_dyn_green_7000 (batch6994), gj_dyn_green_6000 (batch5994),
 * gj_dyn_green_5900 (batch5894), and gj_smoke_green_7100
 * (batch7093 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7094_marker[] = "libcgj-batch7094";

/* Dyn soft-green lamp for wave 7100 (always green). */
#define B7094_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7094_green(void)
{
	return B7094_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_7100 - report dyn soft-green lamp for wave 7100.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch7093) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_7100(void)
{
	(void)NULL;
	return b7094_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_7100(void)
    __attribute__((alias("gj_dyn_green_7100")));
