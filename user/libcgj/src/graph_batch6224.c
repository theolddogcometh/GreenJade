/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6224: adaptive-sync enable default stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adaptive_sync_enabled_6224(void);
 *     - Returns 1 (adaptive-sync / VRR considered enabled by default
 *       for this continuum). Soft product-policy stub; not a live
 *       compositor or DRM property write.
 *   uint32_t __gj_adaptive_sync_enabled_6224  (alias)
 *   __libcgj_batch6224_marker = "libcgj-batch6224"
 *
 * Exclusive continuum CREATE-ONLY (6221-6230: VRR / adaptive sync
 * stubs — vrr_capable_6221, vrr_min_hz_6222, vrr_max_hz_6223,
 * adaptive_sync_enabled_6224, vrr_target_hz_6225, vrr_frame_us_min_6226,
 * vrr_frame_us_max_6227, vrr_lfc_capable_6228, vrr_ready_6229,
 * batch_id_6230). Unique gj_adaptive_sync_enabled_6224 surface only;
 * no multi-def. Distinct from gj_vrr_capable_6221. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6224_marker[] = "libcgj-batch6224";

/* Adaptive-sync default enable (1 = enabled). */
#define B6224_ADAPTIVE_SYNC_ENABLED  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6224_adaptive_sync_enabled(void)
{
	return B6224_ADAPTIVE_SYNC_ENABLED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adaptive_sync_enabled_6224 - report adaptive-sync enable default.
 *
 * Always returns 1. Soft pure-data policy stub; does not toggle DRM
 * adaptive_sync properties. No parent wires.
 */
uint32_t
gj_adaptive_sync_enabled_6224(void)
{
	(void)NULL;
	return b6224_adaptive_sync_enabled();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_adaptive_sync_enabled_6224(void)
    __attribute__((alias("gj_adaptive_sync_enabled_6224")));
