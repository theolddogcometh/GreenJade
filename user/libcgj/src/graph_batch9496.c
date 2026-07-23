/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9496: static smoke soft gate (wave 9500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_9500(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 9500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_9500  (alias)
 *   __libcgj_batch9496_marker = "libcgj-batch9496"
 *
 * Milestone 9500 exclusive continuum CREATE-ONLY (9491-9500). Unique
 * gj_smoke_soft_9500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_9400 (batch9396), gj_smoke_soft_9300 (batch9296),
 * gj_smoke_soft_9200 (batch9196), and gj_dyn_soft_9500
 * (batch9497 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9496_marker[] = "libcgj-batch9496";

/* Smoke soft lamp for wave 9500 (always PASS). */
#define B9496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9496_soft(void)
{
	return B9496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_9500 - report smoke soft lamp for wave 9500.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_9500 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_9500(void)
{
	(void)NULL;
	return b9496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_9500(void)
    __attribute__((alias("gj_smoke_soft_9500")));
