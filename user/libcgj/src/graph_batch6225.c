/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6225: VRR target refresh within band.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vrr_target_hz_6225(void);
 *     - Returns 60 (nominal target refresh in Hz inside the VRR band).
 *   uint32_t __gj_vrr_target_hz_6225  (alias)
 *   __libcgj_batch6225_marker = "libcgj-batch6225"
 *
 * Exclusive continuum CREATE-ONLY (6221-6230: VRR / adaptive sync
 * stubs — vrr_capable_6221, vrr_min_hz_6222, vrr_max_hz_6223,
 * adaptive_sync_enabled_6224, vrr_target_hz_6225, vrr_frame_us_min_6226,
 * vrr_frame_us_max_6227, vrr_lfc_capable_6228, vrr_ready_6229,
 * batch_id_6230). Unique gj_vrr_target_hz_6225 surface only; no multi-def.
 * Distinct from gj_vrr_min_hz_6222 / gj_vrr_max_hz_6223. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6225_marker[] = "libcgj-batch6225";

/* Nominal target refresh inside VRR band (Hz). */
#define B6225_VRR_TARGET_HZ  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6225_vrr_target_hz(void)
{
	return B6225_VRR_TARGET_HZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vrr_target_hz_6225 - report nominal VRR target refresh.
 *
 * Always returns 60. Soft compile-time timing stub; does not reprogram
 * the CRTC. No parent wires.
 */
uint32_t
gj_vrr_target_hz_6225(void)
{
	(void)NULL;
	return b6225_vrr_target_hz();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vrr_target_hz_6225(void)
    __attribute__((alias("gj_vrr_target_hz_6225")));
