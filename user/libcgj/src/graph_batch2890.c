/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2890: milestone 2890 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2890(void);
 *     - Returns the compile-time graph batch number for this TU (2890).
 *   uint32_t __gj_batch_id_2890  (alias)
 *   __libcgj_batch2890_marker = "libcgj-batch2890"
 *
 * Milestone for the exclusive sort/search helpers wave (batches
 * 2881-2889: arr_u64_swap_u, qsort_u32_partition_u, binsearch_u32_u,
 * binsearch_u64_u, lower_bound_u32_u, upper_bound_u32_u,
 * is_sorted_u32_u, is_sorted_u64_u, unique_u32_inplace_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2890_marker[] = "libcgj-batch2890";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2890_id(void)
{
	return 2890u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2890 - report this TU's graph batch number.
 *
 * Always returns 2890. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2890(void)
{
	(void)NULL;
	return b2890_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2890(void)
    __attribute__((alias("gj_batch_id_2890")));
