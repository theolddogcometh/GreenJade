/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13236: audio latency ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_latency_ok_u_13236(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       quantum/latency budget probe for the audio soft continuum.
 *   uint32_t __gj_audio_latency_ok_u_13236  (alias)
 *   __libcgj_batch13236_marker = "libcgj-batch13236"
 *
 * Exclusive continuum CREATE-ONLY (13231-13240: audio soft id
 * stubs — pipewire_ok_u_13231, wireplumber_ok_u_13232,
 * pulse_compat_ok_u_13233, alsa_ok_u_13234, jack_ok_u_13235,
 * audio_latency_ok_u_13236, bluetooth_audio_ok_u_13237,
 * hdmi_audio_ok_u_13238, audio_soft_ready_u_13239, batch_id_13240).
 * Unique surface only; no multi-def. Distinct from
 * gj_audio_latency_ok_u_13036 (batch13036), gj_audio_latency_ok_u_12836
 * (batch12836), gj_audio_latency_ok_u_12636 (batch12636),
 * gj_audio_latency_ok_u_12436 (batch12436), and
 * gj_audio_latency_ok_u_10236 (batch10236). No parent wires. No
 * __int128. No audio latency implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13236_marker[] = "libcgj-batch13236";

/* Soft audio-latency-ok lamp: always off (not a real latency probe). */
#define B13236_AUDIO_LATENCY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13236_audio_latency_ok(void)
{
	return B13236_AUDIO_LATENCY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_latency_ok_u_13236 - audio latency ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not measure quantum or
 * call libc. No parent wires.
 */
uint32_t
gj_audio_latency_ok_u_13236(void)
{
	(void)NULL;
	return b13236_audio_latency_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_latency_ok_u_13236(void)
    __attribute__((alias("gj_audio_latency_ok_u_13236")));
