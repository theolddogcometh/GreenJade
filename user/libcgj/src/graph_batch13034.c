/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13034: ALSA ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_alsa_ok_u_13034(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ALSA pcm/ctl device probe for the audio soft continuum.
 *   uint32_t __gj_alsa_ok_u_13034  (alias)
 *   __libcgj_batch13034_marker = "libcgj-batch13034"
 *
 * Exclusive continuum CREATE-ONLY (13031-13040: audio soft id
 * stubs — pipewire_ok_u_13031, wireplumber_ok_u_13032,
 * pulse_compat_ok_u_13033, alsa_ok_u_13034, jack_ok_u_13035,
 * audio_latency_ok_u_13036, bluetooth_audio_ok_u_13037,
 * hdmi_audio_ok_u_13038, audio_soft_ready_u_13039, batch_id_13040).
 * Unique surface only; no multi-def. Distinct from
 * gj_alsa_ok_u_12834 (batch12834), gj_alsa_ok_u_12634 (batch12634),
 * gj_alsa_ok_u_12434 (batch12434), gj_alsa_ok_u_10334 (batch10334),
 * and gj_alsa_ok_u_10234 (batch10234). No parent wires. No __int128.
 * No ALSA implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13034_marker[] = "libcgj-batch13034";

/* Soft ALSA-ok lamp: always off (not a real ALSA probe). */
#define B13034_ALSA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13034_alsa_ok(void)
{
	return B13034_ALSA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_alsa_ok_u_13034 - ALSA ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open ALSA devices or
 * call libc. No parent wires.
 */
uint32_t
gj_alsa_ok_u_13034(void)
{
	(void)NULL;
	return b13034_alsa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_alsa_ok_u_13034(void)
    __attribute__((alias("gj_alsa_ok_u_13034")));
