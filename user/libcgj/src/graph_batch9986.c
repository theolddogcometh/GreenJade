/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9986: static smoke soft gate (wave 10000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_10000(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 10000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_10000  (alias)
 *   __libcgj_batch9986_marker = "libcgj-batch9986"
 *
 * Milestone 10000 exclusive continuum CREATE-ONLY (9981-9990). Unique
 * gj_smoke_soft_10000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_9900 (batch9896), gj_smoke_soft_9800 (batch9796),
 * gj_smoke_soft_9700 (batch9696), and gj_dyn_soft_10000
 * (batch9987 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9986_marker[] = "libcgj-batch9986";

/* Smoke soft lamp for wave 10000 (always PASS). */
#define B9986_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9986_soft(void)
{
	return B9986_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_10000 - report smoke soft lamp for wave 10000.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_10000 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_10000(void)
{
	(void)NULL;
	return b9986_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_10000(void)
    __attribute__((alias("gj_smoke_soft_10000")));
