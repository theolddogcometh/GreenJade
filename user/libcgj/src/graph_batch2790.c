/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2790: milestone 2790 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2790(void);
 *     - Returns the compile-time graph batch number for this TU (2790).
 *   uint32_t __gj_batch_id_2790  (alias)
 *   __libcgj_batch2790_marker = "libcgj-batch2790"
 *
 * Milestone for the integer-root exclusive wave (batches 2781-2789:
 * isqrt_u32_u, isqrt_u64_u, ilog2_u32_u, ilog2_u64_u, u32_midpoint_u,
 * u64_midpoint_u, bytes_to_u32_le_u, bytes_to_u64_le_u,
 * u32_to_bytes_le_u). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2790_marker[] = "libcgj-batch2790";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2790_id(void)
{
	return 2790u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2790 - report this TU's graph batch number.
 *
 * Always returns 2790. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2790(void)
{
	(void)NULL;
	return b2790_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2790(void)
    __attribute__((alias("gj_batch_id_2790")));
