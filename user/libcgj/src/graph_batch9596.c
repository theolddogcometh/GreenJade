/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9596: static smoke soft gate (wave 9600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_9600(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 9600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_9600  (alias)
 *   __libcgj_batch9596_marker = "libcgj-batch9596"
 *
 * Milestone 9600 exclusive continuum CREATE-ONLY (9591-9600). Unique
 * gj_smoke_soft_9600 surface only; no multi-def. Distinct from
 * gj_smoke_soft_9500 (batch9496), gj_smoke_soft_9400 (batch9396),
 * gj_smoke_soft_9300 (batch9296), and gj_dyn_soft_9600
 * (batch9597 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9596_marker[] = "libcgj-batch9596";

/* Smoke soft lamp for wave 9600 (always PASS). */
#define B9596_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9596_soft(void)
{
	return B9596_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_9600 - report smoke soft lamp for wave 9600.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_9600 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_9600(void)
{
	(void)NULL;
	return b9596_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_9600(void)
    __attribute__((alias("gj_smoke_soft_9600")));
