/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3810: batch identity for wave 3801-3810.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3810(void);
 *     - Returns the compile-time graph batch number for this TU (3810).
 *       Milestone/identity for the exclusive continuum CREATE-ONLY
 *       series (batches 3801-3809: u32_clip_u / u64_clip_u /
 *       u32_wrap_add_u / u64_wrap_add_u / u32_wrap_sub_u /
 *       u64_wrap_sub_u / u32_wrap_mul_u / u64_wrap_mul_u /
 *       u32_is_zero_u).
 *   uint32_t __gj_batch_id_3810  (alias)
 *   __libcgj_batch3810_marker = "libcgj-batch3810"
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3810_marker[] = "libcgj-batch3810";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3810_id(void)
{
	return 3810u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3810 — report this TU's graph batch number.
 *
 * Always returns 3810. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3810(void)
{
	(void)NULL;
	return b3810_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3810(void)
    __attribute__((alias("gj_batch_id_3810")));
