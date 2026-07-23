/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1745: pids.max clamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pids_max_clamp(uint32_t max);
 *     — Clamp a pids controller budget into [1, 2147483647]
 *       (positive pid count up to INT32_MAX). Below 1 → 1; above
 *       2147483647 → 2147483647. Unlimited/"max" encodings are out of
 *       scope for this clamp surface.
 *   uint32_t __gj_pids_max_clamp  (alias)
 *   __libcgj_batch1745_marker = "libcgj-batch1745"
 *
 * Pure data — not a pids.max writer. Does NOT redefine prior batch
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1745_marker[] = "libcgj-batch1745";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1745_clamp(uint32_t u32Max)
{
	if (u32Max < 1u) {
		return 1u;
	}
	if (u32Max > 2147483647u) {
		return 2147483647u;
	}
	return u32Max;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pids_max_clamp — clamp a pids.max-style budget into 1..INT32_MAX.
 *
 * Suitable for sanitizing parsed process-limit fields before policy
 * math. No task_struct, no cgroupfs.
 */
uint32_t
gj_pids_max_clamp(uint32_t u32Max)
{
	return b1745_clamp(u32Max);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pids_max_clamp(uint32_t u32Max)
    __attribute__((alias("gj_pids_max_clamp")));
