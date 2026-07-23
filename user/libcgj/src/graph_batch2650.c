/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2650: milestone 2650 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2650(void);
 *     - Returns the compile-time graph batch number for this TU (2650).
 *   uint32_t __gj_batch_id_2650  (alias)
 *   __libcgj_batch2650_marker = "libcgj-batch2650"
 *
 * Milestone for the exclusive overflow-safe mid/avg/lerp wave
 * (batches 2641-2649: u8/u16/u32/u64 avg_u, u8/u16/u32/u64 mid_u,
 * u64_lerp_q16_u). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2650_marker[] = "libcgj-batch2650";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2650_id(void)
{
	return 2650u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2650 — report this TU's graph batch number.
 *
 * Always returns 2650. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2650(void)
{
	(void)NULL;
	return b2650_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2650(void)
    __attribute__((alias("gj_batch_id_2650")));
