/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10336: audio latency ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_latency_ok_u_10336(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       audio latency/budget probe for the audio soft continuum.
 *   uint32_t __gj_audio_latency_ok_u_10336  (alias)
 *   __libcgj_batch10336_marker = "libcgj-batch10336"
 *
 * Exclusive continuum CREATE-ONLY (10331-10340: audio soft all→0
 * stubs — pipewire_ok_u_10331, wireplumber_ok_u_10332,
 * pulse_compat_ok_u_10333, alsa_ok_u_10334, jack_ok_u_10335,
 * audio_latency_ok_u_10336, bluetooth_audio_ok_u_10337,
 * hdmi_audio_ok_u_10338, audio_soft_ready_u_10339, batch_id_10340).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No latency measurement implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10336_marker[] = "libcgj-batch10336";

/* Soft audio-latency-ok lamp: always off (not a real latency probe). */
#define B10336_AUDIO_LATENCY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10336_audio_latency_ok(void)
{
	return B10336_AUDIO_LATENCY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_latency_ok_u_10336 - audio latency ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not measure latency
 * or call libc. No parent wires.
 */
uint32_t
gj_audio_latency_ok_u_10336(void)
{
	(void)NULL;
	return b10336_audio_latency_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_latency_ok_u_10336(void)
    __attribute__((alias("gj_audio_latency_ok_u_10336")));
