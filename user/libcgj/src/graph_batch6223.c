/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6223: VRR / adaptive-sync maximum refresh.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vrr_max_hz_6223(void);
 *     - Returns 144 (maximum VRR ceiling in Hz for this continuum).
 *   uint32_t __gj_vrr_max_hz_6223  (alias)
 *   __libcgj_batch6223_marker = "libcgj-batch6223"
 *
 * Exclusive continuum CREATE-ONLY (6221-6230: VRR / adaptive sync
 * stubs — vrr_capable_6221, vrr_min_hz_6222, vrr_max_hz_6223,
 * adaptive_sync_enabled_6224, vrr_target_hz_6225, vrr_frame_us_min_6226,
 * vrr_frame_us_max_6227, vrr_lfc_capable_6228, vrr_ready_6229,
 * batch_id_6230). Unique gj_vrr_max_hz_6223 surface only; no multi-def.
 * Distinct from gj_deck_oled_refresh_max_hz_5941. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6223_marker[] = "libcgj-batch6223";

/* VRR maximum refresh ceiling (Hz). */
#define B6223_VRR_MAX_HZ  144u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6223_vrr_max_hz(void)
{
	return B6223_VRR_MAX_HZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vrr_max_hz_6223 - report VRR / adaptive-sync maximum refresh.
 *
 * Always returns 144. Soft compile-time timing stub; does not program
 * the panel controller. No parent wires.
 */
uint32_t
gj_vrr_max_hz_6223(void)
{
	(void)NULL;
	return b6223_vrr_max_hz();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vrr_max_hz_6223(void)
    __attribute__((alias("gj_vrr_max_hz_6223")));
