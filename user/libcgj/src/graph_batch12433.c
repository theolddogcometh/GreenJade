/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12433: PulseAudio compat ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pulse_compat_ok_u_12433(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pulse/pipewire-pulse socket probe for the audio soft continuum.
 *   uint32_t __gj_pulse_compat_ok_u_12433  (alias)
 *   __libcgj_batch12433_marker = "libcgj-batch12433"
 *
 * Exclusive continuum CREATE-ONLY (12431-12440: audio soft id
 * stubs — pipewire_ok_u_12431, wireplumber_ok_u_12432,
 * pulse_compat_ok_u_12433, alsa_ok_u_12434, jack_ok_u_12435,
 * audio_latency_ok_u_12436, bluetooth_audio_ok_u_12437,
 * hdmi_audio_ok_u_12438, audio_soft_ready_u_12439, batch_id_12440).
 * Unique surface only; no multi-def. Distinct from
 * gj_pulse_compat_ok_u_10333 (batch10333), gj_pulse_compat_ok_u_10233
 * (batch10233), and gj_pulse_compat_ok_u_10033 (batch10033). No parent
 * wires. No __int128. No PulseAudio implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12433_marker[] = "libcgj-batch12433";

/* Soft pulse-compat-ok lamp: always off (not a real pulse probe). */
#define B12433_PULSE_COMPAT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12433_pulse_compat_ok(void)
{
	return B12433_PULSE_COMPAT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pulse_compat_ok_u_12433 - PulseAudio compatibility ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not connect to
 * pulse/pipewire-pulse or call libc. No parent wires.
 */
uint32_t
gj_pulse_compat_ok_u_12433(void)
{
	(void)NULL;
	return b12433_pulse_compat_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pulse_compat_ok_u_12433(void)
    __attribute__((alias("gj_pulse_compat_ok_u_12433")));
