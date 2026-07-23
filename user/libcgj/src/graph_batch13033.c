/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13033: PulseAudio compat ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pulse_compat_ok_u_13033(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PulseAudio compatibility layer probe for the audio soft continuum.
 *   uint32_t __gj_pulse_compat_ok_u_13033  (alias)
 *   __libcgj_batch13033_marker = "libcgj-batch13033"
 *
 * Exclusive continuum CREATE-ONLY (13031-13040: audio soft id
 * stubs — pipewire_ok_u_13031, wireplumber_ok_u_13032,
 * pulse_compat_ok_u_13033, alsa_ok_u_13034, jack_ok_u_13035,
 * audio_latency_ok_u_13036, bluetooth_audio_ok_u_13037,
 * hdmi_audio_ok_u_13038, audio_soft_ready_u_13039, batch_id_13040).
 * Unique surface only; no multi-def. Distinct from
 * gj_pulse_compat_ok_u_12833 (batch12833), gj_pulse_compat_ok_u_12633
 * (batch12633), gj_pulse_compat_ok_u_12433 (batch12433),
 * gj_pulse_compat_ok_u_12233 (batch12233), gj_pulse_compat_ok_u_12033
 * (batch12033), and gj_pulse_compat_ok_u_10233 (batch10233). No parent
 * wires. No __int128. No PulseAudio implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13033_marker[] = "libcgj-batch13033";

/* Soft PulseAudio-compat-ok lamp: always off (not a real pulse probe). */
#define B13033_PULSE_COMPAT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13033_pulse_compat_ok(void)
{
	return B13033_PULSE_COMPAT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pulse_compat_ok_u_13033 - PulseAudio compat ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open PulseAudio or
 * call libc. No parent wires.
 */
uint32_t
gj_pulse_compat_ok_u_13033(void)
{
	(void)NULL;
	return b13033_pulse_compat_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pulse_compat_ok_u_13033(void)
    __attribute__((alias("gj_pulse_compat_ok_u_13033")));
