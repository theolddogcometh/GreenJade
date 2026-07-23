/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13231: PipeWire ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_ok_u_13231(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PipeWire session or node probe for the audio soft continuum.
 *   uint32_t __gj_pipewire_ok_u_13231  (alias)
 *   __libcgj_batch13231_marker = "libcgj-batch13231"
 *
 * Exclusive continuum CREATE-ONLY (13231-13240: audio soft id
 * stubs — pipewire_ok_u_13231, wireplumber_ok_u_13232,
 * pulse_compat_ok_u_13233, alsa_ok_u_13234, jack_ok_u_13235,
 * audio_latency_ok_u_13236, bluetooth_audio_ok_u_13237,
 * hdmi_audio_ok_u_13238, audio_soft_ready_u_13239, batch_id_13240).
 * Unique surface only; no multi-def. Distinct from
 * gj_pipewire_ok_u_13031 (batch13031), gj_pipewire_ok_u_12831
 * (batch12831), gj_pipewire_ok_u_12631 (batch12631),
 * gj_pipewire_ok_u_12431 (batch12431), gj_pipewire_ok_u_12231
 * (batch12231), and gj_pipewire_ok_u_12031 (batch12031). No parent
 * wires. No __int128. No PipeWire implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13231_marker[] = "libcgj-batch13231";

/* Soft PipeWire-ok lamp: always off (not a real pipewire probe). */
#define B13231_PIPEWIRE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13231_pipewire_ok(void)
{
	return B13231_PIPEWIRE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_ok_u_13231 - PipeWire ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open a PipeWire
 * context, enumerate nodes, or call libc. No parent wires.
 */
uint32_t
gj_pipewire_ok_u_13231(void)
{
	(void)NULL;
	return b13231_pipewire_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_ok_u_13231(void)
    __attribute__((alias("gj_pipewire_ok_u_13231")));
