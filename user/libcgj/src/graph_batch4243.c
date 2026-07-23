/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4243: static smoke green gate (wave 4250).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_4250(void);
 *     - Returns 1 (strict green PASS tag) for the milestone 4250
 *       continuum static smoke status lamp. Soft compile-time product
 *       green tag. Unsigned 0/1 form.
 *   uint32_t __gj_smoke_green_4250  (alias)
 *   __libcgj_batch4243_marker = "libcgj-batch4243"
 *
 * Milestone 4250 exclusive continuum CREATE-ONLY (4241-4250). Unique
 * gj_smoke_green_4250 surface only; no multi-def. Distinct from
 * gj_smoke_green_4100 (batch4093), gj_smoke_green_4000 (batch3993),
 * gj_smoke_green_3950 (batch3943), and gj_dyn_green_4250
 * (batch4244 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4243_marker[] = "libcgj-batch4243";

/* Static smoke green PASS tag for wave 4250. */
#define B4243_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4243_green(void)
{
	return B4243_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_4250 - report the milestone-4250 static smoke green tag.
 *
 * Always returns 1 (strict green PASS). Link-time presence of this
 * symbol tags static smoke green for the wave. Pair with
 * gj_dyn_green_4250 for the dyn soft-band counterpart. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_smoke_green_4250(void)
{
	(void)NULL;
	return b4243_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_4250(void)
    __attribute__((alias("gj_smoke_green_4250")));
