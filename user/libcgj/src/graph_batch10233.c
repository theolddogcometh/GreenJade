/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10233: audio/pipewire soft pulse-compat-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pulse_compat_ok_u_10233(void);
 *     - Returns 0 (PulseAudio compatibility soft path not asserted for
 *       the audio product soft-id wave). Soft compile-time product tag;
 *       not a live pulse/pipewire-pulse socket probe.
 *   uint32_t __gj_pulse_compat_ok_u_10233  (alias)
 *   __libcgj_batch10233_marker = "libcgj-batch10233"
 *
 * Exclusive audio/pipewire soft product CREATE-ONLY (10231-10240:
 * pipewire_ok, wireplumber_ok, pulse_compat_ok, alsa_ok, jack_ok,
 * audio_latency_ok, bluetooth_audio_ok, hdmi_audio_ok,
 * audio_soft_ready, batch_id_10240). Unique
 * gj_pulse_compat_ok_u_10233 surface only; no multi-def. Distinct from
 * gj_pipewire_ok_u_10231. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10233_marker[] = "libcgj-batch10233";

/* Audio product soft pulse-compat-ok lamp (not asserted). */
#define B10233_PULSE_COMPAT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10233_ok(void)
{
	return B10233_PULSE_COMPAT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pulse_compat_ok_u_10233 - audio product soft pulse-compat-ok lamp.
 *
 * Always returns 0. Soft pure-data product tag. Does not connect to
 * pulse/pipewire-pulse or call libc. No parent wires.
 */
uint32_t
gj_pulse_compat_ok_u_10233(void)
{
	(void)NULL;
	return b10233_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pulse_compat_ok_u_10233(void)
    __attribute__((alias("gj_pulse_compat_ok_u_10233")));
