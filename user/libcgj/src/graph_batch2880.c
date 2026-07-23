/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2880: milestone 2880 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2880(void);
 *     - Returns the compile-time graph batch number for this TU (2880).
 *   uint32_t __gj_batch_id_2880  (alias)
 *   __libcgj_batch2880_marker = "libcgj-batch2880"
 *
 * Milestone for the array exclusive wave (batches 2871-2879:
 * arr_u64_find_u, arr_u32_count_eq_u, arr_u32_fill_u, arr_u64_fill_u,
 * arr_u32_copy_u, arr_u64_copy_u, arr_u32_rev_u, arr_u64_rev_u,
 * arr_u32_swap_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2880_marker[] = "libcgj-batch2880";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2880_id(void)
{
	return 2880u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2880 - report this TU's graph batch number.
 *
 * Always returns 2880. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2880(void)
{
	(void)NULL;
	return b2880_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2880(void)
    __attribute__((alias("gj_batch_id_2880")));
