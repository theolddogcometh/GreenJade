/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13833: PulseAudio compat ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pulse_compat_ok_u_13833(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PulseAudio compatibility layer probe for the audio soft continuum.
 *   uint32_t __gj_pulse_compat_ok_u_13833  (alias)
 *   __libcgj_batch13833_marker = "libcgj-batch13833"
 *
 * Exclusive continuum CREATE-ONLY (13831-13840: audio soft id
 * stubs — pipewire_ok_u_13831, wireplumber_ok_u_13832,
 * pulse_compat_ok_u_13833, alsa_ok_u_13834, jack_ok_u_13835,
 * audio_latency_ok_u_13836, bluetooth_audio_ok_u_13837,
 * hdmi_audio_ok_u_13838, audio_soft_ready_u_13839, batch_id_13840).
 * Unique surface only; no multi-def. Distinct from
 * gj_pulse_compat_ok_u_13633 (batch13633), gj_pulse_compat_ok_u_13433
 * (batch13433), gj_pulse_compat_ok_u_13233 (batch13233),
 * gj_pulse_compat_ok_u_13033 (batch13033), gj_pulse_compat_ok_u_12833
 * (batch12833), and gj_pulse_compat_ok_u_12633 (batch12633). No parent
 * wires. No __int128. No PulseAudio implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13833_marker[] = "libcgj-batch13833";

/* Soft PulseAudio-compat-ok lamp: always off (not a real pulse probe). */
#define B13833_PULSE_COMPAT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13833_pulse_compat_ok(void)
{
	return B13833_PULSE_COMPAT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pulse_compat_ok_u_13833 - PulseAudio compat ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open PulseAudio or
 * call libc. No parent wires.
 */
uint32_t
gj_pulse_compat_ok_u_13833(void)
{
	(void)NULL;
	return b13833_pulse_compat_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pulse_compat_ok_u_13833(void)
    __attribute__((alias("gj_pulse_compat_ok_u_13833")));
