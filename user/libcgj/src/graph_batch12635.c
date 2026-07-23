/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12635: JACK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_jack_ok_u_12635(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       JACK server or client probe for the audio soft continuum.
 *   uint32_t __gj_jack_ok_u_12635  (alias)
 *   __libcgj_batch12635_marker = "libcgj-batch12635"
 *
 * Exclusive continuum CREATE-ONLY (12631-12640: audio soft id
 * stubs — pipewire_ok_u_12631, wireplumber_ok_u_12632,
 * pulse_compat_ok_u_12633, alsa_ok_u_12634, jack_ok_u_12635,
 * audio_latency_ok_u_12636, bluetooth_audio_ok_u_12637,
 * hdmi_audio_ok_u_12638, audio_soft_ready_u_12639, batch_id_12640).
 * Unique surface only; no multi-def. Distinct from
 * gj_jack_ok_u_12435 (batch12435), gj_jack_ok_u_10335 (batch10335),
 * and gj_jack_ok_u_10235 (batch10235). No parent wires. No __int128.
 * No JACK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12635_marker[] = "libcgj-batch12635";

/* Soft JACK-ok lamp: always off (not a real JACK probe). */
#define B12635_JACK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12635_jack_ok(void)
{
	return B12635_JACK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jack_ok_u_12635 - JACK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open JACK or call
 * libc. No parent wires.
 */
uint32_t
gj_jack_ok_u_12635(void)
{
	(void)NULL;
	return b12635_jack_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_jack_ok_u_12635(void)
    __attribute__((alias("gj_jack_ok_u_12635")));
