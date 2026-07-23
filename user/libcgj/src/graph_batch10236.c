/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10236: audio/pipewire soft audio-latency-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_latency_ok_u_10236(void);
 *     - Returns 0 (audio latency soft path not asserted for the audio
 *       product soft-id wave). Soft compile-time product tag; not a
 *       live quantum/period or buffer-size probe.
 *   uint32_t __gj_audio_latency_ok_u_10236  (alias)
 *   __libcgj_batch10236_marker = "libcgj-batch10236"
 *
 * Exclusive audio/pipewire soft product CREATE-ONLY (10231-10240:
 * pipewire_ok, wireplumber_ok, pulse_compat_ok, alsa_ok, jack_ok,
 * audio_latency_ok, bluetooth_audio_ok, hdmi_audio_ok,
 * audio_soft_ready, batch_id_10240). Unique
 * gj_audio_latency_ok_u_10236 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10236_marker[] = "libcgj-batch10236";

/* Audio product soft audio-latency-ok lamp (not asserted). */
#define B10236_AUDIO_LATENCY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10236_ok(void)
{
	return B10236_AUDIO_LATENCY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_latency_ok_u_10236 - audio product soft audio-latency-ok lamp.
 *
 * Always returns 0. Soft pure-data product tag. Does not measure
 * latency or call libc. No parent wires.
 */
uint32_t
gj_audio_latency_ok_u_10236(void)
{
	(void)NULL;
	return b10236_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_latency_ok_u_10236(void)
    __attribute__((alias("gj_audio_latency_ok_u_10236")));
