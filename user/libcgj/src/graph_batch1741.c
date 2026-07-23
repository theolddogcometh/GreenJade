/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1741: cgroup weight clamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cgroup_weight_clamp(uint32_t weight);
 *     — Clamp a cgroup v2-style weight into [1, 10000]
 *       (cpu.weight / io.weight range). Below 1 → 1; above
 *       10000 → 10000.
 *   uint32_t __gj_cgroup_weight_clamp  (alias)
 *   __libcgj_batch1741_marker = "libcgj-batch1741"
 *
 * Pure data — not a cgroupfs writer. Does NOT redefine prior batch
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1741_marker[] = "libcgj-batch1741";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1741_clamp(uint32_t u32W)
{
	if (u32W < 1u) {
		return 1u;
	}
	if (u32W > 10000u) {
		return 10000u;
	}
	return u32W;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cgroup_weight_clamp — clamp weight into the 1..10000 cgroup range.
 *
 * Suitable for sanitizing parsed cpu.weight / io.weight values before
 * policy math. No cgroupfs or kernfs access.
 */
uint32_t
gj_cgroup_weight_clamp(uint32_t u32Weight)
{
	return b1741_clamp(u32Weight);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cgroup_weight_clamp(uint32_t u32Weight)
    __attribute__((alias("gj_cgroup_weight_clamp")));
