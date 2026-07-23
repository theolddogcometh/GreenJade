/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5020: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5020(void);
 *     - Returns the compile-time graph batch number for this TU (5020).
 *   uint32_t __gj_batch_id_5020  (alias)
 *   __libcgj_batch5020_marker = "libcgj-batch5020"
 *
 * Exclusive continuum CREATE-ONLY (5011-5020: lane_extract_u32_u,
 * lane_insert_u32_u, lane_shuffle_u32_u, lane_zip_lo_u16_u,
 * lane_zip_hi_u16_u, lane_add_sat_u8x4_u, lane_sub_sat_u8x4_u,
 * lane_avg_u8x4_u, lane_max_u8x4_u, batch_id_5020). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5020_marker[] = "libcgj-batch5020";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5020_id(void)
{
	return 5020u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5020 - report this TU's graph batch number.
 *
 * Always returns 5020. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5020(void)
{
	(void)NULL;
	return b5020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5020(void)
    __attribute__((alias("gj_batch_id_5020")));
