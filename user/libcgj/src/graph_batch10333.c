/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10333: PulseAudio compat ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pulse_compat_ok_u_10333(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PulseAudio / pipewire-pulse compat probe for the audio soft
 *       continuum.
 *   uint32_t __gj_pulse_compat_ok_u_10333  (alias)
 *   __libcgj_batch10333_marker = "libcgj-batch10333"
 *
 * Exclusive continuum CREATE-ONLY (10331-10340: audio soft all→0
 * stubs — pipewire_ok_u_10331, wireplumber_ok_u_10332,
 * pulse_compat_ok_u_10333, alsa_ok_u_10334, jack_ok_u_10335,
 * audio_latency_ok_u_10336, bluetooth_audio_ok_u_10337,
 * hdmi_audio_ok_u_10338, audio_soft_ready_u_10339, batch_id_10340).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No PulseAudio implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10333_marker[] = "libcgj-batch10333";

/* Soft Pulse compat-ok lamp: always off (not a real pulse probe). */
#define B10333_PULSE_COMPAT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10333_pulse_compat_ok(void)
{
	return B10333_PULSE_COMPAT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pulse_compat_ok_u_10333 - PulseAudio compat ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect pulse or
 * call libc. No parent wires.
 */
uint32_t
gj_pulse_compat_ok_u_10333(void)
{
	(void)NULL;
	return b10333_pulse_compat_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pulse_compat_ok_u_10333(void)
    __attribute__((alias("gj_pulse_compat_ok_u_10333")));
