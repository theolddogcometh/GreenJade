/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3110: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3110(void);
 *     - Returns the compile-time graph batch number for this TU (3110).
 *   uint32_t __gj_batch_id_3110  (alias)
 *   __libcgj_batch3110_marker = "libcgj-batch3110"
 *
 * Milestone for the exclusive continuum CREATE-ONLY wave
 * (batches 3101-3109: u32_cmov_u, u64_cmov_u, u32_select_u, u64_select_u,
 * u32_eq_mask_u, u64_eq_mask_u, u32_lt_mask_u, u64_lt_mask_u,
 * u32_gt_mask_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3110_marker[] = "libcgj-batch3110";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3110_id(void)
{
	return 3110u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3110 - report this TU's graph batch number.
 *
 * Always returns 3110. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3110(void)
{
	(void)NULL;
	return b3110_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3110(void)
    __attribute__((alias("gj_batch_id_3110")));
