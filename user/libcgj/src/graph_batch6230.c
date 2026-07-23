/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6230: batch identity for VRR / adaptive sync.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6230(void);
 *     - Returns the compile-time graph batch number for this TU (6230).
 *   uint32_t __gj_batch_id_6230  (alias)
 *   __libcgj_batch6230_marker = "libcgj-batch6230"
 *
 * Exclusive continuum CREATE-ONLY (6221-6230: VRR / adaptive sync
 * stubs — vrr_capable_6221, vrr_min_hz_6222, vrr_max_hz_6223,
 * adaptive_sync_enabled_6224, vrr_target_hz_6225, vrr_frame_us_min_6226,
 * vrr_frame_us_max_6227, vrr_lfc_capable_6228, vrr_ready_6229,
 * batch_id_6230). Unique gj_batch_id_6230 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6230_marker[] = "libcgj-batch6230";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6230_id(void)
{
	return 6230u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6230 - report this TU's graph batch number.
 *
 * Always returns 6230.
 */
uint32_t
gj_batch_id_6230(void)
{
	(void)NULL;
	return b6230_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6230(void)
    __attribute__((alias("gj_batch_id_6230")));
