/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6185: cgroup v2 weight clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cg_weight_clamp_6185(uint32_t weight);
 *     - Clamp a cgroup v2-style weight into [1, 10000]
 *       (cpu.weight / io.weight range). Below 1 → 1; above 10000 → 10000.
 *   uint32_t __gj_cg_weight_clamp_6185  (alias)
 *   __libcgj_batch6185_marker = "libcgj-batch6185"
 *
 * Exclusive continuum CREATE-ONLY (6181-6190: container runtime cgroup
 * stubs). Unique gj_cg_weight_clamp_6185 surface only; no multi-def.
 * Distinct from gj_cgroup_weight_clamp (batch1741). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6185_marker[] = "libcgj-batch6185";

#define B6185_WEIGHT_MIN  1u
#define B6185_WEIGHT_MAX  10000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6185_clamp(uint32_t uWeight)
{
	if (uWeight < B6185_WEIGHT_MIN) {
		return B6185_WEIGHT_MIN;
	}
	if (uWeight > B6185_WEIGHT_MAX) {
		return B6185_WEIGHT_MAX;
	}
	return uWeight;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cg_weight_clamp_6185 - clamp weight into the 1..10000 cgroup range.
 *
 * Suitable for sanitizing parsed cpu.weight / io.weight values before
 * container runtime policy math. No cgroupfs or kernfs access.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_cg_weight_clamp_6185(uint32_t uWeight)
{
	(void)NULL;
	return b6185_clamp(uWeight);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cg_weight_clamp_6185(uint32_t uWeight)
    __attribute__((alias("gj_cg_weight_clamp_6185")));
