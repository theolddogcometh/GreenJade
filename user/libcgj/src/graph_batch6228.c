/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6228: VRR low-framerate compensation stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vrr_lfc_capable_6228(void);
 *     - Returns 1 (low-framerate compensation / LFC path considered
 *       capable for this continuum). Soft product stub; not a live
 *       FreeSync LFC probe.
 *   uint32_t __gj_vrr_lfc_capable_6228  (alias)
 *   __libcgj_batch6228_marker = "libcgj-batch6228"
 *
 * Exclusive continuum CREATE-ONLY (6221-6230: VRR / adaptive sync
 * stubs — vrr_capable_6221, vrr_min_hz_6222, vrr_max_hz_6223,
 * adaptive_sync_enabled_6224, vrr_target_hz_6225, vrr_frame_us_min_6226,
 * vrr_frame_us_max_6227, vrr_lfc_capable_6228, vrr_ready_6229,
 * batch_id_6230). Unique gj_vrr_lfc_capable_6228 surface only;
 * no multi-def. Distinct from gj_vrr_capable_6221. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6228_marker[] = "libcgj-batch6228";

/* VRR LFC capability stub (1 = path capable). */
#define B6228_VRR_LFC_CAPABLE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6228_vrr_lfc_capable(void)
{
	return B6228_VRR_LFC_CAPABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vrr_lfc_capable_6228 - report VRR low-framerate compensation stub.
 *
 * Always returns 1. Soft pure-data display stub; does not probe
 * FreeSync LFC or double the refresh below the VRR floor. No parent
 * wires.
 */
uint32_t
gj_vrr_lfc_capable_6228(void)
{
	(void)NULL;
	return b6228_vrr_lfc_capable();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vrr_lfc_capable_6228(void)
    __attribute__((alias("gj_vrr_lfc_capable_6228")));
