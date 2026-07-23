/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5448: haptic rumble max intensity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_haptic_rumble_max_5448(void);
 *     - Return full-scale dual-motor rumble intensity (65535).
 *   uint32_t __gj_haptic_rumble_max_5448  (alias)
 *   __libcgj_batch5448_marker = "libcgj-batch5448"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Unique gj_haptic_rumble_max_5448 surface only; no
 * multi-def. Soft stub — does not drive FF/haptic devices. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5448_marker[] = "libcgj-batch5448";

/* Full-scale dual-motor rumble intensity (16-bit unsigned). */
#define B5448_RUMBLE_MAX  ((uint32_t)65535u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5448_rumble_max(void)
{
	return B5448_RUMBLE_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_haptic_rumble_max_5448 - dual-motor rumble full-scale intensity.
 *
 * Always returns 65535. Soft compile-time stub for freestanding haptic
 * intensity scaling; does not open FF devices. No parent wires.
 */
uint32_t
gj_haptic_rumble_max_5448(void)
{
	(void)NULL;
	return b5448_rumble_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_haptic_rumble_max_5448(void)
    __attribute__((alias("gj_haptic_rumble_max_5448")));
