/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10231: audio/pipewire soft pipewire-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_ok_u_10231(void);
 *     - Returns 0 (pipewire soft path not asserted for the audio
 *       product soft-id wave). Soft compile-time product tag; not a
 *       live PipeWire session or node probe.
 *   uint32_t __gj_pipewire_ok_u_10231  (alias)
 *   __libcgj_batch10231_marker = "libcgj-batch10231"
 *
 * Exclusive audio/pipewire soft product CREATE-ONLY (10231-10240:
 * pipewire_ok, wireplumber_ok, pulse_compat_ok, alsa_ok, jack_ok,
 * audio_latency_ok, bluetooth_audio_ok, hdmi_audio_ok,
 * audio_soft_ready, batch_id_10240). Unique gj_pipewire_ok_u_10231
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10231_marker[] = "libcgj-batch10231";

/* Audio product soft pipewire-ok lamp (not asserted). */
#define B10231_PIPEWIRE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10231_ok(void)
{
	return B10231_PIPEWIRE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_ok_u_10231 - audio product soft pipewire-ok lamp.
 *
 * Always returns 0. Soft pure-data product tag. Does not open a
 * PipeWire context, enumerate nodes, or call libc. No parent wires.
 */
uint32_t
gj_pipewire_ok_u_10231(void)
{
	(void)NULL;
	return b10231_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_ok_u_10231(void)
    __attribute__((alias("gj_pipewire_ok_u_10231")));
