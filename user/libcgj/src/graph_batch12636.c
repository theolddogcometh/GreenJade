/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12636: audio latency ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_latency_ok_u_12636(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       audio latency/quantum probe for the audio soft continuum.
 *   uint32_t __gj_audio_latency_ok_u_12636  (alias)
 *   __libcgj_batch12636_marker = "libcgj-batch12636"
 *
 * Exclusive continuum CREATE-ONLY (12631-12640: audio soft id
 * stubs — pipewire_ok_u_12631, wireplumber_ok_u_12632,
 * pulse_compat_ok_u_12633, alsa_ok_u_12634, jack_ok_u_12635,
 * audio_latency_ok_u_12636, bluetooth_audio_ok_u_12637,
 * hdmi_audio_ok_u_12638, audio_soft_ready_u_12639, batch_id_12640).
 * Unique surface only; no multi-def. Distinct from
 * gj_audio_latency_ok_u_12436 (batch12436),
 * gj_audio_latency_ok_u_10336 (batch10336), and
 * gj_audio_latency_ok_u_10236 (batch10236). No parent wires. No
 * __int128. No latency measurement implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12636_marker[] = "libcgj-batch12636";

/* Soft audio-latency-ok lamp: always off (not a real latency probe). */
#define B12636_AUDIO_LATENCY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12636_audio_latency_ok(void)
{
	return B12636_AUDIO_LATENCY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_latency_ok_u_12636 - audio latency ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not measure quantum or
 * call libc. No parent wires.
 */
uint32_t
gj_audio_latency_ok_u_12636(void)
{
	(void)NULL;
	return b12636_audio_latency_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_latency_ok_u_12636(void)
    __attribute__((alias("gj_audio_latency_ok_u_12636")));
