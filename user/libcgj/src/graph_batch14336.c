/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14336: audio latency ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_latency_ok_u_14336(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       quantum/latency budget probe for the audio soft continuum.
 *   uint32_t __gj_audio_latency_ok_u_14336  (alias)
 *   __libcgj_batch14336_marker = "libcgj-batch14336"
 *
 * Exclusive continuum CREATE-ONLY (14331-14340: audio soft id
 * stubs — pipewire_ok_u_14331, wireplumber_ok_u_14332,
 * pulse_compat_ok_u_14333, alsa_ok_u_14334, jack_ok_u_14335,
 * audio_latency_ok_u_14336, bluetooth_audio_ok_u_14337,
 * hdmi_audio_ok_u_14338, audio_soft_ready_u_14339, batch_id_14340).
 * Unique surface only; no multi-def. Distinct from
 * gj_audio_latency_ok_u_13036 (batch13036), gj_audio_latency_ok_u_12836
 * (batch12836), gj_audio_latency_ok_u_12636 (batch12636),
 * gj_audio_latency_ok_u_12436 (batch12436), gj_audio_latency_ok_u_10336
 * (batch10336), and gj_audio_latency_ok_u_10236 (batch10236). No parent
 * wires. No __int128. No audio latency implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14336_marker[] = "libcgj-batch14336";

/* Soft audio-latency-ok lamp: always off (not a real latency probe). */
#define B14336_AUDIO_LATENCY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14336_audio_latency_ok(void)
{
	return B14336_AUDIO_LATENCY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_latency_ok_u_14336 - audio latency ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not measure quantum or
 * call libc. No parent wires.
 */
uint32_t
gj_audio_latency_ok_u_14336(void)
{
	(void)NULL;
	return b14336_audio_latency_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_latency_ok_u_14336(void)
    __attribute__((alias("gj_audio_latency_ok_u_14336")));
