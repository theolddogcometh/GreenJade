/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6221: VRR / adaptive-sync capability stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vrr_capable_6221(void);
 *     - Returns 1 (VRR / adaptive-sync path considered capable for this
 *       continuum). Soft compile-time display-mode product stub; not a
 *       live DRM VRR property probe.
 *   uint32_t __gj_vrr_capable_6221  (alias)
 *   __libcgj_batch6221_marker = "libcgj-batch6221"
 *
 * Exclusive continuum CREATE-ONLY (6221-6230: VRR / adaptive sync
 * stubs — vrr_capable_6221, vrr_min_hz_6222, vrr_max_hz_6223,
 * adaptive_sync_enabled_6224, vrr_target_hz_6225, vrr_frame_us_min_6226,
 * vrr_frame_us_max_6227, vrr_lfc_capable_6228, vrr_ready_6229,
 * batch_id_6230). Unique gj_vrr_capable_6221 surface only; no multi-def.
 * Distinct from gj_deck_vrr_capable_5456 (batch5456). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6221_marker[] = "libcgj-batch6221";

/* VRR capability stub (1 = path capable for this continuum). */
#define B6221_VRR_CAPABLE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6221_vrr_capable(void)
{
	return B6221_VRR_CAPABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vrr_capable_6221 - report VRR / adaptive-sync capability stub.
 *
 * Always returns 1 (VRR path capable). Soft pure-data display stub;
 * does not query DRM VRR properties. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_vrr_capable_6221(void)
{
	(void)NULL;
	return b6221_vrr_capable();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vrr_capable_6221(void)
    __attribute__((alias("gj_vrr_capable_6221")));
