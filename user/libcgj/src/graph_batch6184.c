/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6184: cgroup pids headroom stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cg_pids_headroom_6184(uint32_t current, uint32_t max);
 *     - Return remaining process slots under a pids controller budget:
 *       max - current when max >= current, else 0 (over-budget).
 *       max == 0 is treated as no headroom (unlimited encodings out of
 *       scope for this surface).
 *   uint32_t __gj_cg_pids_headroom_6184  (alias)
 *   __libcgj_batch6184_marker = "libcgj-batch6184"
 *
 * Exclusive continuum CREATE-ONLY (6181-6190: container runtime cgroup
 * stubs). Unique gj_cg_pids_headroom_6184 surface only; no multi-def.
 * Distinct from gj_pids_max_clamp (batch1745). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6184_marker[] = "libcgj-batch6184";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6184_headroom(uint32_t uCurrent, uint32_t uMax)
{
	if (uMax == 0u) {
		return 0u;
	}
	if (uCurrent >= uMax) {
		return 0u;
	}
	return uMax - uCurrent;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cg_pids_headroom_6184 - remaining pids.max capacity for a cgroup.
 *
 * current: pids.current-style count.
 * max:     pids.max-style hard budget (0 → treat as no headroom here).
 * Pure arithmetic for container runtime admission checks. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_cg_pids_headroom_6184(uint32_t uCurrent, uint32_t uMax)
{
	(void)NULL;
	return b6184_headroom(uCurrent, uMax);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cg_pids_headroom_6184(uint32_t uCurrent, uint32_t uMax)
    __attribute__((alias("gj_cg_pids_headroom_6184")));
