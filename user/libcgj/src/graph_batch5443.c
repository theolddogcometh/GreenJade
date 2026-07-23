/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5443: PipeWire/ALSA stereo channel count.
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_ch_stereo_5443(void);
 *     - Return the stereo (two-channel) PCM channel count (2).
 *   uint32_t __gj_audio_ch_stereo_5443  (alias)
 *   __libcgj_batch5443_marker = "libcgj-batch5443"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Unique gj_audio_ch_stereo_5443 surface only; no multi-def.
 * Distinct from gj_audio_ch_stereo_u (batch5315). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5443_marker[] = "libcgj-batch5443";

/* Stereo PCM channel count (PipeWire/ALSA default path). */
#define B5443_CH_STEREO  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5443_ch_stereo(void)
{
	return B5443_CH_STEREO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_ch_stereo_5443 - stereo PCM channel count constant (2).
 *
 * Always returns 2. Soft compile-time constant for freestanding
 * PipeWire/ALSA channel layout; does not probe devices. No parent wires.
 */
uint32_t
gj_audio_ch_stereo_5443(void)
{
	(void)NULL;
	return b5443_ch_stereo();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_ch_stereo_5443(void)
    __attribute__((alias("gj_audio_ch_stereo_5443")));
