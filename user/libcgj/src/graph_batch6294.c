/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6294: dyn smoke soft-green gate (wave 6300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_6300(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 6300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_6300  (alias)
 *   __libcgj_batch6294_marker = "libcgj-batch6294"
 *
 * Milestone 6300 exclusive continuum CREATE-ONLY (6291-6300). Unique
 * gj_dyn_green_6300 surface only; no multi-def. Distinct from
 * gj_dyn_green_6200 (batch6194), gj_dyn_green_6100 (batch6094),
 * gj_dyn_green_6000 (batch5994), and gj_smoke_green_6300
 * (batch6293 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6294_marker[] = "libcgj-batch6294";

/* Dyn soft-green lamp for wave 6300 (always green). */
#define B6294_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6294_green(void)
{
	return B6294_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_6300 - report dyn soft-green lamp for wave 6300.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch6293) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_6300(void)
{
	(void)NULL;
	return b6294_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_6300(void)
    __attribute__((alias("gj_dyn_green_6300")));
