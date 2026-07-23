/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9696: static smoke soft gate (wave 9700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_9700(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 9700
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_9700  (alias)
 *   __libcgj_batch9696_marker = "libcgj-batch9696"
 *
 * Milestone 9700 exclusive continuum CREATE-ONLY (9691-9700). Unique
 * gj_smoke_soft_9700 surface only; no multi-def. Distinct from
 * gj_smoke_soft_9600 (batch9596), gj_smoke_soft_9500 (batch9496),
 * gj_smoke_soft_9400 (batch9396), and gj_dyn_soft_9700
 * (batch9697 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9696_marker[] = "libcgj-batch9696";

/* Smoke soft lamp for wave 9700 (always PASS). */
#define B9696_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9696_soft(void)
{
	return B9696_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_9700 - report smoke soft lamp for wave 9700.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_9700 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_9700(void)
{
	(void)NULL;
	return b9696_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_9700(void)
    __attribute__((alias("gj_smoke_soft_9700")));
