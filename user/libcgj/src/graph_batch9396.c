/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9396: static smoke soft gate (wave 9400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_9400(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 9400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_9400  (alias)
 *   __libcgj_batch9396_marker = "libcgj-batch9396"
 *
 * Milestone 9400 exclusive continuum CREATE-ONLY (9391-9400). Unique
 * gj_smoke_soft_9400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_9300 (batch9296), gj_smoke_soft_9200 (batch9196),
 * gj_smoke_soft_9100 (batch9096), and gj_dyn_soft_9400
 * (batch9397 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9396_marker[] = "libcgj-batch9396";

/* Smoke soft lamp for wave 9400 (always PASS). */
#define B9396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9396_soft(void)
{
	return B9396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_9400 - report smoke soft lamp for wave 9400.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_9400 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_9400(void)
{
	(void)NULL;
	return b9396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_9400(void)
    __attribute__((alias("gj_smoke_soft_9400")));
