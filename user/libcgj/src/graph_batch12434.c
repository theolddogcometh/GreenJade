/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12434: ALSA ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_alsa_ok_u_12434(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ALSA pcm/ctl device probe for the audio soft continuum.
 *   uint32_t __gj_alsa_ok_u_12434  (alias)
 *   __libcgj_batch12434_marker = "libcgj-batch12434"
 *
 * Exclusive continuum CREATE-ONLY (12431-12440: audio soft id
 * stubs — pipewire_ok_u_12431, wireplumber_ok_u_12432,
 * pulse_compat_ok_u_12433, alsa_ok_u_12434, jack_ok_u_12435,
 * audio_latency_ok_u_12436, bluetooth_audio_ok_u_12437,
 * hdmi_audio_ok_u_12438, audio_soft_ready_u_12439, batch_id_12440).
 * Unique surface only; no multi-def. Distinct from
 * gj_alsa_ok_u_10334 (batch10334) and gj_alsa_ok_u_10234 (batch10234).
 * No parent wires. No __int128. No ALSA implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12434_marker[] = "libcgj-batch12434";

/* Soft ALSA-ok lamp: always off (not a real ALSA probe). */
#define B12434_ALSA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12434_alsa_ok(void)
{
	return B12434_ALSA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alsa_ok_u_12434 - ALSA ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open ALSA devices or
 * call libc. No parent wires.
 */
uint32_t
gj_alsa_ok_u_12434(void)
{
	(void)NULL;
	return b12434_alsa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_alsa_ok_u_12434(void)
    __attribute__((alias("gj_alsa_ok_u_12434")));
