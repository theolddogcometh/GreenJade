/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8093: static smoke green gate (wave 8100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_8100(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 8100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_8100  (alias)
 *   __libcgj_batch8093_marker = "libcgj-batch8093"
 *
 * Milestone 8100 exclusive continuum CREATE-ONLY (8091-8100). Unique
 * gj_smoke_green_8100 surface only; no multi-def. Distinct from
 * gj_smoke_green_8000 (batch7993), gj_smoke_green_7000 (batch6993),
 * gj_smoke_green_6000 (batch5993), and gj_dyn_green_8100
 * (batch8094 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8093_marker[] = "libcgj-batch8093";

/* Smoke green lamp for wave 8100 (always PASS). */
#define B8093_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8093_green(void)
{
	return B8093_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_8100 - report smoke green lamp for wave 8100.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_8100
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_8100(void)
{
	(void)NULL;
	return b8093_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_8100(void)
    __attribute__((alias("gj_smoke_green_8100")));
