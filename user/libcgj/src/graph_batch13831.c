/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13831: PipeWire ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_ok_u_13831(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PipeWire session or node probe for the audio soft continuum.
 *   uint32_t __gj_pipewire_ok_u_13831  (alias)
 *   __libcgj_batch13831_marker = "libcgj-batch13831"
 *
 * Exclusive continuum CREATE-ONLY (13831-13840: audio soft id
 * stubs — pipewire_ok_u_13831, wireplumber_ok_u_13832,
 * pulse_compat_ok_u_13833, alsa_ok_u_13834, jack_ok_u_13835,
 * audio_latency_ok_u_13836, bluetooth_audio_ok_u_13837,
 * hdmi_audio_ok_u_13838, audio_soft_ready_u_13839, batch_id_13840).
 * Unique surface only; no multi-def. Distinct from
 * gj_pipewire_ok_u_13631 (batch13631), gj_pipewire_ok_u_13431
 * (batch13431), gj_pipewire_ok_u_13231 (batch13231),
 * gj_pipewire_ok_u_13031 (batch13031), gj_pipewire_ok_u_12831
 * (batch12831), and gj_pipewire_ok_u_12631 (batch12631). No parent
 * wires. No __int128. No PipeWire implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13831_marker[] = "libcgj-batch13831";

/* Soft PipeWire-ok lamp: always off (not a real pipewire probe). */
#define B13831_PIPEWIRE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13831_pipewire_ok(void)
{
	return B13831_PIPEWIRE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_ok_u_13831 - PipeWire ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open a PipeWire
 * context, enumerate nodes, or call libc. No parent wires.
 */
uint32_t
gj_pipewire_ok_u_13831(void)
{
	(void)NULL;
	return b13831_pipewire_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_ok_u_13831(void)
    __attribute__((alias("gj_pipewire_ok_u_13831")));
