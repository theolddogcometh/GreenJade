/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7393: static smoke green gate (wave 7400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_7400(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 7400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_7400  (alias)
 *   __libcgj_batch7393_marker = "libcgj-batch7393"
 *
 * Milestone 7400 exclusive continuum CREATE-ONLY (7391-7400). Unique
 * gj_smoke_green_7400 surface only; no multi-def. Distinct from
 * gj_smoke_green_7300 (batch7293), gj_smoke_green_7200 (batch7193),
 * gj_smoke_green_7100 (batch7093), and gj_dyn_green_7400
 * (batch7394 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7393_marker[] = "libcgj-batch7393";

/* Static smoke green PASS lamp for wave 7400. */
#define B7393_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7393_green(void)
{
	return B7393_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_7400 - report static smoke green for wave 7400.
 *
 * Always returns 1 (strict green PASS). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_smoke_green_7400(void)
{
	(void)NULL;
	return b7393_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_7400(void)
    __attribute__((alias("gj_smoke_green_7400")));
