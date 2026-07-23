/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9296: static smoke soft gate (wave 9300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_9300(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 9300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_9300  (alias)
 *   __libcgj_batch9296_marker = "libcgj-batch9296"
 *
 * Milestone 9300 exclusive continuum CREATE-ONLY (9291-9300). Unique
 * gj_smoke_soft_9300 surface only; no multi-def. Distinct from
 * gj_smoke_soft_9200 (batch9196), gj_smoke_soft_9100 (batch9096),
 * gj_smoke_soft_9000 (batch8996), and gj_dyn_soft_9300
 * (batch9297 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9296_marker[] = "libcgj-batch9296";

/* Smoke soft lamp for wave 9300 (always PASS). */
#define B9296_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9296_soft(void)
{
	return B9296_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_9300 - report smoke soft lamp for wave 9300.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_9300 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_9300(void)
{
	(void)NULL;
	return b9296_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_9300(void)
    __attribute__((alias("gj_smoke_soft_9300")));
