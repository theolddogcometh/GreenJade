/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4020: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4020(void);
 *     - Returns the compile-time graph batch number for this TU (4020).
 *   uint32_t __gj_batch_id_4020  (alias)
 *   __libcgj_batch4020_marker = "libcgj-batch4020"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020: u64_toggle_msb_u,
 * u32_mask_below_u, u64_mask_below_u, u32_mask_above_u,
 * u64_mask_above_u, u32_isolate_msb_u, u64_isolate_msb_u,
 * u32_isolate_lsb_u, u64_isolate_lsb_u, batch_id_4020).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4020_marker[] = "libcgj-batch4020";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4020_id(void)
{
	return 4020u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4020 - report this TU's graph batch number.
 *
 * Always returns 4020. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4020(void)
{
	(void)NULL;
	return b4020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4020(void)
    __attribute__((alias("gj_batch_id_4020")));
