/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5109: saturating IRQ nest-depth increment.
 *
 * Surface (unique symbols):
 *   uint32_t gj_irq_nest_inc_u(uint32_t n);
 *     - Return n + 1 with saturation at UINT32_MAX (no wrap).
 *   uint32_t __gj_irq_nest_inc_u  (alias)
 *   __libcgj_batch5109_marker = "libcgj-batch5109"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110). Unique
 * gj_irq_nest_inc_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5109_marker[] = "libcgj-batch5109";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5109_nest_inc(uint32_t u32N)
{
	if (u32N == UINT32_MAX) {
		return UINT32_MAX;
	}
	return u32N + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_irq_nest_inc_u - saturating increment of soft IRQ nest depth.
 *
 * n: current nest depth
 *
 * Returns n + 1, or UINT32_MAX when n is already UINT32_MAX (no wrap).
 * Pure integer; no parent wires.
 */
uint32_t
gj_irq_nest_inc_u(uint32_t u32N)
{
	(void)NULL;
	return b5109_nest_inc(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_irq_nest_inc_u(uint32_t u32N)
    __attribute__((alias("gj_irq_nest_inc_u")));
