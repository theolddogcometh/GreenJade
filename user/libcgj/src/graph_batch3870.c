/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3870: milestone 3870 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3870(void);
 *     - Returns the compile-time graph batch number for this TU (3870).
 *       Milestone for the exclusive continuum CREATE-ONLY series
 *       (batches 3861-3869: u64_mul_sub_u / u32_madd_sat_u /
 *       u64_madd_sat_u / u32_msub_sat_u / u64_msub_sat_u /
 *       u32_abs_to_i32_u / i32_to_u32_sat_u / i64_to_u64_sat_u /
 *       u32_to_i32_sat_u).
 *   uint32_t __gj_batch_id_3870  (alias)
 *   __libcgj_batch3870_marker = "libcgj-batch3870"
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

const char __libcgj_batch3870_marker[] = "libcgj-batch3870";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3870_id(void)
{
	return 3870u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3870 — report this TU's graph batch number.
 *
 * Always returns 3870. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3870(void)
{
	(void)NULL;
	return b3870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3870(void)
    __attribute__((alias("gj_batch_id_3870")));
