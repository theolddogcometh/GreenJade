/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5441: PipeWire preferred PCM rate 48 kHz.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_rate_48k_5441(void);
 *     - Return the PipeWire/pro-audio preferred sample rate in Hz (48000).
 *   uint32_t __gj_pw_rate_48k_5441  (alias)
 *   __libcgj_batch5441_marker = "libcgj-batch5441"
 *
 * Exclusive continuum CREATE-ONLY (5441-5450: audio PipeWire/ALSA,
 * HID gamepad Steam Deck, haptics, gyro stubs —
 * pw_rate_48k_5441, alsa_rate_44k_5442, audio_ch_stereo_5443,
 * alsa_fmt_s16_bps_5444, hid_gp_btn_a_5445, hid_gp_btn_b_5446,
 * hid_gp_trigger_max_5447, haptic_rumble_max_5448, gyro_fs_mdps_5449,
 * batch_id_5450). Unique gj_pw_rate_48k_5441 surface only; no multi-def.
 * Distinct from gj_audio_rate_48k_u (batch5312). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5441_marker[] = "libcgj-batch5441";

/* PipeWire preferred professional PCM sample rate (Hz). */
#define B5441_PW_RATE_48K  ((uint32_t)48000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5441_pw_rate_48k(void)
{
	return B5441_PW_RATE_48K;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_rate_48k_5441 - PipeWire preferred PCM sample rate (48000 Hz).
 *
 * Always returns 48000. Soft compile-time constant for freestanding
 * audio graph wiring; does not open a PipeWire context. No parent wires.
 */
uint32_t
gj_pw_rate_48k_5441(void)
{
	(void)NULL;
	return b5441_pw_rate_48k();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_rate_48k_5441(void)
    __attribute__((alias("gj_pw_rate_48k_5441")));
