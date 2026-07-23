/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14338: HDMI audio ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdmi_audio_ok_u_14338(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDMI/DisplayPort audio endpoint probe for the audio soft
 *       continuum.
 *   uint32_t __gj_hdmi_audio_ok_u_14338  (alias)
 *   __libcgj_batch14338_marker = "libcgj-batch14338"
 *
 * Exclusive continuum CREATE-ONLY (14331-14340: audio soft id
 * stubs — pipewire_ok_u_14331, wireplumber_ok_u_14332,
 * pulse_compat_ok_u_14333, alsa_ok_u_14334, jack_ok_u_14335,
 * audio_latency_ok_u_14336, bluetooth_audio_ok_u_14337,
 * hdmi_audio_ok_u_14338, audio_soft_ready_u_14339, batch_id_14340).
 * Unique surface only; no multi-def. Distinct from
 * gj_hdmi_audio_ok_u_13038 (batch13038), gj_hdmi_audio_ok_u_12838
 * (batch12838), gj_hdmi_audio_ok_u_12638 (batch12638),
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

const char __libcgj_batch14338_marker[] = "libcgj-batch14338";

/* Soft HDMI-audio-ok lamp: always off (not a real HDMI audio probe). */
#define B14338_HDMI_AUDIO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14338_hdmi_audio_ok(void)
{
	return B14338_HDMI_AUDIO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdmi_audio_ok_u_14338 - HDMI audio ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDMI endpoints
 * or call libc. No parent wires.
 */
uint32_t
gj_hdmi_audio_ok_u_14338(void)
{
	(void)NULL;
	return b14338_hdmi_audio_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdmi_audio_ok_u_14338(void)
    __attribute__((alias("gj_hdmi_audio_ok_u_14338")));
