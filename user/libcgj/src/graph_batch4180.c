/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4180: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4180(void);
 *     - Returns the compile-time graph batch number for this TU (4180).
 *   uint32_t __gj_batch_id_4180  (alias)
 *   __libcgj_batch4180_marker = "libcgj-batch4180"
 *
 * Exclusive continuum CREATE-ONLY (4171-4180: u32_arr_sum_u,
 * u32_arr_xor_u, u32_arr_or_u, u32_arr_and_u, u32_arr_min_u,
 * u32_arr_max_u, u32_arr_is_sorted_u, u32_arr_count_eq_u,
 * u32_arr_find_u, batch_id_4180). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4180_marker[] = "libcgj-batch4180";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4180_id(void)
{
	return 4180u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4180 - report this TU's graph batch number.
 *
 * Always returns 4180. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4180(void)
{
	(void)NULL;
	return b4180_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4180(void)
    __attribute__((alias("gj_batch_id_4180")));
