/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14333: PulseAudio compat ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pulse_compat_ok_u_14333(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PulseAudio compatibility layer probe for the audio soft continuum.
 *   uint32_t __gj_pulse_compat_ok_u_14333  (alias)
 *   __libcgj_batch14333_marker = "libcgj-batch14333"
 *
 * Exclusive continuum CREATE-ONLY (14331-14340: audio soft id
 * stubs — pipewire_ok_u_14331, wireplumber_ok_u_14332,
 * pulse_compat_ok_u_14333, alsa_ok_u_14334, jack_ok_u_14335,
 * audio_latency_ok_u_14336, bluetooth_audio_ok_u_14337,
 * hdmi_audio_ok_u_14338, audio_soft_ready_u_14339, batch_id_14340).
 * Unique surface only; no multi-def. Distinct from
 * gj_pulse_compat_ok_u_13033 (batch13033), gj_pulse_compat_ok_u_12833
 * (batch12833), gj_pulse_compat_ok_u_12633 (batch12633),
 * gj_pulse_compat_ok_u_12433 (batch12433), gj_pulse_compat_ok_u_12233
 * (batch12233), and gj_pulse_compat_ok_u_10333 (batch10333). No parent
 * wires. No __int128. No PulseAudio implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14333_marker[] = "libcgj-batch14333";

/* Soft PulseAudio-compat-ok lamp: always off (not a real pulse probe). */
#define B14333_PULSE_COMPAT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14333_pulse_compat_ok(void)
{
	return B14333_PULSE_COMPAT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pulse_compat_ok_u_14333 - PulseAudio compat ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open PulseAudio or
 * call libc. No parent wires.
 */
uint32_t
gj_pulse_compat_ok_u_14333(void)
{
	(void)NULL;
	return b14333_pulse_compat_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pulse_compat_ok_u_14333(void)
    __attribute__((alias("gj_pulse_compat_ok_u_14333")));
