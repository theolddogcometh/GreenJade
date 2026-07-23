/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5444: ALSA SND_PCM_FORMAT_S16_LE bits/sample.
 *
 * Surface (unique symbols):
 *   uint32_t gj_alsa_fmt_s16_bps_5444(void);
 *     - Return bits-per-sample for signed 16-bit little-endian PCM (16).
 *   uint32_t __gj_alsa_fmt_s16_bps_5444  (alias)
 *   __libcgj_batch5444_marker = "libcgj-batch5444"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Unique gj_alsa_fmt_s16_bps_5444 surface only; no multi-def.
 * Distinct from gj_audio_bps_16_u (batch5316). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5444_marker[] = "libcgj-batch5444";

/* ALSA S16_LE sample format width in bits. */
#define B5444_S16_BPS  ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5444_s16_bps(void)
{
	return B5444_S16_BPS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alsa_fmt_s16_bps_5444 - ALSA S16_LE bits-per-sample constant (16).
 *
 * Always returns 16. Soft compile-time constant for freestanding sample
 * format selection; does not call snd_pcm_* APIs. No parent wires.
 */
uint32_t
gj_alsa_fmt_s16_bps_5444(void)
{
	(void)NULL;
	return b5444_s16_bps();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_alsa_fmt_s16_bps_5444(void)
    __attribute__((alias("gj_alsa_fmt_s16_bps_5444")));
