/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12838: HDMI audio ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdmi_audio_ok_u_12838(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDMI/DisplayPort audio endpoint probe for the audio soft
 *       continuum.
 *   uint32_t __gj_hdmi_audio_ok_u_12838  (alias)
 *   __libcgj_batch12838_marker = "libcgj-batch12838"
 *
 * Exclusive continuum CREATE-ONLY (12831-12840: audio soft id
 * stubs — pipewire_ok_u_12831, wireplumber_ok_u_12832,
 * pulse_compat_ok_u_12833, alsa_ok_u_12834, jack_ok_u_12835,
 * audio_latency_ok_u_12836, bluetooth_audio_ok_u_12837,
 * hdmi_audio_ok_u_12838, audio_soft_ready_u_12839, batch_id_12840).
 * Unique surface only; no multi-def. Distinct from
 * gj_hdmi_audio_ok_u_12638 (batch12638), gj_hdmi_audio_ok_u_12438
 * (batch12438), gj_hdmi_audio_ok_u_10338 (batch10338), and
 * gj_hdmi_audio_ok_u_10238 (batch10238). No parent wires. No
 * __int128. No HDMI audio implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12838_marker[] = "libcgj-batch12838";

/* Soft HDMI-audio-ok lamp: always off (not a real HDMI audio probe). */
#define B12838_HDMI_AUDIO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12838_hdmi_audio_ok(void)
{
	return B12838_HDMI_AUDIO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdmi_audio_ok_u_12838 - HDMI audio ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDMI endpoints
 * or call libc. No parent wires.
 */
uint32_t
gj_hdmi_audio_ok_u_12838(void)
{
	(void)NULL;
	return b12838_hdmi_audio_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdmi_audio_ok_u_12838(void)
    __attribute__((alias("gj_hdmi_audio_ok_u_12838")));
