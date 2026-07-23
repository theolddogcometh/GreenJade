/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6222: VRR / adaptive-sync minimum refresh.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vrr_min_hz_6222(void);
 *     - Returns 48 (minimum VRR floor in Hz for this continuum).
 *   uint32_t __gj_vrr_min_hz_6222  (alias)
 *   __libcgj_batch6222_marker = "libcgj-batch6222"
 *
 * Exclusive continuum CREATE-ONLY (6221-6230: VRR / adaptive sync
 * stubs — vrr_capable_6221, vrr_min_hz_6222, vrr_max_hz_6223,
 * adaptive_sync_enabled_6224, vrr_target_hz_6225, vrr_frame_us_min_6226,
 * vrr_frame_us_max_6227, vrr_lfc_capable_6228, vrr_ready_6229,
 * batch_id_6230). Unique gj_vrr_min_hz_6222 surface only; no multi-def.
 * Distinct from gj_deck_oled_refresh_min_hz_5942. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6222_marker[] = "libcgj-batch6222";

/* VRR minimum refresh floor (Hz). */
#define B6222_VRR_MIN_HZ  48u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6222_vrr_min_hz(void)
{
	return B6222_VRR_MIN_HZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vrr_min_hz_6222 - report VRR / adaptive-sync minimum refresh.
 *
 * Always returns 48. Soft compile-time timing stub; does not program
 * the panel controller. No parent wires.
 */
uint32_t
gj_vrr_min_hz_6222(void)
{
	(void)NULL;
	return b6222_vrr_min_hz();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vrr_min_hz_6222(void)
    __attribute__((alias("gj_vrr_min_hz_6222")));
