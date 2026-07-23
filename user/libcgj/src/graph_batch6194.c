/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6194: dyn smoke soft-green gate (wave 6200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_6200(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 6200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_6200  (alias)
 *   __libcgj_batch6194_marker = "libcgj-batch6194"
 *
 * Milestone 6200 exclusive continuum CREATE-ONLY (6191-6200). Unique
 * gj_dyn_green_6200 surface only; no multi-def. Distinct from
 * gj_dyn_green_6000 (batch5994), gj_dyn_green_5900 (batch5894),
 * gj_dyn_green_5800 (batch5794), and gj_smoke_green_6200
 * (batch6193 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6194_marker[] = "libcgj-batch6194";

/* Dyn soft-green lamp for wave 6200 (always green). */
#define B6194_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6194_green(void)
{
	return B6194_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_6200 - report dyn soft-green lamp for wave 6200.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch6193) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_6200(void)
{
	(void)NULL;
	return b6194_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_6200(void)
    __attribute__((alias("gj_dyn_green_6200")));
