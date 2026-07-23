/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12436: audio latency ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_latency_ok_u_12436(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       quantum/period or buffer-size probe for the audio soft
 *       continuum.
 *   uint32_t __gj_audio_latency_ok_u_12436  (alias)
 *   __libcgj_batch12436_marker = "libcgj-batch12436"
 *
 * Exclusive continuum CREATE-ONLY (12431-12440: audio soft id
 * stubs — pipewire_ok_u_12431, wireplumber_ok_u_12432,
 * pulse_compat_ok_u_12433, alsa_ok_u_12434, jack_ok_u_12435,
 * audio_latency_ok_u_12436, bluetooth_audio_ok_u_12437,
 * hdmi_audio_ok_u_12438, audio_soft_ready_u_12439, batch_id_12440).
 * Unique surface only; no multi-def. Distinct from
 * gj_audio_latency_ok_u_10336 (batch10336) and
 * gj_audio_latency_ok_u_10236 (batch10236). No parent wires. No
 * __int128. No latency measurement.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12436_marker[] = "libcgj-batch12436";

/* Soft audio-latency-ok lamp: always off (not a real latency probe). */
#define B12436_AUDIO_LATENCY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12436_audio_latency_ok(void)
{
	return B12436_AUDIO_LATENCY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_latency_ok_u_12436 - audio latency ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not measure latency or
 * call libc. No parent wires.
 */
uint32_t
gj_audio_latency_ok_u_12436(void)
{
	(void)NULL;
	return b12436_audio_latency_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_latency_ok_u_12436(void)
    __attribute__((alias("gj_audio_latency_ok_u_12436")));
