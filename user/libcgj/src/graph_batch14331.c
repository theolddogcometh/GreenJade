/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14331: PipeWire ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipewire_ok_u_14331(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PipeWire session or node probe for the audio soft continuum.
 *   uint32_t __gj_pipewire_ok_u_14331  (alias)
 *   __libcgj_batch14331_marker = "libcgj-batch14331"
 *
 * Exclusive continuum CREATE-ONLY (14331-14340: audio soft id
 * stubs — pipewire_ok_u_14331, wireplumber_ok_u_14332,
 * pulse_compat_ok_u_14333, alsa_ok_u_14334, jack_ok_u_14335,
 * audio_latency_ok_u_14336, bluetooth_audio_ok_u_14337,
 * hdmi_audio_ok_u_14338, audio_soft_ready_u_14339, batch_id_14340).
 * Unique surface only; no multi-def. Distinct from
 * gj_pipewire_ok_u_13031 (batch13031), gj_pipewire_ok_u_12831
 * (batch12831), gj_pipewire_ok_u_12631 (batch12631),
 * gj_pipewire_ok_u_12431 (batch12431), gj_pipewire_ok_u_12231
 * (batch12231), and gj_pipewire_ok_u_10331 (batch10331). No parent
 * wires. No __int128. No PipeWire implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14331_marker[] = "libcgj-batch14331";

/* Soft PipeWire-ok lamp: always off (not a real pipewire probe). */
#define B14331_PIPEWIRE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14331_pipewire_ok(void)
{
	return B14331_PIPEWIRE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipewire_ok_u_14331 - PipeWire ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open a PipeWire
 * context, enumerate nodes, or call libc. No parent wires.
 */
uint32_t
gj_pipewire_ok_u_14331(void)
{
	(void)NULL;
	return b14331_pipewire_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pipewire_ok_u_14331(void)
    __attribute__((alias("gj_pipewire_ok_u_14331")));
