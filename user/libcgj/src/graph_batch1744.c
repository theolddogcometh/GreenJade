/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1744: io.weight clamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_io_weight_clamp(uint32_t weight);
 *     — Clamp a cgroup v2 io.weight into [1, 10000]. Below 1 → 1;
 *       above 10000 → 10000. Default is 100.
 *   uint32_t __gj_io_weight_clamp  (alias)
 *   __libcgj_batch1744_marker = "libcgj-batch1744"
 *
 * Distinct from gj_cgroup_weight_clamp (batch1741) — separate public
 * name for the IO controller path. Does NOT redefine prior batch
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1744_marker[] = "libcgj-batch1744";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1744_clamp(uint32_t u32W)
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
 * gj_io_weight_clamp — clamp weight into the 1..10000 io.weight range.
 *
 * Pure sanitizer for blkio/io controller weight fields. No device
 * handles, no cgroupfs.
 */
uint32_t
gj_io_weight_clamp(uint32_t u32Weight)
{
	return b1744_clamp(u32Weight);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_io_weight_clamp(uint32_t u32Weight)
    __attribute__((alias("gj_io_weight_clamp")));
