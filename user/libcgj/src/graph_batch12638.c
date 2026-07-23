/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12638: HDMI audio ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdmi_audio_ok_u_12638(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDMI/DisplayPort audio endpoint probe for the audio soft
 *       continuum.
 *   uint32_t __gj_hdmi_audio_ok_u_12638  (alias)
 *   __libcgj_batch12638_marker = "libcgj-batch12638"
 *
 * Exclusive continuum CREATE-ONLY (12631-12640: audio soft id
 * stubs — pipewire_ok_u_12631, wireplumber_ok_u_12632,
 * pulse_compat_ok_u_12633, alsa_ok_u_12634, jack_ok_u_12635,
 * audio_latency_ok_u_12636, bluetooth_audio_ok_u_12637,
 * hdmi_audio_ok_u_12638, audio_soft_ready_u_12639, batch_id_12640).
 * Unique surface only; no multi-def. Distinct from
 * gj_hdmi_audio_ok_u_12438 (batch12438), gj_hdmi_audio_ok_u_10338
 * (batch10338), and gj_hdmi_audio_ok_u_10238 (batch10238). No parent
 * wires. No __int128. No HDMI audio implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12638_marker[] = "libcgj-batch12638";

/* Soft HDMI-audio-ok lamp: always off (not a real HDMI audio probe). */
#define B12638_HDMI_AUDIO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12638_hdmi_audio_ok(void)
{
	return B12638_HDMI_AUDIO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdmi_audio_ok_u_12638 - HDMI audio ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDMI endpoints
 * or call libc. No parent wires.
 */
uint32_t
gj_hdmi_audio_ok_u_12638(void)
{
	(void)NULL;
	return b12638_hdmi_audio_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdmi_audio_ok_u_12638(void)
    __attribute__((alias("gj_hdmi_audio_ok_u_12638")));
