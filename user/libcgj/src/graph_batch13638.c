/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13638: HDMI audio ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdmi_audio_ok_u_13638(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDMI/DisplayPort audio endpoint probe for the audio soft
 *       continuum.
 *   uint32_t __gj_hdmi_audio_ok_u_13638  (alias)
 *   __libcgj_batch13638_marker = "libcgj-batch13638"
 *
 * Exclusive continuum CREATE-ONLY (13631-13640: audio soft id
 * stubs — pipewire_ok_u_13631, wireplumber_ok_u_13632,
 * pulse_compat_ok_u_13633, alsa_ok_u_13634, jack_ok_u_13635,
 * audio_latency_ok_u_13636, bluetooth_audio_ok_u_13637,
 * hdmi_audio_ok_u_13638, audio_soft_ready_u_13639, batch_id_13640).
 * Unique surface only; no multi-def. Distinct from
 * gj_hdmi_audio_ok_u_13438 (batch13438), gj_hdmi_audio_ok_u_13238
 * (batch13238), gj_hdmi_audio_ok_u_13038 (batch13038),
 * gj_hdmi_audio_ok_u_12838 (batch12838), gj_hdmi_audio_ok_u_12638
 * (batch12638), and gj_hdmi_audio_ok_u_12438 (batch12438). No parent
 * wires. No __int128. No HDMI audio implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13638_marker[] = "libcgj-batch13638";

/* Soft HDMI-audio-ok lamp: always off (not a real HDMI audio probe). */
#define B13638_HDMI_AUDIO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13638_hdmi_audio_ok(void)
{
	return B13638_HDMI_AUDIO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdmi_audio_ok_u_13638 - HDMI audio ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDMI endpoints
 * or call libc. No parent wires.
 */
uint32_t
gj_hdmi_audio_ok_u_13638(void)
{
	(void)NULL;
	return b13638_hdmi_audio_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdmi_audio_ok_u_13638(void)
    __attribute__((alias("gj_hdmi_audio_ok_u_13638")));
