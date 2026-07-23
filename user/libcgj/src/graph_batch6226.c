/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6226: VRR minimum frame period (us).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vrr_frame_us_min_6226(void);
 *     - Returns 6944 (approx. 1e6/144 us; frame period at VRR max Hz).
 *   uint32_t __gj_vrr_frame_us_min_6226  (alias)
 *   __libcgj_batch6226_marker = "libcgj-batch6226"
 *
 * Exclusive continuum CREATE-ONLY (6221-6230: VRR / adaptive sync
 * stubs — vrr_capable_6221, vrr_min_hz_6222, vrr_max_hz_6223,
 * adaptive_sync_enabled_6224, vrr_target_hz_6225, vrr_frame_us_min_6226,
 * vrr_frame_us_max_6227, vrr_lfc_capable_6228, vrr_ready_6229,
 * batch_id_6230). Unique gj_vrr_frame_us_min_6226 surface only;
 * no multi-def. Pairs with gj_vrr_max_hz_6223. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6226_marker[] = "libcgj-batch6226";

/* Minimum VRR frame period (us) ≈ 1e6/144. */
#define B6226_VRR_FRAME_US_MIN  6944u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6226_vrr_frame_us_min(void)
{
	return B6226_VRR_FRAME_US_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vrr_frame_us_min_6226 - report VRR minimum frame period (us).
 *
 * Always returns 6944. Soft compile-time timing stub for vsync pacing
 * at the top of the VRR band. No parent wires.
 */
uint32_t
gj_vrr_frame_us_min_6226(void)
{
	(void)NULL;
	return b6226_vrr_frame_us_min();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vrr_frame_us_min_6226(void)
    __attribute__((alias("gj_vrr_frame_us_min_6226")));
