/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5442: ALSA CD-quality PCM rate 44.1 kHz.
 *
 * Surface (unique symbols):
 *   uint32_t gj_alsa_rate_44k_5442(void);
 *     - Return the ALSA/CD-audio sample rate in Hz (44100).
 *   uint32_t __gj_alsa_rate_44k_5442  (alias)
 *   __libcgj_batch5442_marker = "libcgj-batch5442"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Unique gj_alsa_rate_44k_5442 surface only; no multi-def.
 * Distinct from gj_audio_rate_44k_u (batch5311) and gj_pw_rate_48k_5441
 * (batch5441). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5442_marker[] = "libcgj-batch5442";

/* ALSA CD-quality PCM sample rate (Hz). */
#define B5442_ALSA_RATE_44K  ((uint32_t)44100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5442_alsa_rate_44k(void)
{
	return B5442_ALSA_RATE_44K;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alsa_rate_44k_5442 - ALSA CD-quality PCM sample rate (44100 Hz).
 *
 * Always returns 44100. Soft compile-time constant for freestanding
 * ALSA graph wiring; does not open a PCM device. No parent wires.
 */
uint32_t
gj_alsa_rate_44k_5442(void)
{
	(void)NULL;
	return b5442_alsa_rate_44k();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_alsa_rate_44k_5442(void)
    __attribute__((alias("gj_alsa_rate_44k_5442")));
