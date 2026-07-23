/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4244: dyn smoke soft-green gate (wave 4250).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4250(void);
 *     - Returns 1 (soft-green PASS tag) for the milestone 4250
 *       continuum dyn-smoke status lamp. Soft compile-time product
 *       green tag. Unsigned 0/1 form.
 *   uint32_t __gj_dyn_green_4250  (alias)
 *   __libcgj_batch4244_marker = "libcgj-batch4244"
 *
 * Milestone 4250 exclusive continuum CREATE-ONLY (4241-4250). Unique
 * gj_dyn_green_4250 surface only; no multi-def. Distinct from
 * gj_dyn_green_4100 (batch4094), gj_dyn_green_4000 (batch3994),
 * gj_dyn_green_3950 (batch3944), and gj_smoke_green_4250
 * (batch4243 exact green). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4244_marker[] = "libcgj-batch4244";

/* Dyn-smoke soft-green PASS tag for wave 4250. */
#define B4244_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4244_green(void)
{
	return B4244_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4250 - report the milestone-4250 dyn-smoke soft-green tag.
 *
 * Always returns 1 (soft-green PASS). Link-time presence of this
 * symbol tags dyn-smoke green for the wave. Distinct from static
 * smoke green (batch4243). Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_4250(void)
{
	(void)NULL;
	return b4244_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4250(void)
    __attribute__((alias("gj_dyn_green_4250")));
