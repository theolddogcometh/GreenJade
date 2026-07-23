/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3270: MILESTONE 3270 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3270(void);
 *     - Returns the compile-time graph batch number for this TU (3270).
 *   uint32_t __gj_batch_id_3270  (alias)
 *   __libcgj_batch3270_marker = "libcgj-batch3270"
 *
 * MILESTONE 3270 for the exclusive continuum CREATE-ONLY wave
 * (batches 3261-3269: u64_scale_q16_u, u32_map_range_u, u64_map_range_u,
 * u32_in_range_u, u64_in_range_u, u32_wrap_range_u, u64_wrap_range_u,
 * u32_pingpong_u, u64_pingpong_u). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols - avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3270_marker[] = "libcgj-batch3270";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3270_id(void)
{
	return 3270u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3270 - report this TU's graph batch number.
 *
 * Always returns 3270. End-of-wave identity for the range/scale exclusive
 * continuum 3261-3270. No parent wires.
 */
uint32_t
gj_batch_id_3270(void)
{
	(void)NULL;
	return b3270_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3270(void)
    __attribute__((alias("gj_batch_id_3270")));
