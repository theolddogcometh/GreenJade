/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8296: static smoke soft gate (wave 8300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_8300(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 8300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_8300  (alias)
 *   __libcgj_batch8296_marker = "libcgj-batch8296"
 *
 * Milestone 8300 exclusive continuum CREATE-ONLY (8291-8300). Unique
 * gj_smoke_soft_8300 surface only; no multi-def. Distinct from
 * gj_smoke_green_8200 (batch8193), gj_smoke_green_8100 (batch8093),
 * gj_smoke_soft_skip_p (batch2198), and gj_dyn_soft_8300
 * (batch8297 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8296_marker[] = "libcgj-batch8296";

/* Smoke soft lamp for wave 8300 (always PASS). */
#define B8296_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8296_soft(void)
{
	return B8296_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_8300 - report smoke soft lamp for wave 8300.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_8300 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_8300(void)
{
	(void)NULL;
	return b8296_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_8300(void)
    __attribute__((alias("gj_smoke_soft_8300")));
