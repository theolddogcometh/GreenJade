/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6394: dyn smoke soft-green gate (wave 6400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_6400(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 6400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_6400  (alias)
 *   __libcgj_batch6394_marker = "libcgj-batch6394"
 *
 * Milestone 6400 exclusive continuum CREATE-ONLY (6391-6400). Unique
 * gj_dyn_green_6400 surface only; no multi-def. Distinct from
 * gj_dyn_green_6300 (batch6294), gj_dyn_green_6200 (batch6194),
 * gj_dyn_green_6100 (batch6094), and gj_smoke_green_6400
 * (batch6393 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6394_marker[] = "libcgj-batch6394";

/* Dyn soft-green lamp for wave 6400 (always green). */
#define B6394_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6394_green(void)
{
	return B6394_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_6400 - report dyn-smoke soft-green lamp for wave 6400.
 *
 * Always returns 1 (dyn soft-green). Soft compile-time product status
 * tag; not a runtime probe. Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_6400(void)
{
	(void)NULL;
	return b6394_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_6400(void)
    __attribute__((alias("gj_dyn_green_6400")));
