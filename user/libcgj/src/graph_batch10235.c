/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10235: audio/pipewire soft jack-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_jack_ok_u_10235(void);
 *     - Returns 0 (JACK soft path not asserted for the audio product
 *       soft-id wave). Soft compile-time product tag; not a live JACK
 *       server or jackdbus probe.
 *   uint32_t __gj_jack_ok_u_10235  (alias)
 *   __libcgj_batch10235_marker = "libcgj-batch10235"
 *
 * Exclusive audio/pipewire soft product CREATE-ONLY (10231-10240:
 * pipewire_ok, wireplumber_ok, pulse_compat_ok, alsa_ok, jack_ok,
 * audio_latency_ok, bluetooth_audio_ok, hdmi_audio_ok,
 * audio_soft_ready, batch_id_10240). Unique gj_jack_ok_u_10235 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10235_marker[] = "libcgj-batch10235";

/* Audio product soft jack-ok lamp (not asserted). */
#define B10235_JACK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10235_ok(void)
{
	return B10235_JACK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jack_ok_u_10235 - audio product soft jack-ok lamp.
 *
 * Always returns 0. Soft pure-data product tag. Does not connect to
 * JACK or call libc. No parent wires.
 */
uint32_t
gj_jack_ok_u_10235(void)
{
	(void)NULL;
	return b10235_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_jack_ok_u_10235(void)
    __attribute__((alias("gj_jack_ok_u_10235")));
