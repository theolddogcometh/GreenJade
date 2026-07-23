/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10238: audio/pipewire soft hdmi-audio-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdmi_audio_ok_u_10238(void);
 *     - Returns 0 (HDMI audio soft path not asserted for the audio
 *       product soft-id wave). Soft compile-time product tag; not a
 *       live HDMI/DisplayPort ELD or codec probe.
 *   uint32_t __gj_hdmi_audio_ok_u_10238  (alias)
 *   __libcgj_batch10238_marker = "libcgj-batch10238"
 *
 * Exclusive audio/pipewire soft product CREATE-ONLY (10231-10240:
 * pipewire_ok, wireplumber_ok, pulse_compat_ok, alsa_ok, jack_ok,
 * audio_latency_ok, bluetooth_audio_ok, hdmi_audio_ok,
 * audio_soft_ready, batch_id_10240). Unique gj_hdmi_audio_ok_u_10238
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10238_marker[] = "libcgj-batch10238";

/* Audio product soft hdmi-audio-ok lamp (not asserted). */
#define B10238_HDMI_AUDIO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10238_ok(void)
{
	return B10238_HDMI_AUDIO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdmi_audio_ok_u_10238 - audio product soft hdmi-audio-ok lamp.
 *
 * Always returns 0. Soft pure-data product tag. Does not probe HDMI
 * audio endpoints or call libc. No parent wires.
 */
uint32_t
gj_hdmi_audio_ok_u_10238(void)
{
	(void)NULL;
	return b10238_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdmi_audio_ok_u_10238(void)
    __attribute__((alias("gj_hdmi_audio_ok_u_10238")));
