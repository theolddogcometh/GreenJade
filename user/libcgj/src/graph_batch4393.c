/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4393: static smoke green gate (wave 4400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_4400(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 4400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_4400  (alias)
 *   __libcgj_batch4393_marker = "libcgj-batch4393"
 *
 * Milestone 4400 exclusive continuum CREATE-ONLY (4391-4400). Unique
 * gj_smoke_green_4400 surface only; no multi-def. Distinct from
 * gj_smoke_green_4300 (batch4293), gj_smoke_green_4100 (batch4093),
 * gj_smoke_green_3950 (batch3943), and gj_dyn_green_4400
 * (batch4394 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4393_marker[] = "libcgj-batch4393";

/* Smoke green lamp for wave 4400 (always PASS). */
#define B4393_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4393_green(void)
{
	return B4393_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_4400 - report smoke green lamp for wave 4400.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_4400
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_4400(void)
{
	(void)NULL;
	return b4393_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_4400(void)
    __attribute__((alias("gj_smoke_green_4400")));
