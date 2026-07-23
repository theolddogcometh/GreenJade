/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7194: dyn smoke soft-green gate (wave 7200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_7200(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 7200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_7200  (alias)
 *   __libcgj_batch7194_marker = "libcgj-batch7194"
 *
 * Milestone 7200 exclusive continuum CREATE-ONLY (7191-7200). Unique
 * gj_dyn_green_7200 surface only; no multi-def. Distinct from
 * gj_dyn_green_7100 (batch7094), gj_dyn_green_7000 (batch6994),
 * gj_dyn_green_6000 (batch5994), and gj_smoke_green_7200
 * (batch7193 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7194_marker[] = "libcgj-batch7194";

/* Dyn soft-green lamp for wave 7200 (always green). */
#define B7194_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7194_green(void)
{
	return B7194_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_7200 - report dyn soft-green lamp for wave 7200.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch7193) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_7200(void)
{
	(void)NULL;
	return b7194_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_7200(void)
    __attribute__((alias("gj_dyn_green_7200")));
