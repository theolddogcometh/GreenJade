/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13836: audio latency ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_latency_ok_u_13836(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       quantum/latency measurement for the audio soft continuum.
 *   uint32_t __gj_audio_latency_ok_u_13836  (alias)
 *   __libcgj_batch13836_marker = "libcgj-batch13836"
 *
 * Exclusive continuum CREATE-ONLY (13831-13840: audio soft id
 * stubs — pipewire_ok_u_13831, wireplumber_ok_u_13832,
 * pulse_compat_ok_u_13833, alsa_ok_u_13834, jack_ok_u_13835,
 * audio_latency_ok_u_13836, bluetooth_audio_ok_u_13837,
 * hdmi_audio_ok_u_13838, audio_soft_ready_u_13839, batch_id_13840).
 * Unique surface only; no multi-def. Distinct from
 * gj_audio_latency_ok_u_13636 (batch13636),
 * gj_audio_latency_ok_u_13436 (batch13436),
 * gj_audio_latency_ok_u_13236 (batch13236),
 * gj_audio_latency_ok_u_13036 (batch13036),
 * gj_audio_latency_ok_u_12836 (batch12836), and
 * gj_audio_latency_ok_u_12636 (batch12636). No parent wires. No
 * __int128. No latency measurement.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13836_marker[] = "libcgj-batch13836";

/* Soft audio-latency-ok lamp: always off (not a real latency probe). */
#define B13836_AUDIO_LATENCY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13836_audio_latency_ok(void)
{
	return B13836_AUDIO_LATENCY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_latency_ok_u_13836 - audio latency ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not measure quantum or
 * call libc. No parent wires.
 */
uint32_t
gj_audio_latency_ok_u_13836(void)
{
	(void)NULL;
	return b13836_audio_latency_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_latency_ok_u_13836(void)
    __attribute__((alias("gj_audio_latency_ok_u_13836")));
