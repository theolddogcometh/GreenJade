/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2870: milestone 2870 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2870(void);
 *     — Returns the compile-time graph batch number for this TU (2870).
 *   uint32_t __gj_batch_id_2870  (alias)
 *   __libcgj_batch2870_marker = "libcgj-batch2870"
 *
 * Milestone for the exclusive bitmap/array helpers wave (batches
 * 2861-2869: bitmap_xor_u, bitmap_not_u, arr_u32_sum_u, arr_u32_min_u,
 * arr_u32_max_u, arr_u64_sum_u, arr_u64_min_u, arr_u64_max_u,
 * arr_u32_find_u). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2870_marker[] = "libcgj-batch2870";

/* Exclusive-wave milestone id. */
#define B2870_ID  2870u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2870_id(void)
{
	return B2870_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2870 — report this TU's graph batch number.
 *
 * Always returns 2870. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2870(void)
{
	(void)NULL;
	return b2870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2870(void)
    __attribute__((alias("gj_batch_id_2870")));
