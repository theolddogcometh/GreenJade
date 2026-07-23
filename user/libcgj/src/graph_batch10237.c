/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10237: audio/pipewire soft bluetooth-audio-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bluetooth_audio_ok_u_10237(void);
 *     - Returns 0 (Bluetooth audio soft path not asserted for the audio
 *       product soft-id wave). Soft compile-time product tag; not a
 *       live BlueZ/A2DP/HFP device probe.
 *   uint32_t __gj_bluetooth_audio_ok_u_10237  (alias)
 *   __libcgj_batch10237_marker = "libcgj-batch10237"
 *
 * Exclusive audio/pipewire soft product CREATE-ONLY (10231-10240:
 * pipewire_ok, wireplumber_ok, pulse_compat_ok, alsa_ok, jack_ok,
 * audio_latency_ok, bluetooth_audio_ok, hdmi_audio_ok,
 * audio_soft_ready, batch_id_10240). Unique
 * gj_bluetooth_audio_ok_u_10237 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10237_marker[] = "libcgj-batch10237";

/* Audio product soft bluetooth-audio-ok lamp (not asserted). */
#define B10237_BLUETOOTH_AUDIO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10237_ok(void)
{
	return B10237_BLUETOOTH_AUDIO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bluetooth_audio_ok_u_10237 - audio product soft bluetooth-audio-ok lamp.
 *
 * Always returns 0. Soft pure-data product tag. Does not scan Bluetooth
 * audio devices or call libc. No parent wires.
 */
uint32_t
gj_bluetooth_audio_ok_u_10237(void)
{
	(void)NULL;
	return b10237_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bluetooth_audio_ok_u_10237(void)
    __attribute__((alias("gj_bluetooth_audio_ok_u_10237")));
