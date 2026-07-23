/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4110: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4110(void);
 *     - Returns the compile-time graph batch number for this TU (4110).
 *   uint32_t __gj_batch_id_4110  (alias)
 *   __libcgj_batch4110_marker = "libcgj-batch4110"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110: u32_saturate_add_u,
 * u32_saturate_sub_u, u64_saturate_add_u, u64_saturate_sub_u,
 * u32_absdiff_u2, u64_absdiff_u3, u32_avg_floor_u2, u64_avg_floor_u2,
 * u32_avg_ceil_u2, batch_id_4110).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4110_marker[] = "libcgj-batch4110";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4110_id(void)
{
	return 4110u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4110 - report this TU's graph batch number.
 *
 * Always returns 4110. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4110(void)
{
	(void)NULL;
	return b4110_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4110(void)
    __attribute__((alias("gj_batch_id_4110")));
