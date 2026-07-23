/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13434: ALSA ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_alsa_ok_u_13434(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ALSA pcm/ctl device probe for the audio soft continuum.
 *   uint32_t __gj_alsa_ok_u_13434  (alias)
 *   __libcgj_batch13434_marker = "libcgj-batch13434"
 *
 * Exclusive continuum CREATE-ONLY (13431-13440: audio soft id
 * stubs — pipewire_ok_u_13431, wireplumber_ok_u_13432,
 * pulse_compat_ok_u_13433, alsa_ok_u_13434, jack_ok_u_13435,
 * audio_latency_ok_u_13436, bluetooth_audio_ok_u_13437,
 * hdmi_audio_ok_u_13438, audio_soft_ready_u_13439, batch_id_13440).
 * Unique surface only; no multi-def. Distinct from
 * gj_alsa_ok_u_13234 (batch13234), gj_alsa_ok_u_13034 (batch13034),
 * gj_alsa_ok_u_12834 (batch12834), gj_alsa_ok_u_12634 (batch12634),
 * and gj_alsa_ok_u_12434 (batch12434). No parent wires. No __int128.
 * No ALSA implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13434_marker[] = "libcgj-batch13434";

/* Soft ALSA-ok lamp: always off (not a real ALSA probe). */
#define B13434_ALSA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13434_alsa_ok(void)
{
	return B13434_ALSA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alsa_ok_u_13434 - ALSA ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open ALSA devices or
 * call libc. No parent wires.
 */
uint32_t
gj_alsa_ok_u_13434(void)
{
	(void)NULL;
	return b13434_alsa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_alsa_ok_u_13434(void)
    __attribute__((alias("gj_alsa_ok_u_13434")));
