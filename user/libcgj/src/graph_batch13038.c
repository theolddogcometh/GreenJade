/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13038: HDMI audio ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdmi_audio_ok_u_13038(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDMI/DisplayPort audio endpoint probe for the audio soft
 *       continuum.
 *   uint32_t __gj_hdmi_audio_ok_u_13038  (alias)
 *   __libcgj_batch13038_marker = "libcgj-batch13038"
 *
 * Exclusive continuum CREATE-ONLY (13031-13040: audio soft id
 * stubs — pipewire_ok_u_13031, wireplumber_ok_u_13032,
 * pulse_compat_ok_u_13033, alsa_ok_u_13034, jack_ok_u_13035,
 * audio_latency_ok_u_13036, bluetooth_audio_ok_u_13037,
 * hdmi_audio_ok_u_13038, audio_soft_ready_u_13039, batch_id_13040).
 * Unique surface only; no multi-def. Distinct from
 * gj_hdmi_audio_ok_u_12838 (batch12838), gj_hdmi_audio_ok_u_12638
 * (batch12638), gj_hdmi_audio_ok_u_12438 (batch12438),
 * gj_hdmi_audio_ok_u_10338 (batch10338), and gj_hdmi_audio_ok_u_10238
 * (batch10238). No parent wires. No __int128. No HDMI audio
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13038_marker[] = "libcgj-batch13038";

/* Soft HDMI-audio-ok lamp: always off (not a real HDMI audio probe). */
#define B13038_HDMI_AUDIO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13038_hdmi_audio_ok(void)
{
	return B13038_HDMI_AUDIO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdmi_audio_ok_u_13038 - HDMI audio ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDMI endpoints
 * or call libc. No parent wires.
 */
uint32_t
gj_hdmi_audio_ok_u_13038(void)
{
	(void)NULL;
	return b13038_hdmi_audio_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdmi_audio_ok_u_13038(void)
    __attribute__((alias("gj_hdmi_audio_ok_u_13038")));
