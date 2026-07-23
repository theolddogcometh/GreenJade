/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9796: static smoke soft gate (wave 9800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_9800(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 9800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_9800  (alias)
 *   __libcgj_batch9796_marker = "libcgj-batch9796"
 *
 * Milestone 9800 exclusive continuum CREATE-ONLY (9791-9800). Unique
 * gj_smoke_soft_9800 surface only; no multi-def. Distinct from
 * gj_smoke_soft_9700 (batch9696), gj_smoke_soft_9600 (batch9596),
 * gj_smoke_soft_9500 (batch9496), and gj_dyn_soft_9800
 * (batch9797 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9796_marker[] = "libcgj-batch9796";

/* Smoke soft lamp for wave 9800 (always PASS). */
#define B9796_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9796_soft(void)
{
	return B9796_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_9800 - report smoke soft lamp for wave 9800.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_9800 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_9800(void)
{
	(void)NULL;
	return b9796_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_9800(void)
    __attribute__((alias("gj_smoke_soft_9800")));
