/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5447: Steam Deck HID gamepad trigger full-scale.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_gp_trigger_max_5447(void);
 *     - Return full-scale max for analog L2/R2 trigger axis (32767).
 *   uint32_t __gj_hid_gp_trigger_max_5447  (alias)
 *   __libcgj_batch5447_marker = "libcgj-batch5447"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Unique gj_hid_gp_trigger_max_5447 surface only; no
 * multi-def. Distinct from gj_hid_gp_btn_a_5445 / gj_hid_gp_btn_b_5446.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5447_marker[] = "libcgj-batch5447";

/* Full-scale max for signed 16-bit analog trigger (L2/R2). */
#define B5447_TRIGGER_MAX  ((uint32_t)32767u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5447_trigger_max(void)
{
	return B5447_TRIGGER_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_gp_trigger_max_5447 - Steam Deck L2/R2 analog full-scale (32767).
 *
 * Always returns 32767. Soft compile-time constant for freestanding
 * trigger normalization; does not read HID reports. No parent wires.
 */
uint32_t
gj_hid_gp_trigger_max_5447(void)
{
	(void)NULL;
	return b5447_trigger_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_gp_trigger_max_5447(void)
    __attribute__((alias("gj_hid_gp_trigger_max_5447")));
